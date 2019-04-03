#include <utility>

#include <utility>

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
class ResourceBuilder5 : public Publisher<R, EmptyBox> {
public:
    explicit ResourceBuilder5( SceneGraph& _sg ) : sg( _sg ) {}
    ResourceBuilder5( SceneGraph& _sg, const std::string& _name ) : sg( _sg ) {
        this->Name(_name);
    }
    virtual ~ResourceBuilder5() = default;

    void load( CommandResouceCallbackFunction _ccf = nullptr, const std::vector<std::string>& _params = {} ) {
        ccf = _ccf;
        params = _params;
        Http::get( Url( HttpFilePrefix::entities + ResourceVersioning<R>::Prefix() + "/" + url_encode( this->Name() ) ),
                   [&](HttpResponeParams _res) {
                       if ( _res.statusCode == 204 ) return; // empty result, handle defaults??
                       auto buff = SerializableContainer{_res.buffer.get(), _res.buffer.get()+_res.length};
                       if ( tarUtil::isTar(buff) ) {
                           addResources( buff, AddResourcePolicy::Deferred );
                       } else {
                           add<R>( buff, this->Name(), this->Hash(), AddResourcePolicy::Deferred );
                       }
                       if ( ccf ) ccf(params);
                   } );
    }

    // add*: this->Hash() will be empty "" if it comes from a procedural resource (IE not loaded from a file)
    // this way we do not serialize anything with an empty hash. win-win. I think. It smells a bit though, just.

    void addIM( const R& _res ) {
        addInternal<R>( EF::clone(_res), this->Name(), this->Hash(), AddResourcePolicy::Immediate );
    }
    void addDF( const R& _res ) {
        addInternal<R>( EF::clone(_res), this->Name(), this->Hash(), AddResourcePolicy::Deferred );
    }
    void addIM( std::shared_ptr<R> _res ) {
        addInternal<R>( _res, this->Name(), this->Hash(), AddResourcePolicy::Immediate );
    }
    void addDF( std::shared_ptr<R> _res ) {
        addInternal<R>( _res, this->Name(), this->Hash(), AddResourcePolicy::Deferred );
    }
    void add( std::shared_ptr<R> _res, AddResourcePolicy _arp ) {
        addInternal<R>( _res, this->Name(), this->Hash(), _arp );
    }

    void addResources( const SerializableContainer& _data, AddResourcePolicy _arp ) {

        auto fs = tarUtil::untar(_data);
        ASSERT( fs.find(ResourceCatalog::Key) != fs.end() );
        auto dict = deserializeArray<ResourceTarDict>( fs[ResourceCatalog::Key] );

        std::sort( dict.begin(), dict.end(), []( const auto& a, const auto& b ) -> bool {
            return resourcePriority( a.group ) < resourcePriority( b.group );
        } );

        for ( const auto& rd : dict ) {
            if ( rd.group == ResourceGroup::Image ) {
                add<RawImage>( fs[rd.filename], rd.filename, rd.hash, AddResourcePolicy::Deferred );
            } else if ( rd.group == ResourceGroup::Font ) {
                add<Utility::TTFCore::Font>( fs[rd.filename], rd.filename, rd.hash, AddResourcePolicy::Deferred );
            } else if ( rd.group == ResourceGroup::Profile ) {
                add<Profile>( fs[rd.filename], rd.filename, rd.hash, AddResourcePolicy::Deferred );
            } else if ( rd.group == ResourceGroup::Color ) {
                add<MaterialColor>( fs[rd.filename], rd.filename, rd.hash, AddResourcePolicy::Deferred );
            } else if ( rd.group == ResourceGroup::Material ) {
                add<Material>( fs[rd.filename], rd.filename, rd.hash, AddResourcePolicy::Deferred );
            } else {
                LOGRS("{" << rd.group << "} Resource not supported yet in dependency unpacking");
                ASSERT(0);
            }
        }
    }

    std::shared_ptr<R> make( const SerializableContainer& _data, const ResourceRef& _hash = {} ) {
        auto ret = prepAndCheck(_data, _hash );
        if ( ret ) return ret;
        return add<R>(_data, this->Name(), this->Hash(), AddResourcePolicy::Immediate);
    }

    void publishAndAdd( const SerializableContainer& _data, const ResourceDependencyDict& _res = {} ) {
        if ( prepAndCheck(_data ) ) return;

        this->publish( _data, _res, [&]( HttpResponeParams _res ) {
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
                              AddResourcePolicy _arp ) {
        auto ret = EF::create<DEP>(_data);
        addInternal<DEP>( ret, _name, _hash, _arp );
        return ret;
    }

    template <typename DEP>
    void addInternal( std::shared_ptr<DEP> _res,
                      const std::string& _name,
                      const ResourceRef& _hash,
                      AddResourcePolicy _arp ) {
        sg.M<DEP>().add( _res, _name, _hash, _arp, this->Name() );
    }

protected:
    SceneGraph& sg;
    std::map<std::string, std::vector<ResourceRef>> dependencies;
    std::vector<std::string> params;
    CommandResouceCallbackFunction ccf = nullptr;
};
