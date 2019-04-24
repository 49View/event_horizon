//
// Created by Dado on 10/02/2018.
//
#pragma once

#include <core/names.hpp>
#include <core/name_policy.hpp>
#include <core/image_constants.h>
#include <core/htypes_shared.hpp>
#include "graphic_constants.h"

struct TextureRenderData : public ImageParams {

    StringUniqueCollection names;
    TextureSlots preferredGPUSlot = TSLOT_COLOR;
    int forceGPUId = -1;
    bool multisample = false;
    bool HDR = false;
    bool forceHDRTarget16bit = true;
    bool isFramebufferTarget = false;
    bool generateMipMaps = true;

    TextureRenderData( const std::string& _name, const ImageParams& _ip ) : ImageParams(_ip) {
        names.emplace(_name);
    }

    TextureRenderData( const std::string& _name ) {
        names.emplace(_name);
    }

    TextureRenderData( const StringUniqueCollection& _names ) : names( _names ) {
    }

    TextureRenderData( const StringUniqueCollection& _names, const ImageParams& _ip ) : ImageParams(_ip), names( _names ) {
    }

    TextureRenderData(  const std::string& _name, int width, int height, int channels, int bpp ) :
            ImageParams( width, height, channels, bpp ) {
        names.emplace(_name);
        setFormatFromBpp();
    }

    TextureRenderData( const StringUniqueCollection& _names, int width, int height, int channels, int bpp ) :
                       ImageParams( width, height, channels, bpp ),
                       names( _names ) {
        setFormatFromBpp();
    }

    TextureRenderData& GPUId(const int _id) {
        forceGPUId = _id;
        return *this;
    }

    TextureRenderData& ch( int _channels ) {
        channels = _channels;
        return *this;
    }

    TextureRenderData& setFormatFromBpp() {
        HDR = bpp > 8;
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