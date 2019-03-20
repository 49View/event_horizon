//
// Created by Dado on 11/01/2018.
//

#pragma once

#include <memory>
#include <vector>
#include <set>
#include <string>
#include <typeinfo>
#include <utility>

#include <boost/signals2.hpp>

#include "htypes_shared.hpp"
#include "http/webclient.h"
#include <core/util.h>
#include <core/htypes_shared.hpp>

template <typename T>
using SignalsAddSignature = std::tuple<std::shared_ptr<T>, std::string>;

template<typename T, typename C = std::unordered_map<std::string,std::shared_ptr<T>> >
class ResourceManager {
public:
    using SignalsDeferredContainer = std::set<SignalsAddSignature<T>>;

    std::shared_ptr<T> exists( const std::string& _key ) const {
        if ( auto res = resourcesMapper.find( _key ); res != resourcesMapper.end() ) {
            return resources.at(res->second);
        }
        return nullptr;
    };

    std::shared_ptr<T> hashExists( const std::string& _hash ) const {
        if ( auto res = resources.find( _hash ); res != resources.end() ) {
            return res->second;
        }
        return nullptr;
    };

    void update() {
        for ( const auto& s : signalAddElements ) {
            addSignal(s);
        }
        signalAddElements.clear();
    }

    void addToSignal( SignalsDeferredContainer& _container, const SignalsAddSignature<T>& _elem ) {
        _container.emplace(_elem);
    }

    void add( std::shared_ptr<T> _elem, const std::string& _aliasKey = "" ) {
        auto lHash = _elem->Hash();
        resources[lHash] = _elem;
        resourcesMapper[_elem->Hash()] = lHash;
        if ( !_aliasKey.empty() ) resourcesMapper[_aliasKey] = lHash;
        addSignal( { _elem, lHash } );
    }

    void addImmediate( std::shared_ptr<T> _elem, const std::string& _name,
                      const std::string& _hash, const std::string& _aliasKey = "" ) {
        add( _elem, _name, _hash, _aliasKey );
        addSignal( { _elem, _name } );
    }

    void addDeferred( std::shared_ptr<T> _elem, const std::string& _name,
              const std::string& _hash, const std::string& _aliasKey = "" ) {
        add( _elem, _name, _hash, _aliasKey );
        addToSignal( signalAddElements, { _elem, _name } );
    }

    std::shared_ptr<T> getFromHash( const std::string& _hash ) {
        return resources[_hash];
    }

    std::shared_ptr<T> get( const std::string& _key ) {
        if ( auto res = resourcesMapper.find(_key); res != resourcesMapper.end() ) {
            return resources[res->second];
        } else {
            if ( !resources.empty() ) {
                LOGRS("Resource " << _key << " unmapped returning default");
                return resources.begin()->second;
            }
        }
        LOGRS("Resource " << _key << " unmapped and mamanger empty, returning null");
        return nullptr;
    }

    void connect( std::function<void (const SignalsAddSignature<T>&)> _slot ) {
        addSignal.connect( _slot );
    }

protected:
    void add( std::shared_ptr<T> _elem, const std::string& _name,
              const std::string& _hash, const std::string& _aliasKey = "" ) {
        resources[_hash] = _elem;
        resourcesMapper[_name] = _hash;
        if ( !_aliasKey.empty() ) resourcesMapper[_aliasKey] = _hash;
    }

    C& Resources() {
        return resources;
    }
    const C& Resources() const {
        return resources;
    }
private:
    C resources;
    std::unordered_map<std::string, std::string> resourcesMapper;

    SignalsDeferredContainer signalAddElements;
    boost::signals2::signal<void(const SignalsAddSignature<T>&)> addSignal;
};

template< typename B>
class DependencyChain {
public:
    DependencyChain( std::shared_ptr<B> _builder ) {
        builder = _builder;
        mDeps = builder->deps();
    }

    bool isComplete() const {
        return ( ddstatus == DependencyStatus::Complete ||
                 ddstatus == DependencyStatus::CompleteWithErrors );
    }

    void completed( bool allDependenciesLoadedSuccessfully ) {
        ddstatus = allDependenciesLoadedSuccessfully ? DependencyStatus::Complete
                                                     : DependencyStatus::CompleteWithErrors;
    }

    const ddContainer& deps() const {
        return mDeps;
    }

    void execute( bool allDependenciesLoadedSuccessfully ) {
        completed( allDependenciesLoadedSuccessfully );
        if ( allDependenciesLoadedSuccessfully ) {
            builder->assemble();
        }
    };

private:
    ddContainer mDeps;
    DependencyStatus ddstatus = DependencyStatus::Loading;
    std::shared_ptr<B> builder;
};

class DependantBuilder;
class ResourceBuilderBase;
class ResourceBuilderObservable;

using DependencyChainMap = DependencyChain<DependantBuilder>;
using DependencyChainMapPtr = std::shared_ptr<DependencyChainMap>;
using dep_map = std::vector<DependencyChainMapPtr>;

namespace DependencyHandler {
    void update();
    bool needsDependencies( const std::string& _key );

    dep_map& dependencies();
}

namespace DH = DependencyHandler;

using FileCallback = std::function<void(const uint8_p&)>;
using FileStringCallback = std::function<void(const std::string&)>;

bool callbackSuccessfulStatus( DependencyStatus dp );

struct CallbackData {
    virtual void init() {}
    bool hasFinishedLoading() const {
        return ( callbackSuccessfulStatus(status) ||
                 status == DependencyStatus::LoadingFailed );
    }
    bool isComplete() const {
        return ( status == DependencyStatus::CallbackSuccessfullyExectuted ||
                 status == DependencyStatus::CallbackFailedOnExectution );
    }

    std::string asString() const {
        return std::string( reinterpret_cast<const char*>( data.first.get() ), data.second );
    }
    virtual bool needsReload() const = 0;

    DependencyStatus status = DependencyStatus::Loading;
    uint8_p data;
};

struct CallbackDataNoReload : public CallbackData {
    virtual ~CallbackDataNoReload() {}

    bool needsReload() const override { return false; };
};

struct CallbackDataMaker : public CallbackData {
    CallbackDataMaker( std::shared_ptr<ResourceBuilderBase> _builder ) : builder( _builder ) {}

    virtual ~CallbackDataMaker() = default;

    std::shared_ptr<ResourceBuilderBase> builder;
    virtual bool needsReload() const override;
};

struct CallbackDataObservable : public CallbackData {
    CallbackDataObservable( std::shared_ptr<ResourceBuilderObservable> _builder ) : builderObservable( _builder ) {}

    virtual ~CallbackDataObservable() {}

    std::shared_ptr<ResourceBuilderObservable> builderObservable;
    virtual bool needsReload() const override {
        return false;
    };
};

struct FileCallbackHandler {
    virtual bool executeCallback( const std::string& _key, DependencyStatus _status ) = 0;

    bool isComplete() const {
        return cbData->isComplete();
    }
    bool hasCompletedSuccessfully() const {
        return cbData->status == DependencyStatus::CallbackSuccessfullyExectuted;
    }
    bool needsReload() const {
        return cbData->needsReload();
    }

    std::shared_ptr<CallbackData> cbData;
};

struct FileCallbackHandlerSimple : FileCallbackHandler {
    FileCallbackHandlerSimple(std::function<void(const uint8_p&)> _simpleCallback) {
        cbData = std::make_shared<CallbackDataNoReload>();
        simpleCallback = _simpleCallback;
    }

    virtual ~FileCallbackHandlerSimple() {

    }

    bool executeCallback( const std::string& _key, const DependencyStatus _status ) {
        if ( callbackSuccessfulStatus(_status) ) {
            simpleCallback( cbData->data );
            return true;
        }
        return false;
    };

    std::function<void(const uint8_p&)> simpleCallback;
};

struct FileCallbackHandlerMaker : public FileCallbackHandler {
    explicit FileCallbackHandlerMaker( std::shared_ptr<ResourceBuilderBase> _builder );
    virtual ~FileCallbackHandlerMaker() = default;

    bool executeCallback( const std::string& _key, DependencyStatus _status ) override ;
};

struct FileCallbackHandlerObservable : public FileCallbackHandler {
    FileCallbackHandlerObservable( std::shared_ptr<ResourceBuilderObservable> _builder );
    virtual ~FileCallbackHandlerObservable() = default;

    bool executeCallback( const std::string& _key, DependencyStatus _status ) override;
};

template< typename Builder>
std::shared_ptr<FileCallbackHandler> makeHandler( Builder& obj ) {
    return std::make_shared<FileCallbackHandlerMaker>( std::make_shared<Builder>( obj ) );
}

//template< typename Builder>
//std::shared_ptr<FileCallbackHandler> makeHandler( Builder& obj ) {
//    return std::make_shared<FileCallbackHandlerObservable>( std::make_shared<Builder>( obj ) );
//}

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

// Data Exchange
void setCallbackData( const Http::Result& header );

extern dep_map g_deps;
extern std::unordered_map<std::string, std::shared_ptr<FileCallbackHandler> > callbacksDataMap;
extern std::unordered_map<std::string, std::shared_ptr<FileCallbackHandler> > callbacksDataMapExecuted;
