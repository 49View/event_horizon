#pragma once

#include "core/raw_image.h"
#include "core/serialization.hpp"

const static int MomentArraySize = 7;

using HUV = std::array<double, MomentArraySize>;
namespace cv { class Mat; };

JSONDATA_R( HuMomentsBSData, type, name, source, hus )

    std::string type;
    std::string name;
    std::string source;
    std::vector<HUV> hus;
};

class HuMomentsBuilder {
public:
    HuMomentsBuilder() = default;

    HuMomentsBuilder( const std::string& _type, const std::string& _name, const std::string& _source,
                      const std::vector<HUV>& _value );

    HuMomentsBSData build();
public:
    [[nodiscard]] const std::string& getType() const {
        return type;
    }

    void setType( const std::string& _type ) {
        HuMomentsBuilder::type = _type;
    }

    [[nodiscard]] const std::string& getName() const {
        return name;
    }

    HuMomentsBuilder& setName( const std::string& _name ) {
        HuMomentsBuilder::name = _name;
        return *this;
    }

    [[nodiscard]] const std::string& getSource() const {
        return source;
    }

    HuMomentsBuilder& setSource( const std::string& _source ) {
        HuMomentsBuilder::source = _source;
        return *this;
    }

    [[nodiscard]] const std::vector<HUV>& getHus() const {
        return hus;
    }

    HuMomentsBuilder& setHus( const std::vector<HUV>& _hus ) {
        HuMomentsBuilder::hus = _hus;
        return *this;
    }

private:
    std::string name;
    std::string type;
    std::string source;

private:
    std::vector<HUV> hus;
};

namespace HuMomentsService {
    double compare( const HuMomentsBSData& _source, const std::vector<HUV>& _compareto );
    double compare( const std::vector<HUV>& _source, const HUV& _compareto );
    double compare( const HUV& _source, const HUV& _compareto );

    std::vector<HUV> huMomentsOnImage( const cv::Mat& src_gray, int thresh = 84, double lengthThresh = 10.0 );
    HUV huMomentsOnImageRaw( const cv::Mat& src_gray );

    bool isMostlyStraightLines( const std::vector<HUV>& _source );
};
