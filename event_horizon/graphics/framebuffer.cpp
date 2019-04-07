//
// Created by Dado on 12/12/2017.
//

#include "framebuffer.h"
#include "core/service_factory.h"
#include "renderer.h"
#include <graphics/vertex_processing.h>
#include <graphics/vp_builder.hpp>

std::shared_ptr<Framebuffer> FrameBufferBuilder::buildSimple() {
    auto ret = ServiceFactory::create<Framebuffer>();
    ret->mName = mName;
    ret->mWidth = mWidth;
    ret->mHeight = mHeight;
    ret->initSimple();

    return ret;
}

std::shared_ptr<Framebuffer> FrameBufferBuilder::build() {
    auto ret = ServiceFactory::create<Framebuffer>();

    if ( mName == "default" ) {
        GLint dfb;
        GLCALL ( glGetIntegerv( GL_FRAMEBUFFER_BINDING, &dfb ));
        ret->mFramebufferHandle = static_cast<GLuint >(dfb);
        if ( mWidth <= 0 || mHeight <= 0 ) {
            GLCALL( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, dfb ));
            JMATH::Rect2f r = Framebuffer::getCurrentViewport();
            ret->mWidth = static_cast<int>( r.width());
            ret->mHeight = static_cast<int>( r.height());
        } else {
            ret->mWidth = mWidth;
            ret->mHeight = mHeight;
        }
        LOGR( "Default framebuffer: %d [%d,%d]", ret->mFramebufferHandle, ret->mWidth, ret->mHeight );
        return ret;
    }

    ret->mName = mName;
    ret->mFormat = mFormat;
    ret->mUseMipMaps = mUseMipMaps;
    ret->mMultisample = mIsMultisampled;// && canUseMultiSample();
    ret->mHDR = mIsHDR;
    ret->mCubeMap = mIsCubemap;
    ret->mWidth = mWidth;
    ret->mHeight = mHeight;
    ret->mTextureGPUSlot = mTextureGPUSlot;

    if ( mDepthOnly ) {
        ret->initDepth(rr.TM());
        return ret;
    }

    ret->init( rr.TM() );

    if ( mAttachDepth ) ret->attachDepthBuffer();
    for ( const auto& [cbname, index] : mColorBufferAttachments ) {
        ret->attachColorBuffer( index );
    }

    if ( !mIMShaderName.empty() ) {
        std::string vn = mName + std::to_string(mDestViewport.size().x()) + std::to_string(mDestViewport.size().y());
        ret->mVPListIM = VPBuilder<PosTex2dStrip>{ rr, ShaderMaterial{mIMShaderName} }.
                      p(std::make_shared<PosTex2dStrip>( mDestViewport.ss(),
                        QuadVertices2::QUAD_TEX_STRIP_INV_Y_COORDS )).n(vn).build();
    }

    return ret;
}

cubeMapFrameBuffers FrameBufferBuilder::buildCube() {

    cubeMapFrameBuffers retContainer;

    // Create the whole cubemap texture, once
    auto mRenderToTexture = rr.TM()->addCubemapTexture( TextureRenderData{ mName }
                                                               .setSize( mWidth ).format( mFormat )
                                                               .setGenerateMipMaps( mUseMipMaps )
                                                               .setIsFramebufferTarget( true )
                                                               .wm( WRAP_MODE_CLAMP_TO_EDGE )
                                                               .GPUSlot( mTextureGPUSlot ));

    GLuint mips = mUseMipMaps ? 1 + static_cast<GLuint>( floor( log( (float)max( mWidth, mHeight ) ) ) ) : 1;

    for ( uint32_t m = 0; m < mips; m++ ) {
        for ( uint32_t t = 0; t < 6; t++ ) {
            auto ret = ServiceFactory::create<Framebuffer>();

            ret->mName = mName;
            ret->mFormat = mFormat;
            ret->mUseMipMaps = mUseMipMaps;
            ret->mMultisample = mIsMultisampled;// && canUseMultiSample();
            ret->mHDR = mIsHDR;
            ret->mCubeMap = mIsCubemap;
            ret->mWidth = mWidth;
            ret->mHeight = mHeight;
            ret->mTextureGPUSlot = mTextureGPUSlot;

            ret->initCubeMap( mRenderToTexture, t, m );

            for ( const auto&[cbname, index] : mColorBufferAttachments ) {
                ret->attachColorBuffer( index );
            }
            retContainer.emplace_back( ret );
        }
    }

    return retContainer;
}

FrameBufferBuilder& FrameBufferBuilder::dv( const Rect2f& _destViewport, BlitType _bt ) {
    Vector2f oneOverSize = _bt == BlitType::OnScreen ? (1.0f/getScreenSizef) : (1.0f/_destViewport.size());
    mDestViewport = _destViewport * oneOverSize;
    return *this;
}

