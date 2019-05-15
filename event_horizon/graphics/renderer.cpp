#include "renderer.h"

#include <core/math/spherical_harmonics.h>
#include <core/command.hpp>
#include <core/raw_image.h>
#include <core/game_time.h>
#include <core/configuration/app_options.h>
#include <core/suncalc/sun_builder.h>
#include <core/zlib_util.h>
#include <core/camera_rig.hpp>
#include <core/tar_util.h>
#include <core/profiler.h>
#include <core/v_data.hpp>
#include <core/resources/material.h>
#include <core/streaming_mediator.hpp>
#include <core/descriptors/uniform_names.h>
#include <core/resources/resource_utils.hpp>
#include <graphics/graphic_functions.hpp>
#include <graphics/light_manager.h>
#include <graphics/render_list.h>
#include <graphics/render_targets.hpp>
#include <graphics/shader_manager.h>
#include <graphics/shadowmap_manager.h>
#include <graphics/shader_material.hpp>
#include <graphics/render_material_manager.hpp>
#include <graphics/gpuv_data_manager.hpp>

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

std::shared_ptr<PosTexNorTanBinUV2Col3dStrip>
generateGeometryVP( std::shared_ptr<VData> _data ) {
    if ( _data->numIndices() < 3 ) return nullptr;

    std::unique_ptr<int32_t[]> _indices = std::unique_ptr<int32_t[]>( new int32_t[_data->numIndices()] );
    std::memcpy( _indices.get(), _data->Indices(), _data->numIndices() * sizeof( int32_t ));
    auto SOAData = std::make_shared<PosTexNorTanBinUV2Col3dStrip>( _data->numVerts(), PRIMITIVE_TRIANGLES,
                                                                   VFVertexAllocation::PreAllocate, _data->numIndices(),
                                                                   _indices );
    for ( int32_t t = 0; t < _data->numVerts(); t++ ) {
        SOAData->addVertex( _data->soaAt(t) );
    }
    return SOAData;
}

void Renderer::cmdReloadShaders( [[maybe_unused]] const std::vector<std::string>& _params ) {
    sm->loadShaders();
    afterShaderSetup();
}

Renderer::Renderer( StreamingMediator& _ssm) : ssm(_ssm) {
    sm = std::make_shared<ShaderManager>();
    tm = std::make_shared<TextureManager>();
    lm = std::make_shared<LightManager>();
    gm = std::make_shared<GPUVDataManager>();
    rmm = std::make_shared<RenderMaterialManager>(*this);
    mCommandBuffers = std::make_shared<CommandBufferList>(*this);
}

std::shared_ptr<RLTarget> Renderer::getTarget( const std::string& _name ) {
    for ( auto& t : mTargets ) {
        if ( t->cameraRig->Name() == _name ) return t;
    }
    return nullptr;
}

void Renderer::addTarget( std::shared_ptr<RLTarget> _target ) {
    mTargets.emplace_back(_target);
}

void Renderer::clearTargets() {
    for ( const auto& target : mTargets ) {
        target->clearCB();
    }
}

void Renderer::changeTime( const V3f& _solar ) {
    for ( auto& t : mTargets ) {
        t->changeTime( _solar );
    }
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
    sm->loadShaders();
    tm->addTextureWithData(RawImage::WHITE4x4(), FBNames::lightmap, TSLOT_LIGHTMAP );
//    mShadowMapFB = FrameBufferBuilder{ *this, FBNames::shadowmap }.size(4096).build();
    mShadowMapFB = FrameBufferBuilder{ *this, FBNames::shadowmap }.size(4096).depthOnly().build();

    auto trd = ImageParams{}.setSize( 128 ).format( PIXEL_FORMAT_HDR_RGB_16 ).setWrapMode(WRAP_MODE_CLAMP_TO_EDGE);
    tm->addCubemapTexture( TextureRenderData{ MPBRTextures::convolution, trd }
                                                             .setGenerateMipMaps( false )
                                                             .setIsFramebufferTarget( true ) );
    trd.setSize(512);
    tm->addCubemapTexture( TextureRenderData{ MPBRTextures::specular_prefilter, trd }
                                                                   .setGenerateMipMaps( true )
                                                                   .setIsFramebufferTarget( true ) );

    mBRDF = FrameBufferBuilder{ *this, MPBRTextures::ibl_brdf }.size( 512 ).format(
            PIXEL_FORMAT_HDR_RG_16 ).IM(S::IBL_BRDF).noDepth().build();

    rmm->addRenderMaterial( S::SHADOW_MAP );

    afterShaderSetup();
}

void Renderer::injectShader( const std::string& _key, const std::string& _content ) {
    sm->inject( _key, _content );
}

void Renderer::afterShaderSetup() {
    lm->generateUBO( sm );
    am.generateUBO( sm );
    rcm.generateUBO( sm );
}

void Renderer::setGlobalTextures() {
    auto p = sm->P(S::SH);

    auto lmt = tm->TD(FBNames::lightmap);
    if ( lmt ) {
        lmt->bind( lmt->textureSlot(), p->handle(), UniformNames::lightmapTexture.c_str() );
    }

    auto smt = tm->TD(FBNames::shadowmap);
    if ( smt ) {
        smt->bind( smt->textureSlot(), p->handle(), UniformNames::shadowMapTexture.c_str() );
    }
}

void Renderer::directRenderLoop() {

    CB_U().start();
    CB_U().startList( nullptr, CommandBufferFlags::CBF_DoNotSort );
    CB_U().pushCommand( { CommandBufferCommandName::clearDefaultFramebuffer } );
    CB_U().pushCommand( { CommandBufferCommandName::setGlobalTextures } );

    for ( const auto& target : mTargets ) {
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
    lm->setUniforms_r();
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

void Renderer::clearBucket( const int _bucket ) {
    if ( auto it = mCommandLists.find(_bucket); it != mCommandLists.end() ) {
        mCommandLists.erase(it);
    }
}

void Renderer::removeFromCL( const UUID& _uuid ) {

    auto removeUUID = [_uuid]( const auto & us ) -> bool { return us->UUiD() == _uuid; };

    for ( auto& [k, vl] : CL() ) {
        erase_if( vl.mVList, removeUUID );
        erase_if( vl.mVListTransparent, removeUUID );
    }
}

void Renderer::renderCommands( int eye ) {
    //PROFILE_BLOCK("Total Render render");
    CB_U().render( eye );
}

void Renderer::VPL( const int _bucket, std::shared_ptr<VPList> nvp, float alpha ) {
    if ( alpha < 1.0f ) {
        mCommandLists[_bucket].mVListTransparent.push_back(nvp);
    } else {
        mCommandLists[_bucket].mVList.push_back(nvp);
    }
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

std::shared_ptr<Texture> Renderer::addTextureResource( const ResourceTransfer<RawImage>& _val ) {
    return tm->addTextureWithData( *_val.elem, _val.names );
}

std::shared_ptr<RenderMaterial> Renderer::addMaterialResource( const ShaderMaterial& _val, const std::string& _name ) {
    return rmm->addRenderMaterial( _val.SN(), _val.Values(), {_name} );
}

std::shared_ptr<RenderMaterial> Renderer::addMaterialResource( const ResourceTransfer<Material>& _val ) {
    return rmm->addRenderMaterial( _val.elem->Values()->Type(), _val.elem->Values(), _val.names );
}

std::shared_ptr<RenderMaterial> Renderer::getMaterial( const std::string& _key ) {
    return rmm->get(_key);
}

std::shared_ptr<GPUVData> Renderer::addVDataResource( const cpuVBIB& _val, const std::string& _name ) {
    return gm->addGPUVData(_val, {_name} );
}

std::shared_ptr<GPUVData> Renderer::addVDataResource( const ResourceTransfer<VData>& _val ) {
    return gm->addGPUVData( cpuVBIB{ generateGeometryVP(_val.elem) }, _val.names );
}

std::shared_ptr<GPUVData> Renderer::getGPUVData( const std::string& _key ) {
    return gm->get(_key);
}


void Renderer::changeMaterialOnTagsCallback( const ChangeMaterialOnTagContainer& _cmt ) {
    mChangeMaterialCallbacks.emplace_back( _cmt );
}

void Renderer::changeMaterialOnTags( ChangeMaterialOnTagContainer& _cmt ) {
    // ### MAT This will need to be handled differently, I reckon
    auto rmaterial = rmm->getFromHash(_cmt.matHash);

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
        CB_U().pushVP( vp, _forcedMaterial );
    }
}

std::shared_ptr<Program> Renderer::P( const std::string& _id ) {
    return sm->P(_id);
}

std::shared_ptr<Texture> Renderer::TD( const std::string& _id, const int tSlot ) {
    return tm->TD( _id, tSlot );
}

TextureUniformDesc Renderer::TDI( const std::string& _id, unsigned int tSlot ) {
    auto t = TD( _id, tSlot );
    return { t->getHandle(), tSlot, t->getTarget() };
}

void RenderAnimationManager::setTiming() {
    mAnimUniforms->setUBOData( UniformNames::deltaAnimTime, Vector4f{GameTime::getCurrTimeStep(), GameTime::getCurrTimeStamp(),
                                                                     GameTime::getLastTimeStamp(), 0.0f } );
}

void RenderAnimationManager::setUniforms_r() {
    mAnimUniforms->submitUBOData();
}

void RenderAnimationManager::init() {
    mAnimUniforms = std::make_unique<ProgramUniformSet>();
    mAnimUniforms->setUBOStructure( UniformNames::pointLightPos, 16 );
}

void RenderAnimationManager::generateUBO( std::shared_ptr<ShaderManager> sm ) {
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

void RenderCameraManager::generateUBO( std::shared_ptr<ShaderManager> sm ) {
    mCameraUBO->generateUBO( sm, "CameraUniforms" );
}

std::shared_ptr<ProgramUniformSet>& RenderCameraManager::UBO() {
    return mCameraUBO;
}
