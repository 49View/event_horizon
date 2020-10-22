#include "../framebuffer_opengl.h"


JMATH::Rect2f getPlatformWindowRect() {
//            JMATH::Rect2f r = Framebuffer::getCurrentViewport();
    return getScreenRect;
}

void Framebuffer::checkFrameBufferStatus() {
    GLenum fbs = glCheckFramebufferStatus( GL_FRAMEBUFFER );
    if ( fbs == GL_FRAMEBUFFER_COMPLETE ) return;

    std::string errMessage{};
    switch ( fbs ) {
        case GL_FRAMEBUFFER_UNDEFINED:
            errMessage = "Frame Buffer creation error code: GL_FRAMEBUFFER_UNDEFINED";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            errMessage = "Frame Buffer creation error code: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            errMessage = "Frame Buffer creation error code: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            errMessage = "Frame Buffer creation error code: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            errMessage = "Frame Buffer creation error code: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            errMessage = "Frame Buffer creation error code: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
            break;

        case GL_FRAMEBUFFER_UNSUPPORTED:
            errMessage = "Frame Buffer creation error code: GL_FRAMEBUFFER_UNSUPPORTED";
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            errMessage = "Frame Buffer creation error code: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            break;

        default:
            errMessage = "Frame Buffer creation error code: UNKNOWN (You should be very scared about this";
            break;
    }

    LOGRS( "[ERROR] Framebuffer " << mName << " size: [" << mWidth << ":" << mHeight << "] format : ["
                          << glEnumToString( pixelFormatToGlInternalFormat(mFormat) ) << "] "
                          << "Texture Slot: [" << mTextureGPUSlot << "] " << std::endl << errMessage )

    ASSERT( fbs == GL_FRAMEBUFFER_COMPLETE );
}

GLenum Framebuffer::framebufferTextureTarget( bool _multisampled ) {
    return _multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

void Framebuffer::framebufferTexture2D( GLuint rth, const std::string& renderTargetIndex, int mipMapIndex ) {
    if ( mCubeMap ) {
        GLCALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                        nameToCubeMapSide( renderTargetIndex ),
                                        rth, mipMapIndex ));
    } else {
        GLCALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                        framebufferTextureTarget( mMultisample ),
                                        rth, mipMapIndex ));
    }
}

void Framebuffer::clearDepthBuffer( const float _clearDepthValue ) {
    GLCALL( glClearDepth( _clearDepthValue ));
    GLCALL( glClear( GL_DEPTH_BUFFER_BIT ));
}

void Framebuffer::setSRGB( bool value ) {
    if ( value ) {
        glEnable( GL_FRAMEBUFFER_SRGB );
    } else {
        glDisable( GL_FRAMEBUFFER_SRGB );
    }
}

bool Framebuffer::checkHDRSupport() {
    GLuint fbh;
    GLuint fbt;

    GLCALL( glGenTextures(1, &fbt) );
    GLCALL( glBindTexture(GL_TEXTURE_2D, fbt) );
    GLCALL( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGBA, GL_FLOAT, nullptr) );

    GLCALL( glGenFramebuffers( 1, &fbh ));
    GLCALL( glBindFramebuffer(GL_FRAMEBUFFER, fbh) );
    GLCALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                    GL_TEXTURE_2D,
                                    fbt, 0 ));

    GLenum fbs = glCheckFramebufferStatus( GL_FRAMEBUFFER );
    gbIsHDRSupported = fbs == GL_FRAMEBUFFER_COMPLETE;

    GLCALL( glDeleteTextures(1, &fbt) );
    GLCALL( glDeleteFramebuffers(1, &fbh) );

    return gbIsHDRSupported;
}
