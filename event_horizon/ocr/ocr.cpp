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
	mApi.SetImage( (uchar*)source.data, source.size().width, source.size().height, source.channels(), static_cast<int>( source.step1() ) );
	return ocrFromImage();
}

Ocr::Ocr() {
#ifdef OSX
    const char* trainingRoot = "/usr/local/share/";
#elif defined(LINUX)
	const char* trainingRoot = "/usr/share/";
#elif defined(WIN32)
	const char* trainingRoot = "c:/usr/local/share";
#endif
	auto ret = mApi.Init( trainingRoot, "eng" );
	if ( ret != 0 ) LOGE( "[ERROR] OCR **NOT** Initialized");
}

std::string Ocr::ocrTextFromImage( const cv::Mat& source ) {
	setImage( source );

	// Get OCR result
	char *outText = mApi.GetUTF8Text();
	std::string ret( outText );
	delete[] outText;

	return ret;
}
