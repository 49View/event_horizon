#pragma once

#include <memory>
#include <cstring>
#include <string>
#include <core/math/vector2i.h>
#include <core/htypes_shared.hpp>
#include <core/image_constants.h>

namespace JMATH { class Rect2f; }

enum class RawImageMemory {
    Raw,
    Compressed
};

class Vector4f;
struct ImageParamsJSONAble;

class RawImage : public ImageParams {
public:
RESOURCE_CTORS_ONLY( RawImage );

    RawImage( unsigned int _w, unsigned int _h, int channels, uint32_t _col );
    RawImage( unsigned int _w, unsigned int _h, uint8_t _col );

    RawImage( unsigned int _s, TextureTargetMode _ttm );

    // A single float number equals a grayscale (1 channel) image
    RawImage( unsigned int _w, unsigned int _h, float _col );
    RawImage( int width, int height, int channels, const char *rawBtyes );
    RawImage( int _width, int _height, int _depth, int _channels, int _bpp );
    RawImage( int width, int height, int channels, const char *rawBtyes, const std::string& _forcedhash );
    RawImage( unsigned int _w, unsigned int _h, const Vector4f& _value, int _bpp );

    void copyFrom( const char *buffer );

    RawImage( RawImage&& _val ) noexcept {
        width = _val.width;
        height = _val.height;
        channels = _val.channels;
        bpp = _val.bpp;
        rawBtyes = std::move( _val.rawBtyes );
    }

    RawImage( const RawImage& _val ) : ImageParams( _val ) {
        copyFrom( reinterpret_cast<const char *>(_val.data()));
    }

    RawImage& operator=( const RawImage& _val ) {
        ImageParams::operator=( _val );
        copyFrom( reinterpret_cast<const char *>(_val.data()));
        return *this;
    }

    [[nodiscard]] ImageParamsJSONAble serializeParams() const;

    void grayScale();
    RawImage toGrayScale() const;
    RawImage toNormalMap() const;
    void brightnessContrast( float _c, int _b );

    template<typename T>
    T at( unsigned int x, unsigned int y ) const {
        T ret;
        std::memcpy( &ret, rawBtyes.get() + ( y * width * channels + ( x * channels )), channels );
        return ret;
    }

//    template <typename T>
//    T& at( unsigned int x, unsigned int y ) {
//        return reinterpret_cast<T&>((uint8_t*)(rawBtyes.get() + (y*width*channels + (x*channels))));
//    }

    template<typename T>
    void set( unsigned int x, unsigned int y, T _val ) {
        uint8_t *p = rawBtyes.get() + ( y * width * channels + ( x * channels ));
        std::memcpy( p, &_val, sizeof( T ));
    }

    template<typename T>
    void transform( std::function<void( T& _value )> _transF ) {
        for ( int y = 0; y < height; y++ ) {
            for ( int x = 0; x < width; x++ ) {
                T ret;
                std::memcpy( &ret, rawBtyes.get() + ( y * width * channels + ( x * channels )), channels );
                _transF( ret );
                std::memcpy( rawBtyes.get() + ( y * width * channels + ( x * channels )), &ret, channels );
            }
        }
    }

    uint8_t *data() const {
        return rawBtyes.get();
    }

    size_t size() const {
        return width * height * channels;
    }

    std::unique_ptr<uint8_t[]> rawBtyes;

    // NDDADO: unfortunately unique_ptr<[]> cannot be serialized with macros as it needs 2 variables, shame
    // So we have to get a quick custom one here instead, very sad :/
    SerializableContainer serialize() const;

public:
    static RawImage WHITE4x4();
    static RawImage DEBUG_UV();
    static RawImage BLACK_ARGB4x4();
    static RawImage BLACK_RGBA4x4();
    static RawImage NORMAL4x4();
    static RawImage NOISE4x4();
    static RawImage LUT_3D_TEST();
};

//RawImage rawImageDecodeFromMemory( const std::string& _base64, const std::string& _name = "", int forceChannels = 0 );
//RawImage rawImageDecodeFromMemory( const uint8_p& data, const std::string& _name = "", int forceChannels = 0 );
//RawImage rawImageDecodeFromMemory( const unsigned char* buffer, int length, const std::string& _name = "",
//                                   int forceChannels = 0 );

RawImage rawImageSubImage( const RawImage& _source, const JMATH::Rect2f& _area,
                           const Vector2i& _extraPadding = Vector2i::ZERO,
                           const uint8_t& _paddingGradient = 255 );

