//
// Created by Dado on 10/02/2018.
//
#pragma once

#include <core/names.hpp>
#include "graphic_constants.h"

struct TextureRenderData {
    std::string name = S::WHITE;
    int width = 0;
    int height = 0;
    int channels = 3;
    int bpp = 8;

    PixelFormat outFormat = PIXEL_FORMAT_RGB;
    TextureTargetMode ttm = TEXTURE_2D;
    WrapMode wrapMode = WRAP_MODE_REPEAT;
    Filter filterMode = FILTER_LINEAR;

    TextureSlots preferredGPUSlot = TSLOT_COLOR;
    int forceGPUId = -1;
    bool multisample = false;
    bool HDR = false;
    bool forceHDRTarget16bit = true;
    bool isFramebufferTarget = false;
    bool generateMipMaps = true;

    TextureRenderData( const std::string& name ) : name( name ) {}

    TextureRenderData( const std::string& name, int width, int height, int channels, int bpp ) : name( name ),
                                                                                                 width( width ),
                                                                                                 height( height ),
                                                                                                 channels( channels ),
                                                                                                 bpp( bpp ) {
        setFormatFromBpp();
    }

    const std::string& Name() const {
        return name;
    };

    TextureRenderData& setId( const std::string& _id ) {
        name = _id;
        return *this;
    }

    TextureRenderData& GPUId(const int _id) {
        forceGPUId = _id;
        return *this;
    }

    TextureRenderData& setSize( int s ) {
        width = s;
        height = s;
        return *this;
    }

    TextureRenderData& size( int _width, int _height ) {
        width = _width;
        height = _height;
        return *this;
    }

    TextureRenderData& setWidth( int _width ) {
        TextureRenderData::width = _width;
        return *this;
    }

    TextureRenderData& setHeight( int _height ) {
        TextureRenderData::height = _height;
        return *this;
    }

    TextureRenderData& ch( int _channels ) {
        channels = _channels;
        return *this;
    }

    TextureRenderData& setBpp( int s ) {
        bpp = s;
        return *this;
    }

    TextureRenderData& setFormatFromBpp() {
        HDR = bpp > 8 ? true : false;
        int forcedBpp = bpp;
        if ( HDR && bpp == 32 ) {
            forcedBpp = forceHDRTarget16bit ? 16 : 32;
        }
        if ( forcedBpp == 8 ) {
            if ( channels==1 ) outFormat = PIXEL_FORMAT_R;
            if ( channels==2 ) outFormat = PIXEL_FORMAT_RG;
            if ( channels==3 ) outFormat = PIXEL_FORMAT_RGB;
            if ( channels==4 ) outFormat = PIXEL_FORMAT_RGBA;
        }
        if ( forcedBpp == 16 ) {
            if ( channels==1 ) outFormat = PIXEL_FORMAT_HDR_R16;
            if ( channels==2 ) outFormat = PIXEL_FORMAT_HDR_RG_16;
            if ( channels==3 ) outFormat = PIXEL_FORMAT_HDR_RGB_16;
            if ( channels==4 ) outFormat = PIXEL_FORMAT_HDR_RGBA_16;
        }
        if ( forcedBpp == 32 ) {
            if ( channels==1 ) outFormat = PIXEL_FORMAT_HDR_R32;
            if ( channels==2 ) outFormat = PIXEL_FORMAT_HDR_RG32;
            if ( channels==3 ) outFormat = PIXEL_FORMAT_HDR_RGB_32;
            if ( channels==4 ) outFormat = PIXEL_FORMAT_HDR_RGBA_32;
        }
        return *this;
    }

    TextureRenderData& format( PixelFormat _outFormat ) {
        outFormat = _outFormat;
        return *this;
    }

    TextureRenderData& target( TextureTargetMode _ttm ) {
        ttm = _ttm;
        return *this;
    }

    TextureRenderData& wm( WrapMode _wrapMode ) {
        wrapMode = _wrapMode;
        return *this;
    }

    TextureRenderData& fm( Filter _filterMode ) {
        filterMode = _filterMode;
        return *this;
    }

    TextureRenderData& setMultisample( bool _multisample ) {
        multisample = _multisample;
        return *this;
    }

    TextureRenderData& setHDR( bool _HDR ) {
        HDR = _HDR;
        return *this;
    }

    TextureRenderData& setIsFramebufferTarget( bool _isFramebufferTarget ) {
        isFramebufferTarget = _isFramebufferTarget;
        return *this;
    }

    TextureRenderData& setGenerateMipMaps( bool _generateMipMaps ) {
        generateMipMaps = _generateMipMaps;
        return *this;
    }

    TextureRenderData& GPUSlot( TextureSlots _slot ) {
        preferredGPUSlot = _slot;
        return *this;
    }

};

#ifdef _OPENGL

#include "opengl/texture_opengl.h"

#endif