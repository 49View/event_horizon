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
    mComposite->useBloom(true);
    framebuffer = mComposite->getColorFB();
    bucketRanges.emplace_back( CommandBufferLimits::PBRStart, CommandBufferLimits::PBREnd );
    cameraRig->getMainCamera()->Mode( CameraMode::Doom );

    smm = std::make_unique<ShadowMapManager>();

    // Create the SunBuilder to for PBR scenes
    mSunBuilder = std::make_shared<SunBuilder>();

//    changeTime( "spring noon" );

    // Create PBR resources
    mConvolution = std::make_unique<ConvolutionEnvironmentMap>(rr);
    mIBLPrefilterSpecular = std::make_unique<PrefilterSpecularMap>(rr);
}

void RLTargetPBR::createSkybox( const SkyBoxInitParams& _skyboxParams ) {
    mSkybox = std::make_unique<Skybox>(rr, _skyboxParams);
}

std::shared_ptr<Framebuffer> RLTargetPBR::getFrameBuffer( CommandBufferFrameBufferType fbt ) {

    switch ( fbt ) {
        case CommandBufferFrameBufferType::sourceColor:
            return mComposite->getColorFB();
        case CommandBufferFrameBufferType::shadowMap:
            return rr.getShadowMapFB();
        case CommandBufferFrameBufferType::depthMap:
            return rr.getDepthMapFB();
        case CommandBufferFrameBufferType::normalMap:
            return mComposite->getNormalMapFB();
        case CommandBufferFrameBufferType::ssaoMap:
            return mComposite->getSSAOMapFB();
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
        case CommandBufferFrameBufferType::screen:
            return rr.getDefaultFB();
        default:
            ASSERT(0);
    }

    return framebuffer;
}

std::shared_ptr<CameraRig> RLTargetPBR::getProbeRig( int t, const std::string& _probeName, int mipmap ) {
    auto camName = cubeRigName(t, _probeName);
    return mAncillaryCameraRigs[camName];
}

void RLTargetPBR::addProbeToCB( const std::string& _probeCameraName, const Vector3f& _at ) {

    auto convolutionRT = rr.TD(MPBRTextures::convolution);
    auto preFilterSpecularRT = rr.TD(MPBRTextures::specular_prefilter);
    auto probeRenderTarget = rr.TD("probe_render_target");

    int preFilterSize = 128;

    auto cubeMapRig = addCubeMapRig( "cubemapRig", _at, Rect2f(V2f::ZERO, V2f{preFilterSize}, true) );

    auto probe = std::make_shared<RLTargetCubeMap>( cubeMapRig, rr.getProbing(preFilterSize), rr );
    probe->render( probeRenderTarget, preFilterSize, 0, [&]() {
        mSkybox->render();
        for ( const auto& [k, vl] : rr.CL() ) {
            if ( isKeyInRange(k) ) {
                rr.addToCommandBuffer( vl.mVList, nullptr, nullptr, rr.P( S::SH_NOTEXTURE ).get());
            }
        }
    });

    // convolution
    auto cubeMapRigConvolution = addCubeMapRig( "cubemapRigConvolution", Vector3f::ZERO, Rect2f(V2f::ZERO, V2f{32}, true) );
    auto convolutionProbe = std::make_shared<RLTargetCubeMap>( cubeMapRigConvolution, rr.getProbing(32), rr );
    convolutionProbe->render( convolutionRT, 32, 0, [&]() {
        rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestFalse } );
        rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeFront } );
        mConvolution->render( probeRenderTarget );
        rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeBack } );
        rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestTrue } );
    });

    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    auto cubeMapRigPre = addCubeMapRig( "cubemapRigPre", Vector3f::ZERO, Rect2f(V2f::ZERO, V2f{preFilterSize}, true) );
    int preFilterMipMaps = 1 + static_cast<GLuint>( floor( log( (float)preFilterSize ) ) );
    for ( int m = 0; m < preFilterMipMaps; m++ ) {
        auto fbSize = preFilterSize>>m;
        auto preFilterProbe = std::make_shared<RLTargetCubeMap>( cubeMapRigPre, rr.getProbing(fbSize), rr );
        auto lIBLPrefilterSpecular = std::make_unique<PrefilterSpecularMap>(rr);

        preFilterProbe->render( preFilterSpecularRT, fbSize, m, [&]() {
            rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestFalse } );
            rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeFront } );
            float roughness = (float)m / (float)(preFilterMipMaps - 1);
            lIBLPrefilterSpecular->render( probeRenderTarget, roughness );
            rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeBack } );
            rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestTrue } );
        });
    }
}

void RLTargetPBR::addShadowMaps() {
    if ( smm->invalidated() ) {
        rr.CB_U().startList( shared_from_this(), CommandBufferFlags::CBF_DoNotSort );
        rr.CB_U().pushCommand( { CommandBufferCommandName::shadowMapBufferBind } );
        rr.CB_U().pushCommand( { CommandBufferCommandName::depthWriteTrue } );
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
                    rr.addToCommandBuffer( vl.mVList, nullptr, rr.getMaterial(S::SHADOW_MAP), nullptr, 0.91f );
                }
            }
        }
        rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeBack } );
        smm->validated();
    }
}

void RLTargetPBR::renderDepthMap() {

    rr.CB_U().startList( shared_from_this(), CommandBufferFlags::CBF_DoNotSort );
    rr.CB_U().pushCommand( { CommandBufferCommandName::depthMapBufferBindAndClear } );
    rr.CB_U().pushCommand( { CommandBufferCommandName::depthWriteTrue } );
    rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestTrue } );
    rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestLess } );
    rr.CB_U().pushCommand( { CommandBufferCommandName::alphaBlendingFalse } );
    rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeBack } );


    for ( const auto& [k, vl] : rr.CL() ) {
        if ( isKeyInRange( k, CheckEnableBucket::True ) ) {
            rr.addToCommandBuffer( vl.mVList, nullptr, rr.getMaterial(S::DEPTH_MAP), nullptr, 0.91f );
        }
    }
}

void RLTargetPBR::renderNormalMap() {
    rr.CB_U().startList( shared_from_this(), CommandBufferFlags::CBF_DoNotSort );
    rr.CB_U().pushCommand( { CommandBufferCommandName::normalMapBufferBindAndClear } );

    for ( const auto& [k, vl] : rr.CL() ) {
        if ( isKeyInRange( k, CheckEnableBucket::True ) ) {
            rr.addToCommandBuffer( vl.mVList, nullptr, rr.getMaterial(S::NORMAL_MAP) );
        }
    }
}

void RLTargetPBR::renderSSAO() {

    if ( !useSSAO() ) return;

    renderNormalMap();

    rr.CB_U().startList( shared_from_this(), CommandBufferFlags::CBF_DoNotSort );
    rr.CB_U().pushCommand( { CommandBufferCommandName::ssaoRender } );
}

// This does NOT update and invalidate skybox and probes
void RLTargetPBR::cacheShadowMapSunPosition( const Vector3f& _smsp ) {
    mCachedSunPosition = _smsp;
}

// This DOES update and invalida skybox and probes
void RLTargetPBR::setShadowMapPosition( const Vector3f& _sp ) {
    cacheShadowMapSunPosition( _sp );
    smm->SunPosition( _sp );
    if ( mSkybox ) mSkybox->invalidate();
}

void RLTargetPBR::invalidateShadowMaps() {
    if ( smm ) {
        smm->invalidate();
    }
}

void RLTargetPBR::addProbes() {
    addProbeToCB( FBNames::sceneprobe, mProbePosition );
}

std::shared_ptr<CompositePBR> RLTargetPBR::Composite() {
    return mComposite;
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

//    mColorFB = FrameBufferBuilder{rr,"colorFrameBuffer"}.multisampled().size(vsize).format
//            (PIXEL_FORMAT_HDR_RGBA_16).build();
    mBlurHorizontalFB = FrameBufferBuilder{ rr, FBNames::blur_horizontal }.size(vsize*bloomScale).noDepth()
            .format(PIXEL_FORMAT_HDR_RGBA_16).GPUSlot(TSLOT_BLOOM).IM(S::BLUR_HORIZONTAL).build();
    mBlurVerticalFB = FrameBufferBuilder{ rr, FBNames::blur_vertical }.size(vsize*bloomScale).noDepth()
            .format(PIXEL_FORMAT_HDR_RGBA_16).GPUSlot(TSLOT_BLOOM).IM(S::BLUR_VERTICAL).build();

    mColorFinalFB = FrameBufferBuilder{ rr, FBNames::colorFinalFrameBuffer}.size(vsize).noDepth().setViewSpace().
            dv(_destViewport, mCompositeFinalDest).format(PIXEL_FORMAT_HDR_RGBA_16).GPUSlot(TSLOT_COLOR).
            IM(S::FINAL_COMBINE).build();
    if ( mCompositeFinalDest == BlitType::OffScreen) {
        mOffScreenBlitFB = FrameBufferBuilder{ rr, FBNames::offScreenFinalFrameBuffer}.size(vsize).noDepth()
                .dv(_destViewport, mCompositeFinalDest).format(PIXEL_FORMAT_RGBA).GPUSlot(TSLOT_COLOR).
                        IM(S::FINAL_COMBINE).build();
    }

    float ssaoScaling = 1.0f;
    auto ssaoSize = vsize * ssaoScaling;
    mNormalFB = FrameBufferBuilder{ rr, FBNames::normalmap }.size( ssaoSize ).format( PIXEL_FORMAT_RGBA ).build();

    mSSAOFB = FrameBufferBuilder{ rr, FBNames::ssaomap }.size( ssaoSize ).format( PIXEL_FORMAT_RGBA ).IM(S::SSAO).setViewSpace().build();

}

CompositePBR::CompositePBR( Renderer& _rr, [[maybe_unused]] const std::string& _name, const Rect2f& _destViewport,
                            BlitType _bt ) : Composite( _rr ) {
    mCompositeFinalDest = _bt;
    setup(_destViewport);
}

void RLTarget::addToCBCore( CommandBufferList& cb ) {
    auto lcvt = cameraRig->getCvt();

    if ( checkBitWiseFlag( lcvt, ViewportToggles::DrawWireframe ) ) {
        cb.pushCommand( { CommandBufferCommandName::wireFrameModeTrue } );
    } else {
        cb.pushCommand( { CommandBufferCommandName::wireFrameModeFalse } );
    }

    if ( checkBitWiseFlag( lcvt, ViewportToggles::DrawGrid ) ) {
        rr.addToCommandBuffer( CommandBufferLimits::CoreGrid );
    }

    cb.pushCommand( { CommandBufferCommandName::depthTestTrue } );

}

RLTargetPlain::RLTargetPlain( std::shared_ptr<CameraRig> cameraRig, const Rect2f& screenViewport,
                              BlitType _bt, Renderer& rr )
        : RLTarget( cameraRig, screenViewport, _bt, rr ) {
    mComposite = std::make_shared<CompositePlain>( rr, cameraRig->getMainCamera()->Name(), screenViewport );
    framebuffer = mComposite->getColorFB();
    bucketRanges.emplace_back( CommandBufferLimits::UIStart, CommandBufferLimits::UIEnd );
}

void RLTargetPlain::addToCB( CommandBufferList& cb ) {

    cb.startList( shared_from_this(), CommandBufferFlags::CBF_None );

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

    blit(cb);
}

void RLTargetPlain::resize( const Rect2f& _r ) {
    mComposite->setup( _r );
    framebuffer = mComposite->getColorFB();
}

void RLTarget::clearCB() {
    for ( auto& [k, vl] : rr.CL() ) {
        if ( isKeyInRange( k, CheckEnableBucket::False, RLClearFlag::DontIncludeCore ) ) {
            vl.mVListTransparent.clear();
            vl.mVList.clear();
        }
    }
}

void RLTargetPBR::addToCB( CommandBufferList& cb ) {

    cb.startList( shared_from_this(), CommandBufferFlags::CBF_DoNotSort );
    cb.setCameraUniforms( cameraRig->getCamera() );
    rr.LM()->setUniforms( Vector3f::ZERO, smm, mSunBuilder->GoldenHourColor() );

    bool bAddProbe = mSkybox && mSkybox->precalc( 0.0f );
    setDirtyCumulative( S::PBR, cameraRig->getCamera()->isDirty() || bAddProbe );

//    setDirty( S::PBR, true);
    if ( bAddProbe ) {
        addProbes();
    } else {
        if ( isDirty(S::PBR) ) {
            addShadowMaps();

            if( useSSAO() || useDOF() || useMotionBlur() ) {
                renderDepthMap();
            }
            renderSSAO();

            cb.startList( shared_from_this(), CommandBufferFlags::CBF_DoNotSort );
            cb.pushCommand( { CommandBufferCommandName::colorBufferBindAndClear } );

            if ( mbEnableSkybox && mSkybox ) {
                mSkybox->render();
            }

            cb.startList( shared_from_this(), CommandBufferFlags::CBF_None );
            cb.pushCommand( { CommandBufferCommandName::depthWriteTrue } );
            cb.pushCommand( { CommandBufferCommandName::depthTestTrue } );
            cb.pushCommand( { CommandBufferCommandName::cullModeBack } );
            cb.pushCommand( { CommandBufferCommandName::alphaBlendingTrue } );

            for ( const auto&[k, vl] : rr.CL()) {
                if ( isKeyInRange( k, CheckEnableBucket::True )) {
                    rr.addToCommandBuffer( vl.mVList, cameraRig.get());
                }
            }
#ifndef __EMSCRIPTEN__
            blit( cb );
#endif
        }
#ifdef __EMSCRIPTEN__
        blit( cb );
#endif
        cb.pushCommand( { CommandBufferCommandName::blitPBRToScreen } );

        cb.startList( shared_from_this(), CommandBufferFlags::CBF_DoNotSort );
        cb.pushCommand( { CommandBufferCommandName::defaultFrameBufferBind } );
        cb.pushCommand( { CommandBufferCommandName::cullModeNone } );
        cb.pushCommand( { CommandBufferCommandName::depthTestFalse } );
        cb.pushCommand( { CommandBufferCommandName::alphaBlendingTrue } );

        for ( const auto&[k, vl] : rr.CL()) {
            if ( inRange( k,
                 { CommandBufferLimits::UnsortedStart, CommandBufferLimits::UnsortedEnd } ) &&
                 !hiddenCB(k)) {
                rr.addToCommandBuffer( k );
            }
        }

        for ( const auto& [k, vl] : rr.CL() ) {
            if ( inRange( k, { CommandBufferLimits::UI2dStart, CommandBufferLimits::UI2dEnd} ) && !hiddenCB(k) ) {
                rr.addToCommandBuffer( k );
            }
        }

        setDirty( S::PBR,false );
    }

}

void RLTargetPBR::resize( const Rect2f& _r ) {
    mComposite->setup( _r );
    framebuffer = mComposite->getColorFB();
}

void RLTargetPBR::changeTime( const std::string& _time ) {
    mSunBuilder->buildFromString( _time );
//    RR().changeTime( SB().getSunPosition() );
    if ( mSkybox ) mSkybox->invalidate();
    setShadowMapPosition(mSunBuilder->getSunPosition());
}

void RLTargetPBR::invalidateOnAdd() {
    invalidateShadowMaps();
    if ( mSkybox ) mSkybox->invalidate();
}

floata& RLTargetPBR::skyBoxDeltaInterpolation() {
    return mSkybox->DeltaInterpolation();
}

void RLTargetPBR::enableSkybox( bool _value ) {
    mbEnableSkybox = _value;
}

bool RLTargetPBR::useSSAO() const {
    return mbUseSSAO;
}

void RLTargetPBR::useSSAO( bool _flag ) {
    mbUseSSAO = _flag;
}

bool RLTargetPBR::useDOF() const {
    return mbUseDOF;
}

void RLTargetPBR::useDOF( bool _flag ) {
    mbUseDOF = _flag;
}

bool RLTargetPBR::useMotionBlur() const {
    return mbUseMotionBlur;
}

void RLTargetPBR::useMotionBlur( bool _flag ) {
    mbUseMotionBlur = _flag;
    setDirtDelay( _flag ? 2 : 1 );
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


bool RLTarget::isKeyInRange( const int _key, CheckEnableBucket _checkBucketVisibility, RLClearFlag _clearFlags ) const {

    if ( _clearFlags == RLClearFlag::DontIncludeCore ) {
        if ( inRange( _key, {CommandBufferLimits::CoreStart, CommandBufferLimits::CoreEnd} ) ) {
            return false;
        }
    }

    for ( const auto& p : bucketRanges ) {
        if ( ((_checkBucketVisibility == CheckEnableBucket::False ) || p.enabled ) &&
             inRange( _key, p.range) ) return true;
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

void RLTarget::setVisibleCB( int _index, bool _value ) {
    if ( _value ) {
        hiddenSet.erase( _index );
    } else {
        hiddenSet.emplace( _index );
    }
}

bool RLTarget::hiddenCB( int _cbIndex ) {
    return hiddenSet.find( _cbIndex ) != hiddenSet.end();
}

RLTargetCubeMap::RLTargetCubeMap( const CubeMapRigContainer& _rig, std::shared_ptr<Framebuffer> _fb,
                                  Renderer& _rr ) : RLTarget( _rr ) {
    cameraRig = _rig;
    framebuffer = _fb;
}

void RLTargetCubeMap::render( std::shared_ptr<Texture> _renderToTexture, int cmsize, int mip, CubeMapRenderFunction rcb ) {
    Rect2f lViewport(V2f::ZERO, V2f{cmsize}, true);
    for ( uint32_t t = 0; t < 6; t++ ) {
        rr.CB_U().startList( shared_from_this(), CommandBufferFlags::CBF_DoNotSort );
        cameraRig[t]->setViewport( lViewport );
        rr.CB_U().setFramebufferTexture(
                FrameBufferTextureValues{ indexToFBT(t),
                                          _renderToTexture->getHandle(),
                                          static_cast<uint32_t>(mip),
                                          cmsize, cmsize } );
        rr.CB_U().pushCommand( { CommandBufferCommandName::colorBufferBindAndClear } );
        rr.CB_U().setCameraUniforms( cameraRig[t]->getCamera() );
        rcb();
    }
}
