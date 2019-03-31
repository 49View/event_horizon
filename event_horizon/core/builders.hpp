
//
// Created by Dado on 2018-12-30.
//

#pragma once

#include <utility>
#include <core/callback_dependency.h>
#include <core/http/webclient.h>
#include <core/name_policy.hpp>

template<HttpQuery Q, typename T>
Url makeUrl(const std::string& _name) {
    switch (Q) {
        case HttpQuery::JSON:
            // ### restore T::typeName() for private api calls, might need to re-route in a different way
            return Url::privateAPI(HttpFilePrefix::catalog + T::Prefix() + HttpFilePrefix::getname + url_encode( _name ) );
//            return Url::privateAPI(HttpFilePrefix::catalog + T::typeName() + HttpFilePrefix::getname + url_encode( _name ) );
        case HttpQuery::Binary:
        case HttpQuery::Text: {
//            if ( T::usesNotExactQuery() ) {
//                return Url::entityContent( _name );
//            } else {
            return Url( HttpFilePrefix::get + url_encode( _name ) );
//            }
        }
        default:
            break;
    }
    return Url{};
}

void readRemote( const Url& url, std::shared_ptr<FileCallbackHandler> _handler,
                 Http::ResponseFlags rf = Http::ResponseFlags::None );

template<typename R, typename B, HttpQuery Q>
void readRemote( const std::string& _name, B& _builder ) {
    readRemote( makeUrl<Q, R>(_name), makeHandler<B>( _builder ) );
}

using CommandResouceCallbackFunction = std::function<void(const std::vector<std::string>&)>;

class BaseBuilder : public NamePolicy<std::string> {
public:
    BaseBuilder() = default;
    explicit BaseBuilder( const std::string& _name ) {
        this->Name( _name );
    }
};

class ResourceBuilderBase {
public:
    bool make( const std::string& _key, uint8_p&& _data, const DependencyStatus _status ) {
        return makeImpl( _key, std::move(_data), _status );
    }

    bool isReloading() const {
        return bReloading;
    }

protected:
    virtual bool makeImpl( const std::string& _key, uint8_p&& _data, DependencyStatus _status ) = 0;

protected:
    std::vector<std::string> params;
    CommandResouceCallbackFunction ccf = nullptr;
    bool bReloading = false;
};

//template < typename R, typename M >
//class ResourceBuilder : public ResourceBuilderBase,
//                        public NamePolicy<std::string> {
//public:
//    using NamePolicy::NamePolicy;
//
//    explicit ResourceBuilder( M& mm ) : NamePolicy(), mm( mm ) {}
//    ResourceBuilder( M& _mm, const std::string& _name ) : NamePolicy(_name), mm( _mm ) {}
//    virtual ~ResourceBuilder() = default;
//
//    ResourceBuilder& n( const std::string& _name ) {
//        Name(_name);
//        return *this;
//    }
//
//    const std::string NameKey() const {
//        return R::Prefix() + "/" + Name();
//    }
//
//    bool rebuild() {
//        bReloading = true;
//        return build();
//    };
//
//    virtual bool evaluateDirectBuild() {
//        return false;
//    }
//
//    void load( CommandResouceCallbackFunction _ccf = nullptr, const std::vector<std::string>& _params = {} ) {
//        ccf = _ccf;
//        params = _params;
//        build();
//    }
//
//    bool build() {
//        if ( !mm.exists(Name()) || R::usesNotExactQuery() ) {
//            readRemote<R, ResourceBuilder, HttpQuery::Binary>( NameKey(), *this );
//            return true;
//        }
//        return false;
//    }
//
//    virtual void makeDefault() {}
//
//protected:
//    bool makeImpl( const std::string& _key, uint8_p&& _data, const DependencyStatus _status ) override {
//
//        if ( _status == DependencyStatus::LoadedSuccessfully ) {
//            auto res = EF::create<R>( std::move( _data ) );
//            mm.add( res, _key );
//            if ( ccf ) ccf(params);
//        } else {
//            makeDefault();
//        }
//
//        return true;
//    }
//
//protected:
//    M& mm;
//};

class DependantBuilder : public BaseBuilder {
    using BaseBuilder::BaseBuilder;
public:

    virtual void assemble() {};

    void build() {
        if ( validate() ) {
            elemCreate();
            createDependencyList();
        }
    }

    ddContainer& deps()  {
        return mDeps;
    }

protected:
    virtual void elemCreate() = 0;
    virtual bool validate() const = 0;
    template< typename B>
    void assembleNV( std::shared_ptr<B> _builder ) {
        _builder->assemble();
        // We leave room for injecting code that needs to run pre/after assembly
    }

    template< typename B>
    void addDependencies( std::shared_ptr<B> _builder ) {
        if ( mDeps.empty() ) {
            assembleNV( _builder );
        } else {
            g_deps.push_back( std::make_shared<DependencyChainMap>( _builder ));
        }
    };

    template <typename BD>
    void push_dep( const std::string& _dname ) {
        mDeps.push_back( BD::Prefix() + _dname );
    }

    template <typename R, typename BD, typename M>
    bool addDependency( const std::string& _dname, M& _md ) {
        if ( !_md.exists(_dname) ) {
            BD{_md, _dname}.load();
            push_dep<R>( _dname );
            return true;
        }
        return false;
    }

    template <typename R, typename BD, typename M>
    void addDependency( BD& _builder, M& _md ) {
        if ( !_md.exists(_builder.Name()) ) {
            if ( !_builder.evaluateDirectBuild() ) {
                _builder.build( _md );
                push_dep<R>( _builder.Name() );
            }
        }
    }

    virtual void createDependencyList() = 0;

protected:
    ddContainer mDeps;
};
