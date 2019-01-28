#pragma once

#include <string>
#include <map>
#include <cstdint>
#include <memory>

#include "../texture.h"
#include "gl_headers.hpp"
#include "core/math/vector2f.h"
#include "core/util.h"
#include "core/htypes_shared.hpp"
#include "../graphic_constants.h"

//====================================
// Texture
//====================================

class Texture {
public:
    Texture() {
    }

    Texture( TextureRenderData& tb ) {
        ctor( tb );
    }

    void ctor( TextureRenderData& tb ) {
        mId = tb.Name();
        if (tb.forceGPUId != -1) {
            mHandle = static_cast<GLuint>( tb.forceGPUId );
        }
        mFormat = tb.outFormat;
        mFilter = tb.filterMode;
        mWrapMode = tb.wrapMode;
        mTarget = tb.ttm;
        mWidth = tb.width;
        mHeight = tb.height;
        mMultisample = tb.multisample;
#ifdef _OPENGL_ES
        mMultisample = false;
#endif
        mHDR = tb.HDR;
        mIsFramebufferTarget = tb.isFramebufferTarget;
        mGenerateMipMaps = tb.generateMipMaps;
        mGlTextureSlot = tb.preferredGPUSlot;

    }
    void clear() {
        mHandle = 0;
    }

    bool isInited() const {
        return mHandle != 0;
    }

    // OpenGL initialisation
    void init_r( const uint8_t* _data );

    void refresh( const uint8_t *data );
    void refresh( const uint8_t *data, int x, int y, int width, int height );
    void refreshFromFramebuffer( int width, int height );
    void bind( int textureUnit ) const;
    void bind( int textureUnit, unsigned int _handle, const char* _name ) const;

    static int getMaxSize();

    const std::string getId() const {
        return mId;
    }

    std::string name() const {
        return mId;
    }

    GLuint getHandle() const {
        return mHandle;
    }

    TextureUniformDesc TDI( unsigned int _slot ) const {
        return { mHandle, _slot, mTarget };
    };

    TextureTargetMode getTarget() const {
        return mTarget;
    }

    void setHandle( GLuint h ) {
        mHandle = h;
    }

    GLuint textureSlot() const {
        return mGlTextureSlot;
    }

    void textureSlot( GLuint _s )  {
        mGlTextureSlot = _s;
    }

    PixelFormat getFormat() const {
        return mFormat;
    }

    Vector2f getSizef() const {
        return { mWidth, mHeight };
    }

    Vector2i getSizei() const {
        return { mWidth, mHeight };
    }

    Vector2f getPixelCMSize() const {
        return TexturePixelCMScaleInv * Vector2f( mWidth, mHeight );
    }

    int getWidth() const {
        return mWidth;
    }

    int getHeight() const {
        return mHeight;
    }

    float getWidthf() const {
        return static_cast<float>( mWidth );
    }

    float getHeightf() const {
        return static_cast<float>( mHeight );
    }

    float getAspectRatio() const {
        ASSERT( mHeight > 0 );
        return static_cast<float>( mWidth ) / static_cast<float>( mHeight );
    }

    float getMaxAspectRatio() const {
        ASSERT( mHeight > 0 );
        ASSERT( mWidth > 0 );
        if ( mWidth > mHeight ) {
            return static_cast<float>( mWidth ) / static_cast<float>( mHeight );
        }
        return static_cast<float>( mHeight ) / static_cast<float>( mWidth );
    }

    bool hasAlpha() const {
        return mFormat == PIXEL_FORMAT_RGBA || mFormat == PIXEL_FORMAT_LUMINANCE_ALPHA ||
               mFormat == PIXEL_FORMAT_RGBA_QUADRANTS;
    }

    Vector2f getAspectRatioVector() const {
        ASSERT( mHeight > 0 );
        if ( mWidth < mHeight ) {
            return Vector2f( 1.0f, static_cast<float>( mWidth ) / static_cast<float>( mHeight ));
        }
        return Vector2f( static_cast<float>( mWidth ) / static_cast<float>( mHeight ), 1.0f );
    }

    Vector2f getAspectRatioVectorWidthOverHeight() const {
        ASSERT( mHeight > 0 );
        return Vector2f( static_cast<float>( mWidth ) / static_cast<float>( mHeight ), 1.0f );
    }

    Vector2f getAspectRatioVectorHeightOverWidth() const {
        ASSERT( mWidth > 0 );
        return Vector2f( static_cast<float>( mHeight ) / static_cast<float>( mWidth ), 1.0f );
    }

    Vector2f getAspectRatioVectorYWidthOverHeight() const {
        ASSERT( mHeight > 0 );
        return Vector2f( 1.0f, static_cast<float>( mWidth ) / static_cast<float>( mHeight ));
    }

    Vector2f getAspectRatioVectorYHeightOverWidth() const {
        ASSERT( mWidth > 0 );
        return Vector2f( 1.0f, static_cast<float>( mHeight ) / static_cast<float>( mWidth ));
    }

    Vector2f getAspectRatioFillVector( float rectRatio ) const {
        ASSERT( mHeight > 0 );
        ASSERT( mWidth > 0 );
        if ( mWidth > mHeight ) {
            return Vector2f( 1.0f, ( static_cast<float>( mWidth ) / static_cast<float>( mHeight )) * rectRatio );
        }
        return Vector2f(( static_cast<float>( mHeight ) / static_cast<float>( mWidth )) * rectRatio, 1.0f );
    }

    Vector2f getAspectRatioFillOffset() const {
        ASSERT( mHeight > 0 );
        ASSERT( mWidth > 0 );
        if ( mWidth > mHeight ) {
            return -Vector2f((( static_cast<float>( mHeight ) / static_cast<float>( mWidth )) * 0.5f ) - 0.5f, 0.0f );
        }
        return -Vector2f( 0.0f, (( static_cast<float>( mWidth ) / static_cast<float>( mHeight )) * 0.5f ) - 0.5f );
    }

    void setId( const std::string& id ) {
        mId = id;
    }

    void release();
    void reallocateForRenderTarget( const uint8_t* _data );

    bool Multisample() const { return mMultisample; }

    void Multisample( bool val ) { mMultisample = val; }

    void isFramebufferTarget() { mIsFramebufferTarget = true; }
    void generateMipMaps( const bool _val = true ) { mGenerateMipMaps = _val; }

    void setWrapMode( const WrapMode wm );

    GLenum getGlInternalFormat() const {
        return glInternalFormat;
    }

    GLenum getGlFormat() const {
        return glFormat;
    }

    GLenum getGlType() const {
        return glType;
    }

    GLenum getGlFilter() const {
        return glFilter;
    }

    GLenum getGlWrapMode() const {
        return glWrapMode;
    }

    GLenum getGlTextureTarget() const {
        return glTextureTarget;
    }

    GLenum getGlTextureImageTarget() const {
        return glTextureImageTarget;
    }

    Filter getFilter() const {
        return mFilter;
    }

    void setFilter( Filter filter ) {
        mFilter = filter;
    }

private:
    void init_data_r( const uint8_t* _data );

private:
    GLuint mHandle = 0;
    GLenum glInternalFormat = GL_RGB;
    GLenum glFormat = GL_RGB;
    GLenum glType = GL_UNSIGNED_BYTE;
    GLenum glFilter = GL_LINEAR;
    GLenum glWrapMode = GL_REPEAT;
    GLenum glTextureTarget = GL_TEXTURE_2D;
    GLenum glTextureImageTarget = GL_TEXTURE_2D;
    GLuint mGlTextureSlot = 0;

    std::string mId = "undefined";
    PixelFormat mFormat = PIXEL_FORMAT_RGB;
    Filter mFilter = FILTER_LINEAR;
    WrapMode mWrapMode = WRAP_MODE_REPEAT;
    TextureTargetMode mTarget = TEXTURE_2D;
    int mWidth = 0;
    int mHeight = 0;
    bool mMultisample = false;
    bool mHDR = false;
    bool mIsFramebufferTarget = false;
    bool mGenerateMipMaps = true;

    static std::map<std::string, std::pair<int, int>> sCubeMapIndices;

public:
    static Vector2f TexturePixelCMScale;
    static Vector2f TexturePixelCMScaleInv;
};

void texImage2DMultisample( GLsizei samples, GLint glInternalFormat, GLsizei w, GLsizei h);
