#include "framebuffer_opengl.h"
#include "../framebuffer.h"

#include <core/math/vector4f.h>

#include "../texture_manager.h"
#include "../graphic_functions.hpp"
#include "../render_material.hpp"
#include "../render_list.h"

Color4f Framebuffer::clearColorValue = Color4f::XTORGBA("#238FCBFF");
bool Framebuffer::gbIsHDRSupported = true;

bool canUseMultiSample() {
#ifdef _OPENGL_ES
    return false;
#else
    return true;
#endif
}

void Framebuffer::attachDepthBuffer() {
    GLCALL( glBindFramebuffer( GL_FRAMEBUFFER, mFramebufferHandle ));

    GLCALL(glGenRenderbuffers(1, &mDepthBufferHandle) );
    GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, mDepthBufferHandle) );

#ifndef _WEBGL1
    if ( mMultisample ) {
        GLCALL(glRenderbufferStorageMultisample( GL_RENDERBUFFER, getMultiSampleCount(), GL_DEPTH_COMPONENT32F, mWidth, mHeight ) );
    } else {
        GLCALL(glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, mWidth, mHeight ) );
    }
#else
    GLCALL(glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mWidth, mHeight ) );
#endif

    GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBufferHandle) );

//    checkFrameBufferStatus();
}

void Framebuffer::attachColorBuffer( unsigned int index ) {

    GLCALL( glBindFramebuffer( GL_FRAMEBUFFER, mFramebufferHandle ));

    GLuint depthBuffer;
    GLCALL(glGenRenderbuffers(1, &depthBuffer) );
    GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer) );

#ifndef _WEBGL1
    if ( mMultisample ) {
        GLCALL(glRenderbufferStorageMultisample( GL_RENDERBUFFER, getMultiSampleCount(), pixelFormatToGlInternalFormat(mFormat), mWidth, mHeight ) );
    } else {
        GLCALL(glRenderbufferStorage( GL_RENDERBUFFER, pixelFormatToGlInternalFormat(mFormat), mWidth, mHeight ) );
    }
#else
    GLCALL(glRenderbufferStorage( GL_RENDERBUFFER, GL_RGB565, mWidth, mHeight ) );
#endif

    GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_RENDERBUFFER, depthBuffer) );

    checkFrameBufferStatus();
    std::unique_ptr<GLuint[]> attachments( new GLuint[index + 1] );
    for ( unsigned int t = 0; t < index + 1; t++ ) attachments[t] = GL_COLOR_ATTACHMENT0 + t;
    GLCALL( glDrawBuffers( index + 1, attachments.get()) );
    mNumColorAttachments++;
}

void Framebuffer::initDepth( std::shared_ptr<TextureManager> tm ) {
    GLCALL( glGenFramebuffers( 1, &mFramebufferHandle ));
    GLCALL( glBindFramebuffer( GL_FRAMEBUFFER, mFramebufferHandle ));

    mFormat = PIXEL_FORMAT_DEPTH_32;
    auto trd = ImageParams{}.size( mWidth, mHeight ).format( PIXEL_FORMAT_DEPTH_32 ).setWrapMode(WRAP_MODE_CLAMP_TO_EDGE);
    mRenderToTexture = tm->addTextureNoData( TextureRenderData{ mName, trd }
                                                      .fm( FILTER_LINEAR )
                                                      .setIsFramebufferTarget( true )
                                                      .GPUSlot( mTextureGPUSlot ).setGenerateMipMaps(false) );

#ifndef _WEBGL1
    GLCALL( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL ) );
    GLCALL( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE ) );
#endif
//    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//    GLCALL( glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));
    GLCALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mRenderToTexture->getHandle(), 0 ) );
    // ###WEBGL1###
#ifndef _WEBGL1
    // Not sure what this is in the first place
    GLCALL( glDrawBuffers(0, GL_NONE) );
    GLCALL( glReadBuffer(GL_NONE) );
#endif

    checkFrameBufferStatus();
}

void Framebuffer::init( std::shared_ptr<TextureManager> tm ) {
    GLCALL( glGenFramebuffers( 1, &mFramebufferHandle ));
    GLCALL( glBindFramebuffer( GL_FRAMEBUFFER, mFramebufferHandle ));

#ifndef _WEBGL1
    if ( mMultisample ) {
        GLCALL( glGenRenderbuffers(1, &mRenderbufferHandle) );
        GLCALL( glBindRenderbuffer(GL_RENDERBUFFER, mRenderbufferHandle) );
        GLCALL( glRenderbufferStorageMultisample(GL_RENDERBUFFER, getMultiSampleCount(), pixelFormatToGlInternalFormat(mFormat), mWidth,
                                                 mHeight) );
        GLCALL( glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mRenderbufferHandle
        ) );
    } else {
        auto trd = ImageParams{}.size( mWidth, mHeight ).format( mFormat ).setWrapMode(WRAP_MODE_CLAMP_TO_EDGE);
        mRenderToTexture = tm->addTextureNoData( TextureRenderData{ mName, trd }
                                                          .setIsFramebufferTarget( true )
                                                          .GPUSlot( mTextureGPUSlot )
                                                          .setGenerateMipMaps( mUseMipMaps )
                                                          .setMultisample( mMultisample ));

        LOGRS( "[FRAMEBUFFER] " << mName << " Target: [" << mRenderToTexture->getGlTextureImageTargetString()
            << "] Format: [" << glEnumToString( pixelFormatToGlInternalFormat(mFormat) ) << "]" )
        GLCALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                        mRenderToTexture->getGlTextureImageTarget(),
                                        mRenderToTexture->getHandle(), 0 ));

    }
#else
    auto trd = ImageParams{}.size( mWidth, mHeight ).format( mFormat ).setWrapMode(WRAP_MODE_CLAMP_TO_EDGE);
        mRenderToTexture = tm->addTextureNoData( TextureRenderData{ mName, trd }
                                                          .setIsFramebufferTarget( true )
                                                          .GPUSlot( mTextureGPUSlot )
                                                          .setGenerateMipMaps( mUseMipMaps )
                                                          .setMultisample( mMultisample ));

//        LOGRS( "[FRAMEBUFFER] " << mName << " Target: [" << mRenderToTexture->getGlTextureImageTargetString()
//            << "] Format: [" << glEnumToString( pixelFormatToGlInternalFormat(mFormat) ) << "]"
//            << "] AttachmentFormat: [" << glEnumToString( pixelFormatToFrameBufferAttachment(mFormat) ) << "]"
//            )
        GLCALL( glFramebufferTexture2D( GL_FRAMEBUFFER, pixelFormatToFrameBufferAttachment(mFormat),
                                        mRenderToTexture->getGlTextureImageTarget(),
                                        mRenderToTexture->getHandle(), 0 ));
#endif
//    GLenum attch = GL_COLOR_ATTACHMENT0;
//    GLCALL( glDrawBuffers( 1, &attch ));
    checkFrameBufferStatus();
}

void Framebuffer::initSimple() {

    GLCALL(glGenRenderbuffers(1, &mRenderbufferHandle));
    GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, mRenderbufferHandle));
    GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB565, mWidth, mHeight));
    GLCALL( glBindRenderbuffer( GL_RENDERBUFFER, 0 ));

//// Build the texture that will serve as the depth attachment for the framebuffer.
    GLCALL(glGenRenderbuffers(1, &mDepthBufferHandle));
    GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, mDepthBufferHandle));
    GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mWidth, mHeight));
    GLCALL( glBindRenderbuffer( GL_RENDERBUFFER, 0 ));

//// Build the framebuffer.
    GLCALL(glGenFramebuffers(1, &mFramebufferHandle));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferHandle));
    GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mRenderbufferHandle));
    GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBufferHandle));

    checkFrameBufferStatus();

    LOGR("Allocating FRAMEBUFFER %s, [%d,%d], handle %d, renderHandle %d",
         mName.c_str(), mWidth, mHeight, mFramebufferHandle, mRenderbufferHandle);
}

void Framebuffer::initCubeMap( std::shared_ptr<Texture> cubemapTarget, uint32_t cubemapFaceIndex, uint32_t mipIndex ) {
    GLCALL( glGenFramebuffers( 1, &mFramebufferHandle ));
    GLCALL( glGenRenderbuffers(1, &mRenderbufferHandle) );
    GLCALL( glBindFramebuffer( GL_FRAMEBUFFER, mFramebufferHandle ));
    GLCALL( glBindRenderbuffer(GL_RENDERBUFFER, mRenderbufferHandle) );

    mRenderToTexture = cubemapTarget;
    mWidth  = static_cast<unsigned int>( mWidth  * std::pow(0.5, mipIndex) );
    mHeight = static_cast<unsigned int>( mHeight * std::pow(0.5, mipIndex) );

#ifdef _WEBGL1
    GLCALL(glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mWidth, mHeight ) );
#else
    GLCALL(glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mWidth, mHeight ) );
#endif
    GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRenderbufferHandle) );

//    mTargetType = GL_TEXTURE_CUBE_MAP_POSITIVE_X + cubemapFaceIndex;
//    mTargetHandle = cubemapTarget->getHandle();
//    mTargetMipmap = mipIndex;

//    GLCALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
//                                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + cubemapFaceIndex,
//                                    cubemapTarget->getHandle(), mipIndex ));
    checkFrameBufferStatus();

    LOGR( "Allocating FRAMEBUFFER CubeMap %s on target %d at mipmap %d", mName.c_str(), cubemapFaceIndex, mipIndex );
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

void Framebuffer::bind( const FrameBufferTextureValues* _values ) {
    GLCALL( glBindFramebuffer( GL_FRAMEBUFFER, mFramebufferHandle ));
    GLCALL( glBindRenderbuffer( GL_RENDERBUFFER, mRenderbufferHandle) );
    if ( _values ) {
        GLCALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                        frameBufferTargetToGl(_values->targetType),
                                        _values->targetHandle,
#ifdef _WEBGL1
                                        0
#else
                                        _values->targetMipmap
#endif
                                        ));
//        LOGRS( "[FramebufferValues] format: [" << glEnumToString( frameBufferTargetToGl(_values->targetType)) << "]"
//        << " target: [" << _values->targetHandle << "]"
//        << " targetMipmap: [" << _values->targetMipmap << "]"
//        << " Width/mWidth: " << _values->width << "/" << mWidth << " Height/mHeight: " << _values->height << "/" << mHeight
//        << " Handle: " << mFramebufferHandle
//        );
        GLCALL( glViewport( 0, 0, _values->width, _values->height ));
    } else {
        GLCALL( glViewport( 0, 0, mWidth, mHeight ));
    }
    enableMultiSample( mMultisample );
//    checkFrameBufferStatus();
}

void Framebuffer::bindAndClear( const FrameBufferTextureValues* _values ) {
//    LOGRS( "Framebuffer bindclear handle " << mFramebufferHandle << "[" << mWidth << "," << mHeight << "]" );
    bind( _values );
    clearColorBuffer();
    clearDepthBuffer();
}

void Framebuffer::bindAndClearDepthOnly( const FrameBufferTextureValues* _values ) {
    bind( _values );
    clearDepthBuffer();
}

void Framebuffer::bindAndClearWithColor( const Color4f& clearColor, const FrameBufferTextureValues* _values ) {
    bind( _values );
    clearColorBufferWithColor( clearColor );
    clearDepthBuffer();
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
    GLCALL( glClearColor( clearColorValue.x(), clearColorValue.y(), clearColorValue.z(), clearColorValue.w() ));
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
    // ###WEBGL1###
    // Well we need to re-implement blitbuffer for ES2 :/

    // ### Artificially make it crash so we can debug it easier
#ifdef _WEBGL1

#else
    GLCALL( glBindFramebuffer( GL_READ_FRAMEBUFFER, source->Handle()));
    GLCALL( glReadBuffer( atthSource ));

    GLCALL( glBindFramebuffer( GL_DRAW_FRAMEBUFFER, dest->Handle()));
    if ( dest->RenderToTexture() ) {
        GLCALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                        dest->RenderToTexture()->getGlTextureImageTarget(),
                                        dest->RenderToTexture()->getHandle(), 0 ));
    }

    if ( dest->Handle() != 0 ) { // ### NDDado: If we are blitting to the default frame buffer without this if will
                                 // generate a log warning, to be checked
        GLCALL( glDrawBuffers(1, &atthDest ));
    }

    GLCALL( glBlitFramebuffer( 0, 0, source->getWidth(), source->getHeight(), 0, 0, dest->getWidth(), dest->getHeight(),
                               GL_COLOR_BUFFER_BIT, GL_LINEAR ));
#endif
}

void Framebuffer::blitWithRect( std::shared_ptr<Framebuffer> source,
                                std::shared_ptr<Framebuffer> dest,
                                GLenum atthSource,
                                GLenum atthDest,
                                Rect2f _sourceRect,
                                Rect2f _destRect ) {
#ifndef _WEBGL1
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
#endif
}

bool Framebuffer::isHDRSupported() {
    return Framebuffer::gbIsHDRSupported;
}