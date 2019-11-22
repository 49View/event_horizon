//
//  ocr.hpp
//
//  Created by Dado on 04/03/2017.
//
//

#ifndef ocr_hpp
#define ocr_hpp

#include <string>

#include "core/math/rect2f.h"
#include "tesseract/baseapi.h"

namespace cv { class Mat; }

class Ocr {
public:
	static Ocr& getInstance() {
		static Ocr instance; // Guaranteed to be destroyed.
		return instance;// Instantiated on first use.
	}
private:
	Ocr();;                   // Constructor? (the {} brackets) are needed here.
	Ocr( Ocr const& ) = delete;
	void operator=( Ocr const& ) = delete;

public:
	std::string ocrTextFromImage( const cv::Mat& source );
	std::string ocrTextFromSubRect( const JMATH::Rect2f& _rect );
	std::string ocrFromImage();
	std::string setImage( const cv::Mat& source );

private:
	tesseract::TessBaseAPI mApi;
};

#define OCR Ocr::getInstance()

#endif /* ocr_hpp */
