#include "hu_moments.hpp"

#include "core/file_manager.h"
//#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#endif
#include <stb/stb_image_write.h>

HuMomentsBuilder::HuMomentsBuilder( const std::string& _type, const std::string& _name,
                                    const std::string& _source,
                                    const std::vector<HUV>& _value ) {
    type = _type;
    name = _name;
    source = _source;
    hus = _value;
}

HuMomentsBSData HuMomentsBuilder::build() {
    HuMomentsBSData ret;

    ret.type = type;
    ret.name = name;
    ret.source = source;
    ret.hus = hus;

    return ret;
}

namespace HuMomentsService {

    double compare( const HuMomentsBSData& _source,
                    const std::vector<HUV>& _compareto ) {
        double lookalikeness = std::numeric_limits<double>::max();

        for ( auto& moment : _source.hus ) {
            for ( auto& moment_compare : _compareto ) {
                double ll = 0.0;
                for ( int t = 0; t < MomentArraySize; t++ ) {
                    ll += squareDiff( moment_compare[t], moment[t] );
                }
                if ( ll > 0.0 )
                    lookalikeness = std::min( ll, lookalikeness );
            }
        }

        return lookalikeness;
    }

    double compare( const HUV& _source,
                    const HUV& _compareto ) {
        double lookalikeness = std::numeric_limits<double>::max();

        double ll = 0.0;
        for ( int t = 0; t < MomentArraySize; t++ ) {
            ll += squareDiff( fabs(_source[t]), fabs(_compareto[t]) );
        }
        if ( ll > 0.0 )
            lookalikeness = sqrt(std::min( ll, lookalikeness ));

        return lookalikeness;
    }

    double compare( const std::vector<HUV>& _source, const HUV& _compareto ) {
        auto lookalikeness = std::numeric_limits<double>::max();
        for ( const auto& sv : _source ) {
            auto dist = compare( sv, _compareto );
            if ( dist < lookalikeness ) {
                lookalikeness = dist;
            }
        }
        return lookalikeness;
    }

    bool isMostlyStraightLines( const std::vector<HUV>& _source ) {

        if ( _source.empty()) return false;

        size_t numStraights = 0;
        for ( auto& moment : _source ) {
            double ll = 0.0;
            for ( int t = 0; t < MomentArraySize; t++ ) {
                ll += moment[t];
            }
            if ( ll < 0.01f )
                ++numStraights;
        }

        return numStraights >= _source.size() / 2;
    }

    void save( const HuMomentsBSData& _source, RawImage& _sourceImage ) {
//        std::string tfile = FM::cacheFolder() + "tempmoment.png";
//        stbi_write_png( tfile.c_str(), _sourceImage.width, _sourceImage.height, _sourceImage.channels,
//                        _sourceImage.rawBtyes.get(), _sourceImage.width * _sourceImage.channels );
//        uint64_t l;
//        auto b = FM::readDataFileInternal( tfile, l );
//        FM::writeDataFile( _source.source, reinterpret_cast<const char *>(b.get()), l );
//        _source.save();
    }

}