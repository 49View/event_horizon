#pragma once

#include <memory>
#include <string>
#include <set>
#include <functional>
#include <core/image_constants.h>
#include <poly/resources/resource_builder.hpp>

class RawImage;

class ImageBuilder : public ResourceBuilder2<ImageBuilder, RawImage> {
public:
    using ResourceBuilder2::ResourceBuilder2;
    ImageParams imageParams;
    uint32_t backup_color = 0xffffffff;
    bool bForceHDR16BitTarget = true;
    bool useImagePrefix = true;
    bool mbIsRaw = false;

    ImageBuilder& backup( const uint32_t _bc ) {
        backup_color = _bc;
        return *this;
    }

    ImageBuilder& setImageParams( const ImageParams& _ip ) {
        imageParams = _ip;
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

protected:
    void serializeInternal( std::shared_ptr<SerializeBin> writer ) const override;
    void deserializeInternal( std::shared_ptr<DeserializeBin> reader ) override;

    void finalise( std::shared_ptr<RawImage> _elem ) override;
};
