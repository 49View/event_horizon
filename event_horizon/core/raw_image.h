#pragma once

#include <memory>
#include <cstring>
#include <string>
#include <core/math/vector2i.h>
#include <core/htypes_shared.hpp>
#include <core/image_constants.h>
#include <core/name_policy.hpp>

namespace JMATH { class Rect2f; }

class RawImage : public ImageParams, public NamePolicy<> {
public:
    using NamePolicy::NamePolicy;
    RawImage() = default;
    virtual ~RawImage() = default;
    RawImage( uint8_p&& data, const std::string& _name );

    RawImage( const std::string& _name, unsigned int _w, unsigned int _h, uint32_t _col);
    RawImage( const std::string& _name, unsigned int _w, unsigned int _h, uint8_t _col );
    RawImage( const std::string& _name, const ImageParams& _ip, std::unique_ptr<uint8_t[]>&& decodedData );

    // A single float number equals a grayscale (1 channel) image
    RawImage( const std::string& _name, unsigned int _w, unsigned int _h, float _col );
    RawImage( int width, int height, int channels, const char* rawBtyes, const std::string& name = "" );
    RawImage( int width, int height, int channels, const std::string& name = "" );

    void copyFrom( const char* buffer );

    RawImage(RawImage&& _val) {
        width = _val.width;
        height = _val.height;
        channels = _val.channels;
        rawBtyes = std::move(_val.rawBtyes);
        Name(_val.Name());
    }

    RawImage(const RawImage& _val) {
        width = _val.width;
        height = _val.height;
        channels = _val.channels;
        copyFrom( reinterpret_cast<const char *>(_val.data()) );
        Name(_val.Name());
    }

    RawImage& operator=(const RawImage& _val) {
        width = _val.width;
        height = _val.height;
        channels = _val.channels;
        copyFrom( reinterpret_cast<const char *>(_val.data()) );
        Name(_val.Name());
        return *this;
    }

    void grayScale();
    RawImage toGrayScale() const;
    RawImage toNormalMap() const;
    void brightnessContrast( float _c, int _b );

    template <typename T>
    T at( unsigned int x, unsigned int y ) const {
        T ret;
        std::memcpy(&ret, rawBtyes.get() + (y*width*channels + (x*channels)), channels);
        return ret;
    }

    template <typename T>
    T& at( unsigned int x, unsigned int y ) {
        uint8_t* p = rawBtyes.get() + (y*width*channels + (x*channels));
        return reinterpret_cast<T&>(p);
    }

    template <typename T>
    void set( unsigned int x, unsigned int y, T _val ) {
        uint8_t* p = rawBtyes.get() + (y*width*channels + (x*channels));
        std::memcpy(p, &_val, sizeof(T));
    }

    template <typename T>
    void transform( std::function<void( T& _value )> _transF ) {
        for ( int y = 0; y < height; y++ ) {
            for ( int x = 0; x < width; x++ ) {
                T ret;
                std::memcpy(&ret, rawBtyes.get() + (y*width*channels + (x*channels)), channels);
                _transF( ret );
                std::memcpy(rawBtyes.get() + (y*width*channels + (x*channels)), &ret, channels);
            }
        }
    }

    uint8_t* data() const {
        return rawBtyes.get();
    }

    size_t size() const {
        return width * height * channels;
    }

	std::unique_ptr<uint8_t[]> rawBtyes;

public:
	static RawImage WHITE4x4();
	static RawImage DEBUG_UV();
	static RawImage BLACK_ARGB4x4();
	static RawImage BLACK_RGBA4x4();
	static RawImage NORMAL4x4();
};

RawImage rawImageDecodeFromMemory( const std::string& _base64, const std::string& _name = "", int forceChannels = 0 );
RawImage rawImageDecodeFromMemory( const uint8_p& data, const std::string& _name = "", int forceChannels = 0 );
RawImage rawImageDecodeFromMemory( const unsigned char* buffer, int length, const std::string& _name = "",
                                   int forceChannels = 0 );

RawImage rawImageSubImage( const RawImage& _source, const JMATH::Rect2f& _area,
                           const Vector2i& _extraPadding = Vector2i::ZERO,
                           const uint8_t& _paddingGradient = 255 );

