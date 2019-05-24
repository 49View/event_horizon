//
// Created by Dado on 2019-03-20.
//

#pragma once

#include <boost/signals2.hpp>

#include <core/tar_util.h>
#include <core/resources/resource_manager.hpp>
#include <core/resources/publisher.hpp>
#include <core/resources/entity_factory.hpp>
#include <poly/scene_graph.h>

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

template <typename R>
class ResourceBuilder : public Publisher<R, EmptyBox> {
public:
    explicit ResourceBuilder( SceneGraph& _sg ) : sg( _sg ) {}
    ResourceBuilder( SceneGraph& _sg, const std::string& _name ) : sg( _sg ) {
        this->Name(_name);
    }
    virtual ~ResourceBuilder() = default;

    void load( HttpResouceCB _ccf = nullptr ) {
        Http::get( Url( HttpFilePrefix::entities + ResourceVersioning<R>::Prefix() + "/" + url_encode( this->Name() ) ),
                   [](HttpResponeParams _res) {
                       if ( _res.statusCode == 204 ) return; // empty result, handle defaults??
                       auto buff = SerializableContainer{_res.buffer.get(), _res.buffer.get()+_res.length};
                       if ( tarUtil::isTar(buff) ) {
                           SceneGraph::addDeferredComp( std::move(buff), _res.ccf );
                       } else {
                           SceneGraph::addDeferred<R>( getFileNameCallbackKey( _res.uri ),
                                                       std::move(buff),
                                                       _res.ccf );
                       }
                   },
                   nullptr,
                   Http::ResponseFlags::None,
                   _ccf );
    }

    // add*: this->Hash() will be empty "" if it comes from a procedural resource (IE not loaded from a file)
    // it will then be calculated in addInternal because every resource has to have an hash
    ResourceRef addIM( const R& _res ) {
        return addInternal<R>( EF::clone(_res), this->Name(), this->Hash(), AddResourcePolicy::Immediate );
    }
    ResourceRef addDF( const R& _res, HttpResouceCB _ccf = nullptr ) {
        return addInternal<R>( EF::clone(_res), this->Name(), this->Hash(), AddResourcePolicy::Deferred, _ccf );
    }
    ResourceRef addIM( std::shared_ptr<R> _res ) {
        return addInternal<R>( _res, this->Name(), this->Hash(), AddResourcePolicy::Immediate );
    }
    ResourceRef addDF( std::shared_ptr<R> _res, HttpResouceCB _ccf = nullptr ) {
        return addInternal<R>( _res, this->Name(), this->Hash(), AddResourcePolicy::Deferred, _ccf );
    }
    ResourceRef add( std::shared_ptr<R> _res, AddResourcePolicy _arp, HttpResouceCB _ccf = nullptr ) {
        return addInternal<R>( _res, this->Name(), this->Hash(), _arp, _ccf );
    }

    std::shared_ptr<R> make( const SerializableContainer& _data, const ResourceRef& _hash = {} ) {
        auto ret = prepAndCheck(_data, _hash );
        if ( ret ) return ret;
        return add<R>(_data, this->Name(), this->Hash(), AddResourcePolicy::Immediate);
    }

    void publishAndAdd( const SerializableContainer& _data, const ResourceDependencyDict& _res = {} ) {
        if ( prepAndCheck(_data ) ) return;

        this->publish( _data, _res, [&]( HttpResponeParams _res ) {
            if ( _res.statusCode == 204 ) return; // Nothing to do
            JSONResourceResponse resJson( _res.bufferString );
            // We make sure that in case server side has to change name in case
            // of duplicates we reflect it here client side
            this->Name( resJson.metadata.name );
            add < R > ( _data, this->Name(), this->Hash(), AddResourcePolicy::Deferred );
        } );
    }

protected:
    std::shared_ptr<R> prepAndCheck( const SerializableContainer& _data, const ResourceRef& _hash = {} ) {
        if ( _hash.empty() ) {
            this->calcHash( _data );
        } else {
            this->Hash(_hash);
        }
        if ( auto ret = sg.M<R>().hashExists( this->Hash() ); ret!= nullptr ) {
            return ret;
        }
        return nullptr;
    }

    template <typename DEP>
    std::shared_ptr<DEP> add( const SerializableContainer& _data,
                              const std::string& _name,
                              const ResourceRef& _hash,
                              AddResourcePolicy _arp,
                              HttpResouceCB _ccf = nullptr ) {
        auto ret = EF::create<DEP>(_data);
        addInternal<DEP>( ret, _name, _hash, _arp, _ccf );
        return ret;
    }

    template <typename DEP>
    ResourceRef addInternal( std::shared_ptr<DEP> _res,
                      const std::string& _name,
                      const ResourceRef& _hash,
                      AddResourcePolicy _arp,
                             HttpResouceCB _ccf = nullptr ) {
        // NDDADO: This could be very slow, might need to find a flyweight to calculate the whole hash
        ResourceRef resolvedHash = _hash;
        if constexpr ( std::is_same<R, DEP>::value ) {
            if ( resolvedHash.empty() ) {
                this->calcHash( ResourceVersioning<DEP>::HashResolver(_res) );
                resolvedHash = this->Hash();
            }
        }
        ASSERT( !resolvedHash.empty() );

        sg.M<DEP>().add( _res, _name, resolvedHash, _arp, this->Name(), _ccf );
        return resolvedHash;
    }

protected:
    SceneGraph& sg;
    std::map<std::string, std::vector<ResourceRef>> dependencies;
    std::vector<std::string> params;
    HttpResouceCB ccf = nullptr;
};
