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
    const static std::string Hash       = "hash";
    const static std::string Type       = "group";
    const static std::string ContentId  = "content_id";
    const static std::string Thumb      = "thumb";
    const static std::string Version    = "version";
    const static std::string Raw        = "raw";
    const static std::string Tags       = "tags";
}

class CoreMetaData {
public:
    CoreMetaData() = default;
    CoreMetaData( std::string name, std::string _hash, std::string type, std::string _contentId, uint64_t version,
                  std::string thumb, std::string raw, std::set<std::string> tags ) :
                      name( std::move( name )),
                      hash( std::move( _hash )),
                      type( std::move( type )),
                      contentId( std::move( _contentId )),
                      version( version ),
                      thumb( std::move( thumb )),
                      raw( std::move( raw )),
                      tags( std::move( tags )) {}

    std::string& Name() {
        return name;
    }

    std::string& Type() {
        return type;
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

    std::string getHash() const {
        return hash;
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

    void setContentId( const std::string& _cid ) {
        contentId = _cid;
    }

    uint64_t getVersion() const {
        return version;
    }

    void setVersion( uint64_t _version ) {
        CoreMetaData::version = _version;
    }

    std::string getContentId() const {
        return contentId;
    }

    const std::string& getThumb() const {
        return thumb;
    }

    void setThumb( const std::string& thumb ) {
        CoreMetaData::thumb = thumb;
    }

    const std::string& getRaw() const {
        return raw;
    }

    void setRaw( const std::string& raw ) {
        CoreMetaData::raw = raw;
    }

    const std::set<std::string>& getTags() const {
        return tags;
    }

    void setTags( const std::set<std::string>& tags ) {
        CoreMetaData::tags = tags;
    }

    const std::string& getId() const {
        return id;
    }

    void setId( const std::string& id ) {
        CoreMetaData::id = id;
    }

    void setHash( const std::string& _hash ) {
        hash = _hash;
    }

private:
    std::string id;
    std::string name;
    std::string hash;
    std::string type;
    std::string contentId;
    uint64_t    version = 0;
    std::string thumb;
    std::string raw;
    std::set<std::string> tags;
};
