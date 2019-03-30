//
// Created by Dado on 2019-03-20.
//

#pragma once

#include <boost/signals2.hpp>

#include <core/tar_util.h>
#include <poly/resources/resource_manager.hpp>
#include <poly/resources/publisher.hpp>
#include <poly/scene_graph.h>

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
                       if ( _res.statusCode == 204 ) return; // empty result, handle defaults??
                       addResource( SerializableContainer{_res.buffer.get(), _res.buffer.get()+_res.length},
                                    AddResourcePolicy::Deferred );
                       if ( ccf ) ccf(params);
                   } );
    }

    void addIM( std::shared_ptr<R> _res ) {
        addInternal( _res, AddResourcePolicy::Immediate );
    }
    void addDF( std::shared_ptr<R> _res ) {
        addInternal( _res, AddResourcePolicy::Deferred );
    }
    void add( std::shared_ptr<R> _res, AddResourcePolicy _arp ) {
        addInternal( _res, _arp );
    }

    std::shared_ptr<R> make( const SerializableContainer& _data ) {
        if ( auto ret = prepAndCheck(_data ); ret ) return ret;
        return addResource(_data, AddResourcePolicy::Immediate);
    }

    void create( const R& _data ) {
        create( _data.serialize() );
    }

    void create( const SerializableContainer& _data ) {
        if ( prepAndCheck(_data ) ) return;
        if ( B::Version() != 0 ) this->addTag( this->hashFn(B::Version()) );

        this->publish2( _data, [&]( HttpResponeParams _res ) {
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

protected:
    ResourceManager<R>& mm;
    std::vector<std::string> params;
    CommandResouceCallbackFunction ccf = nullptr;
};

//
//
//

template < typename B, typename R >
class ResourceBuilder3 : public Publisher<B, EmptyBox> {
public:
    explicit ResourceBuilder3( SceneGraph& _sg ) : sg( _sg ) {}
    ResourceBuilder3( SceneGraph& _sg, const std::string& _name ) : sg( _sg ) {
        this->Name(_name);
    }
    virtual ~ResourceBuilder3() = default;

    void load( CommandResouceCallbackFunction _ccf = nullptr, const std::vector<std::string>& _params = {} ) {
        ccf = _ccf;
        params = _params;
        Http::get( Url( HttpFilePrefix::entities + B::Prefix() + "/" + url_encode( this->Name() ) ),
                   [&](HttpResponeParams _res) {
                       if ( _res.statusCode == 204 ) return; // empty result, handle defaults??
                       addResource( SerializableContainer{_res.buffer.get(), _res.buffer.get()+_res.length},
                                    AddResourcePolicy::Deferred );
                       if ( ccf ) ccf(params);
                   } );
    }

    void addIM( const R& _res ) {
        addInternal( EF::clone(_res), AddResourcePolicy::Immediate );
    }
    void addDF( const R& _res ) {
        addInternal( EF::clone(_res), AddResourcePolicy::Deferred );
    }
    void addIM( std::shared_ptr<R> _res ) {
        addInternal( _res, AddResourcePolicy::Immediate );
    }
    void addDF( std::shared_ptr<R> _res ) {
        addInternal( _res, AddResourcePolicy::Deferred );
    }
    void add( std::shared_ptr<R> _res, AddResourcePolicy _arp ) {
        addInternal( _res, _arp );
    }

    std::shared_ptr<R> make( const SerializableContainer& _data ) {
        if ( auto ret = prepAndCheck(_data ); ret ) return ret;
        return addResource(_data, AddResourcePolicy::Immediate);
    }

    void create( const R& _data ) {
        create( _data.serialize() );
    }

    void create( const SerializableContainer& _data ) {
        if ( prepAndCheck(_data ) ) return;
        if ( B::Version() != 0 ) this->addTag( this->hashFn(B::Version()) );

        this->publish2( _data, [&]( HttpResponeParams _res ) {
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
        if ( auto ret = sg.M<R>().hashExists( this->Hash() ); ret!= nullptr ) {
            return ret;
        }
        return nullptr;
    }

    std::shared_ptr<R> addResource( const SerializableContainer& _data, AddResourcePolicy _arp ) {
        auto ret = EF::create<R>(_data);
        addInternal( ret, _arp );
        return ret;
    }

    void addInternal( std::shared_ptr<R> _res, AddResourcePolicy _arp ) {

        if ( _arp == AddResourcePolicy::Deferred ) {
            sg.M<R>().addDeferred( _res, this->Name(), this->Hash() );
        } else {
            sg.M<R>().addImmediate( _res, this->Name(), this->Hash() );
        }

    }

protected:
    SceneGraph& sg;
    std::map<std::string, std::vector<ResourceRef>> dependencies;
    std::vector<std::string> params;
    CommandResouceCallbackFunction ccf = nullptr;
};

template <typename R>
class ResourceBuilder4 : public Publisher<R, EmptyBox> {
public:
    explicit ResourceBuilder4( SceneGraph& _sg ) : sg( _sg ) {}
    ResourceBuilder4( SceneGraph& _sg, const std::string& _name ) : sg( _sg ) {
        this->Name(_name);
    }
    virtual ~ResourceBuilder4() = default;

    void load( CommandResouceCallbackFunction _ccf = nullptr, const std::vector<std::string>& _params = {} ) {
        ccf = _ccf;
        params = _params;
        Http::get( Url( HttpFilePrefix::entities + ResourceVersioning<R>::Prefix() + "/" + url_encode( this->Name() ) ),
                   [&](HttpResponeParams _res) {
                       if ( _res.statusCode == 204 ) return; // empty result, handle defaults??
                       addResource( SerializableContainer{_res.buffer.get(), _res.buffer.get()+_res.length},
                                    AddResourcePolicy::Deferred );
                       if ( ccf ) ccf(params);
                   } );
    }

    void addIM( const R& _res ) {
        addInternal( EF::clone(_res), AddResourcePolicy::Immediate );
    }
    void addDF( const R& _res ) {
        addInternal( EF::clone(_res), AddResourcePolicy::Deferred );
    }
    void addIM( std::shared_ptr<R> _res ) {
        addInternal( _res, AddResourcePolicy::Immediate );
    }
    void addDF( std::shared_ptr<R> _res ) {
        addInternal( _res, AddResourcePolicy::Deferred );
    }
    void add( std::shared_ptr<R> _res, AddResourcePolicy _arp ) {
        addInternal( _res, _arp );
    }

    std::shared_ptr<R> make( const SerializableContainer& _data ) {
        if ( auto ret = prepAndCheck(_data ); ret ) return ret;
        return addResource(_data, AddResourcePolicy::Immediate);
    }

    void create( const R& _data ) {
        create( _data.serialize() );
    }

    void create( const SerializableContainer& _data ) {
        if ( prepAndCheck(_data ) ) return;
//        if ( B::Version() != 0 ) this->addTag( this->hashFn(B::Version()) );

        this->publish2( _data, [&]( HttpResponeParams _res ) {
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
        if ( auto ret = sg.M<R>().hashExists( this->Hash() ); ret!= nullptr ) {
            return ret;
        }
        return nullptr;
    }

    std::shared_ptr<R> addResource( const SerializableContainer& _data, AddResourcePolicy _arp ) {
        auto ret = EF::create<R>(_data);
        addInternal( ret, _arp );
        return ret;
    }

    void addInternal( std::shared_ptr<R> _res, AddResourcePolicy _arp ) {

        if ( _arp == AddResourcePolicy::Deferred ) {
            sg.M<R>().addDeferred( _res, this->Name(), this->Hash() );
        } else {
            sg.M<R>().addImmediate( _res, this->Name(), this->Hash() );
        }

    }

protected:
    SceneGraph& sg;
    std::map<std::string, std::vector<ResourceRef>> dependencies;
    std::vector<std::string> params;
    CommandResouceCallbackFunction ccf = nullptr;
};

JSONDATA( ResourceTarDict, group, filename, hash )
    std::string group;
    std::string filename;
    std::string hash;
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
                           auto fs = tarUtil::untar(buff);
                           ASSERT( fs.find(ResourceCatalog::Key) != fs.end() );
                           std::vector<ResourceTarDict> dict;
                           std::string dictString( std::string{fs[ResourceCatalog::Key].begin(), fs[ResourceCatalog::Key].end()} );
                           rapidjson::Document document;
                           document.Parse<rapidjson::kParseStopWhenDoneFlag>( dictString.c_str() );
                           MegaReader reader( document );
                           reader.deserialize( dict );

                           std::sort( dict.begin(), dict.end(), []( const auto& a, const auto& b ) -> bool {
                               return resourcePriority( a.group ) < resourcePriority( b.group );
                           } );

                           for ( const auto& rd : dict ) {
                               if ( rd.group == ResourceGroup::Image ) {
                                   addResource<RawImage>( fs[rd.filename], rd.filename, rd.hash, AddResourcePolicy::Deferred );
                               } else if ( rd.group == ResourceGroup::Font ) {
                                   addResource<Utility::TTFCore::Font>( fs[rd.filename], rd.filename, rd.hash, AddResourcePolicy::Deferred );
                               } else if ( rd.group == ResourceGroup::Profile ) {
                                   addResource<Profile>( fs[rd.filename], rd.filename, rd.hash, AddResourcePolicy::Deferred );
                               } else if ( rd.group == ResourceGroup::Color ) {
                                   addResource<MaterialColor>( fs[rd.filename], rd.filename, rd.hash, AddResourcePolicy::Deferred );
                               } else if ( rd.group == ResourceGroup::Material ) {
                                   addResource<Material>( fs[rd.filename], rd.filename, rd.hash, AddResourcePolicy::Deferred );
                               } else {
                                   LOGRS("{" << rd.group << "} Rescoude not supported yet in GET callback");
                                   ASSERT(0);
                               }
                           }
                       } else {
                           addResource<R>( buff, this->Name(), this->Hash(),AddResourcePolicy::Deferred );
                       }
                       if ( ccf ) ccf(params);
                   } );
    }

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

    std::shared_ptr<R> make( const SerializableContainer& _data ) {
        if ( auto ret = prepAndCheck(_data ); ret ) return ret;
        return addResource<R>(_data, this->Name(), this->Hash(), AddResourcePolicy::Immediate);
    }

    void create( const SerializableContainer& _data, const ResourceDependencyDict& _res = {} ) {
        if ( prepAndCheck(_data ) ) return;
//        if ( B::Version() != 0 ) this->addTag( this->hashFn(B::Version()) );

        this->publish3( _data, _res, [&]( HttpResponeParams _res ) {
            JSONResourceResponse resJson(_res.bufferString);
            // We make sure that in case server side has to change name in case
            // of duplicates we reflect it here client side
            this->Name( resJson.metadata.name );
            addResource<R>(_data, this->Name(), this->Hash(), AddResourcePolicy::Deferred);
        } );
    }

protected:
    std::shared_ptr<R> prepAndCheck( const SerializableContainer& _data ) {
        this->calcHash( _data );
        if ( auto ret = sg.M<R>().hashExists( this->Hash() ); ret!= nullptr ) {
            return ret;
        }
        return nullptr;
    }

//    template <typename DEP>
//    std::shared_ptr<DEP> addDependency( const ResourceTarDict& _rd,
//                                        const SerializableContainer& _data,
//                                        AddResourcePolicy _arp ) {
//        auto ret = EF::create<DEP>(_data);
//        sg.M<DEP>().addDeferred( ret, _rd.filename, _rd.hash );
//        return ret;
//    }

    template <typename DEP>
    std::shared_ptr<DEP> addResource( const SerializableContainer& _data,
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

        if ( _arp == AddResourcePolicy::Deferred ) {
            sg.M<DEP>().addDeferred( _res, _name, _hash );
        } else {
            sg.M<DEP>().addImmediate( _res, _name, _hash );
        }

    }

protected:
    SceneGraph& sg;
    std::map<std::string, std::vector<ResourceRef>> dependencies;
    std::vector<std::string> params;
    CommandResouceCallbackFunction ccf = nullptr;
};

