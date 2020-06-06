//
// Created by Dado on 29/08/2018.
//

#include "image_util.h"
#include <core/util.h>
#include <core/string_util.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_resize.h>
#include <stb/stb_image_write.h>
#include <core/raw_image.h>
#include <core/http/basen.hpp>
#include <core/zlib_util.h>


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

    std::unique_ptr<uint8_t[]> resize( const RawImage& si, int width, int height ) {
        std::unique_ptr<uint8_t[]> output = std::make_unique<uint8_t[]>( width * height * si.channels *(si.bpp/8) );
        stbir_resize_uint8( si.data(), si.width, si.height, si.width*si.channels*(si.bpp/8), output.get(),
                            width, height, width*si.channels*(si.bpp/8), si.channels );
        return output;
    }

    std::unique_ptr<uint8_t[]> resize( std::shared_ptr<RawImage> si, int width, int height ) {
        return resize( *si, width, height );
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

    uint8_p bufferToMemoryCompressed( int w, int h, int comp, void* data, const std::string& mineType ) {
        uint8_p pngBuffer = make_uint8_p( w*h*comp );
        pngBuffer.second = 0;
        auto callbackLamdba = [](void* ctx, void* dataToAdd, int size) {
            auto* src = reinterpret_cast<uint8_p*>(ctx);
            std::memcpy( src->first.get() + src->second, dataToAdd, size );
            src->second += static_cast<uint64_t >(size);
        };

        if ( mineTypePNG == mineType ) {
            stbi_write_png_to_func( callbackLamdba, reinterpret_cast<void*>(&pngBuffer), w, h, comp, data, 0 );
        } else if ( mineTypeJPG == mineType || mineTypeJPEG == mineType ) {
            stbi_write_jpg_to_func( callbackLamdba, reinterpret_cast<void*>(&pngBuffer), w, h, comp, data, 95 );
        }
        return pngBuffer;
    }

    std::string bufferToPng64( int w, int h, int comp, void* data ) {

        auto res = bufferToMemoryCompressed( w, h, comp, data, mineTypePNG );
        auto rawm = bn::encode_b64( SerializableContainer{res.first.get(), res.first.get() + res.second} );
        return std::string{ rawm.begin(), rawm.end() };
    }

    uint8_p rawToPngMemory( const RawImage& _input ) {
        return bufferToMemoryCompressed( _input.width, _input.height, _input.channels, _input.data(), mineTypePNG );
    }

    std::string rawToPng64gzip( const RawImage& _input ) {
        auto dtc = imageUtil::rawToPngMemory( _input );
        return zlibUtil::rawb64gzip( SerializableContainer{ dtc.first.get(), dtc.first.get() + dtc.second } );
    }

    std::string rawToPng64gzip( std::shared_ptr<RawImage> _input ) {
        return rawToPng64gzip( *_input );
    }

    std::string rawResizeToPng64gzip( const RawImage& dt, int tw, int th ) {
        auto dtcr = imageUtil::resize(dt, tw, th );
        auto dtc = imageUtil::bufferToMemoryCompressed( tw, th, dt.channels, dtcr.get(), mineTypePNG );
        auto sc = SerializableContainer{ dtc.first.get(), dtc.first.get() + dtc.second };
        return zlibUtil::rawb64gzip(sc);
    }

    std::string rawResizeToPng64gzip( std::shared_ptr<RawImage> dt, int tw, int th ) {
        return rawResizeToPng64gzip( *dt, tw, th );
    }

    std::string extToMime(const std::string &ext) {
        std::string extLo = toLower( ext );
        if ( extLo == ".png" ) {
            return mineTypePNG;
        }
        if ( extLo == ".jpg" || extLo == ".jpeg" ) {
            return mineTypeJPG;
        }
        return {};
    }

    std::string makeThumbnail64( unsigned char *input_data, int w, int h, int n, int thumbSize ) {
        if ( input_data == nullptr ) return {};
        auto output_data_thumb = make_uint8_p(thumbSize * thumbSize * n);
        stbir_resize_uint8(input_data, w, h, 0, output_data_thumb.first.get(),
                           thumbSize, thumbSize, 0, n);
        return imageUtil::bufferToPng64(thumbSize, thumbSize, n, output_data_thumb.first.get());
    }

    std::string makeThumbnail64( const std::string& filename, int thumbSize ) {
        int w = 0, h = 0, n = 0;
        unsigned char *input_data = stbi_load(filename.c_str(), &w, &h, &n, 0);
        return makeThumbnail64( input_data, w, h, n, thumbSize );
    }

}