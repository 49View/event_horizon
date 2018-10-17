#pragma once

#include "core/raw_image.h"
#include "core/serialization.hpp"

const static int MomentArraySize = 7;

JSONDATA_R( HuMomentsBSData, type, name, source, hus )
    std::string type;
    std::string name;
    std::string source;
    std::vector<std::array<double, MomentArraySize>> hus;
};

class HuMomentsBuilder {
public:
    HuMomentsBuilder() {}
    HuMomentsBuilder( const std::string& _type, const std::string& _name, const std::string& _source,
                      const std::vector<std::array<double, MomentArraySize>>& _value );

    HuMomentsBSData build();
public:
    const std::string& getType() const {
        return type;
    }

    void setType( const std::string& _type ) {
        HuMomentsBuilder::type = _type;
    }

    const std::string& getName() const {
        return name;
    }

    HuMomentsBuilder& setName( const std::string& _name ) {
        HuMomentsBuilder::name = _name;
        return *this;
    }

    const std::string& getSource() const {
        return source;
    }

    HuMomentsBuilder& setSource( const std::string& _source ) {
        HuMomentsBuilder::source = _source;
        return *this;
    }

    const std::vector<std::array<double, 7>>& getHus() const {
        return hus;
    }

    HuMomentsBuilder& setHus( const std::vector<std::array<double, 7>>& _hus ) {
        HuMomentsBuilder::hus = _hus;
        return *this;
    }

private:
    std::string name;
    std::string type;
    std::string source;

private:
    std::vector<std::array<double, MomentArraySize>> hus;
};

namespace HuMomentsService {
    double
    compare( const HuMomentsBSData& _source, const std::vector<std::array<double, MomentArraySize> >& _compareto );
    bool isMostlyStraightLines( const std::vector<std::array<double, MomentArraySize> >& _source );
    void save( const HuMomentsBSData& _source, RawImage& _sourceImage );
};
