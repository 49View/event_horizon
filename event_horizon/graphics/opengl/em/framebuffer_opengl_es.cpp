#include "../framebuffer_opengl.h"

#include "../gl_util.h"
#include "../../graphic_functions.hpp"

void checkFrameBufferStatus() {
    GLenum fbs = glCheckFramebufferStatus( GL_FRAMEBUFFER );
    switch ( fbs ) {
        case GL_FRAMEBUFFER_UNDEFINED:
            LOGR( "Frame Buffer creation error code: GL_FRAMEBUFFER_UNDEFINED" );
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            LOGR( "Frame Buffer creation error code: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" );
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            LOGR( "Frame Buffer creation error code: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" );
            break;

        case GL_FRAMEBUFFER_UNSUPPORTED:
            LOGR( "Frame Buffer creation error code: GL_FRAMEBUFFER_UNSUPPORTED" );
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            LOGR( "Frame Buffer creation error code: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" );
            break;

        case GL_FRAMEBUFFER_COMPLETE:
            break;

        default:
            LOGR( "Frame Buffer creation error code (not handled)" );
            break;
    }
//    ASSERT( fbs == GL_FRAMEBUFFER_COMPLETE );
}

GLenum Framebuffer::framebufferTextureTarget( bool _multisampled ) {
    return GL_TEXTURE_2D;
}

void Framebuffer::framebufferTexture2D( GLuint rth, const std::string& renderTargetIndex, [[maybe_unused]] int mipMapIndex ) {
    if ( mCubeMap ) {
        GLCALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                        nameToCubeMapSide( renderTargetIndex ),
                                        rth, 0 ));
    } else {
        GLCALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                        framebufferTextureTarget( mMultisample ),
                                        rth, 0 ));
    }
}

void Framebuffer::clearDepthBuffer( const float _clearDepthValue ) {
    GLCALL( glClearDepthf( _clearDepthValue ));
    GLCALL( glClear( GL_DEPTH_BUFFER_BIT ));
}

void Framebuffer::setSRGB( bool value ) {
    return;
//    if ( value ) {
//        glEnable( GL_FRAMEBUFFER_SRGB );
//    } else {
//        glDisable( GL_FRAMEBUFFER_SRGB );
//    }
}
