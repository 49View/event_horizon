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

#include "htypes_shared.hpp"
#include "http/webclient.h"
#include <core/util.h>
#include <core/htypes_shared.hpp>

template<typename T, typename C = std::unordered_map<std::string,std::shared_ptr<T>> >
class DependencyMakerPolicy {
public:
    bool exists( const std::string& _key ) const {
        return resources.find( _key) != resources.end();
    };

    void add( std::shared_ptr<T> _elem ) {
        resources[_elem->Name()] = _elem;
    }

    std::shared_ptr<T> get( const std::string& _key ) {
        return resources[_key];
    }

protected:
    C& Resources() {
        return resources;
    }
    const C& Resources() const {
        return resources;
    }
private:
    C resources;
};

class DependencyMaker {
public:
    virtual bool exists( const std::string& _key ) const = 0;
};

#define DEPENDENCY_MAKER_EXIST( _list ) \
public: \
    bool exists( const std::string& _key ) const override { \
    return _list.find( _key) != _list.end(); \
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
class ResourceBuilder;
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
    CallbackDataMaker( std::shared_ptr<ResourceBuilder> _builder, DependencyMaker& _md ) : builder( _builder ), md( _md ) {}

    virtual ~CallbackDataMaker() {}

    std::shared_ptr<ResourceBuilder> builder;
    DependencyMaker& md;
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
    virtual bool executeCallback( const DependencyStatus _status ) = 0;

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

    bool executeCallback( const DependencyStatus _status ) {
        if ( callbackSuccessfulStatus(_status) ) {
            simpleCallback( cbData->data );
            return true;
        }
        return false;
    };

    std::function<void(const uint8_p&)> simpleCallback;
};

struct FileCallbackHandlerMaker : public FileCallbackHandler {
    FileCallbackHandlerMaker( std::shared_ptr<ResourceBuilder> _builder, DependencyMaker& _md );
    virtual ~FileCallbackHandlerMaker() = default;

    bool executeCallback( const DependencyStatus _status );;
};

struct FileCallbackHandlerObservable : public FileCallbackHandler {
    FileCallbackHandlerObservable( std::shared_ptr<ResourceBuilderObservable> _builder );
    virtual ~FileCallbackHandlerObservable() = default;

    bool executeCallback( const DependencyStatus _status );;
};

template< typename Builder>
std::shared_ptr<FileCallbackHandler> makeHandler( Builder& obj, DependencyMaker& _md ) {
    return std::make_shared<FileCallbackHandlerMaker>( std::make_shared<Builder>( obj ), _md );
}

template< typename Builder>
std::shared_ptr<FileCallbackHandler> makeHandler( Builder& obj ) {
    return std::make_shared<FileCallbackHandlerObservable>( std::make_shared<Builder>( obj ) );
}

template<HttpQuery Q, typename T>
Url makeUrl(const std::string& _name) {
    switch (Q) {
        case HttpQuery::JSON:
            return Url::privateAPI(HttpFilePrefix::catalog + T::typeName() + HttpFilePrefix::getname + url_encode( _name ) );
            break;
        case HttpQuery::Binary:
        case HttpQuery::Text: {
            if ( T::usesNotExactQuery() ) {
                return Url::entityContent( T::Version(), _name );
            } else {
                return Url( HttpFilePrefix::get + url_encode( _name ) );
            }
        }
            break;
        default:
            break;
    }
    return Url{};
}

void readRemote( const Url& url, std::shared_ptr<FileCallbackHandler> _handler,
                 Http::ResponseFlags rf = Http::ResponseFlags::None );

template<typename B, HttpQuery Q, typename T = B>
void readRemote( const std::string& _name, B& _builder, DependencyMaker& _md ) {
    readRemote( makeUrl<Q, T>(_name), makeHandler<B>( _builder , _md ) );
}

template<typename B, HttpQuery Q, typename T = B>
void readRemote( const std::string& _name, B& _builder ) {
    readRemote( makeUrl<Q, T>(_name), makeHandler<B>( _builder ) );
}

// Data Exchange
void setCallbackData( const Http::Result& header );

extern dep_map g_deps;
extern std::unordered_map<std::string, std::shared_ptr<FileCallbackHandler> > callbacksDataMap;
extern std::unordered_map<std::string, std::shared_ptr<FileCallbackHandler> > callbacksDataMapExecuted;
