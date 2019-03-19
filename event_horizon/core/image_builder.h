#pragma once

#include <memory>
#include <string>
#include <set>
#include <functional>
#include <core/builders.hpp>
#include <core/image_constants.h>
#include <core/publisher.hpp>

class ImageBuilder;
class RawImage;

enum class ResourcePublishFlag {
    False,
    True
};

template < typename B, typename R >
class ResourceBuilder2 : public ResourceBuilderBase,
                         public Publisher<B, EmptyBox> {
public:
    explicit ResourceBuilder2( ResourceManager<R>& mm ) : mm( mm ) {}
    ResourceBuilder2( ResourceManager<R>& _mm, const std::string& _name ) : mm( _mm ) {
        this->Name(_name);
    }
    virtual ~ResourceBuilder2() = default;

    void load( CommandResouceCallbackFunction _ccf = nullptr, const std::vector<std::string>& _params = {} ) {
        ccf = _ccf;
        params = _params;
        build();
    }

    bool build() {
        if ( !mm.exists(this->Name()) || R::usesNotExactQuery() ) {
            readRemote<R, B, HttpQuery::Binary>( R::Prefix() + "/" + this->Name(), *this );
            return true;
        }
        return false;
    }

    std::shared_ptr<R> make( const SerializableContainer& _data,
                             ResourcePublishFlag _rpf = ResourcePublishFlag::False ) {
        if ( mm.exists( this->Name() ) ) {
            return mm.get( this->Name() );
        } else {
            this->calcHash( _data );
            if ( _rpf == ResourcePublishFlag::True ) {
                this->publish2( this->Name(), _data );
            }
            return makeInternal( _data );
        }
    }

protected:
    virtual std::shared_ptr<R> makeInternal( const SerializableContainer& _data ) = 0;

    bool makeImpl( const std::string& _key, uint8_p&& _data, const DependencyStatus _status ) override {

        if ( _status == DependencyStatus::LoadedSuccessfully ) {
            auto res = std::make_shared<R>(std::move( _data ), _key);// EF::create<R>( std::move( _data ) );
            mm.add( res, _key );
            if ( ccf ) ccf(params);
            return true;
        }

        return false;
    }

protected:
    ResourceManager<R>& mm;
};

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

protected:
    std::string generateThumbnail() const override;
    void serializeInternal( std::shared_ptr<SerializeBin> writer ) const override;
    void deserializeInternal( std::shared_ptr<DeserializeBin> reader ) override;

    std::shared_ptr<RawImage> makeInternal( const SerializableContainer& _data ) override;
};
