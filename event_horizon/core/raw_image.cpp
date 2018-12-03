#include "raw_image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <core/math/vector4f.h>
#include <core/http/basen.hpp>
#include "util.h"
#include "file_manager.h"
#include "service_factory.h"
#include "math/rect2f.h"


RawImage RawImage::BLACK_ARGB1x1{ "black_alpha", 1, 1, static_cast<uint32_t>(0xff000000) };
RawImage RawImage::BLACK_RGBA1x1{ "black_alpha", 1, 1, static_cast<uint32_t>(0x000000ff) };

//struct RawImageCallbackData : public CallbackData {
//    using CallbackData::CallbackData;
//    RawImage img;
//};
//
//struct RawImageCallbackHandler : public FileCallbackHandler {
//    using FileCallbackHandler::FileCallbackHandler;
//    virtual bool executeCallback() {
//        auto iData = std::dynamic_pointer_cast<RawImageCallbackData>(cbData);
//        iData->img = rawImageDecodeFromMemory( iData->data.first.get(), iData->data.second );
//        return iData->data.second > 0;
//    };
//};



RawImage::RawImage( const std::string& _name, unsigned int _w, unsigned int _h, const uint32_t _col ) {
    width = _w;
    height = _h;
    channels = 4;
    rawBtyes = std::make_unique<uint8_t[]>( width * height * channels );
    for ( int t = 0; t < height; t++ ){
        for ( int q = 0; q < width; q++ ){
            std::memcpy( rawBtyes.get() + ((t * width * channels) + q*channels), &_col, channels );
        }
    }
    name = _name;
}

RawImage::RawImage( const std::string& _name, unsigned int _w, unsigned int _h, const uint8_t _col ) {
    width = _w;
    height = _h;
    channels = 1;
    rawBtyes = std::make_unique<uint8_t[]>( width * height );
    std::memset( rawBtyes.get(), width * height, _col);
    name = _name;
}

RawImage::RawImage( const std::string& _name, unsigned int _w, unsigned int _h, const float _col ) {
    width = _w;
    height = _h;
    channels = 1;
    auto _colval = static_cast<uint8_t >(clamp( _col * 255.0f, 0.0f, 255.0f) );
    rawBtyes = std::make_unique<uint8_t[]>( width * height );
    std::memset( rawBtyes.get(), width * height, _colval);
    name = _name;
}

RawImage rawImageDecodeFromMemory( const uint8_p& data, const std::string& _name, int forceChannels ) {
    return rawImageDecodeFromMemory( reinterpret_cast<const unsigned char *>(data.first.get()), data.second, _name,
            forceChannels );
}

RawImage rawImageDecodeFromMemory( const unsigned char* buffer, int length, const std::string& _name, int forceChannels ) {
    RawImage ret;

    unsigned char *ddata = stbi_load_from_memory(buffer, length, &ret.width, &ret.height, &ret.channels, forceChannels);
    if ( forceChannels != 0 ) ret.channels = forceChannels;
    ASSERT(ddata);
    ret.rawBtyes = std::make_unique<uint8_t[]>( ret.width * ret.height * ret.channels );
    std::memcpy( ret.rawBtyes.get(), ddata, ret.width * ret.height * ret.channels );
    stbi_image_free(ddata);

    ret.name = _name;
    return ret;
}

void openImageAsRaw( const std::string& _filename, bool bUseImagePrefix ) {
    RawImage imageRet;
    static const std::string imgPrefix = "images/";
    auto filename = bUseImagePrefix ? ( imgPrefix + _filename ) : _filename;

//    FM::readRemoteFile( filename, ServiceFactory::handler<RawImageCallbackHandler>() );
}

RawImage rawImageSubImage( const RawImage& _source, const JMATH::Rect2f& _area,
                           const Vector2i& _extraPadding, const uint8_t& _paddingGradient ) {
    RawImage ret;

    ret.width = static_cast<int>( _area.width() + _extraPadding.x() * 2 );
    ret.height = static_cast<int>( _area.height() + _extraPadding.y() * 2 );
    ret.channels = _source.channels;
    auto bsize = ret.width * ret.height * _source.channels;
    ret.rawBtyes = std::make_unique<uint8_t[]>( bsize );
    std::memset( ret.rawBtyes.get(), _paddingGradient, bsize );
    uint64_t left = static_cast<uint64_t>( _area.left());
    uint64_t top = _source.height - static_cast<uint64_t>( _area.top());
    uint64_t bottom = _source.height - static_cast<uint64_t>( _area.bottom());
    for ( uint64_t y = bottom, y1 = _extraPadding.y(); y < top; y++, y1++ ) {
        std::memcpy( ret.rawBtyes.get() + ( _extraPadding.x() * _source.channels ) +
                     (( y1 * static_cast<uint64_t>( ret.width ) * _source.channels )),
                     _source.rawBtyes.get() + ( y * _source.width * _source.channels ) + ( left * _source.channels ),
                     static_cast<int>( _area.width() * _source.channels ) );
    }

    return ret;
}

RawImage rawImageDecodeFromMemory( const std::string& _base64, const std::string& _name, int forceChannels ) {
    std::vector<unsigned char> rd;
    bn::decode_b64( _base64.begin(), _base64.end(), std::back_inserter(rd) );
    return rawImageDecodeFromMemory( rd.data(), rd.size(),_name, forceChannels );
}

void RawImage::copyFrom( const char *buffer ) {
    auto bsize = width * height * channels;
    rawBtyes = std::make_unique<uint8_t[]>( bsize );
    std::memcpy( rawBtyes.get(), buffer, bsize );
}

RawImage::RawImage( int width,
                    int height,
                    int channels,
                    const char *buffer,
                    const std::string& name ) : width( width ),
                                           height( height ),
                                           channels( channels ),
                                           name(name) {
    copyFrom( buffer );
}

RawImage::RawImage( int width, int height, int channels, const std::string& name ) : width( width ), height( height ),
                                                                                     channels( channels ),name(name) {

    auto bsize = width * height * channels;
    rawBtyes = std::make_unique<uint8_t[]>( bsize );

//    for ( int i = 0; i < height; i++ ) {
//        for ( int j = 0; j < width; j++ ) {
//            rawBtyes[j*width*channels+i*channels] = buffer[step * j + i];
//            rawBtyes[j*width*channels+i*channels+1] = buffer[step * j + i + 1];
//            rawBtyes[j*width*channels+i*channels+2] = buffer[step * j + i + 2];
//        }
//    }

}

void RawImage::grayScale() {
    if ( channels < 3 ) return;

    auto newBtyes = std::make_unique<uint8_t[]>( width*height );

    for ( int y = 0; y < height; y++ ) {
        for ( int x = 0; x < width; x++ ) {

            uint8_t r = rawBtyes[(y*width*channels + (x*channels))+0];
            uint8_t g = rawBtyes[(y*width*channels + (x*channels))+1];
            uint8_t b = rawBtyes[(y*width*channels + (x*channels))+2];

            float gray = 0.2126f * r + 0.7152f * g + 0.0722f * b;
            gray = clamp(gray, 0.0f, 255.0f);
            newBtyes[(y*width) + x] = static_cast<uint8_t>(gray);
        }
    }
    channels = 1;
    rawBtyes = std::move( newBtyes );
}

RawImage RawImage::toGrayScale() const {
    RawImage ret( *this );
    ret.grayScale();
    return ret;
}

void RawImage::brightnessContrast( float _c, int _b ) {

    transform<uint8_t >( [&]( uint8_t& _value ) {
        float factor = (259.0f * (_c + 255.0f)) / (255.0f * (259.0f - _c));
        _value = static_cast<uint8_t>(clamp(((factor * _value - 128 + _b) + 128), 0.0f, 255.0f));
    } );

}

RawImage RawImage::toNormalMap() const {

    RawImage ret( width, height, 4);

    for ( int y = 0; y < height; y++ ) {
        int prevY = y == 0 ?  height - 1 : y - 1;
        int nextY = y == height - 1 ?  0 : y + 1;
        for ( int x = 0; x < width; x++ ) {
            int prevX = x == 0 ?  width - 1 : x - 1;
            int nextX = x == width - 1 ?  0 : x + 1;

            int ynp = at<uint8_t>(x,prevY);
            int ynn = at<uint8_t>(x,nextY);
            int xnp = at<uint8_t>(prevX, y);
            int xnn = at<uint8_t>(nextX, y);

            Vector2f vn2d = Vector2f{ xnp - xnn, ynp - ynn } / 255.0f;
            Vector3f vnt{ vn2d, sqrt(1.0f - vn2d.x()*vn2d.x() - vn2d.y()*vn2d.y())};
            Vector4f vn = vnt * Vector3f{0.5f, 0.5f, 1.0f} + Vector3f(0.5f, 0.5f, 0.0f);
            uint32_t c = vn.RGBATOI();
//            ret.at<uint32_t>(x, y) = c;
            ret.set<uint32_t>(x, y, c);
        }
    }

    return ret;
}

RawImage RawImage::WHITE4x4() {
    return RawImage{ "white", 4, 4, static_cast<uint32_t>(0xffffffff) };
}

