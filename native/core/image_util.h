#pragma once

#include <memory>
#include <cstring>
#include "stb/stb_image.h"
#include "htypes_shared.hpp"

class RawImage;

namespace imageUtil {

    static const std::string mineTypePNG = "image/png";
    static const std::string mineTypeJPG = "image/jpg";
    static const std::string mineTypeJPEG = "image/jpeg";

    std::string extToMime( const std::string& ext );

    template<typename T>
    std::unique_ptr<uint8_t[]> decodeFromStream( T* _data, int width, int height, int channels ) {
        auto sizeOf = sizeof(T);
        std::unique_ptr<uint8_t[]> decodedData = std::make_unique<uint8_t[]>( width * height * channels * sizeOf );
        memcpy( decodedData.get(), _data, width * height * channels * sizeOf );
        stbi_image_free( _data );
        return decodedData;
    }

    std::unique_ptr<uint8_t[]> resize( const unsigned char* _data, int length, int width, int height,
                                       int& channels, int& bpp );

    std::unique_ptr<uint8_t[]> resize( std::shared_ptr<RawImage>, int width, int height );
    std::unique_ptr<uint8_t[]> resize( const RawImage&, int width, int height );

    std::unique_ptr<uint8_t[]> decodeFromMemoryStream( const unsigned char* _data, int length  );
    std::unique_ptr<uint8_t[]> decodeFromMemory( const unsigned char* _data,  int length, int& width, int& height,
                                                 int& channels, int& bpp, bool _isRaw = false );
    std::unique_ptr<uint8_t[]> decodeFromMemory( const ucchar_p& _data, int& width, int& height, int& channels,
                                                 int& bpp, bool _isRaw = false );
    std::unique_ptr<uint8_t[]> decodeFromMemory( uint8_p&& _data, int& width, int& height, int& channels, int& bpp,
                                                 bool _isRaw = false );
    std::unique_ptr<uint8_t[]> memsetImage( uint32_t color, int& width, int& height, int& channels );
    std::unique_ptr<uint8_t[]> zeroImage( uint32_t color, int& width, int& height, int& channels );
    std::unique_ptr<uint8_t[]> zeroImage3( uint32_t color, int& width, int& height, int& channels );
    std::unique_ptr<uint8_t[]> zeroImage3( uint32_t color, int width, int height );

    uint8_p bufferToMemoryCompressed( int w, int h, int comp, void* data, const std::string& mineType );
    std::string bufferToPng64( int w, int h, int comp, void* data );
    std::string rawToPng64gzip( const RawImage& _input );
    std::string rawToPng64gzip( std::shared_ptr<RawImage> _input );
    std::string rawResizeToPng64gzip( const RawImage& _input, int tw, int th );
    std::string rawResizeToPng64gzip( std::shared_ptr<RawImage> _input, int tw, int th );

    std::string makeThumbnail64( unsigned char *input_data, int w, int h, int n, int thumbSize = 128 );
    std::string makeThumbnail64( const std::string& filename, int thumbSize = 128 );
}
