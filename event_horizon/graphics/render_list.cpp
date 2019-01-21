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

void CommandBuffer::push( const CommandBufferEntry& entry ) {
    mCommandList.push_back( entry );
}

void CommandBuffer::push( const CommandBufferCommand& entry ) {
    mCommandList.push_back( entry );
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
        mCommand.issue( rr, cb );
    } else {
        mVP->mMaterial->setGlobalConstant( UniformNames::modelMatrix, mVP->mModelMatrix );
        mVP->mVPList.renderProgramWith( mVP->mMaterial );
    }
}

void CommandBuffer::render( Renderer& rr ) {
    for ( const auto& i : mCommandList ) {
        i.run( rr, this );
    }
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

std::string CommandBuffer::renderIndex() {
    return mTarget->renderIndex;
}

int CommandBuffer::mipMapIndex() {
    return mTarget->mipMapIndex;
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
    mCurrent->push( { _vp->hash(), VertexProcessing(*_vp.get()), _mat, _modelMatrix } );
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
            rr.getDefaultFB()->bindAndClear("",0);
            break;
        case CommandBufferCommandName::setCameraUniforms:
            rr.CameraUBO()->submitUBOData( cstack->UBOCameraBuffer.get() ); //
            break;
        case CommandBufferCommandName::colorBufferBind:
            cstack->fb(CommandBufferFrameBufferType::sourceColor)->bind( cstack->renderIndex(), cstack->mipMapIndex() );
            break;
        case CommandBufferCommandName::colorBufferBindAndClear:
            cstack->fb(CommandBufferFrameBufferType::sourceColor)->bindAndClear( cstack->renderIndex(), cstack->mipMapIndex() );
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
                    cstack->fb(CommandBufferFrameBufferType::finalResolve)->RenderToTexture());
            cstack->fb(CommandBufferFrameBufferType::finalResolve)->VP()->setMaterialConstant(
                    UniformNames::bloomTexture,
                    cstack->fb(CommandBufferFrameBufferType::blurVertical)->RenderToTexture());
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

std::shared_ptr<CameraRig> RLTarget::addAncillaryRig( const std::string& _name, std::shared_ptr<Framebuffer> _fb ) {
    auto c = std::make_shared<CameraRig>( _name, _fb );
    mAncillaryCameraRigs[_name] = c;
    return c;
}

void RLTarget::addCubeMapRig( const CameraCubeMapRigBuilder& _builder ) {
    const Rect2f cubemapViewport{ Vector2f::ZERO, {_builder.size, _builder.size} };

    auto cbfb = FrameBufferBuilder{ rr, _builder.name}.format(_builder.format).size(_builder.size)
            .cubemap().mipMaps(_builder.useMipMaps).build();

    float cubeMapFOV = 90.0f;//degToRad(90.0f);
    auto ctop = addAncillaryRig( _builder.name + "_" + cubemapFaceToString(CubemapFaces::Top), cbfb );
    ctop->getCamera()->setFoV( cubeMapFOV );
    ctop->getCamera()->setPosition( _builder.pos );
    ctop->getCamera()->setQuatAngles( Vector3f{ M_PI_2, 0.0f, 0.0f } );
    ctop->getCamera()->Mode( CameraMode::Doom );
    ctop->getCamera()->ViewPort( cubemapViewport );
    ctop->getCamera()->enableInputs(false);

    auto cbottom = addAncillaryRig( _builder.name + "_" + cubemapFaceToString(CubemapFaces::Bottom), cbfb );
    cbottom->getCamera()->setFoV( cubeMapFOV );
    cbottom->getCamera()->setPosition( _builder.pos );
    cbottom->getCamera()->setQuatAngles( Vector3f{ -M_PI_2, 0.0f, 0.0f } );
    cbottom->getCamera()->Mode( CameraMode::Doom );
    cbottom->getCamera()->ViewPort( cubemapViewport );
    cbottom->getCamera()->enableInputs(false);

    auto cleft = addAncillaryRig( _builder.name + "_" + cubemapFaceToString(CubemapFaces::Left), cbfb );
    cleft->getCamera()->setFoV( cubeMapFOV );
    cleft->getCamera()->setPosition( _builder.pos );
    cleft->getCamera()->setQuatAngles( Vector3f{ 0.0f, -M_PI_2, 0.0f } );
    cleft->getCamera()->Mode( CameraMode::Doom );
    cleft->getCamera()->ViewPort( cubemapViewport );
    cleft->getCamera()->enableInputs(false);

    auto cright = addAncillaryRig( _builder.name + "_" + cubemapFaceToString(CubemapFaces::Right), cbfb );
    cright->getCamera()->setFoV( cubeMapFOV );
    cright->getCamera()->setPosition( _builder.pos );
    cright->getCamera()->setQuatAngles( Vector3f{ 0.0f, M_PI_2, 0.0f } );
    cright->getCamera()->Mode( CameraMode::Doom );
    cright->getCamera()->ViewPort( cubemapViewport );
    cright->getCamera()->enableInputs(false);

    auto cfront = addAncillaryRig( _builder.name + "_" + cubemapFaceToString(CubemapFaces::Front), cbfb );
    cfront->getCamera()->setFoV( cubeMapFOV );
    cfront->getCamera()->setPosition( _builder.pos );
    cfront->getCamera()->setQuatAngles( Vector3f{ 0.0f } );
    cfront->getCamera()->Mode( CameraMode::Doom );
    cfront->getCamera()->ViewPort( cubemapViewport );
    cfront->getCamera()->enableInputs(false);

    auto cback = addAncillaryRig(  _builder.name + "_" + cubemapFaceToString(CubemapFaces::Back), cbfb );
    cback->getCamera()->setFoV( cubeMapFOV );
    cback->getCamera()->setPosition( _builder.pos );
    cback->getCamera()->setQuatAngles( Vector3f{ 0.0f, M_PI, 0.0f } );
    cback->getCamera()->Mode( CameraMode::Doom );
    cback->getCamera()->ViewPort( cubemapViewport );
    cback->getCamera()->enableInputs(false);
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
    return cameraRig->getFramebuffer();
}

RLTargetPBR::RLTargetPBR( std::shared_ptr<CameraRig> cameraRig, const Rect2f& screenViewport,
                          BlitType _bt, Renderer& rr )
                        : RLTarget( cameraRig, screenViewport, _bt, rr ) {

    mComposite = std::make_shared<CompositePBR>(rr, cameraRig->getMainCamera()->Name(), screenViewport, finalDestBlit);
    cameraRig->setFramebuffer( mComposite->getColorFB() );
    bucketRanges.emplace_back( CommandBufferLimits::PBRStart, CommandBufferLimits::PBREnd );
    bucketRanges.emplace_back( CommandBufferLimits::UIStart, CommandBufferLimits::UIEnd );
    cameraRig->getMainCamera()->Mode( CameraMode::Doom );
    mSkyBoxParams.mode = SkyBoxMode::CubeProcedural;
    mShadowMapFB = FrameBufferBuilder{ rr, FBNames::shadowmap }.size(4096).GPUSlot(TSLOT_SHADOWMAP).depthOnly().build();

    smm = std::make_unique<ShadowMapManager>(rr);

    rr.createGrid( 1.0f, Color4f::ACQUA_T, Color4f::PASTEL_GRAYLIGHT, Vector2f( 10.0f ), 0.075f );

    // Create a default skybox
    mSkybox = createSkybox();

    addCubeMapRig( CameraCubeMapRigBuilder{FBNames::sceneprobe}.s( 512 ) );
    addCubeMapRig( CameraCubeMapRigBuilder{FBNames::convolution}.s(128) );
    addCubeMapRig( CameraCubeMapRigBuilder{FBNames::specular_prefilter}.s( 512 ).useMips() );

    // Create PBR resources
    mConvolution = std::make_unique<CubeEnvironmentMap>(rr);
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

    return cameraRig->getFramebuffer();

}

std::shared_ptr<Skybox> RLTargetPBR::createSkybox() {
    return std::make_unique<Skybox>(rr, mSkyBoxParams);
}

std::shared_ptr<CameraRig> RLTargetPBR::getProbeRig( int t, const std::string& _probeName ) {
    auto cri = cubemapFaceToString( static_cast<CubemapFaces>(t) );
    auto cameraName = _probeName + "_" + cri;
    return mAncillaryCameraRigs[cameraName];
}

void RLTargetPBR::addProbeToCB( const std::string& _probeCameraName, [[maybe_unused]] const Vector3f& _at ) {

    auto lSkybox = createSkybox();
    lSkybox->invalidate();

    for ( int t = 0; t < 6; t++ ) {
        auto probe = std::make_shared<RLTargetProbe>( getProbeRig(t, _probeCameraName), rr );
        probe->startCL( rr.CB_U() );
        lSkybox->render( 1.0f );
    }
    // convolution
    for ( int t = 0; t < 6; t++ ) {
        auto probe = std::make_shared<RLTargetProbe>( getProbeRig(t, FBNames::convolution), rr );
        probe->startCL( rr.CB_U() );
        mConvolution->render( rr.TM().TD(_probeCameraName, TSLOT_CUBEMAP) );
    }
    // pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
    int preFilterMipMaps = 1 + static_cast<GLuint>( floor( log( (float)512 ) ) );
    for ( int m = 0; m < preFilterMipMaps; m++ ) {
        for ( int t = 0; t < 6; t++ ) {
            auto probe = std::make_shared<RLTargetProbe>( getProbeRig(t, FBNames::specular_prefilter), rr, m );
            probe->startCL( rr.CB_U() );
            float roughness = (float)m / (float)(preFilterMipMaps - 1);
            mIBLPrefilterSpecular->render( rr.TM().TD( _probeCameraName, TSLOT_CUBEMAP ), roughness );
        }
    }
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

void RLTargetPBR::renderSkybox() {
    if (!mSkybox) return;
    mSkybox->render( 1.0f );
}

void RLTargetPBR::addProbes() {
    if ( !mSkybox ) return;

    if ( mSkybox->needsRefresh(rr.UpdateCounter(), 2) ) {
        addProbeToCB( FBNames::sceneprobe, Vector3f::ZERO );
    }
}

void CompositePBR::bloom() {
    if ( !isUsingBloom() ) return;

    int amount = 5;

    //	Framebuffer::blit( mColorFB, mBlurVerticalFB, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT0 );
    // New first pre-step of blur code, we couldn't use the code above because glBlitFramebuffer needs the same size of source/dest
    //--
    Framebuffer::blit( mColorFB, mColorFinalFB, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT0 );

    mBlurVerticalFB->bind();
    mBlurVerticalFB->VP()->setMaterialConstant( UniformNames::colorFBTexture, mColorFinalFB->RenderToTexture());
    mBlurVerticalFB->VP()->render_im();
    //--

    for ( int i = 0; i < amount; i++ ) {
        mBlurHorizontalFB->bind();
        mBlurHorizontalFB->VP()->setMaterialConstant( UniformNames::colorFBTexture, mBlurVerticalFB->RenderToTexture());
        mBlurHorizontalFB->VP()->render_im();

        mBlurVerticalFB->bind();
        mBlurVerticalFB->VP()->setMaterialConstant( UniformNames::colorFBTexture, mBlurHorizontalFB->RenderToTexture());
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
    cameraRig->setFramebuffer( mComposite->getColorFB());
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
    cameraRig->setFramebuffer( mComposite->getColorFB() );
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
    addShadowMaps();
    addProbes();
    cb.startList( shared_from_this(), CommandBufferFlags::CBF_None );
    cb.setCameraUniforms( cameraRig->getCamera() );
    cb.pushCommand( { CommandBufferCommandName::colorBufferBindAndClear } );
    cb.pushCommand( { CommandBufferCommandName::cullModeBack } );
    addToCBCore( cb );
}

void RLTargetPBR::endCL( CommandBufferList& cb ) {
    cb.pushCommand( { CommandBufferCommandName::cullModeNone } );
    cb.pushCommand( { CommandBufferCommandName::cullModeNone } );
    cb.pushCommand( { CommandBufferCommandName::depthTestFalse } );
    cb.pushCommand( { CommandBufferCommandName::wireFrameModeFalse } );

    blit(cb);
}

void RLTargetPBR::addToCB( CommandBufferList& cb ) {
    startCL( cb );

    cb.pushCommand( { CommandBufferCommandName::depthWriteFalse } );
    for ( const auto& [k, vl] : rr.CL() ) {
        if ( isKeyInRange(k) ) {
            rr.addToCommandBuffer( vl.mVListTransparent );
        }
    }

    cb.pushCommand( { CommandBufferCommandName::depthWriteTrue } );
    for ( const auto& [k, vl] : rr.CL() ) {
        if ( isKeyInRange(k) ) {
            rr.addToCommandBuffer( vl.mVList );
        }
    }

    renderSkybox();

    endCL( cb );
}

void RLTargetPBR::resize( const Rect2f& _r ) {
    mComposite->setup( _r );
    cameraRig->setFramebuffer( mComposite->getColorFB() );
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

RLTargetProbe::RLTargetProbe( std::shared_ptr<CameraRig> _crig, Renderer& _rr, int _mipmapIndex ) : RLTarget( _rr ) {
    cameraRig = _crig;
    renderIndex = cameraRig->Name().substr( cameraRig->Name().find("_") + 1 );
    mipMapIndex = _mipmapIndex;
}

std::shared_ptr<Framebuffer> RLTargetProbe::getFrameBuffer( [[maybe_unused]] CommandBufferFrameBufferType fbt ) {
    return cameraRig->getFramebuffer();
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

void RLTarget::updateStreams() {
//    uint8_t rt[4*4*4];
//    for ( int t = 0; t < 4*4*4; t+=4 ) {
//        rt[t+0] = static_cast<uint8_t>(unitRand() * 255.0f);
//        rt[t+1] = static_cast<uint8_t>(unitRand() * 255.0f);
//        rt[t+2] = static_cast<uint8_t>(unitRand() * 255.0f);
//        rt[t+3] = 255;
//    }

//    auto packet = rr.SSM().pop( "http://192.168.1.123:8080/video_y" );
//    if ( packet ) {
//        rr.TM().updateTexture( "white_basecolor", packet );
//    }
}
