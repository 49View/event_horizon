#include "renderer.h"

//#include <CImg.h>
#include "graphics/graphic_functions.hpp"
#include "graphics/light_manager.h"
#include "graphics/render_list.h"
#include "graphics/shader_manager.h"
#include "graphics/shadowmap_manager.h"
#include "core/math/spherical_harmonics.h"
#include "core/configuration/app_options.h"
#include "core/suncalc/sun_builder.h"
#include "core/zlib_util.h"
#include "core/tar_util.h"
#include "core/profiler.h"
#include "core/streaming_mediator.hpp"

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include <stb/stb_image_write.h>
#include "core/service_factory.h"

#include "di_modules.h"

namespace FBNames {
    static std::unordered_set<std::string> sFBNames;

    bool isPartOf( const std::string& _val ) {
        if ( sFBNames.empty() ) {
            sFBNames.insert( shadowmap );
            sFBNames.insert( lightmap );
            sFBNames.insert( sceneprobe );
            sFBNames.insert( MPBRTextures::convolution );
            sFBNames.insert( MPBRTextures::specular_prefilter );
            sFBNames.insert( MPBRTextures::ibl_brdf );
            sFBNames.insert( blur_horizontal );
            sFBNames.insert( blur_vertical );
            sFBNames.insert( colorFinalFrameBuffer );
            sFBNames.insert( offScreenFinalFrameBuffer );
        }
        return sFBNames.find( _val ) != sFBNames.end();
    }

}

//#include "vr_manager.hpp"

class CommandScriptRendererManager : public CommandScript {
public:
    CommandScriptRendererManager( Renderer& hm );
    virtual ~CommandScriptRendererManager() {}
};

CommandScriptRendererManager::CommandScriptRendererManager( Renderer& _rr ) {
    addCommandDefinition("reload shaders", std::bind(&Renderer::cmdReloadShaders, &_rr, std::placeholders::_1 ));
}

void Renderer::cmdReloadShaders( [[maybe_unused]] const std::vector<std::string>& _params ) {
    sm.loadShaders();
    afterShaderSetup();
}

bool RenderImageDependencyMaker::addImpl( ImageBuilder& tbd, std::unique_ptr<uint8_t[]>& _data ) {

    tm.addTextureFromCallback(TextureRenderData{ tbd.Name() }.size(tbd.imageParams.width, tbd.imageParams.height).ch(tbd.imageParams.channels)
                                                 .setBpp(tbd.imageParams.bpp)
                                                 .setFormatFromBpp().target(tbd.imageParams.ttm).wm(tbd.imageParams.wrapMode)
                                                 .fm(tbd.imageParams.filterMode), _data );
    return true;
}

Renderer::Renderer( CommandQueue& cq, ShaderManager& sm, TextureManager& tm, StreamingMediator& _ssm, LightManager& _lm ) :
        cq( cq ), sm( sm ), tm(tm), ridm(tm), ssm(_ssm), lm(_lm) {
    mCommandBuffers = std::make_shared<CommandBufferList>(*this);
    hcs = std::make_shared<CommandScriptRendererManager>(*this);
    cq.registerCommandScript(hcs);
}

StreamingMediator& Renderer::SSM() {
    return ssm;
}

void Renderer::resetDefaultFB( const Vector2i& forceSize ) {
    mDefaultFB = FrameBufferBuilder{ *this, "default" }.size(forceSize).build();
}

void Renderer::init() {
    resetDefaultFB();
    rcm.init();
    am.init();
    lm.init();
    sm.loadShaders();
    tm.addTextureWithData(FBNames::lightmap, RawImage::WHITE4x4(), TSLOT_LIGHTMAP );
    afterShaderSetup();
}

void Renderer::injectShader( const std::string& _key, const std::string& _content ) {
    sm.inject( _key, _content );
}

void Renderer::afterShaderSetup() {
    lm.generateUBO( sm );
    am.generateUBO( sm );
    rcm.generateUBO( sm );
}

void Renderer::setGlobalTextures() {
    auto p = sm.P(S::SH);

    auto lmt = tm.TD(FBNames::lightmap);
    if ( lmt ) {
        lmt->bind( lmt->textureSlot(), p->handle(), UniformNames::lightmapTexture.c_str() );
    }

    auto smt = tm.TD(FBNames::shadowmap);
    if ( smt ) {
        smt->bind( smt->textureSlot(), p->handle(), UniformNames::shadowMapTexture.c_str() );
    }
}

void Renderer::directRenderLoop( std::vector<std::shared_ptr<RLTarget>>& _targets ) {

    CB_U().start();
    CB_U().startList( nullptr, CommandBufferFlags::CBF_DoNotSort );
    CB_U().pushCommand( { CommandBufferCommandName::clearDefaultFramebuffer } );
    CB_U().pushCommand( { CommandBufferCommandName::setGlobalTextures } );

    for ( const auto& target : _targets ) {
        if ( target->enabled() ) {
            if ( bInvalidated ) target->invalidateOnAdd();
            target->updateStreams();
            target->addToCB( CB_U() );
        }
    }

    bInvalidated = false;

    CB_U().end();

//    VRM.preRender();
    am.setTiming();
    lm.setUniforms_r();
    am.setUniforms_r();

    for ( auto& mcc : mChangeMaterialCallbacks ) {
        changeMaterialOnTags( mcc );
    }
    mChangeMaterialCallbacks.clear();

    renderCBList();

//    VRM.update();

    mUpdateCounter++;
}

void Renderer::renderCBList() {
//    if ( VRM.IsOn()) {
//        for ( int eye = 0; eye < 2; eye++ ) {
//            renderCommands( eye );
//        }
//    } else
        {
        renderCommands( -1 );
    }

//    if ( VRM.IsOn()) {
//        VRM.postRender( mComposite.getVPFinalCombine() );
//    }
}

void Renderer::clearCommandList() {
    mCommandLists.clear();
}

void Renderer::renderCommands( int eye ) {
    //PROFILE_BLOCK("Total Render render");
    CB_U().render( eye );
}

std::shared_ptr<VPList> Renderer::VPL( const int _bucket,
                                       std::shared_ptr<Matrix4f> m,
                                       float alpha ) {
    auto nvp = std::make_shared<VPList>(m);
    if ( alpha < 1.0f ) {
        mCommandLists[_bucket].mVListTransparent.push_back(nvp);
    } else {
        mCommandLists[_bucket].mVList.push_back(nvp);
    }
    return nvp;
}

bool Renderer::hasTag( uint64_t _tag ) const {
    for ( const auto& [k, vl] : CL() ) {
        if ( CommandBufferLimits::PBRStart <= k && CommandBufferLimits::PBREnd >= k ) {
            for ( const auto& v : vl.mVList ) {
                if ( v->hasTag(_tag) ) return true;
            }
            for ( const auto& v : vl.mVListTransparent ) {
                if ( v->hasTag(_tag) ) return true;
            }
        }
    }
    return false;
}

std::shared_ptr<RenderMaterial> Renderer::addMaterial( const std::string& _shaderName ) {
    auto program = P( _shaderName );
    if ( program ) {
        return addMaterial( program->getDefaultUniforms(), program );
    }
    return nullptr;
}

std::shared_ptr<RenderMaterial> Renderer::addMaterial( std::shared_ptr<Material> _material,
                                                       std::shared_ptr<Program> _program ) {
    auto program = _program ? _program : P( _material->getShaderName() );
    if ( program ) {
        auto rmaterial = std::make_shared<RenderMaterial>( program, _material, *this );
        MaterialMap( rmaterial );
        return rmaterial;
    }
    return nullptr;
}

void Renderer::changeMaterialOnTagsCallback( const ChangeMaterialOnTagContainer& _cmt ) {
    mChangeMaterialCallbacks.emplace_back( _cmt );
}

void Renderer::changeMaterialOnTags( ChangeMaterialOnTagContainer& _cmt ) {
    _cmt.mat->injectIfNotPresent(*P( _cmt.mat->getShaderName() )->getDefaultUniforms().get());
    _cmt.mat->resolveDynamicConstants();

    auto rmaterial = addMaterial(_cmt.mat);


    if ( !rmaterial ) return;

    for ( const auto& [k, vl] : CL() ) {
        if ( CommandBufferLimits::PBRStart <= k && CommandBufferLimits::PBREnd >= k ) {
            for ( auto& v : vl.mVList ) {
                v->setMaterialWithTag(rmaterial, _cmt.tag);
            }
            for ( auto& v : vl.mVListTransparent ) {
                v->setMaterialWithTag(rmaterial, _cmt.tag);
            }
        }
    }
}

void Renderer::changeMaterialColorOnTags( uint64_t _tag, const Color4f& _color ) {
    // NDDado: we only use RGB, not Alpha, in here
    for ( const auto& [k, vl] : CL() ) {
        if ( CommandBufferLimits::PBRStart <= k && CommandBufferLimits::PBREnd >= k ) {
            for ( const auto& v : vl.mVList ) {
                v->setMaterialColorWithTag(_color, _tag);
            }
            for ( const auto& v : vl.mVListTransparent ) {
                v->setMaterialColorWithTag(_color, _tag);
            }
        }
    }
}

void Renderer::changeMaterialColorOnUUID( const UUID& _tag, const Color4f& _color, Color4f& _oldColor ) {
    // NDDado: we only use RGB, not Alpha, in here
    for ( const auto& [k, vl] : CL() ) {
            for ( const auto& v : vl.mVList ) {
                v->setMaterialColorWithUUID(_color, _tag, _oldColor);
            }
            for ( const auto& v : vl.mVListTransparent ) {
                v->setMaterialColorWithUUID(_color, _tag, _oldColor);
            }
    }
}

void Renderer::removeFromCL( const UUID& _uuid ) {
    auto removeUUID = [_uuid]( const auto & us ) -> bool { return us->Name() == _uuid; };

    for ( auto& [k, vl] : CL() ) {
        erase_if( vl.mVList, removeUUID );
        erase_if( vl.mVListTransparent, removeUUID );
    }
}

void Renderer::invalidateOnAdd() {
    bInvalidated = true;
}

void Renderer::setRenderHook( const std::string& _key, std::weak_ptr<CommandBufferEntry>& _hook ) {
    CB_U().getCommandBufferEntry( _key, _hook );
}

void Renderer::addToCommandBuffer( const CommandBufferLimitsT _entry ) {
    addToCommandBuffer( CL()[_entry].mVList );
    addToCommandBuffer( CL()[_entry].mVListTransparent );
}

void Renderer::addToCommandBuffer( const std::vector<std::shared_ptr<VPList>> _map,
                                   std::shared_ptr<RenderMaterial> _forcedMaterial ) {
    for ( const auto& vp : _map ) {
        vp->addToCommandBuffer( *this, vp->getTransform(), _forcedMaterial );
    }
}

std::shared_ptr<Program> Renderer::P( const std::string& _id ) {
    return sm.P(_id);
}

void Renderer::MaterialCache( uint64_t mt, std::shared_ptr<RenderMaterial> _mat ) {
    materialCache[mt] = _mat;
}

void Renderer::MaterialMap( std::shared_ptr<RenderMaterial> _mat ) {
    materialMap[_mat->Hash()] = _mat;
}

std::shared_ptr<Texture> Renderer::TD( const std::string& _id, const int tSlot ) {
    return tm.TD( _id, tSlot );
}

TextureIndex Renderer::TDI( const std::string& _id, unsigned int tSlot ) {
    auto t = TD( _id, tSlot );
    return { t->name(),  t->getHandle(), tSlot, t->getTarget() };
}

void RenderAnimationManager::setTiming() {
    mAnimUniforms->setUBOData( UniformNames::deltaAnimTime, Vector4f{GameTime::getCurrTimeStep(), GameTime::getCurrTimeStamp(),
                                                                     GameTime::getLastTimeStamp(), 0.0f } );
}

void RenderAnimationManager::setUniforms_r() {
    mAnimUniforms->submitUBOData();
}

void RenderAnimationManager::init() {
    mAnimUniforms = std::make_unique<ProgramUniformSet>("anim", "ubo");
    mAnimUniforms->setUBOStructure( UniformNames::pointLightPos, 16 );
}

void RenderAnimationManager::generateUBO( const ShaderManager& sm ) {
    mAnimUniforms->generateUBO( sm, "AnimationUniforms" );// u_deltaAnimTime );
}

void RenderCameraManager::init() {
    mCameraUBO = std::make_shared<ProgramUniformSet>("camera", "ubo");
    mCameraUBO->setUBOStructure( UniformNames::mvpMatrix, 64 );
    mCameraUBO->setUBOStructure( UniformNames::viewMatrix, 64 );
    mCameraUBO->setUBOStructure( UniformNames::projMatrix, 64 );
    mCameraUBO->setUBOStructure( UniformNames::screenSpaceMatrix, 64 );
    mCameraUBO->setUBOStructure( UniformNames::eyePos, 16 );
}

void RenderCameraManager::generateUBO( const ShaderManager& sm ) {
    mCameraUBO->generateUBO( sm, "CameraUniforms" );
}

std::shared_ptr<ProgramUniformSet>& RenderCameraManager::UBO() {
    return mCameraUBO;
}
