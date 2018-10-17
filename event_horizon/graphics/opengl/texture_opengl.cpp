#include "texture_opengl.h"

#include "gl_util.h"


std::map<std::string, std::pair<int, int>> Texture::sCubeMapIndices;
// These are default values for mapping, they can be changed by the client app
// Defaults are 4 pixels per millimeter (4000)
Vector2f Texture::TexturePixelCMScale = Vector2f( 4000.0f, 4000.0f );
Vector2f Texture::TexturePixelCMScaleInv = reciprocal( Texture::TexturePixelCMScale );

void Texture::init_data_r() {
    bool updateStorage = true;
    if ( mTarget == TEXTURE_CUBE_MAP ) {
        if ( sCubeMapIndices[cubeMapTName( mId )].second > 1 ) {
            updateStorage = false;
        }
        glTextureImageTarget = nameToCubeMapSide(mId);
    }

    if ( mMultisample ) {
        texImage2DMultisample(4, glInternalFormat, mWidth, mHeight );
    } else {
        if ( mGenerateMipMaps ) {
            // Always use trilinear filtering
            GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR ) );
            GLuint num_mipmaps = 1 + static_cast<GLuint>( floor( log( (float)max( mWidth, mHeight ) ) ) );
            // if it's a ui texture or is a framebuffer target do not generate mipmaps
            if ( updateStorage ) {
                GLCALL( glTexStorage2D( glTextureTarget, num_mipmaps, glInternalFormat, mWidth, mHeight ));
            }
            auto ptr = mData.get();
            if ( ptr != nullptr ) {
                GLCALL( glTexSubImage2D( glTextureImageTarget, 0, 0, 0, mWidth, mHeight, glFormat, glType,
                                         mData.get() ) );
            }
        } else {
            GLCALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0) );
            GLCALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0) );
            GLCALL( glTexImage2D( glTextureImageTarget, 0, glInternalFormat, mWidth, mHeight, 0, glFormat, glType,
                                  mData.get()));
        }
    }
}

void Texture::init_r() {
    //	ASSERT( mWrapMode == WRAP_MODE_CLAMP_TO_EDGE || (JMATH::isPowerOfTwo(mWidth) && JMATH::isPowerOfTwo(mHeight)));
    //	mWrapMode = (JMATH::isPowerOfTwo(mWidth) && JMATH::isPowerOfTwo(mHeight)) ? WRAP_MODE_REPEAT : WRAP_MODE_CLAMP_TO_EDGE;

    if ( mHandle != 0) return; // This texture has already been initialized

    glInternalFormat     = pixelFormatToGlInternalFormat( mFormat );
    glFormat             = pixelFormatToGlFormat( mFormat );
    glType               = pixelFormatToGlType( mFormat );
    glFilter             = filterToGl( mFilter );
    glWrapMode           = wrapModeToGl( mWrapMode );
    glTextureTarget      = targetToGl( mTarget );
    glTextureImageTarget = imageTargetToGl( mTarget, mMultisample );

    // Make sure we are not recreating the handle every time
    if ( mTarget != TEXTURE_CUBE_MAP ) {
        GLCALL( glGenTextures( 1, &mHandle ));
    } else {
        std::string cubemapName = cubeMapTName( mId );
        if ( auto it = sCubeMapIndices.find(cubemapName); it == sCubeMapIndices.end() ) {
            GLCALL( glGenTextures( 1, &mHandle ));
            sCubeMapIndices[cubemapName] = {mHandle, 1};
        } else {
            mHandle = it->second.first;
            sCubeMapIndices[cubemapName] = {it->second.first, it->second.second+1};
        }
    }
    GLCALL( glBindTexture( glTextureImageTarget, mHandle ));

    GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_WRAP_R, glWrapMode ));
    GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_WRAP_S, glWrapMode ));
    GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_WRAP_T, glWrapMode ));
    GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_MAG_FILTER, glFilter ));
    GLCALL( glTexParameteri( glTextureTarget, GL_TEXTURE_MIN_FILTER, glFilter ));

    LOGI( "Initialising texture %s %dx%d (%s) handle=%d", pixelFormatToString( mFormat ), mWidth, mHeight,
          mId.c_str(), mHandle );

    init_data_r();

    if ( mGenerateMipMaps ) {
        bool bFinalizeCubeMap = true;
        if ( mTarget == TEXTURE_CUBE_MAP ) {
           if ( sCubeMapIndices[cubeMapTName( mId )].second != 6 ) {
               bFinalizeCubeMap = false;
           }
        }
        if ( bFinalizeCubeMap ) {
            GLCALL( glGenerateMipmap( glTextureTarget ));
        }  //Generate num_mipmaps number of
        // mipmaps here.
    }
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

void Texture::reallocateForRenderTarget() {
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
    GLCALL( glTexImage2D( GL_TEXTURE_2D, 0, glInternalFormat, mWidth, mHeight, 0, glFormat, glType, mData.get() ));
    GLCALL( glBindTexture( targetToGl( mTarget ), 0 ));
}

void Texture::refresh( const uint8_t *data ) {
    refresh( data, 0, 0, mWidth, mHeight );
}

void Texture::refresh( const uint8_t *data, int x, int y, int width, int height ) {
    GLenum glFormat = pixelFormatToGlFormat( mFormat );
    GLenum glType = pixelFormatToGlType( mFormat );
    GLCALL( glActiveTexture( GL_TEXTURE0 ));
    GLCALL( glBindTexture( targetToGl( mTarget ), mHandle ));
    GLCALL( glTexSubImage2D( GL_TEXTURE_2D, 0, x, y, width, height, glFormat, glType, data ));
    GLCALL( glBindTexture( targetToGl( mTarget ), 0 ));
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
