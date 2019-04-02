//
// Created by Dado on 2019-03-20.
//

#pragma once

#include <unordered_set>
#include <boost/signals2.hpp>
#include <core/util.h>
#include <core/resources/resource_utils.hpp>

using CommandResouceCallbackFunction = std::function<void(const std::vector<std::string>&)>;
using ResourceMapperDict = std::unordered_map<std::string, ResourceRef>;
using DependencyDict = std::unordered_map<std::string, std::unordered_map<std::string, bool>>;
using ResourceDependencyMap = std::unordered_map<std::string, std::string>;

template<typename T, typename C = ResourceManagerContainer<T>>
class ResourceManager {
public:
    std::shared_ptr<T> exists( const std::string& _key ) const {
        if ( auto res = resourcesMapper.find( _key ); res != resourcesMapper.end() ) {
            return resources.at(res->second);
        }
        return nullptr;
    };

    ResourceRef hash( const std::string& _key ) const {
        if ( auto res = resourcesMapper.find( _key ); res != resourcesMapper.end() ) {
            return res->second;
        }
        return "";
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

    void addToSignal( SignalsDeferredContainer<T>& _container, const ResourceSignalsAddSignature<T>& _elem ) {
        _container.emplace(_elem);
    }

    void add( std::shared_ptr<T> _elem, const std::string& _name,
                       const std::string& _hash, AddResourcePolicy _arp, const std::string& _aliasKey = "" ) {
        if ( _arp == AddResourcePolicy::Deferred ) {
            addDeferred( _elem, _name, _hash, _aliasKey );
        } else {
            addImmediate( _elem, _name, _hash, _aliasKey );
        }
    }

    void addImmediate( std::shared_ptr<T> _elem, const std::string& _name,
                       const std::string& _hash, const std::string& _aliasKey = "" ) {
        add( _elem, _name, _hash, _aliasKey );
        addSignal( { _elem, {_name, _hash, _aliasKey} } );
    }

    void addDeferred( std::shared_ptr<T> _elem, const std::string& _name,
                      const ResourceRef& _hash, const std::string& _aliasKey = "" ) {
        add( _elem, _name, _hash, _aliasKey );
        addToSignal( signalAddElements, { _elem, {_name, _hash, _aliasKey} } );
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

    void connect( std::function<void (const ResourceSignalsAddSignature<T>&)> _slot ) {
        addSignal.connect( _slot );
    }

    std::vector<std::shared_ptr<T>> list() const {
        std::vector<std::shared_ptr<T>> ret;
        for ( const auto& [k, v] : Resources() ) {
            ret.push_back(v);
        }
        return ret;
    }

    const C& listResources() const {
        return Resources();
    }

    void addDependencyHook( const std::string& _k, const std::string& _v ) {
        dependencyDict[_k].emplace(_v, false);
    }

    void tagDependencyLoaded( const std::string& _k, const std::string& _v ) {
        if ( auto it = dependencyDict.find(_k); it != dependencyDict.end() ) {
            it->second[_v] = true;
        }
    }

    bool checkDependencyCompleted( const std::string& _k, ResourceDependencyMap& _ret ) {
        if ( auto it = dependencyDict.find(_k); it != dependencyDict.end() ) {
            for ( const auto& [k,v] : it->second ) {
                if ( !v ) return false;
                _ret.emplace( k, hash(k) );
            }
            // returns true, after deleting the entry
            dependencyDict.erase(_k);
            return true;
        }
        return false;
    }

protected:
    void add( std::shared_ptr<T> _elem, const std::string& _name,
              const std::string& _hash, const std::string& _aliasKey = "" ) {
        resources[_hash] = _elem;
        resourcesMapper[_hash] = _hash;
        if ( !_name.empty() ) resourcesMapper[_name] = _hash;
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
    ResourceMapperDict resourcesMapper;
    DependencyDict dependencyDict;

    SignalsDeferredContainer<T> signalAddElements;
    boost::signals2::signal<void(const ResourceSignalsAddSignature<T>&)> addSignal;
};
