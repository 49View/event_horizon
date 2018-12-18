#include "renderer.h"

//#include <CImg.h>
#include "camera_manager.h"
#include "graphic_functions.hpp"
#include "light_manager.h"
#include "render_list.h"
#include "shader_manager.h"
#include "shadowmap_manager.h"
#include "font_manager.h"
#include "core/math/spherical_harmonics.h"
#include "core/configuration/app_options.h"
#include "core/suncalc/sun_builder.h"
#include "core/zlib_util.h"
#include "core/tar_util.h"
#include "core/profiler.h"

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include <stb/stb_image_write.h>
#include "core/service_factory.h"

namespace FBNames {
    static std::unordered_set<std::string> sFBNames;

    bool isPartOf( const std::string& _val ) {
        if ( sFBNames.empty() ) {
            sFBNames.insert( shadowmap );
            sFBNames.insert( lightmap );
            sFBNames.insert( sceneprobe );
            sFBNames.insert( convolution );
            sFBNames.insert( specular_prefilter );
            sFBNames.insert( ibl_brdf );
            sFBNames.insert( blur_horizontal );
            sFBNames.insert( blur_vertical );
            sFBNames.insert( colorFinalFrameBuffer );
            sFBNames.insert( offScreenFinalFrameBuffer );
        }
        return sFBNames.find( _val ) != sFBNames.end();
    }

}

//#include "vr_manager.hpp"

CommandScriptRendererManager::CommandScriptRendererManager( Renderer& _rr ) {
    addCommandDefinition("reload shaders", std::bind(&Renderer::cmdReloadShaders, &_rr, std::placeholders::_1 ));
}

void Renderer::cmdReloadShaders( [[maybe_unused]] const std::vector<std::string>& _params ) {
    ShaderAssetBuilder{ "shaders" }.rebuild( *this );
}

bool RenderImageDependencyMaker::addImpl( ImageBuilder& tbd, std::unique_ptr<uint8_t[]>& _data ) {

    tm.addTextureFromCallback(TextureRenderData{ tbd.Name() }.size(tbd.imageParams.width, tbd.imageParams.height).ch(tbd.imageParams.channels)
                                                 .setBpp(tbd.imageParams.bpp)
                                                 .setFormatFromBpp().target(tbd.imageParams.ttm).wm(tbd.imageParams.wrapMode)
                                                 .fm(tbd.imageParams.filterMode), _data );
    return true;
}

bool ShaderAssetBuilder::makeImpl( DependencyMaker& _md, uint8_p&& _data, const DependencyStatus _status ) {
    Renderer& rr = static_cast<Renderer&>(_md);

    if ( _status == DependencyStatus::LoadedSuccessfully ) {
        tarUtil::untar<ShaderBuilder>( zlibUtil::inflateFromMemory( std::move( _data )), rr.sm);
    }

    rr.sm.loadShaders();
    rr.postInit();
    rr.afterShaderSetup();

    return true;
}

Renderer::Renderer( CommandQueue& cq, ShaderManager& sm, FontManager& fm, TextureManager& tm, CameraManager& _cm ) :
        cq( cq ), sm( sm ), fm(fm), tm(tm), cm(_cm), ridm(tm) {
    mCommandBuffers = std::make_shared<CommandBufferList>(*this);
    hcs = std::make_shared<CommandScriptRendererManager>(*this);
    cq.registerCommandScript(hcs);
}

void Renderer::postInit() {

    if ( mbIsInitialized ) return;

    tm.addTextureWithData(FBNames::lightmap, RawImage::WHITE4x4(), TSLOT_LIGHTMAP );

    mbIsInitialized = true;
}

void Renderer::resetDefaultFB() {
    mDefaultFB = FrameBufferBuilder{ *this, "default" }.build();
}

void Renderer::init() {
    resetDefaultFB();
    rcm.init();
    am.init();
    lm.init();
}

void Renderer::afterShaderSetup() {
    lm.generateUBO( sm );
    am.generateUBO( sm );
    rcm.generateUBO( sm );
    for ( const auto& target : mTargets ) {
        target->afterShaderSetup();
    }

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

void Renderer::directRenderLoop( const GameTime& gt ) {

    if ( !mbIsInitialized ) return;

    CB_U().start();
    CB_U().startList( nullptr, CommandBufferFlags::CBF_DoNotSort );
    CB_U().pushCommand( { CommandBufferCommandName::clearDefaultFramebuffer } );
    CB_U().pushCommand( { CommandBufferCommandName::setGlobalTextures } );

    for ( const auto& target : mTargets ) {
        if ( target->enabled() ) {
            target->addToCB( CB_U() );
        }
    }
    CB_U().end();

//    VRM.preRender();
    am.setTiming( gt );
    lm.setUniforms_r();
    am.setUniforms_r();

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
    CB_R().render( eye );
}

std::shared_ptr<VPList> Renderer::VPL( const int _bucket,
                                       [[maybe_unused]] const std::string& _key,
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

void Renderer::clearVPAddList() {
    mVPToAdd.clear();
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

void Renderer::changeMaterialOnTags( uint64_t _tag, std::shared_ptr<PBRMaterial> _mat ) {
    auto rmaterial = RenderMaterialBuilder{*this}.m(_mat).build();

    for ( const auto& [k, vl] : CL() ) {
        if ( CommandBufferLimits::PBRStart <= k && CommandBufferLimits::PBREnd >= k ) {
            for ( const auto& v : vl.mVList ) {
                v->setMaterialWithTag(rmaterial, _tag);
            }
            for ( const auto& v : vl.mVListTransparent ) {
                v->setMaterialWithTag(rmaterial, _tag);
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

void Renderer::setRenderHook( const std::string& _key, std::weak_ptr<CommandBufferEntry>& _hook ) {
    CB_R().getCommandBufferEntry( _key, _hook );
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

void Renderer::MaterialCache( const MaterialType& mt, std::shared_ptr<RenderMaterial> _mat ) {
    materialCache[mt] = _mat;
}

void Renderer::MaterialMap( std::shared_ptr<RenderMaterial> _mat ) {
    materialMap[_mat->Hash()] = _mat;
}

std::shared_ptr<Texture> Renderer::TD( const std::string& _id, const int tSlot ) {
    return tm.TD( _id, tSlot );
}

bool Renderer::isInitialized() const {
    return mbIsInitialized;
}

void Renderer::clearTargets() {
    for ( const auto& target : mTargets ) {
        target->clearCB( CB_U() );
    }
}

void RenderAnimationManager::setTiming( const GameTime& _gt ) {
    gt = _gt;
    mAnimUniforms->setUBOData( UniformNames::deltaAnimTime, Vector4f{gt.mCurrTimeStep, gt.mCurrTimeStamp,
                                                                      gt.mLastTimeStamp, 0.0f } );
}

void RenderAnimationManager::setUniforms_r() {
    mAnimUniforms->submitUBOData();
}

void RenderAnimationManager::init() {
    mAnimUniforms = std::make_unique<ProgramUniformSet>();
    mAnimUniforms->setUBOStructure( UniformNames::pointLightPos, 16 );
}

void RenderAnimationManager::generateUBO( const ShaderManager& sm ) {
    mAnimUniforms->generateUBO( sm, "AnimationUniforms" );// u_deltaAnimTime );
}

void RenderCameraManager::init() {
    mCameraUBO = std::make_shared<ProgramUniformSet>();
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
