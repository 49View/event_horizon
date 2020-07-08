#include "texture_opengl.h"

#include "gl_util.h"

// These are default values for mapping, they can be changed by the client app
// Defaults are 4 pixels per millimeter (4000)
Vector2f Texture::TexturePixelCMScale = Vector2f( 4000.0f, 4000.0f );
Vector2f Texture::TexturePixelCMScaleInv = reciprocal( Texture::TexturePixelCMScale );

void Texture::init_data_r( const uint8_t* _data ) {
    bool updateStorage = true;

    if ( mMultisample ) {
        GLCALL(texImage2DMultisample(4, glInternalFormat, mWidth, mHeight ));
    } else {
        if ( mGenerateMipMaps ) {
            // Always use trilinear filtering
            GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR ) );
            GLuint num_mipmaps = 1 + static_cast<GLuint>( floor( log( (float)max( mWidth, mHeight ) ) ) );
            // if it's a ui texture or is a framebuffer target do not generate mipmaps
            if ( updateStorage ) {
#ifndef _WEBGL1
                GLCALL( glTexStorage2D( glTextureTarget, num_mipmaps, glInternalFormat, mWidth, mHeight ));
#else
                auto lWidth = mWidth;
                auto lHeight = mHeight;
    for (auto i = 0; i < num_mipmaps; i++)
    {
        GLCALL( glTexImage2D(glTextureTarget, i, glInternalFormat, lWidth, lHeight, 0, glFormat, glType, NULL));
        lWidth = max(1, (lWidth / 2));
        lHeight = max(1, (lHeight / 2));
    }
#endif
            }
            if ( _data ) {
                GLCALL( glTexSubImage2D( glTextureImageTarget, 0, 0, 0, mWidth, mHeight, glFormat, glType,
                                         _data ) );
            }
        } else {
            if ( glTextureImageTarget == GL_TEXTURE_3D ) {
#ifndef _WEBGL1
                GLCALL( glTexImage3D( glTextureImageTarget, 0, glInternalFormat, mWidth, mHeight, mDepth, 0, glFormat, glType,
                                      _data));
#endif
            } else {
#ifdef _WEBGL1
                if ( glInternalFormat == GL_DEPTH_COMPONENT ) {
                // ###WEBGL1### it should be glType = GL_UNSIGNED_INT_24_8 but for some reason it's not working
                    glType = GL_UNSIGNED_INT;
                }
                // ###WEBGL1### GL_LUMINANCE_ALPHA doesn't allow floating point values, to be checked why
                if ( glInternalFormat == GL_LUMINANCE_ALPHA ) {
                    glType = GL_UNSIGNED_BYTE;
                    glInternalFormat = GL_RGB;
                    glFormat  = GL_RGB;
                }

#endif
                LOGRS("glTextureImageTarget: " << glEnumToString(glTextureImageTarget));
                LOGRS("glInternalFormat: " << glEnumToString(glInternalFormat));
                LOGRS("glFormat: " << glEnumToString(glFormat));
                LOGRS("glType: " << glEnumToString(glType));
                LOGRS("glType: " << glType);
                GLCALL( glTexImage2D( glTextureImageTarget, 0, glInternalFormat, mWidth, mHeight, 0, glFormat, glType, _data));
            }
        }
    }
}

void Texture::init_null() {
    GLCALL( glGenTextures( 1, &mHandle ));
    GLCALL( glBindTexture(GL_TEXTURE_2D, mHandle) );
//    float f = 1.0f;
//    GLCALL( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, &f) );
    unsigned char emissive[] = { 255, 255, 255, 255 };
    GLCALL( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, emissive) );

}

void Texture::init_r( const uint8_t* _data ) {
    //	ASSERT( mWrapMode == WRAP_MODE_CLAMP_TO_EDGE || (JMATH::isPowerOfTwo(mWidth) && JMATH::isPowerOfTwo(mHeight)));
    //	mWrapMode = (JMATH::isPowerOfTwo(mWidth) && JMATH::isPowerOfTwo(mHeight)) ? WRAP_MODE_REPEAT : WRAP_MODE_CLAMP_TO_EDGE;

    if ( mHandle != 0) return; // This texture has already been initialized

    glInternalFormat     = pixelFormatToGlInternalFormat( mFormat );
    glFormat             = pixelFormatToGlFormat( mFormat );
    glType               = pixelFormatToGlType( mFormat );
    glFilter             = filterToGl( mFilter );
    glTextureTarget      = targetToGl( mTarget );
    glTextureImageTarget = imageTargetToGl( mTarget, mMultisample );
    glWrapMode           = wrapModeToGl( mWrapMode );

    // Make sure we are not recreating the handle every time
    GLCALL( glGenTextures( 1, &mHandle ));
    LOGRS("Binding texture " << Name() << " with handle " << mHandle << " to imageTarget " << glEnumToString(glTextureImageTarget) )
    GLCALL( glBindTexture( glTextureImageTarget, mHandle ));
#ifndef _WEBGL1
    GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_WRAP_R, glWrapMode ));
#endif
    GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_WRAP_S, glWrapMode ));
    GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_WRAP_T, glWrapMode ));
    GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_MAG_FILTER, glFilter ));
    GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_MIN_FILTER, glFilter ));

    LOGR( "Initialising texture %s %dx%d (%s) handle=%d", pixelFormatToString( mFormat ), mWidth, mHeight,
          Name().c_str(), mHandle );

    mGenerateMipMaps &= _data && isPowerOfTwo(getWidth()) && isPowerOfTwo(getHeight());
    init_data_r( _data );

    if ( mGenerateMipMaps ) {
        GLCALL( glGenerateMipmap( glTextureTarget ));
    }
}

void Texture::init_cubemap_r() {
    if ( mHandle != 0) return; // This texture has already been initialized

    glInternalFormat     = pixelFormatToGlInternalFormat( mFormat );
    glFormat             = pixelFormatToGlFormat( mFormat );
    glType               = pixelFormatToGlType( mFormat );
    glFilter             = filterToGl( mFilter );
    glWrapMode           = wrapModeToGl( mWrapMode );
    glTextureTarget      = targetToGl( mTarget );
    glTextureImageTarget = imageTargetToGl( mTarget, mMultisample );

    // Make sure we are not recreating the handle every time
    GLCALL( glGenTextures( 1, &mHandle ));
    GLCALL( glBindTexture( glTextureImageTarget, mHandle ));

#ifndef _WEBGL1
    GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_WRAP_R, glWrapMode ));
#endif
    GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_WRAP_S, glWrapMode ));
    GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_WRAP_T, glWrapMode ));
    GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_MAG_FILTER, glFilter ));

    auto minFilter = mGenerateMipMaps ? GL_LINEAR_MIPMAP_LINEAR : glFilter;
    GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_MIN_FILTER, minFilter ));

//    GLuint mips = mGenerateMipMaps ? 1 + static_cast<GLuint>( floor( log( (float)max( mWidth, mHeight ) ) ) ) : 1;

//    GLCALL( glTexParameteri(glTextureTarget, GL_TEXTURE_BASE_LEVEL, 0) );
//    GLCALL( glTexParameteri(glTextureTarget, GL_TEXTURE_MAX_LEVEL, mips) );

    for (unsigned int i = 0; i < 6; ++i)
    {
//        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, glInternalFormat, mWidth, mHeight, 0, glFormat, glType, nullptr);
        GLCALL( glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_FLOAT, nullptr));
    }

//    GLCALL( glTexStorage2D( glTextureTarget, mips, glInternalFormat, mWidth, mHeight ));

    if ( mGenerateMipMaps ) {
        GLCALL( glGenerateMipmap( glTextureTarget ));
    }

    LOGR( "Initialising texture cubemap %s %dx%d (%s) handle=%d", pixelFormatToString( mFormat ), mWidth, mHeight,
          Name().c_str(), mHandle );
}

//void Texture::init( int width, int height, PixelFormat format, Filter filter, WrapMode wrapMode ) {
//	ASSERTV( width > 0, "Error initialising texture: %s", mId.c_str() );
//	ASSERTV( height > 0, "Error initialising texture: %s", mId.c_str() );
//
//	mFormat = format;
//	mWidth = width;
//	mHeight = height;
//	mFilter = filter;
//	mWrapMode = wrapMode;
//
//	// Just being paranoid here, always make sure this is greater than 0, even in release
//	if ( mWidth < 1 ) {
//		mWidth = 1;
//	}
//	if ( mHeight < 1 ) {
//		mHeight = 1;
//	}
//
//	init_r();
//}

void Texture::reallocateForRenderTarget( const uint8_t* _data ) {
    GLenum glInternalFormat = pixelFormatToGlInternalFormat( mFormat );
    GLenum glFormat = pixelFormatToGlFormat( mFormat );
    GLenum glType = PIXEL_FORMAT_INVALID;
    if ( mFormat == PIXEL_FORMAT_RGB ) {
        glType = GL_UNSIGNED_SHORT_5_6_5;
    } else if ( mFormat == PIXEL_FORMAT_RGBA ) {
        glType = GL_UNSIGNED_SHORT_4_4_4_4;
    } else {
        ASSERT( false );
    }

    GLCALL( glActiveTexture( GL_TEXTURE0 ));
    GLCALL( glBindTexture( targetToGl( mTarget ), mHandle ));
    GLCALL( glTexImage2D( GL_TEXTURE_2D, 0, glInternalFormat, mWidth, mHeight, 0, glFormat, glType, _data ));
    GLCALL( glBindTexture( targetToGl( mTarget ), 0 ));
}

void Texture::refresh( const uint8_t *data ) {
    refresh( data, 0, 0, mWidth, mHeight );
}

void Texture::refresh( const uint8_t *data, int x, int y, int width, int height ) {
    GLCALL( glBindTexture( glTextureImageTarget, mHandle ));
    GLCALL( glTexSubImage2D( glTextureImageTarget, 0, x, y, width, height, glFormat, glType, data ));
}

void Texture::refreshFromFramebuffer( int width, int height ) {
    GLCALL( glActiveTexture( GL_TEXTURE0 ));
    GLCALL( glBindTexture( targetToGl( mTarget ), mHandle ));
    GLCALL( glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height ));
}

void Texture::bind( int textureUnit ) const {
    GLCALL( glActiveTexture( GL_TEXTURE0 + textureUnit ));
    GLCALL( glBindTexture( targetToGl( mTarget ), mHandle ));
}

void Texture::bind( int textureUnit, unsigned int _handle, const char* _name ) const {
    GLCALL( glUseProgram( _handle) );
    GLint location = -1;
    GLCALLRET( location, glGetUniformLocation( _handle, _name ) );
    if ( location == -1 )  return;
    GLCALL( glActiveTexture( GL_TEXTURE0 + textureUnit ));
    GLCALL( glBindTexture( targetToGl( mTarget ), mHandle ));
    GLCALL( glUniform1i( location, textureUnit ) );
}

void Texture::setWrapMode( const WrapMode wm ) {
    mWrapMode = wm;
    //	GLenum glTextureTarget = mMultisample ? GL_TEXTURE_2D_MULTISAMPLE : targetToGl( mTarget );
    //	GLCALL( glBindTexture( glTextureTarget, mHandle ) );
    //	GLenum glWrapMode = wrapModeToGl( mWrapMode );
    //	GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_WRAP_R, glWrapMode ) );
    //	GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_WRAP_S, glWrapMode ) );
    //	GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_WRAP_T, glWrapMode ) );
}

int Texture::getMaxSize() {
    int size = 0;
    GLCALL( glGetIntegerv( GL_MAX_TEXTURE_SIZE, &size ));
    return size;
}

void Texture::release() {
    if ( mHandle != 0 ) {
        GLCALL( glDeleteTextures( 1, &mHandle ));
        mHandle = 0;
    }
}

std::string Texture::getGlTextureImageTargetString() const {
    return glEnumToString( glTextureImageTarget );
}
