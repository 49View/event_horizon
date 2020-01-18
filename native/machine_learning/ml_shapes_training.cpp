#include <iostream>

#include "ml_shapes_training.hpp"

#include "core/file_manager.h"

//cv::Mat deskew( cv::Mat& img ) {
//	float affineFlags = cv::WARP_INVERSE_MAP | cv::INTER_LINEAR;
//
//	cv::Moments m = moments( img );
//	if ( std::abs( m.mu02 ) < 1e-2 ) {
//		return img.clone();
//	}
//	float skew = m.mu11 / m.mu02;
//	cv::Mat warpMat = ( cv::Mat_<float>( 2, 3 ) << 1, skew, -0.5*SZ*skew, 0, 1, 0 );
//	cv::Mat imgOut = cv::Mat::zeros( img.rows, img.cols, img.type() );
//	warpAffine( img, imgOut, warpMat, imgOut.size(), affineFlags );
//
//	return imgOut;
//}

void loadTrainSet( const std::string &pathName, int label, std::vector<cv::Mat> &trainCells, std::vector<int> &trainLabels ) {
	cv::Mat img = cv::imread( "data/SURFData/" + pathName, cv::IMREAD_GRAYSCALE );

	for ( auto t = 0; t < img.cols; t += 64 ) {
		cv::Mat sample = img.colRange( t, t + 64 ).clone();
		trainCells.push_back( sample );
		trainLabels.push_back( label );
	}

	//cv::Mat img = cv::imread( FM::DataRoot() + pathName, CV_LOAD_IMAGE_GRAYSCALE );
	//int ImgCount = 0;
	//for ( int i = 0; i < img.rows; i = i + SZ ) {
	//	for ( int j = 0; j < img.cols; j = j + SZ ) {
	//		cv::Mat digitImg = ( img.colRange( j, j + SZ ).rowRange( i, i + SZ ) ).clone();
	//		if ( j < int( 0.9*img.cols ) ) {
	//			trainCells.push_back( digitImg );
	//		} else {
	//			testCells.push_back( digitImg );
	//		}
	//		ImgCount++;
	//	}
	//}

	//std::cout << "Image Count : " << ImgCount << std::endl;
	//float digitClassNumber = 0;

	//for ( int z = 0; z<int( 0.9*ImgCount ); z++ ) {
	//	if ( z % 450 == 0 && z != 0 ) {
	//		digitClassNumber = digitClassNumber + 1;
	//	}
	//	trainLabels.push_back( digitClassNumber );
	//}
	//digitClassNumber = 0;
	//for ( int z = 0; z<int( 0.1*ImgCount ); z++ ) {
	//	if ( z % 50 == 0 && z != 0 ) {
	//		digitClassNumber = digitClassNumber + 1;
	//	}
	//	testLabels.push_back( digitClassNumber );
	//}
}

void CreateTrainTestHOG( std::vector<std::vector<float> > &trainHOG, std::vector<cv::Mat> &cell ) {
	for ( size_t y = 0; y < cell.size(); y++ ) {
		std::vector<float> descriptors;
		cv::HOGDescriptor hog(
			cv::Size( cell[y].cols, cell[y].rows ), //winSize
			cv::Size( cell[y].cols / 2, cell[y].rows / 2 ), //blocksize
			cv::Size( cell[y].cols / 4, cell[y].rows / 4 ), //blockStride,
			cv::Size( cell[y].cols / 2, cell[y].rows / 2 ), 9 );

		hog.compute( cell[y], descriptors );
		trainHOG.push_back( descriptors );
	}

	//for ( int y = 0; y < deskewedtestCells.size(); y++ ) {
	//	std::vector<float> descriptors;
	//	hog.compute( deskewedtestCells[y], descriptors );
	//	testHOG.push_back( descriptors );
	//}
}

void ConvertvectortoMatrix( std::vector<std::vector<float> > &trainHOG, cv::Mat &trainMat ) {
	int descriptor_size = static_cast<int>( trainHOG[0].size() );

	for ( int i = 0; i < static_cast<int>(trainHOG.size()); i++ ) {
		for ( int j = 0; j < descriptor_size; j++ ) {
			trainMat.at<float>( i, j ) = trainHOG[i][j];
		}
	}
	//for ( int i = 0; i < testHOG.size(); i++ ) {
	//	for ( int j = 0; j < descriptor_size; j++ ) {
	//		testMat.at<float>( i, j ) = testHOG[i][j];
	//	}
	//}
}

void getSVMParams( cv::ml::SVM *svm ) {
	std::cout << "Kernel type     : " << svm->getKernelType() << std::endl;
	std::cout << "Type            : " << svm->getType() << std::endl;
	std::cout << "C               : " << svm->getC() << std::endl;
	std::cout << "Degree          : " << svm->getDegree() << std::endl;
	std::cout << "Nu              : " << svm->getNu() << std::endl;
	std::cout << "Gamma           : " << svm->getGamma() << std::endl;
}

void SVMevaluate( cv::Mat &testResponse, float &count, float &accuracy, std::vector<int> &testLabels ) {
	for ( int i = 0; i < testResponse.rows; i++ ) {
		//std::cout << testResponse.at<float>(i,0) << " " << testLabels[i] << std::endl;
		if ( testResponse.at<float>( i, 0 ) == testLabels[i] ) {
			count = count + 1;
		}
	}
	accuracy = ( count / testResponse.rows ) * 100;
}

void HouseTraining::train( const std::string& trainSetName ) {
	// Example implementation, crashing
	std::vector<cv::Mat> trainCells;
	std::vector<cv::Mat> testCells;
	std::vector<int> trainLabels;
	std::vector<int> testLabels;

	loadTrainSet( "mc_doors64.png", 0, trainCells, trainLabels );
	loadTrainSet( "mc_windows64.png", 1, trainCells, trainLabels );

	//std::vector<cv::Mat> deskewedTrainCells;
	//std::vector<cv::Mat> deskewedTestCells;
	//CreateDeskewedTrainTest( deskewedTrainCells, deskewedTestCells, trainCells, testCells );

	std::vector<std::vector<float> > trainHOG;
	CreateTrainTestHOG( trainHOG, trainCells );

	int descriptor_size = static_cast<int>( trainHOG[0].size() );
	//std::cout << "Descriptor Size : " << descriptor_size << std::endl;

	cv::Mat trainMat( static_cast<int>( trainHOG.size() ), descriptor_size, CV_32FC1 );

	ConvertvectortoMatrix( trainHOG, trainMat );

	mSvm = cv::ml::SVM::create();
	mSvm->setGamma( 0.50625 );
	mSvm->setC( 12.5 );
	mSvm->setKernel( cv::ml::SVM::RBF );
	mSvm->setType( cv::ml::SVM::C_SVC );
	cv::Ptr<cv::ml::TrainData> td = cv::ml::TrainData::create( trainMat, cv::ml::ROW_SAMPLE, trainLabels );
	//mSvm->train( td );
	mSvm->trainAuto( td );
	mSvm->save( "data/SURFData/" + trainSetName + ".yml" );

	//	getSVMParams( svm );
}

int HouseTraining::predict( const cv::Mat& source ) {
	cv::Mat testResponse;

	cv::Mat testImg;
	cv::resize( source, testImg, cv::Size( 64, 64 ) );
	std::vector<float> descriptors;
	cv::HOGDescriptor hogTest(
		cv::Size( testImg.size() ), //winSize
		cv::Size( testImg.size() / 2 ), //blocksize
		cv::Size( testImg.size() / 4 ), //blockStride,
		cv::Size( testImg.size() / 2 ), //cellSize,
		9 );

	hogTest.compute( testImg, descriptors );
	cv::Mat testMat( 1, static_cast<int>( descriptors.size() ), CV_32F );
	for ( size_t j = 0; j < descriptors.size(); j++ ) {
		testMat.at<float>( 0, j ) = descriptors[j];
	}

//	float acc = -1.0f;
	/*acc =*/ mSvm->predict( testMat, testResponse );
	//std::cout << testResponse.at<float>( 0, 0 ) << " Accuracy " << acc << std::endl;
	return static_cast<int>( testResponse.at<float>( 0, 0 ) );
}

void HouseTraining::load( const std::string & trainSetFilename ) {
	if ( mSvm == nullptr ) {
		//	static cv::Ptr<cv::ml::SVM> ret = mSvm->load( FM::DataRoot() + "SURFData/" + trainSetFilename + ".yml" );
		std::string filename = +"SURFData/" + trainSetFilename + ".yml";
		if ( FM::fileExist( filename ) ) {
			cv::String cfname = "data/" + filename;
#ifndef OSX
			mSvm = cv::ml::SVM::load( cfname );
#else
#ifndef NDEBUG
			mSvm = cv::ml::SVM::load( cfname );
#else
			mSvm = cv::ml::SVM::load( cfname );
#endif
#endif
		} else {
			mSvm = cv::ml::SVM::create();
			train( trainSetFilename );
		}
	}
}
