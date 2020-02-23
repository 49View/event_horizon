//
//  ocr.cpp
//
//  Created by Dado on 04/03/2017.
//
//

#include "ocr.hpp"

#include <memory>
#include <core/profiler.h>
#include <opencv2/imgproc.hpp>

namespace OCR {

    void ocrInitEngine( const std::string &dnnModelName, tesseract::TessBaseAPI &tesseract, cv::dnn::Net &dnnNet ) {
        PROFILE_BLOCK( "ocrInitEngine()" );
//        std::locale::global( std::locale( "C" ));
        auto ret = tesseract.Init( "/usr/local/share/opencv4/tesseract", "eng", tesseract::OEM_LSTM_ONLY );
        if ( ret != 0 ) {
            LOGE( "[ERROR] OCR **NOT** Initialized" );
            return;
        }
        tesseract.SetPageSegMode( tesseract::PSM_AUTO );

        dnnNet = cv::dnn::readNet( dnnModelName );
    }

    cv::Mat prepImageForSceneDetection( cv::dnn::Net &net, const cv::Mat &frame ) {
        cv::Mat blob;
        int inpWidth = frame.cols + ( 32 - frame.cols % 32 );
        int inpHeight = frame.rows + ( 32 - frame.rows % 32 );

        cv::Mat frameCol = frame;
        if ( frameCol.channels() != 3 ) {
            cv::cvtColor( frame, frameCol, cv::COLOR_GRAY2BGR );
        }
        cv::dnn::blobFromImage( frameCol, blob, 1.0, cv::Size( inpWidth, inpHeight ),
                                cv::Scalar( 123.68, 116.78, 103.94 ), true, false );
        net.setInput( blob );
        return blob;
    }

    void prepareImage( tesseract::TessBaseAPI &ocrEngine, const cv::Mat &source ) {
        auto channels = source.channels();
        auto w = source.size().width;
        auto h = source.size().height;
        auto bytePerLine = w * channels; //source.step1();
        ocrEngine.SetImage((uchar *) source.data, w, h, channels, bytePerLine );
//        ocrEngine.SetSourceResolution( 600 );
    }

    void textDetection( const cv::Mat &scores, const cv::Mat &geometry, float scoreThresh,
                        std::vector<cv::RotatedRect> &detections, std::vector<float> &confidences ) {
        detections.clear();
        CV_Assert( scores.dims == 4 );
        CV_Assert( geometry.dims == 4 );
        CV_Assert( scores.size[0] == 1 );
        CV_Assert( geometry.size[0] == 1 );
        CV_Assert( scores.size[1] == 1 );
        CV_Assert( geometry.size[1] == 5 );
        CV_Assert( scores.size[2] == geometry.size[2] );
        CV_Assert( scores.size[3] == geometry.size[3] );

        const int height = scores.size[2];
        const int width = scores.size[3];
        for ( int y = 0; y < height; ++y ) {
            const float *scoresData = scores.ptr<float>( 0, 0, y );
            const float *x0_data = geometry.ptr<float>( 0, 0, y );
            const float *x1_data = geometry.ptr<float>( 0, 1, y );
            const float *x2_data = geometry.ptr<float>( 0, 2, y );
            const float *x3_data = geometry.ptr<float>( 0, 3, y );
            const float *anglesData = geometry.ptr<float>( 0, 4, y );
            for ( int x = 0; x < width; ++x ) {
                float score = scoresData[x];
                if ( score < scoreThresh )
                    continue;

                // Decode a prediction.
                // Multiple by 4 because feature maps are 4 time less than input image.
                float offsetX = x * 4.0f, offsetY = y * 4.0f;
                float angle = anglesData[x];
                float cosA = std::cos( angle );
                float sinA = std::sin( angle );
                float h = x0_data[x] + x2_data[x];
                float w = x1_data[x] + x3_data[x];

                cv::Point2f offset( offsetX + cosA * x1_data[x] + sinA * x2_data[x],
                                    offsetY - sinA * x1_data[x] + cosA * x2_data[x] );
                cv::Point2f p1 = cv::Point2f( -sinA * h, -cosA * h ) + offset;
                cv::Point2f p3 = cv::Point2f( -cosA * w, sinA * w ) + offset;
                cv::RotatedRect r( 0.5f * ( p1 + p3 ), cv::Size2f( w, h ), -angle * 180.0f / (float) CV_PI );
                detections.push_back( r );
                confidences.push_back( score );
            }
        }
    }

    std::string ocrTextDetection( tesseract::TessBaseAPI &ocrEngine, cv::dnn::Net &dnnNet, const cv::Mat &source ) {

        auto dnnImage = prepImageForSceneDetection( dnnNet, source );
        std::vector<cv::Mat> outs;
        std::vector<cv::String> outNames( 2 );
        outNames[0] = "feature_fusion/Conv_7/Sigmoid";
        outNames[1] = "feature_fusion/concat_3";
        dnnNet.forward( outs, outNames );

        cv::Mat scores = outs[0];
        cv::Mat geometry = outs[1];
        float confThreshold = 0.5f;
        float nmsThreshold = 0.4f;

        std::vector<cv::RotatedRect> boxes;
        std::vector<float> confidences;
        textDetection( scores, geometry, confThreshold, boxes, confidences );

        // Apply non-maximum suppression procedure.
        std::vector<int> indices;
        cv::dnn::NMSBoxes( boxes, confidences, confThreshold, nmsThreshold, indices );

        // Prep up image to process
        prepareImage( ocrEngine, source );

        // Get OCR result
        char *outText = ocrEngine.GetUTF8Text();
        std::string ret( outText );
        delete[] outText;

        return ret;
    }

    std::string ocrTextRecognition( tesseract::TessBaseAPI &ocrEngine, const cv::Mat &source ) {

        // Prep up image to process
        prepareImage( ocrEngine, source );

        // Get OCR result
        char *outText = ocrEngine.GetUTF8Text();
        std::string ret( outText );
        delete[] outText;

        return ret;
    }

}
