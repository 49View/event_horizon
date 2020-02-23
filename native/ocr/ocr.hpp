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
    void ocrInitEngine( const std::string& dnnModelName, tesseract::TessBaseAPI& tesseract, cv::dnn::Net& dnnNet );
	std::string ocrTextDetection( tesseract::TessBaseAPI& ocrEngine, cv::dnn::Net &dnnNet, const cv::Mat& source );
    std::string ocrTextRecognition( tesseract::TessBaseAPI &ocrEngine, const cv::Mat &source );
};

