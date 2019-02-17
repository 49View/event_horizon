//
// Created by Dado on 29/08/2018.
//

#include "image_util.h"
#include <core/util.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>
#include <stb/stb_image_write.h>
#include <core/raw_image.h>
#include <core/http/basen.hpp>


namespace imageUtil {

    void resizeCallbackb64(void* ctx, void*data, int size) {
        auto* rawThumbl = reinterpret_cast<std::string*>(ctx);
        std::string img{ reinterpret_cast<const char*>(data), static_cast<size_t>(size) };
        auto p = bn::encode_b64( img );
        *rawThumbl = { p.begin(), p.end() };
    }

    std::unique_ptr<uint8_t[]> resize( const unsigned char* _data, int length, int width, int height,
                                       int& channels, int& bpp ) {
        int sw = 0;
        int sh = 0;
        auto image = decodeFromMemory( _data, length, sw, sh, channels, bpp );
        std::unique_ptr<uint8_t[]> output = std::make_unique<uint8_t[]>( width * height * channels *(bpp/8) );
        stbir_resize_uint8( image.get(), sw, sh, sw*channels*(bpp/8), output.get(),
                            width, height, width*channels*(bpp/8), channels );
        return output;
    }

    std::unique_ptr<uint8_t[]> decodeFromMemoryStream( const unsigned char* _data, int length  ) {
        std::unique_ptr<uint8_t[]> decodedData = std::make_unique<uint8_t[]>( length );
        memcpy( decodedData.get(), _data, length );

        return decodedData;
    }

    std::unique_ptr<uint8_t[]> decodeFromMemory( const unsigned char* _data,  int length, int& width, int& height,
                                                 int& channels, int& bpp, bool _isRaw ) {

        if ( _isRaw ) {
            return decodeFromMemoryStream( _data, length );
        } else {
            bool bHDR = stbi_is_hdr_from_memory( _data, length );
            if ( bHDR ) {
                float *ddata = stbi_loadf_from_memory( _data,length,
                                                       &width, &height, &channels, 0 );
                bpp = 32;
                return decodeFromStream( ddata, width, height, channels );
            } else {
                unsigned char *ddata = stbi_load_from_memory( _data, length,
                                                              &width, &height, &channels, 0 );
                bpp = 8;
                return decodeFromStream( ddata, width, height, channels );
            }
        }
    }

    std::unique_ptr<uint8_t[]> decodeFromMemory( const ucchar_p& _data, int& width, int& height, int& channels,
                                                 int& bpp, bool _isRaw ) {
        return decodeFromMemory( _data.first, static_cast<int>( _data.second ), width, height, channels, bpp, _isRaw );
    }

    std::unique_ptr<uint8_t[]> decodeFromMemory( uint8_p&& _data, int& width, int& height, int& channels, int& bpp,
                                                 bool _isRaw ) {
        return decodeFromMemory( _data.first.get(), static_cast<int>( _data.second ), width, height, channels, bpp,
                                 _isRaw );
    }

    std::unique_ptr<uint8_t[]> memsetImage( uint32_t color, int& width, int& height, int& channels ) {
        width = width == 0 ? 4 : width;
        height = height == 0 ? 4 : height;
        std::unique_ptr<uint8_t[]> decodedData = std::make_unique<uint8_t[]>( width * height * channels );
        for ( auto w = 0; w < width * height * channels; w+=channels ) {
            decodedData[w+0] = static_cast<uint8_t >(color & 0x000000ff);
            if (channels > 1 ) {
                decodedData[w+1] = static_cast<uint8_t >((color & 0x0000ff00) >> 8);
                if ( channels > 2 ) {
                    decodedData[w+2] = static_cast<uint8_t >((color & 0x00ff0000) >> 16);
                    if ( channels > 3 ) {
                        decodedData[w+3] = static_cast<uint8_t >((color & 0xff000000) >> 24);
                    }
                }
            }
        }
        return decodedData;
    }

    std::unique_ptr<uint8_t[]> zeroImage( uint32_t color, int& width, int& height, int& channels ) {
        channels = 4;
        width = 4;
        height = 4;
        std::unique_ptr<uint8_t[]> decodedData = std::make_unique<uint8_t[]>( width * height * channels );
        for ( auto w = 0; w < width * height * channels; w+=channels ) {
            decodedData[w+0] = static_cast<uint8_t >(color & 0x000000ff);
            decodedData[w+1] = static_cast<uint8_t >((color & 0x0000ff00) >> 8);
            decodedData[w+2] = static_cast<uint8_t >((color & 0x00ff0000) >> 16);
            decodedData[w+3] = static_cast<uint8_t >((color & 0xff000000) >> 24);
        }
        return decodedData;
    }

    std::unique_ptr<uint8_t[]> zeroImage3( uint32_t color, int& width, int& height, int& channels ) {
        channels = 3;
        width = 4;
        height = 4;
        std::unique_ptr<uint8_t[]> decodedData = std::make_unique<uint8_t[]>( width * height * channels );
        for ( auto w = 0; w < width * height * channels; w+=channels ) {
            uint8_t r = static_cast<uint8_t>(color & 0x000000ff);
            uint8_t g = static_cast<uint8_t>((color & 0x0000ff00) >> 8);
            uint8_t b = static_cast<uint8_t>((color & 0x00ff0000) >> 16);
            decodedData[w+0] = r;
            decodedData[w+1] = g;
            decodedData[w+2] = b;
        }
        return decodedData;
    }

    std::unique_ptr<uint8_t[]> zeroImage3( uint32_t color, int width, int height ) {
        std::unique_ptr<uint8_t[]> decodedData = std::make_unique<uint8_t[]>( width * height * 3 );
        for ( auto w = 0; w < width * height * 3; w+=3 ) {
            uint8_t r = static_cast<uint8_t>(color & 0x000000ff);
            uint8_t g = static_cast<uint8_t>((color & 0x0000ff00) >> 8);
            uint8_t b = static_cast<uint8_t>((color & 0x00ff0000) >> 16);
            decodedData[w+0] = r;
            decodedData[w+1] = g;
            decodedData[w+2] = b;
        }
        return decodedData;
    }

    uint8_p bufferToPngMemory( int w, int h, int comp, void* data ) {
        uint8_p pngBuffer;
        stbi_write_png_to_func( [](void* ctx, void*data, int size) {
            auto* src = reinterpret_cast<uint8_p*>(ctx);
            src->second = static_cast<uint64_t >(size);
            src->first = std::make_unique<uint8_t[]>(src->second);
            std::memcpy( src->first.get(), data, src->second );
        }, reinterpret_cast<void*>(&pngBuffer), w, h, comp, data, 0 );
        return pngBuffer;
    }

    uint8_p rawToPngMemory( const RawImage& _input ) {
        return bufferToPngMemory( _input.width, _input.height, _input.channels, _input.data() );
    }
}