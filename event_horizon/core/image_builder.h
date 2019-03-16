#pragma once

#include <memory>
#include <string>
#include <set>
#include <functional>
#include "util.h"
#include <core/builders.hpp>
#include "image_constants.h"

class ImageBuilder;
class RawImage;

class ImageManager : public DependencyMakerPolicy<RawImage> {
public:
    virtual ~ImageManager() = default;
//    bool add( ImageBuilder& tbd, std::unique_ptr<uint8_t []>& _data );
//    virtual bool addImpl( ImageBuilder& tbd, std::unique_ptr<uint8_t []>& _data ) = 0;
    const ImageParams& ip( const std::string& _key );
private:
    std::map<std::string, ImageParams> imageParamsMap;
};

class ImageBuilder : public ResourceBuilder<RawImage, ImageManager> {
public:
    using ResourceBuilder::ResourceBuilder;
    ImageParams imageParams;
    uint32_t backup_color = 0xffffffff;
    bool bForceHDR16BitTarget = true;
    bool useImagePrefix = true;
    bool mbIsRaw = false;

    ImageBuilder& setId( const std::string& _id ) {
        Name( _id );
        return *this;
    }

    ImageBuilder& backup( const uint32_t _bc ) {
        backup_color = _bc;
        return *this;
    }

    ImageBuilder& setImageParams( const ImageParams& _ip ) {
        imageParams = _ip;
        setFormatFromChannels( imageParams.channels );
        return *this;
    }

    ImageBuilder& setSize( int s ) {
        imageParams.width = s;
        imageParams.height = s;
        return *this;
    }

    ImageBuilder& setBpp( int s ) {
        imageParams.bpp = s;
        return *this;
    }

    ImageBuilder& size( int _width, int _height ) {
        imageParams.width = _width;
        imageParams.height = _height;
        return *this;
    }

    ImageBuilder& setWidth( int _width ) {
        imageParams.width = _width;
        return *this;
    }

    ImageBuilder& setHeight( int _height ) {
        imageParams.height = _height;
        return *this;
    }

    ImageBuilder& setChannels( int _channels ) {
        imageParams.channels = _channels;
        return *this;
    }

    ImageBuilder& setFormatFromChannels( const int _channels ) {
        imageParams.channels = _channels;

        if ( imageParams.bpp == 8 ) {
            if ( imageParams.channels==1 ) imageParams.outFormat = PIXEL_FORMAT_R;
            if ( imageParams.channels==2 ) imageParams.outFormat = PIXEL_FORMAT_RG;
            if ( imageParams.channels==3 ) imageParams.outFormat = PIXEL_FORMAT_RGB;
            if ( imageParams.channels==4 ) imageParams.outFormat = PIXEL_FORMAT_RGBA;
        }
        if ( imageParams.bpp == 16 ) {
            if ( imageParams.channels==1 ) imageParams.outFormat = PIXEL_FORMAT_HDR_R16;
            if ( imageParams.channels==2 ) imageParams.outFormat = PIXEL_FORMAT_HDR_RG_16;
            if ( imageParams.channels==3 ) imageParams.outFormat = PIXEL_FORMAT_HDR_RGB_16;
            if ( imageParams.channels==4 ) imageParams.outFormat = PIXEL_FORMAT_HDR_RGBA_16;
        }
        if ( imageParams.bpp == 32 ) {
            if ( imageParams.channels==1 ) imageParams.outFormat = PIXEL_FORMAT_HDR_R32;
            if ( imageParams.channels==2 ) imageParams.outFormat = PIXEL_FORMAT_HDR_RG32;
            if ( imageParams.channels==3 ) imageParams.outFormat = PIXEL_FORMAT_HDR_RGB_32;
            if ( imageParams.channels==4 ) imageParams.outFormat = PIXEL_FORMAT_HDR_RGBA_32;
        }
        return *this;
    }

    ImageBuilder& format( PixelFormat _outFormat ) {
        imageParams.outFormat = _outFormat;
        return *this;
    }

    ImageBuilder& target( TextureTargetMode _ttm ) {
        imageParams.ttm = _ttm;
        return *this;
    }

    ImageBuilder& setWrapMode( WrapMode _wrapMode ) {
        imageParams.wrapMode = _wrapMode;
        return *this;
    }

    ImageBuilder& setFilterMode( Filter _filterMode ) {
        imageParams.filterMode = _filterMode;
        return *this;
    }

    ImageBuilder& noImagePrefix() {
        useImagePrefix = false;
        return *this;
    }

    ImageBuilder& raw( const bool _value ) {
        mbIsRaw = _value;
        return *this;
    }

public:
    bool makeDirect( const SerializableContainer& _data );
    bool makeDirect( const ucchar_p& _data );
    bool makeDirect( const uint8_p& _data );
    bool makeDirect( const RawImage& _data );
    void makeDefault() override;
private:
    bool finalizaMake( std::unique_ptr<uint8_t[]>&& decodedData );
};
