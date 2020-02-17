#include "hu_moments.hpp"

#include "core/file_manager.h"
//#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#endif
#include <stb/stb_image_write.h>
#include <opencv4/opencv2/imgproc.hpp>

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

    std::vector<HUV > huMomentsOnImage( const cv::Mat& src_gray, int thresh, double lengthThresh ) {
        std::vector<HUV> hus;

        cv::Mat canny_output;
        std::vector<std::vector<cv::Point>> contoursSource;
        std::vector<cv::Vec4i> hierarchy;
        Canny( src_gray, canny_output, thresh, thresh * 2, 3 );
        cv::findContours( canny_output, contoursSource, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE,
                          cv::Point( 0, 0 ));

//	std::vector<std::vector<cv::Point>> contours = contoursSource;

        std::vector<double> lengths;
        for ( auto& i : contoursSource ) {
            double le = 0;
            for ( size_t m = 0; m < i.size() - 1; m++ ) {
                le += cv::norm( i[m] - i[m + 1] );
            }
            lengths.push_back( le );
        }

        std::vector<std::vector<cv::Point> > contours;
        for ( size_t i = 0; i < contoursSource.size(); i++ ) {
            if ( lengths[i] < lengthThresh ) continue;
            contours.emplace_back();
            cv::approxPolyDP( cv::Mat( contoursSource[i] ), contours[contours.size() - 1],
                              cv::arcLength( cv::Mat( contoursSource[i] ), true ) * 0.005, true );
        }

        std::vector<cv::Moments> mu( contours.size());
        for ( size_t i = 0; i < contours.size(); i++ ) {
            mu[i] = moments( contours[i], false );
            hus.push_back( std::array<double, 7>{} );
            double hu[7];
            HuMoments( mu[i], hu );
            for ( int q = 0; q < 7; q++ ) hus[i][q] = hu[q];
        }

        return hus;
    }

    HUV huMomentsOnImageRaw( const cv::Mat& src_gray ) {

        cv::Mat imgGray;
        cv::Mat imgInv;
        cv::Mat img;

        if ( src_gray.channels() == 1 ) {
            imgGray = src_gray;
        } else {
            cv::cvtColor( src_gray, imgGray, cv::COLOR_BGR2GRAY );
        }

        threshold( imgGray, img, 128, 255, cv::THRESH_BINARY );
        cv::Moments moments = cv::moments( img, true );

        HUV huMoments{};
        HuMoments( moments, huMoments );
        for ( double& huMoment : huMoments ) {
            huMoment = -1 * copysign( 1.0, huMoment ) * log10( abs( huMoment ));
        }

        return huMoments;
    }

}