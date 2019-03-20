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

enum class AddResourcePolicy {
    Immediate,
    Deferred
};

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
                       addResource( SerializableContainer{_res.buffer.get(), _res.buffer.get()+_res.length},
                                    AddResourcePolicy::Deferred );
                       if ( ccf ) ccf(params);
                   } );
    }

    void add( std::shared_ptr<R> _res, AddResourcePolicy _arp ) {
        addInternal( _res, _arp );
    }

    std::shared_ptr<R> make( const SerializableContainer& _data ) {
        if ( auto ret = prepAndCheck(_data ); ret ) return ret;
        return addResource(_data, AddResourcePolicy::Immediate);
    }

    void create( const SerializableContainer& _data ) {
        if ( prepAndCheck(_data ) ) return;

        if ( B::Version() != 0 ) this->addTag( this->hashFn(B::Version()) );
        this->publish2( _data, [&](HttpResponeParams res) {
            addResource(_data, AddResourcePolicy::Deferred);
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

    std::shared_ptr<R> addResource( const SerializableContainer& _data, AddResourcePolicy _arp ) {
        auto ret = EF::create<R>(_data);
        finalise(ret);
        addInternal( ret, _arp );
        return ret;
    }

    void addInternal( std::shared_ptr<R> _res, AddResourcePolicy _arp ) {
        if ( _arp == AddResourcePolicy::Deferred ) {
            mm.addDeferred( _res, this->Name(), this->Hash() );
        } else {
            mm.addImmediate( _res, this->Name(), this->Hash() );
        }

    }

    virtual void finalise( std::shared_ptr<RawImage> _elem ) = 0;

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

    void finalise( std::shared_ptr<RawImage> _elem ) override;
};
