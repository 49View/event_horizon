#include "lut_3d.hpp"
#include "raw_image.h"

#include <core/image_util.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <core/math/vector4f.h>
#include <core/math/rect2f.h>
#include <core/http/basen.hpp>
#include <core/util.h>
#include <core/serializebin.hpp>
#include <core/image_params.hpp>

#define TINYEXR_IMPLEMENTATION
#include <core/tinyexr.h>

RawImage::RawImage( unsigned int _w, unsigned int _h, int _channels, const uint32_t _col ) {
    width = _w;
    height = _h;
    channels = _channels;
    rawBtyes = std::make_unique<uint8_t[]>( memorySize());
    for ( int t = 0; t < height; t++ ) {
        for ( int q = 0; q < width; q++ ) {
            std::memcpy( rawBtyes.get() + (( t * width * channels ) + q * channels ), &_col, channels );
        }
    }
}

RawImage::RawImage( unsigned int _w, unsigned int _h, const V4f& _value, int _bpp ) {
    width = _w;
    height = _h;
    channels = 4;
    bpp = _bpp;
    rawBtyes = std::make_unique<uint8_t[]>( memorySize());
    for ( int t = 0; t < height; t++ ) {
        for ( int q = 0; q < width; q++ ) {
            std::memcpy( rawBtyes.get() + (( t * width * channels ) + q * bppStride()), &_value, bppStride());
        }
    }
}

RawImage::RawImage( unsigned int _w, unsigned int _h, const uint8_t _col ) {
    width = _w;
    height = _h;
    channels = 1;
    rawBtyes = std::make_unique<uint8_t[]>( memorySize());
    std::memset( rawBtyes.get(), memorySize(), _col );
}

RawImage::RawImage( unsigned int _w, unsigned int _h, const float _col ) {
    width = _w;
    height = _h;
    channels = 1;
    auto _colval = static_cast<uint8_t >(clamp( _col * 255.0f, 0.0f, 255.0f ));
    rawBtyes = std::make_unique<uint8_t[]>( memorySize());
    std::memset( rawBtyes.get(), memorySize(), _colval );
}

// Shortcut ctor for 3D Lut or generally cube 3d textures
RawImage::RawImage( unsigned int _s, TextureTargetMode _ttm ) {
    ASSERT( _ttm == TextureTargetMode::TEXTURE_3D );
    ttm = _ttm;
    width = _s;
    height = _s;
    depth = _s;
    channels = 3;
    rawBtyes = std::make_unique<uint8_t[]>( memorySize());
    std::memset( rawBtyes.get(), 0xff, memorySize());
}

RawImage::RawImage( int _width, int _height, int _depth, int _channels, int _bpp ) {
    width = _width;
    height = _height;
    depth = _depth;
    bpp = _bpp;
    channels = _channels;
}

RawImage::RawImage( int width,
                    int height,
                    int channels,
                    const char *buffer ) : ImageParams( width, height, channels ) {
    copyFrom( buffer );
}

RawImage::RawImage( int width,
                    int height,
                    int channels,
                    const unsigned char *buffer ) : ImageParams( width, height, channels ) {
    copyFrom( buffer );
}

RawImage::RawImage( int width, int height, int channels, const char *_buffer, const std::string& _forcedhash )
        : ImageParams( width, height, channels ) {
    copyFrom( _buffer );
}

RawImage rawImageSubImage( const RawImage& _source, const JMATH::Rect2f& _area,
                           const Vector2i& _extraPadding, const uint8_t& _paddingGradient ) {

    auto rwidth = static_cast<int>( _area.width() + _extraPadding.x() * 2 );
    auto rheight = static_cast<int>( _area.height() + _extraPadding.y() * 2 );
    auto rchannels = _source.channels;
    auto bsize = rwidth * rheight * _source.channels;
    auto buffer = std::make_unique<uint8_t[]>( bsize );
    std::memset( buffer.get(), _paddingGradient, bsize );
    auto left = static_cast<uint64_t>( _area.left());
    uint64_t top = _source.height - static_cast<uint64_t>( _area.top());
    uint64_t bottom = _source.height - static_cast<uint64_t>( _area.bottom());
    for ( uint64_t y = bottom, y1 = _extraPadding.y(); y < top; y++, y1++ ) {
        std::memcpy( buffer.get() + ( _extraPadding.x() * _source.channels ) +
                     (( y1 * static_cast<uint64_t>( rwidth ) * _source.channels )),
                     _source.rawBtyes.get() + ( y * _source.width * _source.channels ) + ( left * _source.channels ),
                     static_cast<int>( _area.width() * _source.channels ));
    }

    return RawImage( rwidth, rheight, rchannels, reinterpret_cast<const char *>(buffer.get()));
}

void RawImage::copyFrom( const char *buffer ) {
    rawBtyes = std::make_unique<uint8_t[]>( memorySize());
    std::memcpy( rawBtyes.get(), buffer, memorySize());
}

void RawImage::copyFrom( const unsigned char *buffer ) {
    rawBtyes = std::make_unique<uint8_t[]>( memorySize());
    std::memcpy( rawBtyes.get(), buffer, memorySize());
}

void RawImage::grayScale() {
    if ( channels < 3 ) return;

    auto newBtyes = std::make_unique<uint8_t[]>( width * height );

    for ( int y = 0; y < height; y++ ) {
        for ( int x = 0; x < width; x++ ) {

            uint8_t r = rawBtyes[( y * width * channels + ( x * channels )) + 0];
            uint8_t g = rawBtyes[( y * width * channels + ( x * channels )) + 1];
            uint8_t b = rawBtyes[( y * width * channels + ( x * channels )) + 2];

            float gray = 0.2126f * r + 0.7152f * g + 0.0722f * b;
            gray = clamp( gray, 0.0f, 255.0f );
            newBtyes[( y * width ) + x] = static_cast<uint8_t>(gray);
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

    transform<uint8_t>( [&]( uint8_t& _value ) {
        float factor = ( 259.0f * ( _c + 255.0f )) / ( 255.0f * ( 259.0f - _c ));
        _value = static_cast<uint8_t>(clamp((( factor * _value - 128 + _b ) + 128 ), 0.0f, 255.0f ));
    } );

}

RawImage RawImage::toNormalMap() const {

    //    auto bsize = width * height * channels;
    auto normalRaw = std::make_unique<uint8_t[]>( size());

    for ( int y = 0; y < height; y++ ) {
        int prevY = y == 0 ? height - 1 : y - 1;
        int nextY = y == height - 1 ? 0 : y + 1;
        for ( int x = 0; x < width; x++ ) {
            int prevX = x == 0 ? width - 1 : x - 1;
            int nextX = x == width - 1 ? 0 : x + 1;

            int ynp = at<uint8_t>( x, prevY );
            int ynn = at<uint8_t>( x, nextY );
            int xnp = at<uint8_t>( prevX, y );
            int xnn = at<uint8_t>( nextX, y );

            Vector2f vn2d = Vector2f{ xnp - xnn, ynp - ynn } / 255.0f;
            Vector3f vnt{ vn2d, sqrt( 1.0f - vn2d.x() * vn2d.x() - vn2d.y() * vn2d.y()) };
            Vector4f vn = vnt * Vector3f{ 0.5f, 0.5f, 1.0f } + Vector3f( 0.5f, 0.5f, 0.0f );
            uint32_t c = vn.RGBATOI();
            uint8_t *p = normalRaw.get() + ( y * width * channels + ( x * channels ));
            std::memcpy( p, &c, channels );
        }
    }

    return RawImage( width, height, 3, reinterpret_cast<const char *>(normalRaw.get()));
}

RawImage RawImage::WHITE4x4() {
    return RawImage{ 4, 4, 3, static_cast<uint32_t>(0xffffffff) };
}

RawImage RawImage::DEBUG_UV() {
    const int is = 16;
    unsigned char buff[is * is * 3];

#define bcol( t, m, r, g, b ) \
    { buff[(t*is*3)+(m*3+0)] = r; \
      buff[(t*is*3)+(m*3+1)] = g; \
      buff[(t*is*3)+(m*3+2)] = b; }


    for ( size_t t = 0; t < is; t++ ) {
        for ( size_t m = 0; m < is; m++ ) {
            if ( t < is / 2 && m < is / 2 ) bcol( t, m, 255, 0, 0 );
            if ( t >= is / 2 && m < is / 2 ) bcol( t, m, 255, 0, 255 );
            if ( t < is / 2 && m >= is / 2 ) bcol( t, m, 0, 255, 0 );
            if ( t >= is / 2 && m >= is / 2 ) bcol( t, m, 0, 0, 255 );
        }
    }
    return RawImage{ is, is, 3, reinterpret_cast<const char *>(buff) };
}


RawImage RawImage::BLACK_ARGB4x4() {                           //AABBGGRR
    return RawImage{ 4, 4, 4, static_cast<uint32_t>(0xff000000) };
}

RawImage RawImage::BLACK_RGBA4x4() {                            //AABBGGRR
    return RawImage{ 4, 4, 4, static_cast<uint32_t>(0x000000ff) };
}

RawImage RawImage::NORMAL4x4() {                           //AABBGGRR
    return RawImage{ 4, 4, 3, static_cast<uint32_t>(0xffff7f7f) };
}

RawImage RawImage::NOISE4x4() {

    const int is = 4;
    unsigned char buff[is * is * 3];

#define bcol( t, m, r, g, b ) \
    { buff[(t*is*3)+(m*3+0)] = r; \
      buff[(t*is*3)+(m*3+1)] = g; \
      buff[(t*is*3)+(m*3+2)] = b; }

    for ( size_t t = 0; t < is; t++ ) {
        for ( size_t m = 0; m < is; m++ ) {
            V3f v3{ unitRand(), unitRand(), 0.0f };
            v3 = normalize( v3 );
            bcol( t, m, static_cast<unsigned char>(v3.x() * 255.0f), static_cast<unsigned char>(v3.y() * 255.0f), 0 );
        }
    }
    return RawImage{ is, is, 3, reinterpret_cast<const char *>(buff) };
}

RawImage RawImage::LUT_3D_TEST() {
    int dimension = 17;

    RawImage ret{ dimension, dimension, dimension, 3, 8 };
    ret.target( TextureTargetMode::TEXTURE_3D );
    ret.setWrapMode( WrapMode::WRAP_MODE_CLAMP_TO_EDGE );
    createPlainLUT3D( dimension, ret.rawBtyes );

    return ret;

//    return RawImage{ FM::readLocalFile("W-ARAPAHO-33-STD.cube") };
//    return RawImage{ FM::readLocalFile("StrollingStiffs_CLog_C100.cube") };
//    return RawImage{ FM::readLocalFile("Hackmanite - Rec709.cube") };
}

ImageHeaderType detectHeader( const unsigned char *_buffer, size_t _length ) {

    if ( memcmp( "\x89PNG", _buffer, 4 ) == 0 ) {
        return ImageHeaderType::Png;
    }
    if ( memcmp( "\xFF\xD8\xFF", _buffer, 3 ) == 0 ) {
        return ImageHeaderType::Jpg;
    }
    if ( memcmp( "#?RADIANCE", _buffer, 10 ) == 0 ) {
        return ImageHeaderType::Radiance;
    }

    EXRVersion exr_headers;
    if ( ParseEXRVersionFromMemory( &exr_headers, _buffer, _length ) == 0 ) {
        return ImageHeaderType::Exr;
    }

    // ### Implement header check for LUT3D
    return ImageHeaderType::LUT3D;

//    return ImagaHeaderType::STB_Compatible;
}

void RawImage::bufferDecode( const unsigned char *_buffer, size_t _length ) {

    compressLength = _length;
    headerType = detectHeader( _buffer, _length );
    if ( headerType == ImageHeaderType::Png || headerType == ImageHeaderType::Jpg || headerType == ImageHeaderType::Radiance ) {
        // ### fix this!!
        stbi_set_flip_vertically_on_load( false );
        auto _mt = RawImageMemory::Compressed;
        rawBtyes = imageUtil::decodeFromMemory( ucchar_p{ _buffer, _length },
                                                width, height, channels, bpp, _mt == RawImageMemory::Raw );
    } else if ( headerType == ImageHeaderType::LUT3D ) {
        int dimension = 0;
        rawBtyes = loadLUT3D( _buffer, _length, dimension );
        width = dimension;
        height = dimension;
        depth = dimension;
        bpp = 8;
        channels = 3;
        ttm = TextureTargetMode::TEXTURE_3D;
        wrapMode = WrapMode::WRAP_MODE_CLAMP_TO_EDGE;
    } else if ( headerType == ImageHeaderType::Exr ) {
        float *image;
        const char *err;

        int ret = LoadEXRFromMemory( &image, &width, &height, _buffer, _length, &err );
        if ( ret != 0 ) {
            LOGR( "Load EXR err: %s\n", err );
        }
        channels = 4;
        bpp = sizeof( float ) * 8;
        rawBtyes = std::make_unique<uint8_t[]>( memorySize());
        memcpy( rawBtyes.get(), image, memorySize());
        free( image );
    }

    setFormatFromChannels();
}

SerializableContainer RawImage::serialize() const {
    SerializeBin mw;
    mw.write( width );
    mw.write( height );
    mw.write( rawBtyes, static_cast<int32_t >(size()));

    return mw.serialize();
}

ImageParamsJSONAble RawImage::serializeParams() const {

    ImageParamsJSONAble sr{
            width, height, depth, channels, bpp,
            compressLength, size(),
            imageHeaderTypeToString(headerType),
            PixelFormatToString(outFormat),
            TextureTargetModeModeFormatToString(ttm),
            WrapModeFormatToString(wrapMode),
            FilterFormatToString(filterMode)
    };
    return sr;
}

std::ostream& operator<<( std::ostream& os, const RawImage& image ) {
    os << static_cast<const ImageParams&>(image);
    return os;
}


