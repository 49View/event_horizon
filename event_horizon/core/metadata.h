//
// Created by Dado on 2018-10-23.
//

#pragma once

#include <set>

namespace MetaData {
    const static std::string Name  = "name";
    const static std::string Type  = "type";
    const static std::string Thumb = "thumb";
    const static std::string Raw   = "raw";
    const static std::string Tags  = "tags";
}

class CoreMetaData {
public:
    CoreMetaData() = default;
    CoreMetaData( const std::string& name, const std::string& type, const std::string& thumb, const std::string& raw,
                  const std::set<std::string>& tags ) : name( name ), type( type ), thumb( thumb ), raw( raw ),
                                                           tags( tags ) {}

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

    void setName( const std::string& name ) {
        CoreMetaData::name = name;
    }

    const std::string& getType() const {
        return type;
    }

    void setType( const std::string& type ) {
        CoreMetaData::type = type;
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

private:
    std::string name;
    std::string type;
    std::string thumb;
    std::string raw;
    std::set<std::string> tags;
};
