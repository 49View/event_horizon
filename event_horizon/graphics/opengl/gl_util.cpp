#include "gl_util.h"

const int SHADER_ERROR_BUFFER_SIZE = 1024;
const int SHADER_ERROR_BUFFER_COUNT = 16;

static char gShaderErrorBuffer[SHADER_ERROR_BUFFER_COUNT][SHADER_ERROR_BUFFER_SIZE];
static int gShaderErrorCount = 0;

void setTextureAligment( int i ) {
    GLCALL( glPixelStorei( GL_PACK_ALIGNMENT, i ));
    GLCALL( glPixelStorei( GL_UNPACK_ALIGNMENT, i ));
}

const char *getGlErrorString( GLint error ) {
    switch ( error ) {
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_NO_ERROR:
            return "GL_NO_ERROR";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
    }
    return "Unknown GL Error";
}

void shaderError( [[maybe_unused]] const char *error, [[maybe_unused]] int32_t title,
                  [[maybe_unused]] const char *details ) {
    if ( gShaderErrorCount < SHADER_ERROR_BUFFER_COUNT ) {
        memset( gShaderErrorBuffer[gShaderErrorCount], 0, SHADER_ERROR_BUFFER_SIZE );
        LOGI( "%s - %d - %s", error, title, details );
        ++gShaderErrorCount;
    }
}

void checkGlError( const char *op, int line, const char *file ) {
    GLint error = glGetError();
    if ( error != GL_NO_ERROR ) {
        LOGE( "OpenGL Error: %s - %s, (%s, %d)", op, getGlErrorString( error ), file, line );
        GLint currentFramebuffer = -1;
        GLint currentTexture = -1;
        GLint currentProgram = -1;
        glGetIntegerv( GL_FRAMEBUFFER_BINDING, &currentFramebuffer );
        glGetIntegerv( GL_TEXTURE_BINDING_2D, &currentTexture );
        glGetIntegerv( GL_CURRENT_PROGRAM, &currentProgram );
        LOGI( "Current framebuffer: %d, Current texture: %d, Current program: %d", currentFramebuffer, currentTexture,
              currentProgram );
        //		ASSERT(false);
    }
}

#ifdef CPP_DEBUG
void glPerfTimerStart() {
    glFlush();
    glFinish();
    perfTimerStart( 0 );
}

void glPerfTimerEnd( const char* name ) {
    glFlush();
    glFinish();
    perfTimerEnd( 0, name );
}
#endif

const char *glFormatToString( int format ) {
    if ( format == GL_ALPHA ) {
        return "GL_ALPHA";
    } else if ( format == GL_RGB ) {
        return "GL_RGB";
    } else if ( format == GL_RGBA ) {
        return "GL_RGBA";
    }
    return "Unknown";
}

const char *glPixelTypeToString( int pixelType ) {
    if ( pixelType == GL_UNSIGNED_BYTE ) {
        return "GL_UNSIGNED_BYTE";
    } else if ( pixelType == GL_UNSIGNED_SHORT_5_6_5 ) {
        return "GL_UNSIGNED_SHORT_5_6_5";
    } else if ( pixelType == GL_UNSIGNED_SHORT_4_4_4_4 ) {
        return "GL_UNSIGNED_SHORT_4_4_4_4";
    } else if ( pixelType == GL_UNSIGNED_SHORT_5_5_5_1 ) {
        return "GL_UNSIGNED_SHORT_5_5_5_1";
    } else if ( pixelType == GL_BYTE ) {
        return "GL_BYTE";
    } else if ( pixelType == GL_FLOAT ) {
        return "GL_FLOAT";
    }
    return "Unknown";
}

const char *glRenderBufferFormatToString( int renderBufferFormat ) {
    if ( renderBufferFormat == GL_RGBA4 ) {
        return "GL_RGBA4";
    } else if ( renderBufferFormat == GL_RGB565 ) {
        return "GL_RGB565";
    } else if ( renderBufferFormat == GL_RGB5_A1 ) {
        return "GL_RGB5_A1";
    } else if ( renderBufferFormat == GL_DEPTH_COMPONENT16 ) {
        return "GL_DEPTH_COMPONENT16";
    } else if ( renderBufferFormat == GL_STENCIL_INDEX8 ) {
        return "GL_STENCIL_INDEX8";
    } else if ( renderBufferFormat ==
                #if OPENGLES
                GL_RGB8_OES
                #else
                GL_RGB8
#endif
            ) {
        return "GL_RGB8";
    } else if ( renderBufferFormat ==
                #if OPENGLES
                GL_RGBA8_OES
                #else
                GL_RGBA8
#endif
            ) {
        return "GL_RGBA8";
    }
    return "Unknown";
}

GLenum pixelFormatToGlInternalFormat( PixelFormat pixelFormat ) {
    switch ( pixelFormat ) {
        case PIXEL_FORMAT_DEPTH_16:
            return GL_DEPTH_COMPONENT16;
        case PIXEL_FORMAT_DEPTH_24:
            return GL_DEPTH_COMPONENT24;
        case PIXEL_FORMAT_DEPTH_32:
            return GL_DEPTH_COMPONENT32F;
        case PIXEL_FORMAT_HDR_RGB_16:
            return GL_RGB16F;
        case PIXEL_FORMAT_HDR_RGBA_16:
            return GL_RGBA16F;
        case PIXEL_FORMAT_HDR_RGB_32:
            return GL_RGB32F;
        case PIXEL_FORMAT_HDR_RGBA_32:
            return GL_RGBA32F;
        case PIXEL_FORMAT_SRGB:
            return GL_SRGB8;
        case PIXEL_FORMAT_SRGBA:
            return GL_SRGB8_ALPHA8;
        case PIXEL_FORMAT_RGB:
            return GL_RGB8;
        case PIXEL_FORMAT_RGB565:
            return GL_RGB565;
        case PIXEL_FORMAT_RGBA:
        case PIXEL_FORMAT_BGRA:
            return GL_RGBA8;
        case PIXEL_FORMAT_RG:
            return GL_RG8;
        case PIXEL_FORMAT_HDR_RG_16:
            return GL_RG16F;
        case PIXEL_FORMAT_HDR_RG32:
            return GL_RG32F;
        case PIXEL_FORMAT_R:
            return GL_R8;
        case PIXEL_FORMAT_HDR_R16:
            return GL_R16F;
        case PIXEL_FORMAT_HDR_R32:
            return GL_R32F;
        case PIXEL_FORMAT_LUMINANCE:
#if OPENGLES
            return GL_LUMINANCE;
#else
            return GL_RED;
#endif
        case PIXEL_FORMAT_LUMINANCE_ALPHA:
#if OPENGLES
            return GL_LUMINANCE_ALPHA;
#else
            return GL_RG8;
#endif
            break;
        default:
            ASSERT( false );
            break;
    }
    return 0;
}

GLenum pixelFormatToGlFormat( PixelFormat pixelFormat ) {
    switch ( pixelFormat ) {
        case PIXEL_FORMAT_DEPTH_16:
            return GL_DEPTH_COMPONENT;
        case PIXEL_FORMAT_DEPTH_24:
            return GL_DEPTH_COMPONENT;
        case PIXEL_FORMAT_DEPTH_32:
            return GL_DEPTH_COMPONENT;
        case PIXEL_FORMAT_SRGB:
            return GL_SRGB8;
        case PIXEL_FORMAT_SRGBA:
            return GL_SRGB8_ALPHA8;
        case PIXEL_FORMAT_RGB:
        case PIXEL_FORMAT_HDR_RGB_32:
        case PIXEL_FORMAT_HDR_RGB_16:
        case PIXEL_FORMAT_RGB565:
            return GL_RGB;
        case PIXEL_FORMAT_RGBA:
        case PIXEL_FORMAT_HDR_RGBA_16:
        case PIXEL_FORMAT_HDR_RGBA_32:
            return GL_RGBA;
        case PIXEL_FORMAT_HDR_RG32:
        case PIXEL_FORMAT_HDR_RG_16:
        case PIXEL_FORMAT_RG:
            return GL_RG;
        case PIXEL_FORMAT_HDR_R32:
        case PIXEL_FORMAT_HDR_R16:
        case PIXEL_FORMAT_R:
            return GL_RED;
        case PIXEL_FORMAT_BGRA:
#ifdef GL_BGRA_EXT
            return GL_BGRA_EXT;
#else
            return GL_RGBA;
#endif
        case PIXEL_FORMAT_LUMINANCE:
#if OPENGLES
            return GL_LUMINANCE;
#else
            return GL_RED;
#endif
        case PIXEL_FORMAT_LUMINANCE_ALPHA:
#if OPENGLES
            return GL_LUMINANCE_ALPHA;
#else
            return GL_RG;
#endif
        default:
            ASSERT( false );
            break;
    }
    return 0;
}

GLenum pixelFormatToGlType( PixelFormat pixelFormat ) {
    switch ( pixelFormat ) {
        case PIXEL_FORMAT_DEPTH_16:
        case PIXEL_FORMAT_DEPTH_24:
        case PIXEL_FORMAT_DEPTH_32:
        case PIXEL_FORMAT_HDR_RGB_16:
        case PIXEL_FORMAT_HDR_RGBA_16:
        case PIXEL_FORMAT_HDR_RGB_32:
        case PIXEL_FORMAT_HDR_RGBA_32:
        case PIXEL_FORMAT_HDR_RG_16:
        case PIXEL_FORMAT_HDR_RG32:
        case PIXEL_FORMAT_HDR_R16:
        case PIXEL_FORMAT_HDR_R32:
            return GL_FLOAT;
        case PIXEL_FORMAT_SRGB:
        case PIXEL_FORMAT_SRGBA:
        case PIXEL_FORMAT_R:
        case PIXEL_FORMAT_RG:
        case PIXEL_FORMAT_RGB:
        case PIXEL_FORMAT_RGBA:
        case PIXEL_FORMAT_BGRA:
        case PIXEL_FORMAT_LUMINANCE:
        case PIXEL_FORMAT_LUMINANCE_ALPHA:
            return GL_UNSIGNED_BYTE;
        case PIXEL_FORMAT_RGB565:
            return GL_UNSIGNED_SHORT_5_6_5;
        default:
            ASSERT( false );
            break;
    }
    return 0;
}

GLenum filterToGl( Filter filter ) {
    switch ( filter ) {
        case FILTER_LINEAR:
            return GL_LINEAR;
        case FILTER_NEAREST:
            return GL_NEAREST;
        case FILTER_LINEAR_MIPMAP_LINEAR:
            return GL_LINEAR_MIPMAP_LINEAR;
    }
    ASSERT( false );
    return 0;
}

GLenum wrapModeToGl( WrapMode mode ) {
    switch ( mode ) {
        case WRAP_MODE_CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;
        case WRAP_MODE_REPEAT:
            return GL_REPEAT;
    }
    ASSERT( false );
    return 0;
}

GLenum targetToGl( TextureTargetMode mode ) {
    switch ( mode ) {
        case TEXTURE_2D:
            return GL_TEXTURE_2D;
        case TEXTURE_3D:
            return GL_TEXTURE_3D;
        case TEXTURE_CUBE_MAP:
            return GL_TEXTURE_CUBE_MAP;
    }
    ASSERT( false );
    return 0;
}

GLenum cullModeToGl( CullMode mode ) {
    switch ( mode ) {
        case CULL_BACK:
            return GL_BACK;
        case CULL_FRONT:
            return GL_FRONT;
        case CULL_FRONT_AND_BACK:
            return GL_FRONT_AND_BACK;
        default:
            break;
    }
    ASSERT( false );
    return 0;
}

GLenum depthFunctionToGl( DepthFunction mode ) {
    switch ( mode ) {
        case GREATER:
            return GL_GREATER;
            break;
        case GEQUAL:
            return GL_GEQUAL;
            break;
        case LESS:
            return GL_LESS;
            break;
        case LEQUAL:
            return GL_LEQUAL;
            break;
    }
    ASSERT( false );
    return 0;
}

void printGLString( const char *name, GLenum s ) {
    const char *v = (const char *) glGetString( s );
    LOGR( "GL %s = %s\n", name, v );
}

bool isExtensionAvailable( const char *extensionName ) {
    // This is a bit wrong, since we could detect a superset of this string
    return strstr((const char *) glGetString( GL_EXTENSIONS ), extensionName ) != NULL;
}

int getMaxTextureSize() {
    int maxTextureSize = 0;
    GLCALL( glGetIntegerv( GL_MAX_TEXTURE_SIZE, &maxTextureSize ));
    return maxTextureSize;
}

int getMaxRenderbufferSize() {
    int maxRenderBufferSize = 0;
    GLCALL( glGetIntegerv( GL_MAX_RENDERBUFFER_SIZE, &maxRenderBufferSize ));
    return maxRenderBufferSize;
}

bool get24BitRenderbuffersSupported() {
    return isExtensionAvailable( "GL_OES_rgb8_rgba8" );
}

const char *pixelFormatToString( PixelFormat pixelFormat ) {
    switch ( pixelFormat ) {
        case PIXEL_FORMAT_DEPTH_16:
            return "DEPTH_16";
        case PIXEL_FORMAT_DEPTH_24:
            return "DEPTH_24";
        case PIXEL_FORMAT_DEPTH_32:
            return "DEPTH_32";
        case PIXEL_FORMAT_HDR_RGB_16:
            return "HDR16";
        case PIXEL_FORMAT_HDR_RG_16:
            return "HDRRG16";
        case PIXEL_FORMAT_HDR_RG32:
            return "HDRRG32";
        case PIXEL_FORMAT_HDR_RGBA_16:
            return "HDRA16";
        case PIXEL_FORMAT_HDR_RGB_32:
            return "HDR32";
        case PIXEL_FORMAT_HDR_RGBA_32:
            return "HDRA32";
        case PIXEL_FORMAT_SRGB:
            return "SRGB";
        case PIXEL_FORMAT_SRGBA:
            return "SRGBA";
        case PIXEL_FORMAT_RGB:
            return "RGB";
        case PIXEL_FORMAT_RG:
            return "RG";
        case PIXEL_FORMAT_HDR_R32:
            return "HDRR32";
        case PIXEL_FORMAT_HDR_R16:
            return "HDRR16";
        case PIXEL_FORMAT_R:
            return "R";
        case PIXEL_FORMAT_RGBA:
            return "RGBA";
        case PIXEL_FORMAT_BGRA:
            return "BGRA";
        case PIXEL_FORMAT_RGB565:
            return "RGB565";
        case PIXEL_FORMAT_LUMINANCE:
            return "LUMINANCE";
        case PIXEL_FORMAT_LUMINANCE_ALPHA:
            return "LUMINANCE_ALPHA";
        case PIXEL_FORMAT_RGBA_QUADRANTS:
            return "PIXEL_FORMAT_RGBA_QUADRANTS";
        default:
            ASSERT( false );
            return "UNKNOWN";
    }
}

const char *wrapModeToString( WrapMode wrapMode ) {
    switch ( wrapMode ) {
        case WRAP_MODE_CLAMP_TO_EDGE:
            return "CLAMP_TO_EDGE";
        case WRAP_MODE_REPEAT:
            return "REPEAT";
    }
    ASSERT( false );
    return "UNKNOWN";
}

int getPixelFormatChannels( PixelFormat pixelFormat ) {
    switch ( pixelFormat ) {
        case PIXEL_FORMAT_RGB:
        case PIXEL_FORMAT_SRGB:
            return 3;
        case PIXEL_FORMAT_RGBA:
        case PIXEL_FORMAT_SRGBA:
        case PIXEL_FORMAT_BGRA:
            return 4;
        case PIXEL_FORMAT_LUMINANCE:
            return 1;
        case PIXEL_FORMAT_LUMINANCE_ALPHA:
            return 2;
        default:
            LOGE( "Unknown pixel format: %d", pixelFormat );
            ASSERT( false );
            break;
    }

    return 0;
}

GLenum nameToCubeMapSide( const std::string& name ) {
    std::string cubeSide = name.substr( name.find_last_of( '_' ) + 1,
                                        name.find_last_of( '.' ) - name.find_last_of( '_' ) - 1 );
    if ( cubeSide == "posx" || cubeSide == "right" ) return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    if ( cubeSide == "negx" || cubeSide == "left" ) return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
    if ( cubeSide == "posz" || cubeSide == "front" ) return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
    if ( cubeSide == "negz" || cubeSide == "back" ) return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
    if ( cubeSide == "posy" || cubeSide == "top" ) return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
    if ( cubeSide == "negy" || cubeSide == "bottom" ) return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;

    return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
}

UniformFormat GLToUniformFormat( GLenum uf ) {
    switch ( uf ) {
       case GL_FLOAT:
           return UF_FLOAT;
       case GL_BOOL:
           return UF_BOOL;
       case GL_INT:
           return UF_INT;
       case GL_FLOAT_VEC2:
           return UF_VEC2F;
       case GL_FLOAT_VEC3:
           return UF_VEC3F;
       case GL_FLOAT_VEC4:
           return UF_VEC4F;
       case GL_INT_VEC2:
           return UF_VEC2I;
       case GL_INT_VEC3:
           return UF_VEC3I;
       case GL_INT_VEC4:
           return UF_VEC4I;
       case GL_BOOL_VEC2:
           return UF_VEC2B;
       case GL_BOOL_VEC3:
           return UF_VEC3B;
       case GL_BOOL_VEC4:
           return UF_VEC4B;
       case GL_FLOAT_MAT2:
           return UF_MAT2F;
       case GL_FLOAT_MAT3:
           return UF_MAT3F;
       case GL_FLOAT_MAT4:
           return UF_MAT4F;
       case GL_SAMPLER_2D:
           return UF_SAMPLER_2D;
       case GL_SAMPLER_CUBE:
           return UF_SAMPLER_CUBE;
        case GL_SAMPLER_2D_SHADOW:
           return UF_SAMPLER_2D_SHADOW;
    };
    ASSERTV(0, "Unknown uniform mapping %d", uf );
    return UF_FLOAT;
}