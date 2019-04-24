//
// Created by Dado on 2019-03-09.
//

#include "render_targets.hpp"

#include <core/camera.h>
#include <core/camera_rig.hpp>
#include <core/raw_image.h>
#include <core/suncalc/sun_builder.h>
#include <core/streaming_mediator.hpp>
#include <core/descriptors/uniform_names.h>
#include <graphics/renderer.h>
#include <graphics/light_manager.h>

std::shared_ptr<RLTarget>
RenderTargetFactory::make( RenderTargetType rtt, std::shared_ptr<CameraRig> _rig, const Rect2f& screenViewport,
                           BlitType _bt, Renderer& _rr ) {

    std::shared_ptr<RLTarget> ret;

    switch ( rtt ) {
        case RenderTargetType::Plain:
            ret = std::make_shared<RLTargetPlain>( _rig, screenViewport, _bt, _rr );
            break;
        case RenderTargetType::PBR:
            ret = std::make_shared<RLTargetPBR>( _rig, screenViewport, _bt, _rr );
            break;
        default:
            return nullptr;
    }
    _rr.addTarget( ret );
    return ret;
}

std::shared_ptr<Framebuffer> RLTargetPlain::getFrameBuffer( CommandBufferFrameBufferType fbt ) {

    switch ( fbt ) {
        case CommandBufferFrameBufferType::sourceColor:
            return mComposite->getColorFB();
        case CommandBufferFrameBufferType::finalBlit:
            if ( mComposite->blitOnScreen() )
                return rr.getDefaultFB();
            else
                return mComposite->getOffScreenFB();
        default:
            ASSERT(0);
    }
    return framebuffer;
}

RLTargetPBR::RLTargetPBR( std::shared_ptr<CameraRig> cameraRig, const Rect2f& screenViewport,
                          BlitType _bt, Renderer& rr )
        : RLTarget( cameraRig, screenViewport, _bt, rr ) {

    mComposite = std::make_shared<CompositePBR>(rr, cameraRig->getMainCamera()->Name(), screenViewport, finalDestBlit);
    framebuffer = mComposite->getColorFB();
    bucketRanges.emplace_back( CommandBufferLimits::PBRStart, CommandBufferLimits::PBREnd );
    bucketRanges.emplace_back( CommandBufferLimits::UIStart, CommandBufferLimits::UIEnd );
    cameraRig->getMainCamera()->Mode( CameraMode::Doom );

    mShadowMapFB = FrameBufferBuilder{ rr, FBNames::shadowmap }.size(4096).GPUSlot(TSLOT_SHADOWMAP).depthOnly().build();

    smm = std::make_unique<ShadowMapManager>();

    rr.createGrid( 1.0f, Color4f::ACQUA_T, Color4f::PASTEL_GRAYLIGHT, Vector2f( 10.0f ), 0.075f );

    // Create a default skybox
    mSkybox = createSkybox();

    // Create the SunBuilder to for PBR scenes
    mSunBuilder = std::make_shared<SunBuilder>();

    changeTime( mSunBuilder->getSunPosition() );

    // Create PBR resources
    mConvolution = std::make_unique<ConvolutionEnvironmentMap>(rr);
    mIBLPrefilterSpecular = std::make_unique<PrefilterSpecularMap>(rr);
    mIBLPrefilterBRDF = std::make_unique<PrefilterBRDF>(rr);
}

std::shared_ptr<Framebuffer> RLTargetPBR::getFrameBuffer( CommandBufferFrameBufferType fbt ) {

    switch ( fbt ) {
        case CommandBufferFrameBufferType::sourceColor:
            return mComposite->getColorFB();
        case CommandBufferFrameBufferType::shadowMap:
            return getShadowMapFB();
        case CommandBufferFrameBufferType::finalResolve:
            return mComposite->getColorFinalFB();
        case CommandBufferFrameBufferType::blurVertical:
            return mComposite->getBlurVerticalFB();
        case CommandBufferFrameBufferType::blurHorizontal:
            return mComposite->getBlurHorizontalFB();
        case CommandBufferFrameBufferType::finalBlit:
            if ( mComposite->blitOnScreen() )
                return rr.getDefaultFB();
            else
                return mComposite->getOffScreenFB();
        default:
            ASSERT(0);
    }

    return framebuffer;
}

std::shared_ptr<Skybox> RLTargetPBR::createSkybox() {
    mSkyBoxParams.mode = SkyBoxMode::CubeProcedural;
    return std::make_unique<Skybox>(rr, mSkyBoxParams);
}

std::shared_ptr<CameraRig> RLTargetPBR::getProbeRig( int t, const std::string& _probeName, int mipmap ) {
    auto camName = cubeRigName(t, _probeName);
    return mAncillaryCameraRigs[camName];
}

void RLTargetPBR::addProbeToCB( const std::string& _probeCameraName, const Vector3f& _at ) {

    auto cbfb = FrameBufferBuilder{rr, "convolution"+_probeCameraName}.size(128).buildSimple();

    auto cubeMapRig = addCubeMapRig( "cubemapRig", _at, Rect2f(V2f::ZERO, V2f{512}, true) );

    auto trd = ImageParams{}.setSize( 128 ).format( PIXEL_FORMAT_HDR_RGBA_16 ).setWrapMode(WRAP_MODE_CLAMP_TO_EDGE);
    auto convolutionRT = rr.TM()->addCubemapTexture( TextureRenderData{ MPBRTextures::convolution, trd }
                                                            .setGenerateMipMaps( false )
                                                            .setIsFramebufferTarget( true ) );
    trd.setSize(512);
    auto preFilterSpecularRT = rr.TM()->addCubemapTexture( TextureRenderData{ MPBRTextures::specular_prefilter, trd }
                                                                  .setGenerateMipMaps( true )
                                                                  .setIsFramebufferTarget( true ) );

    // convolution
    auto convolutionProbe = std::make_shared<RLTargetCubeMap>( cubeMapRig, cbfb, rr );
    convolutionProbe->render( convolutionRT, 128, 0, [&]() {
        rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestFalse } );
        rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeFront } );
        mConvolution->render( mSkybox->getSkyboxTexture() );
        rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeBack } );
        rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestTrue } );
    });

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    int preFilterMipMaps = 1 + static_cast<GLuint>( floor( log( (float)512 ) ) );
    for ( int m = 0; m < preFilterMipMaps; m++ ) {
        auto fbSize = 512>>m;
        auto cbfbPrefilter = FrameBufferBuilder{rr, "prefilter"+std::to_string(m)+_probeCameraName}.size(fbSize).buildSimple();
        auto preFilterProbe = std::make_shared<RLTargetCubeMap>( cubeMapRig, cbfbPrefilter, rr );
        preFilterProbe->render( preFilterSpecularRT, fbSize, m, [&]() {
            rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestFalse } );
            rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeFront } );
            float roughness = (float)m / (float)(preFilterMipMaps - 1);
            mIBLPrefilterSpecular->render( mSkybox->getSkyboxTexture(), roughness );
            rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeBack } );
            rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestTrue } );
        });
    };

    mIBLPrefilterBRDF->render();
}

void RLTargetPBR::addShadowMaps() {
    if ( !smm ) return;

    rr.LM()->setUniforms( Vector3f::ZERO, smm, mSunBuilder->GoldenHourColor() );

    if ( smm->needsRefresh(rr.UpdateCounter()) ) {
        rr.CB_U().startList( shared_from_this(), CommandBufferFlags::CBF_DoNotSort );
        rr.CB_U().pushCommand( { CommandBufferCommandName::shadowMapBufferBind } );
        rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestTrue } );
        rr.CB_U().pushCommand( { CommandBufferCommandName::alphaBlendingTrue } );
        rr.CB_U().pushCommand( { CommandBufferCommandName::wireFrameModeFalse } );
        rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeFront } );
        if ( UseInfiniteHorizonForShadows() && smm->SunPosition().y() < 0.0f ) {
            rr.CB_U().pushCommand( { CommandBufferCommandName::shadowMapClearDepthBufferZero } );
        } else {
            rr.CB_U().pushCommand( { CommandBufferCommandName::shadowMapClearDepthBufferOne } );

            for ( const auto& [k, vl] : rr.CL() ) {
                if ( isKeyInRange(k) ) {
                    rr.addToCommandBuffer( vl.mVList, rr.getMaterial(S::SHADOW_MAP) );
                }
            }
        }
        rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeBack } );
    }
}

// This does NOT update and invalidate skybox and probes
void RLTargetPBR::cacheShadowMapSunPosition( const Vector3f& _smsp ) {
    mCachedSunPosition = _smsp;
}

// This DOES update and invalida skybox and probes
void RLTargetPBR::setShadowMapPosition( const Vector3f& _sp ) {
    cacheShadowMapSunPosition( _sp );
    smm->SunPosition( _sp );
    mSkybox->invalidate();
}

void RLTargetPBR::invalidateShadowMaps() {
    if ( smm ) {
        smm->invalidate();
    }
}

void RLTargetPBR::addProbes() {
    addProbeToCB( FBNames::sceneprobe, Vector3f::ZERO );
}

void CompositePBR::bloom() {
    if ( !isUsingBloom() ) return;

    int amount = 5;

    //	Framebuffer::blit( mColorFB, mBlurVerticalFB, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT0 );
    // New first pre-step of blur code, we couldn't use the code above because glBlitFramebuffer needs the same size of source/dest
    //--
    Framebuffer::blit( mColorFB, mColorFinalFB, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT0 );

    mBlurVerticalFB->bind();
    mBlurVerticalFB->VP()->setMaterialConstant( UniformNames::colorFBTexture, mColorFinalFB->RenderToTexture()->TDI(0));
    mBlurVerticalFB->VP()->draw();
    //--

    for ( int i = 0; i < amount; i++ ) {
        mBlurHorizontalFB->bind();
        mBlurHorizontalFB->VP()->setMaterialConstant( UniformNames::colorFBTexture, mBlurVerticalFB->RenderToTexture()->TDI(0));
        mBlurHorizontalFB->VP()->draw();

        mBlurVerticalFB->bind();
        mBlurVerticalFB->VP()->setMaterialConstant( UniformNames::colorFBTexture, mBlurHorizontalFB->RenderToTexture()->TDI(0));
        mBlurVerticalFB->VP()->draw();
    }
}

std::shared_ptr<VPList> CompositePBR::getVPFinalCombine() const {
    return mColorFinalFB->VP();
}

void RLTargetPlain::blit(CommandBufferList& cbl) {
    mComposite->blit( cbl );
}

void CompositePlain::blit(CommandBufferList& cbl) {
    cbl.pushCommand( { CommandBufferCommandName::blitToScreen } );
}

CompositePlain::CompositePlain( Renderer& _rr, [[maybe_unused]] const std::string& _name, const Rect2f& _destViewport,
                                BlitType _bt ) : Composite(_rr ) {
    mCompositeFinalDest = _bt;
    setup( _destViewport );
}

void CompositePlain::setup( const Rect2f& _destViewport ) {
    auto vsize = _destViewport.size();
    mColorFB = FrameBufferBuilder{ rr, "plainFrameBuffer" }.size(vsize).build();
    if ( mCompositeFinalDest == BlitType::OffScreen){
        mOffScreenBlitFB = FrameBufferBuilder{ rr, "offScreenFinalFrameBuffer"}.size(vsize).noDepth().
                dv(_destViewport, mCompositeFinalDest).format(PIXEL_FORMAT_RGBA).GPUSlot(TSLOT_COLOR).
                IM(S::FINAL_COMBINE).build();
    }
}

void RLTargetPBR::blit(CommandBufferList& cbl) {
    mComposite->blit( cbl );
}

void CompositePBR::blit( CommandBufferList& cbl ) {
    cbl.pushCommand( { CommandBufferCommandName::blitPRB } );
}

void CompositePBR::setup( const Rect2f& _destViewport ) {
    float bloomScale = 1.0f/8.0f;
    Vector2f vsize = _destViewport.size();
    mColorFB = FrameBufferBuilder{rr,"colorFrameBuffer"}.multisampled().size(vsize).format
            (PIXEL_FORMAT_HDR_RGBA_16).addColorBufferAttachments({ "colorFrameBufferAtth1", 1 }).build();
    mBlurHorizontalFB = FrameBufferBuilder{ rr, FBNames::blur_horizontal }.size(vsize*bloomScale).noDepth()
            .format(PIXEL_FORMAT_HDR_RGBA_16).GPUSlot(TSLOT_BLOOM).IM(S::BLUR_HORIZONTAL).build();
    mBlurVerticalFB = FrameBufferBuilder{ rr, FBNames::blur_vertical }.size(vsize*bloomScale).noDepth()
            .format(PIXEL_FORMAT_HDR_RGBA_16).GPUSlot(TSLOT_BLOOM).IM(S::BLUR_VERTICAL).build();
    mColorFinalFB = FrameBufferBuilder{ rr, FBNames::colorFinalFrameBuffer}.size(vsize).noDepth().
            dv(_destViewport, mCompositeFinalDest).format(PIXEL_FORMAT_HDR_RGBA_16).GPUSlot(TSLOT_COLOR).
            IM(S::FINAL_COMBINE).build();
    if ( mCompositeFinalDest == BlitType::OffScreen) {
        mOffScreenBlitFB = FrameBufferBuilder{ rr, FBNames::offScreenFinalFrameBuffer}.size(vsize).noDepth()
                .dv(_destViewport, mCompositeFinalDest).format(PIXEL_FORMAT_RGBA).GPUSlot(TSLOT_COLOR).
                        IM(S::FINAL_COMBINE).build();
    }
}

CompositePBR::CompositePBR( Renderer& _rr, [[maybe_unused]] const std::string& _name, const Rect2f& _destViewport,
                            BlitType _bt ) : Composite( _rr ) {
    mCompositeFinalDest = _bt;
    setup(_destViewport);
}

void RLTarget::addToCBCore( CommandBufferList& cb ) {
    auto lcvt = cameraRig->getCvt();

    cb.pushCommand( { CommandBufferCommandName::depthTestTrue } );

    if ( checkBitWiseFlag( lcvt, ViewportToggles::DrawWireframe ) ) {
        cb.pushCommand( { CommandBufferCommandName::wireFrameModeTrue } );
    } else {
        cb.pushCommand( { CommandBufferCommandName::wireFrameModeFalse } );
    }

    if ( checkBitWiseFlag( lcvt, ViewportToggles::DrawGrid ) ) {
        rr.addToCommandBuffer( CommandBufferLimits::CoreGrid );
    }

}

RLTargetPlain::RLTargetPlain( std::shared_ptr<CameraRig> cameraRig, const Rect2f& screenViewport,
                              BlitType _bt, Renderer& rr )
        : RLTarget( cameraRig, screenViewport, _bt, rr ) {
    mComposite = std::make_shared<CompositePlain>( rr, cameraRig->getMainCamera()->Name(), screenViewport );
    framebuffer = mComposite->getColorFB();
    bucketRanges.push_back( {CommandBufferLimits::UIStart, CommandBufferLimits::UIEnd} );
}

void RLTargetPlain::startCL( CommandBufferList& cb ) {
    cb.startList( shared_from_this(), CommandBufferFlags::CBF_DoNotSort );
    cb.setCameraUniforms( cameraRig->getCamera() );
    cb.pushCommand( { CommandBufferCommandName::colorBufferBindAndClear } );

//    cb.pushCommand( { CommandBufferCommandName::cullModeBack } );
//    cb.pushCommand( { CommandBufferCommandName::depthTestTrue } );
    cb.pushCommand( { CommandBufferCommandName::alphaBlendingTrue } );

    addToCBCore( cb );
}

void RLTargetPlain::endCL( CommandBufferList& cb ) {
    blit(cb);
}

void RLTargetPlain::addToCB( CommandBufferList& cb ) {
    startCL( cb );

    cb.pushCommand( { CommandBufferCommandName::depthWriteFalse } );
    for ( const auto& [k, vl] : rr.CL() ) {
        if ( isKeyInRange(k) ) {
            rr.addToCommandBuffer( vl.mVListTransparent );
        }
    }

    cb.pushCommand( { CommandBufferCommandName::depthWriteTrue } );
    cb.pushCommand( { CommandBufferCommandName::depthTestTrue } );

    for ( const auto& [k, vl] : rr.CL() ) {
        if ( isKeyInRange(k) ) {
            rr.addToCommandBuffer( vl.mVList );
        }
    }

    endCL( cb );
}

void RLTargetPlain::resize( const Rect2f& _r ) {
    mComposite->setup( _r );
    framebuffer = mComposite->getColorFB();
}

void RLTarget::clearCB() {
    for ( auto& [k, vl] : rr.CL() ) {
        if ( isKeyInRange( k, RLClearFlag::DontIncludeCore ) ) {
            vl.mVListTransparent.clear();
            vl.mVList.clear();
        }
    }
}

void RLTargetPBR::startCL( CommandBufferList& cb ) {
    // Add Shadowmaps sets all the lighting information, so it needs to be used before pretty much everything else
    // Especially Skyboxes and probes!!
    addShadowMaps();
    if ( mSkybox->precalc( 1.0f ) ) {
        addProbes();
    }
    cb.startList( shared_from_this(), CommandBufferFlags::CBF_None );
    cb.setCameraUniforms( cameraRig->getCamera() );
    cb.pushCommand( { CommandBufferCommandName::colorBufferBindAndClear } );
    cb.pushCommand( { CommandBufferCommandName::cullModeBack } );
    addToCBCore( cb );
}

void RLTargetPBR::endCL( CommandBufferList& cb ) {
    cb.pushCommand( { CommandBufferCommandName::cullModeNone } );
    cb.pushCommand( { CommandBufferCommandName::depthTestFalse } );
    cb.pushCommand( { CommandBufferCommandName::wireFrameModeFalse } );

    blit(cb);
}

void RLTargetPBR::addToCB( CommandBufferList& cb ) {

    startCL( cb );

    mSkybox->render();

    for ( const auto& [k, vl] : rr.CL() ) {
        if ( isKeyInRange(k) ) {
            rr.addToCommandBuffer( vl.mVList );
        }
    }

    cb.pushCommand( { CommandBufferCommandName::depthWriteFalse } );
    for ( const auto& [k, vl] : rr.CL() ) {
        if ( isKeyInRange(k) ) {
            rr.addToCommandBuffer( vl.mVListTransparent );
        }
    }
    cb.pushCommand( { CommandBufferCommandName::depthWriteTrue } );
    cb.pushCommand( { CommandBufferCommandName::depthTestTrue } );


    endCL( cb );
}

void RLTargetPBR::resize( const Rect2f& _r ) {
    mComposite->setup( _r );
    framebuffer = mComposite->getColorFB();
}

std::shared_ptr<Framebuffer> RLTargetPBR::getShadowMapFB() {
    return mShadowMapFB;
}

void RLTargetPBR::changeTime( const V3f& _solarTime ) {
    mSkybox->invalidate();
    setShadowMapPosition(_solarTime);
}

void RLTargetPBR::invalidateOnAdd() {
    invalidateShadowMaps();
}

RLTargetFB::RLTargetFB( std::shared_ptr<Framebuffer> _fbt, Renderer& _rr ) : RLTarget( _rr ) {
    framebuffer = _fbt;
}

std::shared_ptr<Framebuffer> RLTargetFB::getFrameBuffer( [[maybe_unused]] CommandBufferFrameBufferType fbt ) {
    return framebuffer;
}

void RLTargetFB::resize( [[maybe_unused]] const Rect2f& _r ) {
}

RLTargetProbe::RLTargetProbe( std::shared_ptr<CameraRig> _crig, Renderer& _rr ) : RLTarget( _rr ) {
    cameraRig = _crig;
}

std::shared_ptr<Framebuffer> RLTargetProbe::getFrameBuffer( [[maybe_unused]] CommandBufferFrameBufferType fbt ) {
    return framebuffer;
}

void RLTargetProbe::startCL( CommandBufferList& cb ) {
    cb.startList( shared_from_this(), CommandBufferFlags::CBF_DoNotSort );

    cb.setCameraUniforms( cameraRig->getCamera() );

    cb.pushCommand( { CommandBufferCommandName::colorBufferBindAndClear } );

    cb.pushCommand( { CommandBufferCommandName::cullModeBack } );
    cb.pushCommand( { CommandBufferCommandName::depthTestTrue } );
    cb.pushCommand( { CommandBufferCommandName::alphaBlendingTrue } );
}

bool RLTarget::isKeyInRange( const int _key, RLClearFlag _clearFlags ) const {

    if ( _clearFlags == RLClearFlag::DontIncludeCore ) {
        if ( inRange( _key, {CommandBufferLimits::CoreStart, CommandBufferLimits::CoreEnd} ) ) {
            return false;
        }
    }

    for ( const auto& p : bucketRanges ) {
        if ( inRange( _key, p) ) return true;
    }

    return false;
}

std::shared_ptr<Camera> RLTarget::getCamera() {
    return cameraRig->getMainCamera();
}

std::shared_ptr<CameraRig> RLTarget::getRig() {
    return cameraRig;
}

void RLTarget::updateStreamPacket( const std::string& _streamName ) {
    auto packet = rr.SSM().pop( _streamName );
    if ( packet.data ) {
        RawImage p{ packet.width, packet.height, 1, reinterpret_cast<const char* >(packet.data), _streamName };
        rr.TM()->updateTexture( p, _streamName );
    }
}

void RLTarget::updateStreams() {
    rr.SSM().update();

    updateStreamPacket("http://192.168.1.123:8080/video_y");
    updateStreamPacket("http://192.168.1.123:8080/video_u");
    updateStreamPacket("http://192.168.1.123:8080/video_v");
}

RLTargetCubeMap::RLTargetCubeMap( const CubeMapRigContainer& _rig, std::shared_ptr<Framebuffer> _fb,
                                  Renderer& _rr ) : RLTarget( _rr ) {
    cameraRig = _rig;
    framebuffer = _fb;
}

void RLTargetCubeMap::startCL( CommandBufferList& cb ) {
    cb.startList( shared_from_this(), CommandBufferFlags::CBF_DoNotSort );
//    cb.pushCommand( { CommandBufferCommandName::cullModeBack } );
//    cb.pushCommand( { CommandBufferCommandName::depthTestTrue } );
//    cb.pushCommand( { CommandBufferCommandName::alphaBlendingTrue } );
}

void RLTargetCubeMap::render( std::shared_ptr<Texture> _renderToTexture, int cmsize, int mip, CubeMapRenderFunction rcb ) {
    Rect2f lViewport(V2f::ZERO, V2f{cmsize}, true);
    for ( uint32_t t = 0; t < 6; t++ ) {
        startCL( rr.CB_U() );
        rr.CB_U().setFramebufferTexture(
                FrameBufferTextureValues{ indexToFBT(t),
                                          _renderToTexture->getHandle(),
                                          static_cast<uint32_t>(mip),
                                          cmsize, cmsize } );
        cameraRig[t]->setViewport( lViewport );
        rr.CB_U().setCameraUniforms( cameraRig[t]->getCamera() );
        rr.CB_U().pushCommand( { CommandBufferCommandName::colorBufferBindAndClear } );
        rcb();
    }
}
