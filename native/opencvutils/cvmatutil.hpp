//
//  cvmatutil_hpp
//
//  Created by Dado on 14/06/2016.
//
//

#pragma once

#include <vector>
#include <array>
#include <algorithm>
#include "opencv2/imgproc.hpp"
#include <opencv2/highgui.hpp>
#include "core/math/vector2f.h"

namespace cv { class Mat; }
struct RawImage;

void toGrayScale( const cv::Mat& _source, cv::Mat& _dest );

void add1PixelWhiteContour( const cv::Mat& source, cv::Mat& dest );
void add1PixelWhiteContour( const cv::Mat& source, cv::Mat& dest, const int numberOfPixels, const int scalar );

cv::Mat decodeRawImageDataIntoMat( int width, int height, int channels, unsigned char* ddata );

cv::Mat decodeImageIntoMat( const std::string& _floorplanName );
cv::Mat decodeRawImageIntoMat( const RawImage& _ri );

ImageParams getImageParamsFromMat( const cv::Mat& _source );

std::vector<std::array<double, 7> >
huMomentsOnImage( const cv::Mat& src_gray, int thresh = 84, double lengthThresh = 10.0 );

void convertContoursArrayToFloats( const std::vector<std::vector<cv::Point2i> >& contoursi,
                                   std::vector<std::vector<Vector2f> >& contours );
void convertContoursArrayToInt( const std::vector<std::vector<Vector2f> >& contoursi,
                                std::vector<std::vector<cv::Point2i> >& contours );


struct ImageBuilderCVMat : public ImageBuilder {
    ImageBuilderCVMat( const std::string& _name, cv::Mat& source ) : ImageBuilder(_name), mSource(source){}

    bool makeDirect( DependencyMaker& _md );
protected:
    cv::Mat& mSource;
};
