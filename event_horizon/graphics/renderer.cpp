#include "renderer.h"

#include <core/math/spherical_harmonics.h>
#include <core/command.hpp>
#include <core/raw_image.h>
#include <core/game_time.h>
#include <core/configuration/app_options.h>
#include <core/suncalc/sun_builder.h>
#include <core/zlib_util.h>
#include <core/camera_rig.hpp>
#include <core/camera.h>
#include <core/tar_util.h>
#include <core/profiler.h>
#include <core/v_data.hpp>
#include <core/resources/material.h>
#include <core/streaming_mediator.hpp>
#include <core/lightmap_exchange_format.h>
#include <core/descriptors/uniform_names.h>
#include <core/resources/resource_utils.hpp>
#include <graphics/graphic_functions.hpp>
#include <graphics/render_light_manager.h>
#include <graphics/render_list.h>
#include <graphics/render_targets.hpp>
#include <graphics/shader_manager.h>
#include <graphics/shadowmap_manager.h>
#include <graphics/shader_material.hpp>
#include <graphics/render_material_manager.hpp>
#include <graphics/gpuv_data_manager.hpp>

#ifdef _USE_IMGUI_
#include <graphics/imgui/imgui.h>
#endif


#include <stb/stb_image_write.h>
#include "core/service_factory.h"

namespace FBNames {
    static std::unordered_set<std::string> sFBNames;

    bool isPartOf( const std::string& _val ) {
        if ( sFBNames.empty()) {
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

    auto _indices = std::unique_ptr<uint32_t[]>( new uint32_t[_data->numIndices()] );
    std::memcpy( _indices.get(), _data->Indices(), _data->numIndices() * sizeof( int32_t ));
    auto SOAData = std::make_shared<PosTexNorTanBinUV2Col3dStrip>( _data->numVerts(), PRIMITIVE_TRIANGLES,
                                                                   VFVertexAllocation::PreAllocate, _data->numIndices(),
                                                                   std::move( _indices ));
    for ( int32_t t = 0; t < _data->numVerts(); t++ ) {
        SOAData->addVertex( _data->soaAt( t ));
    }
    return SOAData;
}

void Renderer::cmdReloadShaders( [[maybe_unused]] const std::vector<std::string>& _params ) {
    sm->loadShaders();
    afterShaderSetup();
    invalidateOnAdd();
}

Renderer::Renderer( StreamingMediator& _ssm ) : ssm( _ssm ) {
}

std::shared_ptr<RLTarget> Renderer::getTarget( const std::string& _name ) {
    for ( auto& t : mTargets ) {
        if ( t->cameraRig->Name() == _name ) return t;
    }
    return nullptr;
}

void Renderer::addTarget( std::shared_ptr<RLTarget> _target ) {
    mTargets.emplace_back( _target );
}

void Renderer::clearTargets() {
    for ( const auto& target : mTargets ) {
        target->clearCB();
    }
}

StreamingMediator& Renderer::SSM() {
    return ssm;
}

const Vector2i& Renderer::getForcedFrameBufferSize() const {
    return mForcedFrameBufferSize;
}

void Renderer::setForcedFrameBufferSize( const Vector2i& mForcedFrameBufferSize ) {
    Renderer::mForcedFrameBufferSize = mForcedFrameBufferSize;
}

void Renderer::resetDefaultFB( const Vector2i& forceSize ) {
    LOGR("ResetDefaultFB with [%d,%d]", forceSize.x(), forceSize.y());
    mDefaultFB = FrameBufferBuilder{ *this, "default" }.size( forceSize ).build();
}

void Renderer::useVignette(bool _flag) {
    SM()->injectDefine( S::FINAL_COMBINE, Shader::TYPE_FRAGMENT_SHADER, "plain_final_combine", "_VIGNETTING_", boolAlphaBinary(_flag) );
}

void Renderer::useFilmGrain(bool _flag) {
SM()->injectDefine( S::FINAL_COMBINE, Shader::TYPE_FRAGMENT_SHADER, "plain_final_combine",
"_GRAINING_", boolAlphaBinary(_flag) );
}

void Renderer::useBloom(bool _flag) {
SM()->injectDefine( S::FINAL_COMBINE, Shader::TYPE_FRAGMENT_SHADER, "plain_final_combine",
"_BLOOMING_", boolAlphaBinary(_flag) );
}

void Renderer::useDOF(bool _flag) {
SM()->injectDefine( S::FINAL_COMBINE, Shader::TYPE_FRAGMENT_SHADER, "plain_final_combine",
"_DOFING_", boolAlphaBinary(_flag) );
}

void Renderer::useSSAO(bool _flag) {
    //useSSAO(_flag);
SM()->injectDefine( S::FINAL_COMBINE, Shader::TYPE_FRAGMENT_SHADER, "plain_final_combine",
"_SSAOING_", boolAlphaBinary(_flag) );
}

void Renderer::useMotionBlur(bool _flag) {
    //useSSAO(_flag);
    SM()->injectDefine( S::FINAL_COMBINE, Shader::TYPE_FRAGMENT_SHADER, "plain_final_combine",
                        "_CAMERA_MOTION_BLURRING_", boolAlphaBinary(_flag) );
}

void Renderer::init() {
    sm = std::make_shared<ShaderManager>();
    tm = std::make_shared<TextureManager>();
    lm = std::make_shared<RenderLightManager>();
    gm = std::make_shared<GPUVDataManager>();
    rmm = std::make_shared<RenderMaterialManager>( *this );
    mCommandBuffers = std::make_shared<CommandBufferList>( *this );

    setMultiSampleCount(1);
//    auto bSupportsHDR = Framebuffer::checkHDRSupport();
//    LOGRS( "Is HDR Framebuffer supported: " << bSupportsHDR );
    resetDefaultFB(mForcedFrameBufferSize);
    rcm.init();
    am.init();
    sm->loadShaders();
//    tm->addTextureWithData(RawImage{54, 54, V4f::ONE, 32}, FBNames::lightmap, TSLOT_LIGHTMAP );
    tm->addTextureRef( FBNames::lightmap );
    mShadowMapFB = FrameBufferBuilder{ *this, FBNames::shadowmap }.size( 1024 ).depthOnly().build();
    mDepthFB = FrameBufferBuilder{ *this, FBNames::depthmap }.size( mDefaultFB->getWidth(), mDefaultFB->getHeight() ).format(PIXEL_FORMAT_HDR_R16).build();

    auto trd = ImageParams{}.setSize( 32 ).format( pixelFormatResolver( PIXEL_FORMAT_HDR_RGBA_16, Framebuffer::isHDRSupported()) ).setWrapMode( WRAP_MODE_CLAMP_TO_EDGE );
    tm->addCubemapTexture( TextureRenderData{ MPBRTextures::convolution, trd }
                                   .setGenerateMipMaps( false )
                                   .setIsFramebufferTarget( true ));
    trd.setSize( 128 );
    tm->addCubemapTexture( TextureRenderData{ MPBRTextures::specular_prefilter, trd }
                                   .setGenerateMipMaps( true )
                                   .setIsFramebufferTarget( true ));

    tm->addCubemapTexture( TextureRenderData{ "probe_render_target", trd }
                                   .setGenerateMipMaps( false )
                                   .setIsFramebufferTarget( true ));

    mBRDF = FrameBufferBuilder{ *this, MPBRTextures::ibl_brdf }.size( 512 ).format(
            PIXEL_FORMAT_HDR_RG_16 ).IM( S::IBL_BRDF ).noDepth().build();
    mBRDF->bindAndClear();
    mBRDF->VP()->draw();

    // add cascades framebuffer for downsampling, as OpenGLES does not support difference between render_frame_buffer
    // sizes and render target texture sizes on framebuffer, or at least this seems to be the case in my observations

    mProbingsFB.emplace( 512, FrameBufferBuilder{ *this, "Probing512" }.size( 512 ).buildSimple());
    mProbingsFB.emplace( 256, FrameBufferBuilder{ *this, "Probing256" }.size( 256 ).buildSimple());
    mProbingsFB.emplace( 128, FrameBufferBuilder{ *this, "Probing128" }.size( 128 ).buildSimple());
    mProbingsFB.emplace( 64, FrameBufferBuilder{ *this, "Probing_64" }.size( 64 ).buildSimple());
    mProbingsFB.emplace( 32, FrameBufferBuilder{ *this, "Probing_32" }.size( 32 ).buildSimple());
    mProbingsFB.emplace( 16, FrameBufferBuilder{ *this, "Probing_16" }.size( 16 ).buildSimple());
    mProbingsFB.emplace( 8, FrameBufferBuilder{ *this, "Probing__8" }.size( 8 ).buildSimple());
    mProbingsFB.emplace( 4, FrameBufferBuilder{ *this, "Probing__4" }.size( 4 ).buildSimple());

    rmm->addRenderMaterial( S::SHADOW_MAP );
    rmm->addRenderMaterial( S::DEPTH_MAP );
    rmm->addRenderMaterial( S::NORMAL_MAP );

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
//    auto p = sm->P(S::SH);
//
//    auto lmt = tm->TD( FBNames::lightmap);
//    if ( lmt ) {
//        lmt->bind( lmt->textureSlot(), p->handle(), UniformNames::lightmapTexture.c_str() );
//    }
//
//    auto smt = tm->TD(FBNames::shadowmap);
//    if ( smt ) {
//        smt->bind( smt->textureSlot(), p->handle(), UniformNames::shadowMapTexture.c_str() );
//    }
}

void Renderer::directRenderLoop() {

    if ( SM()->reloadDirtyPrograms() ) {
        afterShaderSetup();
    }

#ifdef _USE_IMGUI_
    ImGui::NewFrame();
#endif

    CB_U().start();
    CB_U().startList( nullptr, CommandBufferFlags::CBF_DoNotSort );
//    CB_U().pushCommand( { CommandBufferCommandName::clearDefaultFramebuffer } );
    CB_U().pushCommand( { CommandBufferCommandName::setGlobalTextures } );

    for ( const auto& target : mTargets ) {
        if ( target->enabled() ) {
            if ( bInvalidated ) target->invalidateOnAdd();
            target->updateStreams();
            target->addToCB( CB_U());
        }
    }

    CB_U().pushCommand( { CommandBufferCommandName::preFlush } );

    bInvalidated = false;

    CB_U().end();

//    VRM.preRender();
    am.setTiming();
    lm->setUniforms_r();
    am.setUniforms_r();

    mDrawCallsPerFrame = renderCBList();

//    VRM.update();

#ifdef _USE_IMGUI_
    ImGui::Begin("Renderer Console");
    ImGui::Text("Application average %.3f", 1000.0f / ImGui::GetIO().Framerate );
    ImGui::Text("Current FrameRate (%.1f FPS)", ImGui::GetIO().Framerate );
    ImGui::Text("Number drawcalls: %lu", mDrawCallsPerFrame );
    ImGui::End();

    ImGui::Render();
#endif

    mUpdateCounter++;
}

size_t Renderer::renderCBList() {
//    if ( VRM.IsOn()) {
//        for ( int eye = 0; eye < 2; eye++ ) {
//            renderCommands( eye );
//        }
//    } else
    {
        return renderCommands( -1 );
    }

//    if ( VRM.IsOn()) {
//        VRM.postRender( mComposite.getVPFinalCombine() );
//    }
}

void Renderer::clearCommandList() {
    mCommandLists.clear();
}

void Renderer::clearBucket( const int _bucket ) {
    if ( auto it = mCommandLists.find( _bucket ); it != mCommandLists.end()) {
        mCommandLists.erase( it );
    }
}

void Renderer::removeFromCL( const UUID& _uuid ) {

    auto removeUUID = [_uuid]( const auto& us ) -> bool { return us->UUiD() == _uuid; };

    for ( auto&[k, vl] : CL()) {
        erase_if( vl.mVList, removeUUID );
        erase_if( vl.mVListTransparent, removeUUID );
    }
}

size_t Renderer::renderCommands( int eye ) {
    //PROFILE_BLOCK("Total Render render");
    return CB_U().render( eye );
}

void Renderer::VPL( const int _bucket, std::shared_ptr<VPList> nvp, float alpha ) {
    if ( alpha < 1.0f ) {
        mCommandLists[_bucket].mVListTransparent.push_back( nvp );
    } else {
        mCommandLists[_bucket].mVList.push_back( nvp );
    }
    mVPLMap.emplace( nvp->UUiD(), nvp );
}

bool Renderer::hasTag( uint64_t _tag ) const {
    for ( const auto&[k, vl] : CL()) {
        if ( CommandBufferLimits::PBRStart <= k && CommandBufferLimits::PBREnd >= k ) {
            for ( const auto& v : vl.mVList ) {
                if ( v->hasTag( _tag )) return true;
            }
            for ( const auto& v : vl.mVListTransparent ) {
                if ( v->hasTag( _tag )) return true;
            }
        }
    }
    return false;
}

std::shared_ptr<Texture> Renderer::addTextureResource( const ResourceTransfer<RawImage>& _val ) {
    return tm->addTextureWithData( *_val.elem, _val.names );
}

std::shared_ptr<RenderMaterial> Renderer::addMaterialResource( const ShaderMaterial& _val, const std::string& _name ) {
    return rmm->addRenderMaterial( _val.SN(), _val.Values(), { _name } );
}

std::shared_ptr<RenderMaterial> Renderer::addMaterialResource( const ResourceTransfer<Material>& _val ) {
    return rmm->addRenderMaterial( _val.elem->Values()->Type(), _val.elem->Values(), _val.names );
}

std::shared_ptr<RenderMaterial> Renderer::getMaterial( const std::string& _key ) {
    return rmm->get( _key );
}

std::shared_ptr<GPUVData> Renderer::addVDataResource( cpuVBIB&& _val, const std::string& _name ) {
    return gm->addGPUVData( std::move( _val ), { _name } );
}

std::shared_ptr<GPUVData> Renderer::addVDataResource( const ResourceTransfer<VData>& _val ) {
    return gm->addGPUVData( cpuVBIB{ generateGeometryVP( _val.elem ) }, _val.names );
}

std::shared_ptr<GPUVData> Renderer::getGPUVData( const std::string& _key ) {
    return gm->get( _key );
}


void Renderer::changeMaterialOnTagsCallback( const ChangeMaterialOnTagContainer& _cmt ) {
    mChangeMaterialCallbacks.emplace_back( _cmt );
}

void Renderer::remapLightmapUVs( const scene_t& scene ) {

    for ( const auto&[k, v] : scene.ggLImap ) {
        auto vl = generateGeometryVP( v );
        mVPLMap[k]->updateGPUVData( cpuVBIB{ vl } );
    }

//    for ( const auto& vo : scene.unchart ) {
//        std::vector<V2f> ev{};
//        std::vector<V3f> pv{};
//        std::vector<size_t> xrefs{};
//
//        auto _indices = std::unique_ptr<uint32_t[]>( new uint32_t[vo.isize] );
//        for ( size_t t = 0; t < vo.isize; t++ ) {
//            _indices[t] = scene.indices[t + vo.ioffset] - vo.ioffset;
//        }
//
//        auto SOAData = std::make_shared<PosTexNorTanBinUV2Col3dStrip>( (int32_t)vo.vsize, PRIMITIVE_TRIANGLES,
//                                                                       VFVertexAllocation::PreAllocate, (int32_t)vo.isize,
//                                                                       std::move(_indices) );
//        for ( size_t t = 0; t < vo.vsize; t++ ) {
//            const auto& v = scene.vertices[vo.voffset+t];
//            PUUNTBC p = scene.vertices[vo.voffset+t].orig;
//            p.a2  = V2f{ v.t[0], v.t[1] };
//            LOGRS( "UV Index [" << t << "] value: " << p.a2 );
//
//            SOAData->addVertex( p );
//        }
//
//        mVPLMap[vo.uuid]->updateGPUVData( cpuVBIB{SOAData} );
//    }
}

void Renderer::changeMaterialOnTags( const ChangeMaterialOnTagContainer& _cmt ) {
    auto rmaterial = rmm->getFromHash( _cmt.matHash );
    if ( !rmaterial ) {
        rmaterial = getMaterial( _cmt.matHash );
    }

    for ( const auto&[k, vl] : CL()) {
        if ( CommandBufferLimits::PBRStart <= k && CommandBufferLimits::PBREnd >= k ) {
            for ( auto& v : vl.mVList ) {
                v->setMaterialWithTag( rmaterial, _cmt.tag );
            }
            for ( auto& v : vl.mVListTransparent ) {
                v->setMaterialWithTag( rmaterial, _cmt.tag );
            }
        }
    }
}

void Renderer::changeMaterialColorOnTags( uint64_t _tag, float r, float g, float b ) {
    // NDDado: we only use RGB, not Alpha, in here
    V3f color{r,g,b};

    for ( const auto&[k, vl] : CL()) {
        if ( CommandBufferLimits::PBRStart <= k && CommandBufferLimits::PBREnd >= k ) {
            for ( const auto& v : vl.mVList ) {
                v->setMaterialColorWithTag( color, _tag );
            }
            for ( const auto& v : vl.mVListTransparent ) {
                v->setMaterialColorWithTag( color, _tag );
            }
        }
    }
    invalidateOnAdd();
}

void Renderer::changeMaterialColorOnTags( uint64_t _tag, const Color4f& _color ) {
    // NDDado: we only use RGB, not Alpha, in here
    for ( const auto&[k, vl] : CL()) {
        if ( CommandBufferLimits::PBRStart <= k && CommandBufferLimits::PBREnd >= k ) {
            for ( const auto& v : vl.mVList ) {
                v->setMaterialColorWithTag( _color, _tag );
            }
            for ( const auto& v : vl.mVListTransparent ) {
                v->setMaterialColorWithTag( _color, _tag );
            }
        }
    }
    invalidateOnAdd();
}

void Renderer::changeMaterialColorOnUUID( const UUID& _tag, const Color4f& _color, Color4f& _oldColor ) {
    // NDDado: we only use RGB, not Alpha, in here
    for ( const auto&[k, vl] : CL()) {
        for ( const auto& v : vl.mVList ) {
            v->setMaterialColorWithUUID( _color, _tag, _oldColor );
        }
        for ( const auto& v : vl.mVListTransparent ) {
            v->setMaterialColorWithUUID( _color, _tag, _oldColor );
        }
    }
}

void Renderer::replaceMaterial( const std::string& _oldMatRef, const std::string& _newMatRef ) {
    auto oldMat = rmm->getFromHash( _oldMatRef );
    auto newMat = rmm->getFromHash( _newMatRef );
    for ( const auto&[k, vl] : CL()) {
        for ( const auto& v : vl.mVList ) {
            if ( v->getMaterial() == oldMat ) v->setMaterial( newMat );
        }
        for ( const auto& v : vl.mVListTransparent ) {
            if ( v->getMaterial() == oldMat ) v->setMaterial( newMat );
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
                                   CameraRig *_cameraRig,
                                   std::shared_ptr<RenderMaterial> _forcedMaterial,
                                   Program *_forceProgram,
                                   float _alphaDrawThreshold ) {
    Camera *cam = _cameraRig ? _cameraRig->getCamera().get() : nullptr;
    bool addVP = true;
    for ( const auto& vp : _map ) {
        if ( vp->isHidden() ) {
            continue;
        }
        if ( cam ) {
            addVP = cam->frustomClipping( vp->BBox3d());
        }
        if ( addVP ) {
            CB_U().pushVP( vp, _forcedMaterial, nullptr, _forceProgram, _alphaDrawThreshold );
        }
    }
}

std::shared_ptr<Program> Renderer::P( const std::string& _id ) {
    return sm->P( _id );
}

std::shared_ptr<Texture> Renderer::TD( const std::string& _id, const int tSlot ) {
    return tm->TD( _id, tSlot );
}

TextureUniformDesc Renderer::TDI( const std::string& _id, unsigned int tSlot ) {
    auto t = TD( _id, tSlot );
    return { t->getHandle(), tSlot, t->getTarget() };
}

std::shared_ptr<Framebuffer> Renderer::getProbing( int _index ) {
    return mProbingsFB[_index];
}

std::shared_ptr<RenderMaterial> Renderer::getRenderMaterialFromHash( CResourceRef _hash ) {
    return rmm->getFromHash( _hash );
}

void Renderer::clearColor( const C4f& _color ) {
    Framebuffer::clearColorValue = _color;
}

void Renderer::setLoadingFlag( bool _value ) {
    bIsLoading = _value;
}

bool Renderer::isLoading() const {
    return bIsLoading;
}

void Renderer::setShadowOverBurnCofficient( float _value ) {
    LM()->setShadowOverBurnCofficient( _value );
}

void Renderer::setShadowZFightCofficient( float _value ) {
    LM()->setShadowZFightCofficient(_value);
}

void Renderer::setIndoorSceneCoeff( float _value ) {
    LM()->setIndoorSceneCoeff(_value);
}

void Renderer::setProgressionTiming( float _progress ) {
    am.setProgressionTiming( _progress );
}

void RenderAnimationManager::setTiming() {
    mAnimUniforms->setUBOData( UniformNames::deltaAnimTime,
                               Vector4f{ GameTime::getCurrTimeStep(), GameTime::getCurrTimeStamp(),
                                         GameTime::getLastTimeStamp(), progress } );
}

void RenderAnimationManager::setProgressionTiming( float _progress ) {
    progress = _progress;
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
    mCameraUBO->setUBOStructure( UniformNames::eyeDir, 16 );
    mCameraUBO->setUBOStructure( UniformNames::nearFar, 16 );
    mCameraUBO->setUBOStructure( UniformNames::inverseMvMatrix, 64 );
    mCameraUBO->setUBOStructure( UniformNames::prevMvpMatrix, 64 );
}

void RenderCameraManager::generateUBO( std::shared_ptr<ShaderManager> sm ) {
    mCameraUBO->generateUBO( sm, "CameraUniforms" );
}

std::shared_ptr<ProgramUniformSet>& RenderCameraManager::UBO() {
    return mCameraUBO;
}
