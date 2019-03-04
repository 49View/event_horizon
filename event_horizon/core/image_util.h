#pragma once

#include <memory>
#include <cstring>
#include "stb/stb_image.h"
#include "htypes_shared.hpp"

struct RawImage;

namespace imageUtil {
    template<typename T>
    std::unique_ptr<uint8_t[]> decodeFromStream( T* _data, int& width, int& height, int& channels ) {
        std::unique_ptr<uint8_t[]> decodedData = std::make_unique<uint8_t[]>( width * height * channels * sizeof(T) );
        memcpy( decodedData.get(), _data, width * height * channels * sizeof(T) );
        stbi_image_free( _data );
        return decodedData;
    }

    std::unique_ptr<uint8_t[]> resize( const unsigned char* _data, int length, int width, int height,
                                       int& channels, int& bpp );

    void resizeCallbackb64(void* ctx, void*data, int size);

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

    uint8_p bufferToPngMemory( int w, int h, int comp, void* data );
    uint8_p rawToPngMemory( const RawImage& _input );
}
