//
// Created by Dado on 11/02/2018.
//

#pragma once

#include <core/math/vector2f.h>

enum PixelFormat {
    PIXEL_FORMAT_UNFORCED = -1,
    PIXEL_FORMAT_RGB = 0,
    PIXEL_FORMAT_RGBA,
    PIXEL_FORMAT_LUMINANCE,
    PIXEL_FORMAT_LUMINANCE_ALPHA,
    PIXEL_FORMAT_BGRA,
    PIXEL_FORMAT_RGB565,
    PIXEL_FORMAT_RGBA_QUADRANTS,
    PIXEL_FORMAT_SRGB,
    PIXEL_FORMAT_SRGBA,
    PIXEL_FORMAT_HDR_RGB_16,
    PIXEL_FORMAT_HDR_RGBA_16,
    PIXEL_FORMAT_HDR_RGB_32,
    PIXEL_FORMAT_HDR_RGBA_32,
    PIXEL_FORMAT_DEPTH_16,
    PIXEL_FORMAT_DEPTH_24,
    PIXEL_FORMAT_DEPTH_32,
    PIXEL_FORMAT_RG,
    PIXEL_FORMAT_HDR_RG_16,
    PIXEL_FORMAT_HDR_RG_32,
    PIXEL_FORMAT_R,
    PIXEL_FORMAT_HDR_R16,
    PIXEL_FORMAT_HDR_RG32,
    PIXEL_FORMAT_HDR_R32,
    PIXEL_FORMAT_INVALID
};

enum Filter {
    FILTER_LINEAR,
    FILTER_NEAREST,
    FILTER_LINEAR_MIPMAP_LINEAR
};

enum WrapMode {
    WRAP_MODE_CLAMP_TO_EDGE = 0,
    WRAP_MODE_REPEAT = 1,
};

enum TextureTargetMode {
    TEXTURE_2D = 0,
    TEXTURE_3D,
    TEXTURE_CUBE_MAP,
};

enum FrameBufferTextureTarget2d {
    FBT_TEXTURE_2D = 0,
    FBT_TEXTURE_CUBE_MAP_POS_X,
    FBT_TEXTURE_CUBE_MAP_NEG_X,
    FBT_TEXTURE_CUBE_MAP_POS_Y,
    FBT_TEXTURE_CUBE_MAP_NEG_Y,
    FBT_TEXTURE_CUBE_MAP_POS_Z,
    FBT_TEXTURE_CUBE_MAP_NEG_Z,
};

inline FrameBufferTextureTarget2d indexToFBT( const int index ) {
    switch (index) {
        case 0:
            return FBT_TEXTURE_CUBE_MAP_POS_X;
        case 1:
            return FBT_TEXTURE_CUBE_MAP_NEG_X;
        case 2:
            return FBT_TEXTURE_CUBE_MAP_POS_Y;
        case 3:
            return FBT_TEXTURE_CUBE_MAP_NEG_Y;
        case 4:
            return FBT_TEXTURE_CUBE_MAP_POS_Z;
        case 5:
            return FBT_TEXTURE_CUBE_MAP_NEG_Z;
        default:
            return FBT_TEXTURE_CUBE_MAP_POS_X;
    }
}

struct ImageParams {
    int width = 0;
    int height = 0;
    int channels = 3;
    int bpp = 8;
    PixelFormat outFormat = PIXEL_FORMAT_RGB;
    TextureTargetMode ttm = TEXTURE_2D;
    WrapMode wrapMode = WRAP_MODE_REPEAT;
    Filter filterMode = FILTER_LINEAR;

    Vector2f getAspectRatioV() const {
        float r = getAspectRatio();
        return r > 1.0f ? V2f{ r, 1.0f} : V2f{ 1.0f, 1.0f/r};
    }

    float getAspectRatio() const {
        return static_cast<float>(width) / static_cast<float>(height);
    }

    int bppStride() const {
        return channels * (bpp/8);
    }

    size_t memorySize() const {
        return width * height * channels * (bpp/8);
    }

    void setFormatFromChannels() {
        if ( bpp == 8 ) {
            if ( channels==1 ) outFormat = PIXEL_FORMAT_LUMINANCE;//PIXEL_FORMAT_R;
            if ( channels==2 ) outFormat = PIXEL_FORMAT_LUMINANCE_ALPHA;//PIXEL_FORMAT_RG;
            if ( channels==3 ) outFormat = PIXEL_FORMAT_RGB;
            if ( channels==4 ) outFormat = PIXEL_FORMAT_RGBA;
        }
        if ( bpp == 16 ) {
            if ( channels==1 ) outFormat = PIXEL_FORMAT_HDR_R16;
            if ( channels==2 ) outFormat = PIXEL_FORMAT_HDR_RG_16;
            if ( channels==3 ) outFormat = PIXEL_FORMAT_HDR_RGB_16;
            if ( channels==4 ) outFormat = PIXEL_FORMAT_HDR_RGBA_16;
        }
        if ( bpp == 32 ) {
            if ( channels==1 ) outFormat = PIXEL_FORMAT_HDR_R32;
            if ( channels==2 ) outFormat = PIXEL_FORMAT_HDR_RG32;
            if ( channels==3 ) outFormat = PIXEL_FORMAT_HDR_RGB_32;
            if ( channels==4 ) outFormat = PIXEL_FORMAT_HDR_RGBA_32;
        }
    }

    ImageParams() = default;
    ImageParams( int width, int height, int channels ) : width( width ), height( height ), channels( channels ) {}
    ImageParams( int width, int height, int channels, int bpp ) : width( width ), height( height ),
                                                                  channels( channels ), bpp( bpp ) {}

    ImageParams& setSize( int s ) {
        width = s;
        height = s;
        return *this;
    }

    ImageParams& setBpp( int s ) {
        bpp = s;
        return *this;
    }

    ImageParams& size( int _width, int _height ) {
        width = _width;
        height = _height;
        return *this;
    }

    ImageParams& setWidth( int _width ) {
        width = _width;
        return *this;
    }

    ImageParams& setHeight( int _height ) {
        height = _height;
        return *this;
    }

    ImageParams& setChannels( int _channels ) {
        channels = _channels;
        return *this;
    }

    ImageParams& format( PixelFormat _outFormat ) {
        outFormat = _outFormat;
        return *this;
    }

    ImageParams& target( TextureTargetMode _ttm ) {
        ttm = _ttm;
        return *this;
    }

    ImageParams& setWrapMode( WrapMode _wrapMode ) {
        wrapMode = _wrapMode;
        return *this;
    }

    ImageParams& setFilterMode( Filter _filterMode ) {
        filterMode = _filterMode;
        return *this;
    }
                                                                      
};

