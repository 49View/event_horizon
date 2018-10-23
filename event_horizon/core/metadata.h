//
// Created by Dado on 2018-10-23.
//

#pragma once

namespace MetaData {
    const static std::string Name = "name";
    const static std::string Type = "type";
    const static std::string Thumb = "thumb";
    const static std::string Raw = "raw";
}

class CoreMetaData {
public:
    CoreMetaData() = default;
    explicit CoreMetaData( const std::string& _n, const std::string& _e, std::string&& _t, std::string&& _r ) :
                  name(_n), type(_e), thumb(_t), raw(_r) {}

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

private:
    std::string name;
    std::string type;
    std::string thumb;
    std::string raw;
};
