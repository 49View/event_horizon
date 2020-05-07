//
//  ocr.hpp
//
//  Created by Dado on 04/03/2017.
//
//

#pragma once

#include <string>

#include "core/math/rect2f.h"
#include "tesseract/baseapi.h"
#include <opencv2/dnn.hpp>

namespace cv { class Mat; }

namespace OCR {
    void ocrInitEngine();
	std::string ocrTextDetection( cv::dnn::Net &dnnNet, const cv::Mat& source );
    std::string ocrTextRecognition( const cv::Mat &source );
};

