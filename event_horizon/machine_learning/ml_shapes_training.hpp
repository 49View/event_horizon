#pragma once

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/ml.hpp"

class HouseTraining {
public:
	void train( const std::string& trainSetName );
	int predict( const cv::Mat& source );
	void load( const std::string& trainSetFilename );

	cv::Ptr<cv::ml::SVM> Svm() const { return mSvm; }
	void Svm( cv::Ptr<cv::ml::SVM> val ) { mSvm = val; }

private:
	cv::Ptr<cv::ml::SVM> mSvm;
};