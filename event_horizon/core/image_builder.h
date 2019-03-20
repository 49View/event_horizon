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

template < typename B, typename R >
class ResourceBuilder2 : public Publisher<B, EmptyBox> {
public:
    explicit ResourceBuilder2( ResourceManager<R>& mm ) : mm( mm ) {}
    ResourceBuilder2( ResourceManager<R>& _mm, const std::string& _name ) : mm( _mm ) {
        this->Name(_name);
    }
    virtual ~ResourceBuilder2() = default;

    void load( CommandResouceCallbackFunction _ccf = nullptr, const std::vector<std::string>& _params = {} ) {
        ccf = _ccf;
        params = _params;
        Http::get( Url( HttpFilePrefix::entities + B::Prefix() + "/" + url_encode( this->Name() ) ),
                   [&](HttpResponeParams _res) {
                       // EF::create<R>( std::move( _data ) );
                       auto ret = std::make_shared<R>( uint8_p{std::move(_res.buffer), _res.length}, this->Name());
                       mm.addDeferred( ret, this->Name(), this->Hash() );
                       if ( ccf ) ccf(params);
                   } );
    }

    std::shared_ptr<R> make( const SerializableContainer& _data ) {
        if ( auto ret = prepAndCheck(_data ); ret ) return ret;
        auto res = makeInternal( _data );
        mm.addImmediate( res, this->Name(), this->Hash() );
        return res;
    }

    void create( const SerializableContainer& _data ) {
        if ( prepAndCheck(_data ) ) return;

        if ( B::Version() != 0 ) this->addTag( this->hashFn(B::Version()) );
        this->publish2( _data, [&](HttpResponeParams res) {
            mm.addDeferred( makeInternal( _data ), this->Name(), this->Hash() );
        } );
    }

protected:
    std::shared_ptr<R> prepAndCheck( const SerializableContainer& _data ) {
        this->calcHash( _data );
        if ( auto ret = mm.hashExists( this->Hash() ); ret!= nullptr ) {
            return ret;
        }
        return nullptr;
    }

    virtual std::shared_ptr<R> makeInternal( const SerializableContainer& _data ) = 0;

protected:
    ResourceManager<R>& mm;
    std::vector<std::string> params;
    CommandResouceCallbackFunction ccf = nullptr;
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
    std::string generateThumbnail() const override;
    void serializeInternal( std::shared_ptr<SerializeBin> writer ) const override;
    void deserializeInternal( std::shared_ptr<DeserializeBin> reader ) override;

    std::shared_ptr<RawImage> makeInternal( const SerializableContainer& _data ) override;
};
