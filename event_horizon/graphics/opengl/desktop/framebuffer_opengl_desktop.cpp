#include "../framebuffer_opengl.h"

void Framebuffer::checkFrameBufferStatus() {
    GLenum fbs = glCheckFramebufferStatus( GL_FRAMEBUFFER );
    switch ( fbs ) {
        case GL_FRAMEBUFFER_UNDEFINED:
            LOGE( "Frame Buffer creation error code: GL_FRAMEBUFFER_UNDEFINED" );
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            LOGE( "Frame Buffer creation error code: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT" );
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            LOGE( "Frame Buffer creation error code: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" );
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            LOGE( "Frame Buffer creation error code: GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" );
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            LOGE( "Frame Buffer creation error code: GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" );
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            LOGE( "Frame Buffer creation error code: GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS" );
            break;

        case GL_FRAMEBUFFER_UNSUPPORTED:
            LOGE( "Frame Buffer creation error code: GL_FRAMEBUFFER_UNSUPPORTED" );
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            LOGE( "Frame Buffer creation error code: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE" );
            break;

        default:
            break;
    }
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
