//
// Created by Dado on 11/02/2018.
//

#pragma once

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