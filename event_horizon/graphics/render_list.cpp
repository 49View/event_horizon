//
//  render_list.cpp
//  SixthView
//
//  Created byDado on 11/01/2013.
//  Copyright (c) 2013Dado. All rights reserved.
//

#include "render_list.h"

#include "core/math/spherical_harmonics.h"
#include "graphic_functions.hpp"
#include "camera_manager.h"
#include "framebuffer.h"
#include "renderer.h"
//#include <CImg.h>

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
        case CommandBufferCommandName::blitPBROffscreen:
            Framebuffer::blit( cstack->fb(CommandBufferFrameBufferType::sourceColor),
                               cstack->fb(CommandBufferFrameBufferType::finalResolve),
                               GL_COLOR_ATTACHMENT0,
                               GL_COLOR_ATTACHMENT0 );
            break;
        case CommandBufferCommandName::blitPRB:

            Framebuffer::blit( cstack->fb(CommandBufferFrameBufferType::sourceColor),
                               cstack->fb(CommandBufferFrameBufferType::finalResolve),
                               GL_COLOR_ATTACHMENT0,
                               GL_COLOR_ATTACHMENT0 );

            rr.getDefaultFB()->bind();
            cstack->fb(CommandBufferFrameBufferType::finalResolve)->VP()->setMaterialConstant(
                    UniformNames::colorFBTexture,
                    cstack->fb(CommandBufferFrameBufferType::finalResolve)->RenderToTexture());
            cstack->fb(CommandBufferFrameBufferType::finalResolve)->VP()->setMaterialConstant(
                    UniformNames::bloomTexture,
                    cstack->fb(CommandBufferFrameBufferType::blurVertical)->RenderToTexture());
            cstack->fb(CommandBufferFrameBufferType::finalResolve)->VP()->setMaterialConstant(
                    UniformNames::shadowMapTexture,
                    rr.getShadowMapFB()->RenderToTexture());
//    mColorFinalFB->VP()->setMaterialConstant( UniformNames::shadowMapTexture, mShadowMapFB->RenderToTexture());
            cstack->fb(CommandBufferFrameBufferType::finalResolve)->VP()->render_im();

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
        default:
            ASSERT(0);
    }
    return cameraRig->getFramebuffer();
}

RLTargetPBR::RLTargetPBR( std::shared_ptr<CameraRig> cameraRig, const Rect2f& screenViewport, Renderer& rr )
                        : RLTarget( cameraRig, screenViewport, rr ) {

    float svtop = getScreenSizef.y()-screenViewport.bottom();
    Rect2f svss = { screenViewport.left(), svtop, screenViewport.right(), svtop + screenViewport.height() };
    mComposite = std::make_shared<CompositePBR>( rr, cameraRig->getMainCamera()->Name(), svss );
    cameraRig->setFramebuffer( mComposite->getColorFB() );
    bucketRanges.push_back( {CommandBufferLimits::PBRStart, CommandBufferLimits::PBREnd} );

//    cameraRig->getMainCamera()->goTo( Vector3f{ 1.0f, 1.0f, 0.0f },
//                   Vector3f{ 0.0f, 0.0f, 0.0f },
//                   Vector3f{ 0.0f, -M_PI_2, 0.0f }, 1.0f );
    cameraRig->getMainCamera()->goTo( Vector3f{ 0.0f, 1.6f, 1.0f },
                                      Vector3f{ 0.0f, 0.0f, 0.0f },
                                      Vector3f{ 0.0f, 0.0f, 0.0f }, 1.0f );
    cameraRig->getMainCamera()->Mode( CameraMode::Doom );

}

std::shared_ptr<Framebuffer> RLTargetPBR::getFrameBuffer( CommandBufferFrameBufferType fbt ) {

    switch ( fbt ) {
        case CommandBufferFrameBufferType::sourceColor:
            return mComposite->getColorFB();
        case CommandBufferFrameBufferType::shadowMap:
            return rr.getShadowMapFB();
        case CommandBufferFrameBufferType::finalResolve:
            return mComposite->getColorFinalFB();
        case CommandBufferFrameBufferType::blurVertical:
            return mComposite->getBlurVerticalFB();
        case CommandBufferFrameBufferType::blurHorizontal:
            return mComposite->getBlurHorizontalFB();
        default:
            ASSERT(0);
    }

    return cameraRig->getFramebuffer();

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
    if ( mCompositeFinalDest == CompositeFinalDest::OnScreen ) {
        cbl.pushCommand( { CommandBufferCommandName::blitToScreen } );
    }
}

CompositePlain::CompositePlain( Renderer& _rr, const std::string& _name, const Rect2f& _destViewport,
                                CompositeFinalDest _cfd ) : Composite(_rr ) {
    mCompositeFinalDest = _cfd;
    mColorFB = FrameBufferBuilder{ rr, _name }.size(_destViewport.size()).build();
}

void RLTargetPBR::blit(CommandBufferList& cbl) {
    mComposite->blit( cbl );
}

void CompositePBR::blit( CommandBufferList& cbl ) {
    if ( mCompositeFinalDest == CompositeFinalDest::OnScreen ) {
        cbl.pushCommand( { CommandBufferCommandName::blitPRB } );
    } else {
        cbl.pushCommand( { CommandBufferCommandName::blitPBROffscreen } );
    }
}

CompositePBR::CompositePBR( Renderer& _rr, const std::string& _name, const Rect2f& _destViewport,
                            CompositeFinalDest _cfd ) : Composite(_rr ) {
    mCompositeFinalDest = _cfd;
    float bloomScale = 1.0f/8.0f;
    Vector2f vsize = _destViewport.size();
    mColorFB = FrameBufferBuilder{rr,"colorFrameBuffer"}.multisampled().size(vsize).format
            (PIXEL_FORMAT_HDR_RGBA_16).addColorBufferAttachments({ "colorFrameBufferAtth1", 1 }).build();
    mBlurHorizontalFB = FrameBufferBuilder{ rr, "blur_horizontal_b" }.size(vsize*bloomScale).noDepth()
            .format(PIXEL_FORMAT_HDR_RGBA_16).GPUSlot(TSLOT_BLOOM).IM(S::BLUR_HORIZONTAL).build();
    mBlurVerticalFB = FrameBufferBuilder{ rr, "blur_vertical_b" }.size(vsize*bloomScale).noDepth()
            .format(PIXEL_FORMAT_HDR_RGBA_16).GPUSlot(TSLOT_BLOOM).IM(S::BLUR_VERTICAL).build();
    mColorFinalFB = FrameBufferBuilder{ rr, "colorFinalFrameBuffer"}.size(vsize).noDepth().dv(_destViewport)
            .format(PIXEL_FORMAT_HDR_RGBA_16).GPUSlot(TSLOT_COLOR).IM(S::FINAL_COMBINE).build();
}

void RLTarget::addToCBCore( CommandBufferList& cb ) {
    auto lcvt = cameraRig->getMainCamera()->getCvt();

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

RLTargetPlain::RLTargetPlain( std::shared_ptr<CameraRig> cameraRig, const Rect2f& screenViewport, Renderer& rr )
        : RLTarget( cameraRig, screenViewport, rr ) {
    mComposite = std::make_shared<CompositePlain>( rr, cameraRig->getMainCamera()->Name(), screenViewport );
    cameraRig->setFramebuffer( mComposite->getColorFB());
//    cameraRig->getMainCamera()->goTo( cameraRig->getMainCamera()->centerScreenOn( {5.0f, 5.0f}, true ),
//            Vector3f::ZERO,
//            .33f, 0.0f );
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

void RLTarget::clearCB( CommandBufferList& cb ) {
    for ( auto& [k, vl] : rr.CL() ) {
        if ( isKeyInRange( k, RLClearFlag::DontIncludeCore ) ) {
            vl.mVListTransparent.clear();
            vl.mVList.clear();
        }
    }
}

void RLTargetPBR::startCL( CommandBufferList& cb ) {
    rr.addShadowMaps( shared_from_this() );
    rr.addProbes();
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


    rr.renderSkybox();

    endCL( cb );
}

RLTargetFB::RLTargetFB( std::shared_ptr<Framebuffer> _fbt, Renderer& _rr ) : RLTarget( _rr ) {
    framebuffer = _fbt;
}

std::shared_ptr<Framebuffer> RLTargetFB::getFrameBuffer( CommandBufferFrameBufferType fbt ) {
    return framebuffer;
}

RLTargetProbe::RLTargetProbe( const std::string& _cameraRig, const int _faceIndex,
                              Renderer& _rr, int _mipmapIndex ) : RLTarget( _rr ) {
    renderIndex = cubemapFaceToString( static_cast<CubemapFaces>(_faceIndex) );
    mipMapIndex = _mipmapIndex;
    cameraName = _cameraRig + "_" + renderIndex;
    cameraRig = rr.CM().getRig( cameraName );
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

void Composite::takeScreenshot( std::shared_ptr<Framebuffer> _fb, bool bCrop, const Vector2f& cropSize ) {
    auto outB = std::make_unique<unsigned char[]>(_fb->getWidth() * _fb->getHeight() * 4);
    glReadPixels( 0, 0, _fb->getWidth(), _fb->getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, outB.get() );
//    cimg_library::CImg<unsigned char> image( _fb->getWidth(), _fb->getHeight(), 1, 4 );
//    for ( auto y = 0; y < _fb->getHeight(); y++ ) {
//        for ( auto x = 0; x < _fb->getWidth(); x++ ) {
//            image( x, y, 0, 0 ) = outB[y * _fb->getWidth() * 4 + x * 4 + 0];
//            image( x, y, 0, 1 ) = outB[y * _fb->getWidth() * 4 + x * 4 + 1];
//            image( x, y, 0, 2 ) = outB[y * _fb->getWidth() * 4 + x * 4 + 2];
//            image( x, y, 0, 3 ) = outB[y * _fb->getWidth() * 4 + x * 4 + 3];
//        }
//    }

    if ( bCrop ) {
//        image.autocrop();
//        float ratio = static_cast<float>( image.width()) / static_cast<float>( image.height());
//        image.resize( cropSize.x(), static_cast<int>( cropSize.y() / ratio ), 1, 4, 6 );
//        unsigned char *outB2 = new unsigned char[image.width() * image.height() * 4];
//
//        for ( auto x = 0; x < image.width(); x++ ) {
//            for ( auto y = 0; y < image.height(); y++ ) {
//                outB2[y * image.width() * 4 + x * 4 + 0] = image( x, y, 0, 0 );
//                outB2[y * image.width() * 4 + x * 4 + 1] = image( x, y, 0, 1 );
//                outB2[y * image.width() * 4 + x * 4 + 2] = image( x, y, 0, 2 );
//                outB2[y * image.width() * 4 + x * 4 + 3] = image( x, y, 0, 3 );
//            }
//        }
//        stbi_write_png(_screenshotFilename.c_str(), image.width(), image.height(), 4,
//                       outB2, image.width() * 4 );
//        delete[] outB2;
    } else {
//        stbi_write_png(_screenshotFilename.c_str(), image.width(), image.height(), 4,
//                       outB, image.width() * 4 );
    }

//    delete[] outB;
}
