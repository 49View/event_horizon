#pragma once

#include <memory>
#include <string>
#include "math/vector2i.h"
#include "htypes_shared.hpp"

namespace JMATH { class Rect2f; }

struct RawImage {
    RawImage() {}
    RawImage( const std::string& _name, int _w, int _h, const uint32_t _col);
    RawImage( int width, int height, int channels, const char* rawBtyes, const std::string& name = "" );
    RawImage( int width, int height, int channels, const char* rawBtyes, int step, const std::string& name = "" );

    void copyFrom( const char* buffer );

    RawImage(RawImage&& _val) {
        width = _val.width;
        height = _val.height;
        channels = _val.channels;
        rawBtyes = std::move(_val.rawBtyes);
        name = _val.name;
    }

    RawImage(RawImage& _val) {
        width = _val.width;
        height = _val.height;
        channels = _val.channels;
        rawBtyes = std::move(_val.rawBtyes);
        name = _val.name;
    }

    RawImage& operator=(RawImage _val) {
        width = _val.width;
        height = _val.height;
        channels = _val.channels;
        rawBtyes = std::move(_val.rawBtyes);
        name = _val.name;
        return *this;
    }

    int width = 0;
    int height = 0;
	int channels = 0;
	mutable std::unique_ptr<uint8_t[]> rawBtyes;
	std::string name;

	static RawImage WHITE4x4;
	static RawImage BLACK_ARGB1x1;
	static RawImage BLACK_RGBA1x1;
};

RawImage rawImageDecodeFromMemory( const uint8_p& data, const std::string& _name = "", int forceChannels = 0 );
RawImage rawImageDecodeFromMemory( const unsigned char* buffer, int length, const std::string& _name = "",
                                   int forceChannels = 0 );
void openImageAsRaw( const std::string& _filename, bool bUseImagePrefix = false );

RawImage rawImageSubImage( const RawImage& _source, const JMATH::Rect2f& _area,
                           const Vector2i& _extraPadding = Vector2i::ZERO,
                           const uint8_t& _paddingGradient = 255 );

