#include "framebuffer_opengl.h"

#include "../texture_manager.h"
#include "../graphic_functions.hpp"
#include "../render_material.hpp"
#include "../render_list.h"

bool canUseMultiSample() {
#ifdef _OPENGL_ES
    return false;
#else
    return true;
#endif
}

void Framebuffer::attachDepthBuffer() {
    GLCALL( glBindFramebuffer( GL_FRAMEBUFFER, mFramebufferHandle ));

    GLCALL(glGenRenderbuffers(1, &depthTexture) );
    GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, depthTexture) );

    if ( mMultisample ) {
        GLCALL(glRenderbufferStorageMultisample( GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT32F, mWidth, mHeight ) );
    } else {
        GLCALL(glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, mWidth, mHeight ) );
    }

    GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTexture) );

    checkFrameBufferStatus();
}

void Framebuffer::attachColorBuffer( unsigned int index ) {

    GLCALL( glBindFramebuffer( GL_FRAMEBUFFER, mFramebufferHandle ));

    GLuint depthBuffer;
    GLCALL(glGenRenderbuffers(1, &depthBuffer) );
    GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer) );

    if ( mMultisample ) {
        GLCALL(glRenderbufferStorageMultisample( GL_RENDERBUFFER, 4, pixelFormatToGlInternalFormat(mFormat), mWidth, mHeight ) );
    } else {
        GLCALL(glRenderbufferStorage( GL_RENDERBUFFER, pixelFormatToGlInternalFormat(mFormat), mWidth, mHeight ) );
    }

    GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_RENDERBUFFER, depthBuffer) );

    checkFrameBufferStatus();
    std::unique_ptr<GLuint[]> attachments( new GLuint[index + 1] );
    for ( unsigned int t = 0; t < index + 1; t++ ) attachments[t] = GL_COLOR_ATTACHMENT0 + t;
    GLCALL( glDrawBuffers( index + 1, attachments.get()) );
}

void Framebuffer::initDepth( TextureManager& tm ) {
    GLCALL( glGenFramebuffers( 1, &mFramebufferHandle ));
    GLCALL( glBindFramebuffer( GL_FRAMEBUFFER, mFramebufferHandle ));

    mRenderToTexture = tm.addTextureNoData( TextureRenderData{ mName }.size( mWidth, mHeight )
                                                      .wm( WRAP_MODE_REPEAT )
                                                      .fm( FILTER_LINEAR )
                                                      .format( PIXEL_FORMAT_DEPTH_32 ).setIsFramebufferTarget( true )
                                                      .GPUSlot( mTextureGPUSlot ).setGenerateMipMaps(false) );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mRenderToTexture->getHandle(), 0 );

    checkFrameBufferStatus();
}

void Framebuffer::init( TextureManager& tm ) {
    GLCALL( glGenFramebuffers( 1, &mFramebufferHandle ));
    GLCALL( glBindFramebuffer( GL_FRAMEBUFFER, mFramebufferHandle ));

    if ( mMultisample ) {
        GLCALL( glGenRenderbuffers(1, &mRenderbufferHandle) );
        GLCALL( glBindRenderbuffer(GL_RENDERBUFFER, mRenderbufferHandle) );
        GLCALL( glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, pixelFormatToGlInternalFormat(mFormat), mWidth,
                                                 mHeight) );
        GLCALL( glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mRenderbufferHandle
        ) );
    } else {
        if ( mCubeMap ) {
            mRenderToTexture = tm.addCubemapTexture( TextureRenderData{ mName }.setSize( mWidth ).format( mFormat )
                                                             .setGenerateMipMaps( mUseMipMaps )
                                                             .setIsFramebufferTarget( true )
                                                             .wm( WRAP_MODE_CLAMP_TO_EDGE )
                                                             .GPUSlot( mTextureGPUSlot ));
        } else {
            mRenderToTexture = tm.addTextureNoData( TextureRenderData{ mName }.size( mWidth, mHeight )
                                                              .wm( WRAP_MODE_CLAMP_TO_EDGE ).format( mFormat )
                                                              .setIsFramebufferTarget( true )
                                                              .GPUSlot( mTextureGPUSlot )
                                                              .setGenerateMipMaps( mUseMipMaps )
                                                              .setMultisample( mMultisample ));
        }

        LOGR( "Allocating FRAMEBUFFER %s on target %d", mName.c_str(), mRenderToTexture->getGlTextureImageTarget());

        GLCALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                        mRenderToTexture->getGlTextureImageTarget(),
                                        mRenderToTexture->getHandle(), 0 ));

    }
    GLenum attch = GL_COLOR_ATTACHMENT0;
    GLCALL( glDrawBuffers( 1, &attch ));
    checkFrameBufferStatus();
}

void Framebuffer::release() {
    LOGI( "Releasing framebuffer %dx%d, handle=%d", mWidth, mHeight, mFramebufferHandle );

    if ( mFramebufferHandle != 0 ) {
        GLCALL( glDeleteFramebuffers( 1, &mFramebufferHandle ));
        mFramebufferHandle = 0;
    }

    if ( mRenderbufferHandle != 0 ) {
        GLCALL( glDeleteRenderbuffers( 1, &mRenderbufferHandle ));
        mRenderbufferHandle = 0;
    }
}

void Framebuffer::bind( const std::string& renderTargetIndex, const int _mipMapIndex ) {
    GLCALL( glBindFramebuffer( GL_FRAMEBUFFER, mFramebufferHandle ));
    GLCALL( glBindRenderbuffer(GL_RENDERBUFFER, mRenderbufferHandle) );

    auto vWidth  = static_cast<unsigned int>( mWidth * std::pow(0.5, _mipMapIndex) );
    auto vHeight = static_cast<unsigned int>( mHeight * std::pow(0.5, _mipMapIndex) );

    GLCALL( glViewport( 0, 0, vWidth, vHeight ));
    enableMultiSample( mMultisample );
    if ( !renderTargetIndex.empty()) {
        framebufferTexture2D( mRenderToTexture->getHandle(), renderTargetIndex, _mipMapIndex );
    }
    checkFrameBufferStatus();
}

void Framebuffer::bindAndClear( const std::string& renderTargetIndex, const int _mipMapIndex ) {
    bind( renderTargetIndex, _mipMapIndex );
    clearColorBuffer();
    if ( depthTexture ) clearDepthBuffer();
}

void Framebuffer::bindAndClearWithColor( const Color4f& clearColor, const std::string& renderTargetIndex, const int _mipMapIndex ) {
    bind( renderTargetIndex, _mipMapIndex );
    clearColorBufferWithColor( clearColor );
    if ( depthTexture ) clearDepthBuffer();
}

void Framebuffer::setViewport( int x, int y, int width, int height ) {
    GLCALL( glViewport( x, y, width, height ));
}

void Framebuffer::setViewport( int width, int height ) {
    GLCALL( glViewport( 0, 0, width, height ));
}

void Framebuffer::setViewport( const Vector2i& size ) {
    GLCALL( glViewport( 0, 0, size.x(), size.y()));
}

void Framebuffer::setViewport( const Rect2f& rect ) {
    Vector2f size = rect.size();
    GLCALL( glViewport( static_cast<GLint>( rect.origin().x()), static_cast<GLint>( rect.origin().y()),
                        static_cast<GLsizei>( size.x()), static_cast<GLsizei>( size.y())));
}

JMATH::Rect2f Framebuffer::getCurrentViewport() {
    GLint v[4];
    glGetIntegerv( GL_VIEWPORT, v );
    return JMATH::Rect2f( static_cast<float>( v[0] ), static_cast<float>( v[1] ), static_cast<float>( v[2] ),
                          static_cast<float>( v[3] ));
}

void Framebuffer::clearColorBuffer() {
    GLCALL( glClearColor( 0.15f, 0.15f, 0.15f, 1.0f ));
    GLCALL( glClear( GL_COLOR_BUFFER_BIT ));
}

void Framebuffer::clearColorBufferWithColor( const Vector4f& color ) {
    GLCALL( glClearColor( color.x(), color.y(), color.z(), color.w()));
    GLCALL( glClear( GL_COLOR_BUFFER_BIT ));
}

void Framebuffer::enableDepthTest( bool enabled ) {
    if ( enabled ) {
        GLCALL( glEnable( GL_DEPTH_TEST ));
    } else {
        GLCALL( glDisable( GL_DEPTH_TEST ));
    }
}

void Framebuffer::blit( std::shared_ptr<Framebuffer> source, std::shared_ptr<Framebuffer> dest,
                        GLenum atthSource, GLenum atthDest ) {
    GLCALL( glBindFramebuffer( GL_READ_FRAMEBUFFER, source->Handle()));
    GLCALL( glReadBuffer( atthSource ));

    GLCALL( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, dest->Handle()));
    if ( dest->Handle() != 0 ) { // ### NDDado: If we are blitting to the default frame buffer without this if will
                                 // generate a log warning, to be checked
        GLCALL( glDrawBuffers(1, &atthDest ));
    }

    GLCALL( glBlitFramebuffer( 0, 0, source->getWidth(), source->getHeight(), 0, 0, dest->getWidth(), dest->getHeight(),
                               GL_COLOR_BUFFER_BIT, GL_LINEAR ));
}

void Framebuffer::blitWithRect( std::shared_ptr<Framebuffer> source,
                                std::shared_ptr<Framebuffer> dest,
                                GLenum atthSource,
                                GLenum atthDest,
                                Rect2f _sourceRect,
                                Rect2f _destRect ) {
    GLCALL( glBindFramebuffer( GL_READ_FRAMEBUFFER, source->Handle()));
    GLCALL( glReadBuffer( atthSource ));

    GLCALL( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, dest->Handle()));
    if ( dest->Handle() != 0 ) { // ### NDDado: If we are blitting to the default frame buffer without this if will
        // generate a log warning, to be checked
        GLCALL( glDrawBuffers(1, &atthDest ));
    }

    GLCALL( glBlitFramebuffer( _sourceRect.left(), _sourceRect.top(),
                               _sourceRect.right(), _sourceRect.bottom(),
                               _destRect.left(), _destRect.top(),
                               _destRect.right(), _destRect.bottom(),
                               GL_COLOR_BUFFER_BIT, GL_LINEAR ));

}
