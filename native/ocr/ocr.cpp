//
//  ocr.cpp
//
//  Created by Dado on 04/03/2017.
//
//

#include "ocr.hpp"

#include <memory>
#include <opencvutils/cvmatutil.hpp>
#include <leptonica/allheaders.h>

#include "core/file_manager.h"

std::string Ocr::ocrTextFromSubRect( const JMATH::Rect2f& _rect ) {
	mApi.SetRectangle( static_cast<int>( _rect.left() ), static_cast<int>( _rect.top() ), static_cast<int>( _rect.width() ), static_cast<int>( _rect.height() ) );

	char *outText = mApi.GetUTF8Text();
	std::string ret( outText );
	//    delete [] outText;

	return ret;
}

std::string Ocr::ocrFromImage() {
	char *outText = mApi.GetUTF8Text();
	std::string ret( outText );
	LOGR( "[ROOM OCR] Candidate name: %s", outText );
	return ret;
}

std::string Ocr::setImage( const cv::Mat& source ) {
    auto channels = source.channels();
    auto w = source.size().width;
    auto h = source.size().height;
    auto bytePerLine = w * channels; //source.step1();
	mApi.SetImage( (uchar*)source.data, w, h, channels, bytePerLine );
	mApi.SetSourceResolution(600);
	return ocrFromImage();
}

Ocr::Ocr() {
	auto ret = mApi.Init( nullptr, "eng", tesseract::OEM_LSTM_ONLY );
	if ( ret != 0 ) {
	    LOGE( "[ERROR] OCR **NOT** Initialized");
	    return;
	}
	mApi.SetPageSegMode( tesseract::PSM_AUTO );
}

std::string Ocr::ocrTextFromImage( const cv::Mat& source ) {
	setImage( source );

	// Get OCR result
	char *outText = mApi.GetUTF8Text();
	std::string ret( outText );
	delete[] outText;

	return ret;
}
