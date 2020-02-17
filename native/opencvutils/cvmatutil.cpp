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
