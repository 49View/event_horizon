//
//  cvmatutil_cpp
//
//  Created by Dado on 14/06/2016.
//
//

#include "cvmatutil.hpp"

#include "core/file_manager.h"
#include "core/util.h"
#include <stb/stb_image.h>
#include "core/raw_image.h"

void toGrayScale( const cv::Mat& _source, cv::Mat& _dest ) {
    if ( _source.channels() == 1 ) {
        _dest = _source;
    } else {
        cv::cvtColor( _source, _dest, cv::COLOR_BGR2GRAY );
    }
}

void add1PixelWhiteContour( const cv::Mat& source, cv::Mat& dest ) {
    dest = cv::Mat( source.size() + cv::Size( 2, 2 ), source.type());
    dest.setTo( cv::Scalar( 255, 255, 255 ));
    source.copyTo( dest( cv::Rect( 1, 1, source.cols, source.rows )));
}

cv::Mat decodeRawImageDataIntoMat( int width, int height, int channels, unsigned char *ddata ) {
    uint8_t *retainedBufferForOpenCVMat = new uint8_t[height * width * channels];
    std::memcpy( retainedBufferForOpenCVMat, ddata, height * width * channels );

    int cvChannel = CV_8U;
    switch ( channels ) {
        case 1:
            cvChannel = CV_8UC1;
            break;
        case 2:
            cvChannel = CV_8UC2;
            break;
        case 3:
            cvChannel = CV_8UC3;
            break;
        case 4:
            cvChannel = CV_8UC4;
            break;
    }

    return cv::Mat( height, width, cvChannel, retainedBufferForOpenCVMat );
}

cv::Mat decodeImageIntoMat( const std::string& _filename ) {

    int channels = 0;
    int width = 0;
    int height = 0;

    // we need to copy this buffer over because opencv is crap at modern c++
    unsigned char *ddata = stbi_load( _filename.c_str(), &width, &height, &channels, 0 );

    cv::Mat pngImage = decodeRawImageDataIntoMat( width, height, channels, ddata );
    stbi_image_free( ddata );
    return pngImage;
}

cv::Mat decodeRawImageIntoMat( const RawImage& _ri ) {
    return decodeRawImageDataIntoMat( _ri.width, _ri.height, _ri.channels, _ri.rawBtyes.get());
}

std::vector<std::array<double, 7> > huMomentsOnImage( const cv::Mat& src_gray, int thresh, double lengthThresh ) {
    std::vector<std::array<double, 7> > hus;

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

std::array<double, 7> huMomentsOnImageRaw( const cv::Mat& src_gray ) {

    cv::Mat imgGray;
    cv::Mat imgInv;
    cv::Mat img;
    toGrayScale(src_gray, imgGray);
    cv::bitwise_not(imgGray, img);
    threshold( img, img, 128, 255, cv::THRESH_BINARY );
    cv::Moments moments = cv::moments( img, false );

    std::array<double, 7> huMoments{};
    HuMoments( moments, huMoments );
    for ( double& huMoment : huMoments ) {
        huMoment = -1 * copysign( 1.0, huMoment ) * log10( abs( huMoment ));
    }

    return huMoments;
}

void convertContoursArrayToFloats( const std::vector<std::vector<cv::Point2i> >& contoursi,
                                   std::vector<std::vector<Vector2f> >& contours ) {
    for ( unsigned long i = 0; i < contoursi.size(); i++ ) {
        if ( contoursi[i].size() < 4 ) continue;
        std::vector<Vector2f> vf;
        vf.clear();
        for ( auto p : contoursi[i] ) {
            Vector2f v = Vector2f( p.x, p.y );
            vf.push_back( v );
        }
        contours.push_back( vf );
    }
}

void convertContoursArrayToInt( const std::vector<std::vector<Vector2f> >& contoursi,
                                std::vector<std::vector<cv::Point2i> >& contours ) {
    for ( unsigned long i = 0; i < contoursi.size(); i++ ) {
        if ( contoursi[i].size() < 4 ) continue;
        std::vector<cv::Point2i> vf;
        for ( auto p : contoursi[i] ) {
            cv::Point2i v = cv::Point2i( static_cast<int>( p.x()), static_cast<int>( p.y()));
            vf.push_back( v );
        }
        contours.push_back( vf );
    }
}

void add1PixelWhiteContour( const cv::Mat& source, cv::Mat& dest, const int numberOfPixels, const int scalar ) {
    dest = cv::Mat( source.size() + cv::Size( numberOfPixels, numberOfPixels ), source.type());
    dest.setTo( scalar );
    source.copyTo( dest( cv::Rect( numberOfPixels / 2 - 1, numberOfPixels / 2 - 1, source.cols, source.rows )));
}

ImageParams getImageParamsFromMat( const cv::Mat& _source ) {
    int origSize = _source.total() * _source.elemSize();
    int bpp = ( origSize / ( _source.cols * _source.rows * _source.channels())) * 8;
    return { _source.cols, _source.rows, _source.channels(), bpp };
}


//bool ImageBuilderCVMat::makeDirect( DependencyMaker& _md ) {
//	setImageParams(getImageParamsFromMat(mSource));
//	raw( true );
//	return ImageBuilder::makeDirect( _md, ucchar_p{mSource.data, mSource.total() * mSource.elemSize()} );
//}
