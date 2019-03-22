#include <utility>

#include <utility>

#include <utility>

//
// Created by Dado on 2018-10-23.
//

#pragma once

#include <set>

namespace MetaData {
    const static std::string Id         = "_id";
    const static std::string Name       = "name";
    const static std::string Type       = "group";
    const static std::string Hash       = "hash";
    const static std::string Thumb      = "thumb";
    const static std::string Raw        = "raw";
    const static std::string Tags       = "tags";
}

class CoreMetaData {
public:
    CoreMetaData() = default;
    CoreMetaData( std::string name, std::string type, std::string _hash,
                  std::string thumb, std::string raw, std::set<std::string> tags ) :
                      name( std::move( name )),
                      type( std::move( type )),
                      hash( std::move( _hash )),
                      thumb( std::move( thumb )),
                      raw( std::move( raw )),
                      tags( std::move( tags )) {}

    std::string& Name() {
        return name;
    }

    std::string& Type() {
        return type;
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

    const std::string& getName() const {
        return name;
    }

    void setName( const std::string& _name ) {
        CoreMetaData::name = _name;
    }

    const std::string& getType() const {
        return type;
    }

    void setType( const std::string& _type ) {
        CoreMetaData::type = _type;
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

    const std::string& getId() const {
        return id;
    }

    void setId( const std::string& _id ) {
        CoreMetaData::id = _id;
    }

    const std::string& getHash() const {
        return hash;
    }

    void setHash( const std::string& _hash ) {
        CoreMetaData::hash = _hash;
    }

private:
    std::string id;
    std::string name;
    std::string type;
    std::string hash;
    std::string thumb;
    std::string raw;
    std::set<std::string> tags;
};
