//
// Created by Dado on 2019-03-20.
//

#pragma once

#include <boost/signals2.hpp>

#include <poly/resources/resource_manager.hpp>
#include <poly/resources/publisher.hpp>

enum class AddResourcePolicy {
    Immediate,
    Deferred
};

JSONDATA( ResourceMetadata, name, hash, BBox3d, tags )
    std::string name;
    std::string hash;
    std::vector<float> BBox3d;
    std::vector<std::string> tags;
};

JSONDATA( JSONResourceResponse, _id, project, group, isPublic, isRestricted, metadata )
    std::string _id;
    std::string project;
    std::string group;
    bool isPublic = false;
    bool isRestricted = false;
    ResourceMetadata metadata;
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
        this->publish2( _data, [&](HttpResponeParams _res) {
            JSONResourceResponse resJson(_res.bufferString);
            // We make sure that in case server side has to change name in case
            // of duplicates we reflect it here client side
            this->Name( resJson.metadata.name );
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

    virtual void finalise( std::shared_ptr<R> _elem ) = 0;

protected:
    ResourceManager<R>& mm;
    std::vector<std::string> params;
    CommandResouceCallbackFunction ccf = nullptr;
};
