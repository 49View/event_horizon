//
// Created by Dado on 11/01/2018.
//

#pragma once

#include <memory>
#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include <typeinfo>
#include <utility>

#include <core/htypes_shared.hpp>

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

    virtual ~FileCallbackHandlerSimple() = default;

    bool executeCallback( const std::string& _key, const DependencyStatus _status ) override {
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
    explicit FileCallbackHandlerObservable( std::shared_ptr<ResourceBuilderObservable> _builder );
    virtual ~FileCallbackHandlerObservable() = default;

    bool executeCallback( const std::string& _key, DependencyStatus _status ) override;
};

template< typename Builder>
std::shared_ptr<FileCallbackHandler> makeHandler( Builder& obj ) {
    return std::make_shared<FileCallbackHandlerMaker>( std::make_shared<Builder>( obj ) );
}

// Data Exchange
void setCallbackData( const Http::Result& header );

extern dep_map g_deps;
extern std::unordered_map<std::string, std::shared_ptr<FileCallbackHandler> > callbacksDataMap;
extern std::unordered_map<std::string, std::shared_ptr<FileCallbackHandler> > callbacksDataMapExecuted;
