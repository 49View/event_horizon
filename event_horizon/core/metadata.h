#include <utility>

#include <utility>

#include <utility>

//
// Created by Dado on 2018-10-23.
//

#pragma once

#include <set>
#include <core/serialization.hpp>

class CoreMetaData {
public:
    CoreMetaData() = default;
    CoreMetaData( std::string _name, std::string _group, std::string _hash,
                  std::string _thumb, std::string _raw, std::set<std::string> _tags, ResourceDependencyDict _deps = {} ) :
                      name( std::move( _name )),
                      group( std::move( _group )),
                      hash( std::move( _hash )),
                      thumb( std::move( _thumb )),
                      raw( std::move( _raw )),
                      tags( std::move( _tags )),
                      deps( std::move( _deps )) {}

    std::string& Name() {
        return name;
    }

    std::string& Group() {
        return group;
    }

    std::string& Hash() {
        return hash;
    }

    std::string& Thumb() {
        return thumb;
    }

    std::string& Raw() {
        return raw;
    }

    std::set<std::string>& Tags() {
        return tags;
    }

    ResourceDependencyDict& Deps() {
        return deps;
    }

    const std::string& getName() const {
        return name;
    }

    void setName( const std::string& _name ) {
        CoreMetaData::name = _name;
    }

    const std::string& getGroup() const {
        return group;
    }

    void setGroup( const std::string& _group ) {
        CoreMetaData::group = _group;
    }

    const std::string& getThumb() const {
        return thumb;
    }

    void setThumb( const std::string& _thumb ) {
        CoreMetaData::thumb = _thumb;
    }

    const std::string& getRaw() const {
        return raw;
    }

    void setRaw( const std::string& _raw ) {
        CoreMetaData::raw = _raw;
    }

    const std::set<std::string>& getTags() const {
        return tags;
    }

    void setTags( const std::set<std::string>& _tags ) {
        CoreMetaData::tags = _tags;
    }

    const std::string& getHash() const {
        return hash;
    }

    void setHash( const std::string& _hash ) {
        CoreMetaData::hash = _hash;
    }

    const ResourceDependencyDict& getDeps() const {
        return deps;
    }

    void setDeps( const ResourceDependencyDict& _deps ) {
        CoreMetaData::deps = _deps;
    }

    JSONSERIAL( CoreMetaData, name, group, hash, thumb, raw, tags, deps );
private:
    std::string name;
    std::string group;
    std::string hash;
    std::string thumb;
    std::string raw;
    std::set<std::string> tags;
    ResourceDependencyDict deps;
};
