//
//  render_list.cpp
//  SixthView
//
//  Created byDado on 11/01/2013.
//  Copyright (c) 2013Dado. All rights reserved.
//

#include "render_list.h"

#include <stb/stb_image_write.h>
#include <core/http/basen.hpp>
#include <core/camera.h>
#include <core/camera_rig.hpp>
#include <core/descriptors/uniform_names.h>
#include <graphics/renderer.h>
#include <graphics/graphic_functions.hpp>
#include <graphics/framebuffer.h>
#include <graphics/shadowmap_manager.h>
#include <graphics/render_targets.hpp>
#include <graphics/shader_manager.h>

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
        case CommandBufferCommandName::colorBufferBindAndClearDepthOnly:
            return "colorBufferBindAndClearDepthOnly";
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
        case CommandBufferCommandName::depthMapBufferBindAndClear:
            return "depthMapBufferBindAndClear";
        case CommandBufferCommandName::normalMapBufferBindAndClear:
            return "normalMapBufferBindAndClear";
        case CommandBufferCommandName::ssaoRender:
            return "ssaoMapBufferBindAndClear";
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

size_t CommandBufferEntryCommand::run( Renderer& rr, CommandBuffer* cb ) const {
    if ( Type() == CommandBufferEntryCommandType::Comamnd ) {
//        LOGRS("        Command: " << commandToNmeHumanReadable(mCommand.name) );
        mCommand.issue( rr, cb );
        return 0;
    } else {
//        LOGRS("        Render geom: " << mVP->mVPList.Name() );
        if ( mVP->mMaterial ) {
            mVP->mVPList->drawWith(mVP->mMaterial.get(), mVP->mProgram);
        } else {
            mVP->mVPList->draw(mVP->mProgram);
        }
        return 1;
    }
}

size_t CommandBuffer::render( Renderer& rr ) {
//    LOGR("CommandList RENDER START %d", mCommandList.size() );
//    LOGR("****************************************************************************");
    size_t ret = 0;
    for ( const auto& i : mCommandList ) {
        ret += i.run( rr, this );
    }
    return ret;
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

void CommandBufferList::pushVP( std::shared_ptr<VPList> _vp,
                                std::shared_ptr<RenderMaterial> _mat,
                                std::shared_ptr<Matrix4f> _modelMatrix,
                                Program* _forceProgram,
                                float alphaDrawThreshold ) {
    if ( _vp->transparencyValue() > alphaDrawThreshold ) {
        mCurrent->push( CommandBufferEntry{ _vp, _mat, _modelMatrix, _forceProgram } );
    }
}

void CommandBufferList::pushCommand( const CommandBufferCommand& cmd ) {
    mCurrent->push( cmd );
}

void CommandBufferList::startTarget( std::shared_ptr<Framebuffer> _fbt, Renderer& _rr ) {
    startList( std::make_shared<RLTargetFB>( _fbt, _rr ), CommandBufferFlags::CBF_DoNotSort );
    pushCommand( { CommandBufferCommandName::colorBufferBindAndClear } );
    pushCommand( { CommandBufferCommandName::cullModeNone } );
}

void CommandBufferList::startList( std::shared_ptr<RLTarget> _target, CommandBufferFlags flags ) {
    mCommandBuffers.emplace_back( CommandBuffer{flags} );
    mCurrent = &mCommandBuffers.back();
    mCurrent->mTarget = _target;
}

size_t CommandBufferList::render( [[maybe_unused]] int eye ) {
    size_t numDrawCalls = 0;
    for ( auto& cb : mCommandBuffers ) {
        numDrawCalls += cb.render( rr );
    }
    return numDrawCalls;
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
    UBO::mapUBOData( rr.CameraUBO(), UniformNames::eyeDir, c0->getDirection(), mCurrent->UBOCameraBuffer.get());
    UBO::mapUBOData( rr.CameraUBO(), UniformNames::nearFar, c0->getNearFar(), mCurrent->UBOCameraBuffer.get());

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
        case CommandBufferCommandName::colorBufferBindAndClearDepthOnly:
            cstack->fb(CommandBufferFrameBufferType::sourceColor)->bindAndClearDepthOnly(cstack->frameBufferTextureValues.get());
            break;
        case CommandBufferCommandName::colorBufferClear:
            cstack->fb(CommandBufferFrameBufferType::sourceColor)->clearColorBuffer();
            break;
        case CommandBufferCommandName::shadowMapBufferBind:
            cstack->fb(CommandBufferFrameBufferType::shadowMap)->bind();
            break;
        case CommandBufferCommandName::depthMapBufferBindAndClear:
            cstack->fb(CommandBufferFrameBufferType::depthMap)->bind();
            cstack->fb(CommandBufferFrameBufferType::depthMap)->clearDepthBuffer( 1.0f );
            break;
        case CommandBufferCommandName::normalMapBufferBindAndClear:
            cstack->fb(CommandBufferFrameBufferType::normalMap)->bindAndClearWithColor(C4f::GREEN);
            break;
        case CommandBufferCommandName::ssaoRender: {
            cstack->fb( CommandBufferFrameBufferType::ssaoMap )->bind();
            enableDepthTest( false );
            setCullMode( CULL_NONE );
            cstack->fb( CommandBufferFrameBufferType::ssaoMap )->VP()->setMaterialConstant(
                    UniformNames::depthMapTexture,
                    rr.getDepthMapFB()->RenderToTexture()->TDI( 0 ));
            cstack->fb( CommandBufferFrameBufferType::ssaoMap )->VP()->setMaterialConstant(
                    UniformNames::normalMapTexture,
                    cstack->fb( CommandBufferFrameBufferType::normalMap )->RenderToTexture()->TDI( 1 ));
            cstack->fb( CommandBufferFrameBufferType::ssaoMap )->VP()->setMaterialConstant(
                    UniformNames::noise4x4Texture,
                    rr.TM()->TD( S::NOISE4x4 )->TDI( 2 ));

//            cstack->fb( CommandBufferFrameBufferType::ssaoMap )->VP()->updateGPUVData( vbib );
            auto farClipCorners = cstack->Target()->getCamera()->frustumFarViewPort();
            cstack->fb( CommandBufferFrameBufferType::ssaoMap )->VP()->updateP3V3(farClipCorners);
            cstack->fb( CommandBufferFrameBufferType::ssaoMap )->VP()->draw();
            enableDepthTest( true );
            setCullMode( CULL_BACK );
        }
            break;
        case CommandBufferCommandName::shadowMapClearDepthBufferZero:
            cstack->fb(CommandBufferFrameBufferType::shadowMap)->clearDepthBuffer( 0.0f );
            break;
        case CommandBufferCommandName::shadowMapClearDepthBufferOne:
            cstack->fb(CommandBufferFrameBufferType::shadowMap)->clearDepthBuffer( 1.0f );
            break;
        case CommandBufferCommandName::defaultFrameBufferBind:
            rr.getDefaultFB()->bind();
            break;
        case CommandBufferCommandName::blitToScreen:
            Framebuffer::blitWithRect( cstack->fb(CommandBufferFrameBufferType::sourceColor), rr.getDefaultFB(),
                               GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT0,
                               cstack->sourceViewport(), cstack->destViewport() );
            break;
        case CommandBufferCommandName::blitPBRToScreen:
            Framebuffer::blit( cstack->fb(CommandBufferFrameBufferType::finalBlit),
                               cstack->fb(CommandBufferFrameBufferType::screen),
                               GL_COLOR_ATTACHMENT0,
                               GL_COLOR_ATTACHMENT0 );
            break;
        case CommandBufferCommandName::blitPRB:

            setCullMode( CULL_NONE );
            enableDepthTest( false );
            setWireFrame( false );

            dynamic_cast<CompositePBR*>(dynamic_cast<RLTargetPBR*>(cstack->Target().get())->Composite().get())->bloom();
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

            cstack->fb(CommandBufferFrameBufferType::finalResolve)->VP()->setMaterialConstant(
                    UniformNames::shadowMapTexture,
                    rr.getShadowMapFB()->RenderToTexture()->TDI(2));

            cstack->fb(CommandBufferFrameBufferType::finalResolve)->VP()->setMaterialConstant(
                    UniformNames::lut3dTexture,
                    rr.TM()->TD(UniformNames::lut3dTexture)->TDI(3));

            cstack->fb(CommandBufferFrameBufferType::finalResolve)->VP()->setMaterialConstant(
                    UniformNames::depthMapTexture,
                    rr.getDepthMapFB()->RenderToTexture()->TDI(4));

            cstack->fb(CommandBufferFrameBufferType::finalResolve)->VP()->setMaterialConstant(
                    UniformNames::ssaoMapTexture,
                    cstack->fb(CommandBufferFrameBufferType::ssaoMap)->RenderToTexture()->TDI(5));

            if ( rr.isLoading() ) {
                cstack->fb(CommandBufferFrameBufferType::finalResolve)->VP()->drawWithProgram(
                        rr.SM()->P(S::LOADING_SCREEN).get() );
            } else {
                cstack->fb(CommandBufferFrameBufferType::finalResolve)->VP()->draw();
            }

            cstack->postBlit();

            break;
        case CommandBufferCommandName::targetVP:
            break;

        default:
            break;
    };
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
//    ### REF this entryName is dodgy anyway, as it's used only by RenderHook which is going to be killed soon
//    if ( Type() == CommandBufferEntryCommandType::VP ) {
//       return mVP->mVPList.Name();
//    }
    return "";
}

std::shared_ptr<CommandBufferEntry>& CommandBufferEntryCommand::entry() {
    return mVP;
}
