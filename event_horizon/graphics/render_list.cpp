//
//  render_list.cpp
//  SixthView
//
//  Created byDado on 11/01/2013.
//  Copyright (c) 2013Dado. All rights reserved.
//

#include "render_list.h"

#include <stb/stb_image_write.h>
#include <core/math/spherical_harmonics.h>
#include <core/camera.h>
#include <core/streaming_mediator.hpp>
#include <graphics/graphic_functions.hpp>
#include <graphics/framebuffer.h>
#include <graphics/renderer.h>
#include <graphics/shadowmap_manager.h>
#include <graphics/camera_rig.hpp>

std::string commandToNmeHumanReadable( CommandBufferCommandName cname ) {
    switch (cname) {
        case CommandBufferCommandName::nop:
            return "nop";
        case CommandBufferCommandName::depthWriteTrue:
            return "depthWriteTrue";
        case CommandBufferCommandName::depthWriteFalse:
            return "depthWriteFalse";
        case CommandBufferCommandName::depthTestFalse:
            return "depthTestFalse";
        case CommandBufferCommandName::depthTestTrue:
            return "depthTestTrue";
        case CommandBufferCommandName::depthTestLEqual:
            return "depthTestLEqual";
        case CommandBufferCommandName::depthTestLess:
            return "depthTestLess";
        case CommandBufferCommandName::cullModeNone:
            return "cullModeNone";
        case CommandBufferCommandName::cullModeFront:
            return "cullModeFront";
        case CommandBufferCommandName::cullModeBack:
            return "cullModeBack";
        case CommandBufferCommandName::alphaBlendingTrue:
            return "alphaBlendingTrue";
        case CommandBufferCommandName::alphaBlendingFalse:
            return "alphaBlendingFalse";
        case CommandBufferCommandName::wireFrameModeTrue:
            return "wireFrameModeTrue";
        case CommandBufferCommandName::wireFrameModeFalse:
            return "wireFrameModeFalse";
        case CommandBufferCommandName::colorBufferBind:
            return "colorBufferBind";
        case CommandBufferCommandName::colorBufferBindAndClear:
            return "colorBufferBindAndClear";
        case CommandBufferCommandName::colorBufferClear:
            return "colorBufferClear";
        case CommandBufferCommandName::clearDefaultFramebuffer:
            return "clearDefaultFramebuffer";
        case CommandBufferCommandName::setCameraUniforms:
            return "setCameraUniforms";
        case CommandBufferCommandName::setGlobalTextures:
            return "setGlobalTextures";
        case CommandBufferCommandName::shadowMapBufferBind:
            return "shadowMapBufferBind";
        case CommandBufferCommandName::shadowMapClearDepthBufferZero:
            return "shadowMapClearDepthBufferZero";
        case CommandBufferCommandName::shadowMapClearDepthBufferOne:
            return "shadowMapClearDepthBufferOne";
        case CommandBufferCommandName::blitToScreen:
            return "blitToScreen";
        case CommandBufferCommandName::blitPRB:
            return "blitPRB";
        case CommandBufferCommandName::takeScreenShot:
            return "takeScreenShot";
        case CommandBufferCommandName::targetVP:
            return "targetVP";
        default:
            ASSERT(0);
            return "COMMAND NOT TRANSLATED TO HUMAN READABLE FORMAT";
    }
}

void CommandBuffer::push( const CommandBufferEntry& entry ) {
    mCommandList.emplace_back( entry );
}

void CommandBuffer::push( const CommandBufferCommand& entry ) {
    mCommandList.emplace_back( entry );
}

void CommandBuffer::clear() {
    mCommandList.clear();
}

void CommandBuffer::sort() {
//    std::sort( mCommandList.begin(), mCommandList.end(),
//               []( const auto& a, const auto& b ) -> bool { return a.mHash > b.mHash; } );
}

void CommandBufferEntryCommand::run( Renderer& rr, CommandBuffer* cb ) const {
    if ( Type() == CommandBufferEntryCommandType::Comamnd ) {
//        LOGRS("        Command: " << commandToNmeHumanReadable(mCommand.name) );
        mCommand.issue( rr, cb );
    } else {
//        LOGRS("        Render geom: " << mVP->mVPList.Name() );
        mVP->mMaterial->setGlobalConstant( UniformNames::modelMatrix, mVP->mModelMatrix );
        mVP->mVPList.renderProgramWith( mVP->mMaterial );
    }
}

void CommandBuffer::render( Renderer& rr ) {
//    LOGR("CommandList RENDER START %d", mCommandList.size() );
//    LOGR("****************************************************************************");
    for ( const auto& i : mCommandList ) {
        i.run( rr, this );
    }
//    LOGR("****************************************************************************");
//    LOGR("CommandList RENDER END");
}

std::shared_ptr<Framebuffer> CommandBuffer::fb( CommandBufferFrameBufferType fbt ) {
    return mTarget->getFrameBuffer( fbt );
}

Rect2f CommandBuffer::destViewport() {
    return mTarget->screenViewport;
}

Rect2f CommandBuffer::sourceViewport() {
    return mTarget->cameraRig->getViewport();
}

bool CommandBuffer::findEntry( const std::string& _key, std::weak_ptr<CommandBufferEntry>& _wp ) {

    for ( auto& i : mCommandList ) {
        if ( i.entryName() == _key ) {
            _wp = i.entry();
            return true;
        }
    }

    return false;
}

void screenShotBase64Callback( void* ctx, void*data, int size ) {
    auto* rawThumbl = reinterpret_cast<ScreenShotContainer*>(ctx);
    ScreenShotContainer img{  reinterpret_cast<unsigned char*>(data), reinterpret_cast<unsigned char*>(data) + size };
    *rawThumbl = bn::encode_b64( img );
}

void CommandBuffer::postBlit() {
    if ( Target()->isTakingScreenShot() ) {
        auto lfb = fb(CommandBufferFrameBufferType::finalBlit);
        lfb->bind();
        auto ssd = Target()->ScreenShotData();
        ssd->clear();
        int w = lfb->getWidth();
        int h = lfb->getHeight();
        auto outB = std::make_unique<unsigned char[]>( w * h * 4 );
        grabScreen( 0, 0, w, h , reinterpret_cast<void*>(outB.get()) );
        stbi_write_png_to_func(screenShotBase64Callback, reinterpret_cast<void*>(ssd.get()), w, h, 4, outB.get(), 0);
        Target()->takeScreenShot(false);
    }
}

void CommandBufferList::start() {
    mCommandBuffers.clear();
    mCurrent = nullptr;
}

void CommandBufferList::end() {
    for ( auto& i : mCommandBuffers ) {
        if ( !checkBitWiseFlag( i.flags, CommandBufferFlags::CBF_DoNotSort )) {
            i.sort();
        }
    }
}

void CommandBufferList::pushVP( std::shared_ptr<VertexProcessing> _vp,
                                std::shared_ptr<RenderMaterial> _mat,
                                std::shared_ptr<Matrix4f> _modelMatrix ) {
    mCurrent->push( { VertexProcessing(*_vp.get()), _mat, _modelMatrix } );
}

void CommandBufferList::pushCommand( const CommandBufferCommand& cmd ) {
    mCurrent->push( cmd );
}

void CommandBufferList::startTarget( std::shared_ptr<Framebuffer> _fbt, Renderer& _rr ) {
    startList( std::make_shared<RLTargetFB>( _fbt, _rr ), CommandBufferFlags::CBF_DoNotSort );
    pushCommand( { CommandBufferCommandName::colorBufferBindAndClear } );
}

void CommandBufferList::startList( std::shared_ptr<RLTarget> _target, CommandBufferFlags flags ) {
    mCommandBuffers.emplace_back( CommandBuffer{flags} );
    mCurrent = &mCommandBuffers.back();
    mCurrent->mTarget = _target;
}

void CommandBufferList::render( [[maybe_unused]] int eye ) {
    if ( mCommandBuffers.size() == 0 ) return;

    // Might remove this ones once debugging and profiles state changes is over
    VertexProcessing::sNumDrawCalls = 0;
    VertexProcessing::sNumStateChanges = 0;
    VertexProcessing::sMatHash.clear();

    for ( auto& cb : mCommandBuffers ) {
        cb.render( rr );
    }

//    LOGR( "Draw  Calls:   %d", VertexProcessing::sNumDrawCalls );
//    LOGR( "State Changes: %d", VertexProcessing::sNumStateChanges );
//    LOGR( "Num Materials: %d", VertexProcessing::sMatHash.size() );
}

void CommandBufferList::getCommandBufferEntry( const std::string& _key, std::weak_ptr<CommandBufferEntry>& wp ) {

    for ( auto& cb : mCommandBuffers ) {
        if ( bool bFound = cb.findEntry( _key, wp ); bFound ) {
            return;
        }
    }
}


void CommandBufferList::setFramebufferTexture( const FrameBufferTextureValues& values ) {
    mCurrent->frameBufferTextureValues = std::make_unique<FrameBufferTextureValues>(values);
}

void CommandBufferList::setCameraUniforms( std::shared_ptr<Camera> c0 ) {
    mCurrent->UBOCameraBuffer = std::make_unique<char[]>( rr.CameraUBO()->getUBOSize() );
    UBO::mapUBOData( rr.CameraUBO(), UniformNames::mvpMatrix, c0->MVP(), mCurrent->UBOCameraBuffer.get());
    UBO::mapUBOData( rr.CameraUBO(), UniformNames::viewMatrix, c0->getViewMatrix(), mCurrent->UBOCameraBuffer.get());
    UBO::mapUBOData( rr.CameraUBO(), UniformNames::projMatrix, c0->getProjectionMatrix(), mCurrent->UBOCameraBuffer.get());
    UBO::mapUBOData( rr.CameraUBO(), UniformNames::screenSpaceMatrix, c0->ScreenAspectRatio(),mCurrent->UBOCameraBuffer.get());
    UBO::mapUBOData( rr.CameraUBO(), UniformNames::eyePos, c0->getPosition(), mCurrent->UBOCameraBuffer.get());

    pushCommand( { CommandBufferCommandName::setCameraUniforms } );
}

void CommandBufferCommand::issue( Renderer& rr, CommandBuffer* cstack ) const {
    switch ( name ) {
        case CommandBufferCommandName::depthWriteTrue:
            setDepthWrite( true );
            break;
        case CommandBufferCommandName::depthWriteFalse:
            setDepthWrite( false );
            break;
        case CommandBufferCommandName::depthTestLEqual:
            setDepthTest( true, DepthFunction::LEQUAL );
            break;
        case CommandBufferCommandName::depthTestLess:
            setDepthTest( true, DepthFunction::LESS );
            break;
        case CommandBufferCommandName::depthTestFalse:
            enableDepthTest( false );
            break;
        case CommandBufferCommandName::depthTestTrue:
            enableDepthTest( true );
            break;
        case CommandBufferCommandName::cullModeNone:
            setCullMode( CULL_NONE );
            break;
        case CommandBufferCommandName::cullModeFront:
            setCullMode( CULL_FRONT );
            break;
        case CommandBufferCommandName::cullModeBack:
            setCullMode( CULL_BACK );
            break;
        case CommandBufferCommandName::alphaBlendingTrue:
            setAlphaBlending( true );
            break;
        case CommandBufferCommandName::alphaBlendingFalse:
            setAlphaBlending( false );
            break;
        case CommandBufferCommandName::wireFrameModeTrue:
            setWireFrame( true );
            break;
        case CommandBufferCommandName::wireFrameModeFalse:
            setWireFrame( false );
            break;
        case CommandBufferCommandName::setGlobalTextures:
            rr.setGlobalTextures();
            break;
        case CommandBufferCommandName::clearDefaultFramebuffer:
            rr.getDefaultFB()->bindAndClear();
            break;
        case CommandBufferCommandName::setCameraUniforms:
            rr.CameraUBO()->submitUBOData( cstack->UBOCameraBuffer.get() ); //
            break;
        case CommandBufferCommandName::colorBufferBind:
            cstack->fb(CommandBufferFrameBufferType::sourceColor)->bind(cstack->frameBufferTextureValues.get());
            break;
        case CommandBufferCommandName::colorBufferBindAndClear:
            cstack->fb(CommandBufferFrameBufferType::sourceColor)->bindAndClear(cstack->frameBufferTextureValues.get());
            break;
        case CommandBufferCommandName::colorBufferClear:
            cstack->fb(CommandBufferFrameBufferType::sourceColor)->clearColorBuffer();
            break;
        case CommandBufferCommandName::shadowMapBufferBind:
            cstack->fb(CommandBufferFrameBufferType::shadowMap)->bind();
            break;
        case CommandBufferCommandName::shadowMapClearDepthBufferZero:
            cstack->fb(CommandBufferFrameBufferType::shadowMap)->clearDepthBuffer( 0.0f );
            break;
        case CommandBufferCommandName::shadowMapClearDepthBufferOne:
            cstack->fb(CommandBufferFrameBufferType::shadowMap)->clearDepthBuffer( 1.0f );
            break;
        case CommandBufferCommandName::blitToScreen:
            Framebuffer::blitWithRect( cstack->fb(CommandBufferFrameBufferType::sourceColor), rr.getDefaultFB(),
                               GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0,
                               cstack->sourceViewport(), cstack->destViewport() );
            break;
        case CommandBufferCommandName::blitPRB:

            Framebuffer::blit( cstack->fb(CommandBufferFrameBufferType::sourceColor),
                               cstack->fb(CommandBufferFrameBufferType::finalResolve),
                               GL_COLOR_ATTACHMENT0,
                               GL_COLOR_ATTACHMENT0 );

            cstack->fb(CommandBufferFrameBufferType::finalBlit)->bind();

            cstack->fb(CommandBufferFrameBufferType::finalResolve)->VP()->setMaterialConstant(
                    UniformNames::colorFBTexture,
                    cstack->fb(CommandBufferFrameBufferType::finalResolve)->RenderToTexture()->TDI(0));
            cstack->fb(CommandBufferFrameBufferType::finalResolve)->VP()->setMaterialConstant(
                    UniformNames::bloomTexture,
                    cstack->fb(CommandBufferFrameBufferType::blurVertical)->RenderToTexture()->TDI(1));
//            cstack->fb(CommandBufferFrameBufferType::finalResolve)->VP()->setMaterialConstant(
//                    UniformNames::shadowMapTexture,
//                    rr.getShadowMapFB()->RenderToTexture());
            cstack->fb(CommandBufferFrameBufferType::finalResolve)->VP()->render_im();

            cstack->postBlit();

        break;
        case CommandBufferCommandName::targetVP:
            break;

        default:
            break;
    };
}

//RLTarget RLTargetPBR::skyboxProbe( std::shared_ptr<CameraRig> _cameraRig, const std::string& _face ) {
////    RLTarget ret( _cameraRig, Rect2f::INVALID );
////    ret.rflags = RLFlagMatrix::skyboxOnly();
////    ret.cameraName = _cameraRig->getMainCamera()->Name() + + "_" + _face;
////    ret.renderIndex = _face;
////    return ret;
//}

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

    smm = std::make_unique<ShadowMapManager>(rr);

    rr.createGrid( 1.0f, Color4f::ACQUA_T, Color4f::PASTEL_GRAYLIGHT, Vector2f( 10.0f ), 0.075f );

    // Create a default skybox
    mSkybox = createSkybox();

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

    auto cbfb = FrameBufferBuilder{rr, "ProbeFB"}.size(512).buildSimple();

    auto cubeMapRig = addCubeMapRig( "cubemapRig", _at, Rect2f(V2f::ZERO, V2f{512}, true) );

    auto convolutionRT = rr.TM().addCubemapTexture( TextureRenderData{ MPBRTextures::convolution }
                                .setSize( 128 ).format( PIXEL_FORMAT_HDR_RGBA_16 )
                                .setGenerateMipMaps( false )
                                .setIsFramebufferTarget( true )
                                .wm( WRAP_MODE_CLAMP_TO_EDGE ) );

    auto preFilterSpecularRT = rr.TM().addCubemapTexture( TextureRenderData{ MPBRTextures::specular_prefilter }
                                                            .setSize( 512 ).format( PIXEL_FORMAT_HDR_RGBA_16 )
                                                            .setGenerateMipMaps( true )
                                                            .setIsFramebufferTarget( true )
                                                            .wm( WRAP_MODE_CLAMP_TO_EDGE ) );

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
//    int preFilterMipMaps = log2((float)512 );
    for ( int m = 0; m < preFilterMipMaps; m++ ) {
        convolutionProbe->render( preFilterSpecularRT, 512>>m, m, [&]() {
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

    rr.LM().setUniforms( Vector3f::ZERO, smm );

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
                    rr.addToCommandBuffer( vl.mVList, smm->getMaterial() );
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
    mBlurVerticalFB->VP()->render_im();
    //--

    for ( int i = 0; i < amount; i++ ) {
        mBlurHorizontalFB->bind();
        mBlurHorizontalFB->VP()->setMaterialConstant( UniformNames::colorFBTexture, mBlurVerticalFB->RenderToTexture()->TDI(0));
        mBlurHorizontalFB->VP()->render_im();

        mBlurVerticalFB->bind();
        mBlurVerticalFB->VP()->setMaterialConstant( UniformNames::colorFBTexture, mBlurHorizontalFB->RenderToTexture()->TDI(0));
        mBlurVerticalFB->VP()->render_im();
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
    if ( mSkybox->precalc( 1.0f ) ) {
        addProbes();
    }
    addShadowMaps();
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

CommandBufferEntryCommand::CommandBufferEntryCommand( CommandBufferCommand _command ) {
    mType = CommandBufferEntryCommandType::Comamnd;
    mCommand = _command;
}

CommandBufferEntryCommand::CommandBufferEntryCommand( CommandBufferEntry _vp ) {
    mType = CommandBufferEntryCommandType::VP;
    mVP = std::make_shared<CommandBufferEntry>(_vp);
}

CommandBufferEntryCommandType CommandBufferEntryCommand::Type() const {
    return mType;
}

void CommandBufferEntryCommand::Type( CommandBufferEntryCommandType type ) {
    mType = type;
}

std::string CommandBufferEntryCommand::entryName() const {
    if ( Type() == CommandBufferEntryCommandType::VP ) {
       return mVP->mVPList.Name();
    }
    return "";
}

std::shared_ptr<CommandBufferEntry>& CommandBufferEntryCommand::entry() {
    return mVP;
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

void RLTarget::updateStreamPacket( const std::string& _streamName ) {
    auto packet = rr.SSM().pop( _streamName );
    if ( packet.data ) {
        RawImage p{ packet.width, packet.height, 1, reinterpret_cast<const char* >(packet.data), _streamName };
        rr.TM().updateTexture( p );
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