#include "vector4f.h"

#include <sstream>
#include "math_util.h"
#include <core/string_util.h>

namespace V4fc {
    [[maybe_unused]] Vector4f ITORGBA( uint32_t number, int32_t numBits ) {
        uint32_t blue = ( number & 0x00ff0000u ) >> 16u;
        uint32_t green = ( number & 0x0000ff00u ) >> 8u;
        uint32_t red = number & 0x000000ffu;
        uint32_t alpha = 0xff;
        if ( numBits == 32 ) alpha = ( number & 0xff000000 ) >> 24u;

        return Vector4f( static_cast<float>( red ) / 255.0f, static_cast<float>( green ) / 255.0f, static_cast<float>( blue ) / 255.0f, static_cast<float>( alpha ) / 255.0f );
    }

    [[maybe_unused]] Vector4f XTORGBA( const std::string& _hexString ) {
        if ( _hexString.empty() ) return V4fc::WHITE;
        std::string sanitized = string_trim_after(_hexString, "#");
        if ( sanitized.size() != 3 && sanitized.size() != 6 && sanitized.size() != 8 ) return V4fc::WHITE;
        std::string red  {};
        std::string green{};
        std::string blue {};
        std::string alpha{};

        if ( sanitized.size() != 3 ) {
            red    = "0x" + sanitized.substr(0, 2);
            green  = "0x" + sanitized.substr(2, 2);
            blue   = "0x" + sanitized.substr(4, 2);
            alpha  = "0x" + (sanitized.size() == 8 ? sanitized.substr(6, 2) : "ff");
        } else {
            red    = "0x" + sanitized.substr(0, 1) + "0";
            green  = "0x" + sanitized.substr(1, 1) + "0";
            blue   = "0x" + sanitized.substr(2, 1) + "0";
            alpha  = "0xFF";
        }

        int redI   = (int)strtol(red.c_str(), nullptr, 0);
        int greenI = (int)strtol(green.c_str(), nullptr, 0);
        int blueI  = (int)strtol(blue.c_str(), nullptr, 0);
        int alphaI = (int)strtol(alpha.c_str(), nullptr, 0);

        return ITORGBA(redI, greenI, blueI, alphaI );
    }

    [[maybe_unused]] Vector3f ITORGB( const unsigned int r, const unsigned int g, const unsigned int b ) {
        return Vector3f( (static_cast<float>(r) * 1.0f) / 255.0f, (static_cast<float>(g) * 1.0f) / 255.0f, (static_cast<float>(b) * 1.0f) / 255.0f );
    }

    [[maybe_unused]] Vector4f ITORGBA( const unsigned int r, const unsigned int g, const unsigned int b, const unsigned int a ) {
        return Vector4f( (static_cast<float>(r) * 1.0f) / 255.0f, (static_cast<float>(g) * 1.0f) / 255.0f, (static_cast<float>(b) * 1.0f) / 255.0f, (static_cast<float>(a) * 1.0f) / 255.0f );
    }

    [[maybe_unused]] Vector3f FTORGB( const float r, const float g, const float b ) {
        return Vector3f( r / 255.0f, g / 255.0f, b / 255.0f );
    }

    [[maybe_unused]] Vector4f FTORGBA( const float r, const float g, const float b, const float a ) {
        return  Vector4f( r / 255.0f, g / 255.0f, b / 255.0f, a );
    }

    [[maybe_unused]] Vector4f FTORGBA( const Vector4f& _val ) {
        return Vector4f( Vector3f{ _val.xyz() / 255.0f }, _val.w() );
    }

    [[maybe_unused]] Vector4f COLORA( const Vector4f& source, const float a ) {
        return Vector4f( source.xyz(), a );
    }

    [[maybe_unused]] Vector4f RANDA1() {
        return V4f{unitRand(), unitRand(), unitRand(), 1.0f};
    }
};

Vector3f::Vector3f( const Vector4f& v4 ) {
	mX = v4.x();
	mY = v4.y();
	mZ = v4.z();
}

std::string Vector4f::toString() const {
	return "X = " + floatToFixedDigits( mX ) + " Y = " + floatToFixedDigits( mY ) + " Z = " + floatToFixedDigits( mZ ) + " W = " + floatToFixedDigits( mW );
}

std::string Vector4f::toStringCommaSeparated() const {
	return floatToFixedDigits( mX ) + "," + floatToFixedDigits( mY ) + "," + floatToFixedDigits( mZ ) + "," +
		   floatToFixedDigits( mW );
}

[[maybe_unused]] std::string Vector4f::toStringJSONArray() const {
	return "[" + floatToFixedDigits( mX ) + "," + floatToFixedDigits( mY ) + "," + floatToFixedDigits( mZ ) + "," +
		   floatToFixedDigits( mW ) + "]";
}

std::string Vector4f::toStringObj( const std::string& _prefix ) const {
    std::ostringstream ss;
    ss << _prefix << " " << mX << " " << mY << " " << mZ << " " << mW <<std::endl;
    return ss.str();
}

Vector4f::Vector4f( int xyzw ) {
    mX = static_cast<float>(xyzw);
    mY = static_cast<float>(xyzw);
    mZ = static_cast<float>(xyzw);
    mW = static_cast<float>(xyzw);
}

Vector4f::Vector4f( float xyzw ) {
    mX = xyzw;
    mY = xyzw;
    mZ = xyzw;
    mW = xyzw;
}

Vector4f::Vector4f( float x, float y, float z, float w ) {
    mX = x;
    mY = y;
    mZ = z;
    mW = w;
}

std::string Vector4f::toStringColor() const {
    if ( *this == V4fc::WHITE ) return "white";
    if ( *this == V4fc::BLACK ) return "black";
    if ( *this == V4fc::YELLOW ) return "yellow";
    if ( *this == V4fc::RED ) return "red";
    if ( *this == V4fc::GREEN ) return "green";
    if ( *this == V4fc::BLUE ) return "blue";
    if ( *this == V4fc::BROWN ) return "brown";
    if ( *this == V4fc::CYAN ) return "cyan";
    if ( *this == V4fc::PURPLE ) return "purple";

    return "R" + std::to_string( (int)( x()*255.0f ) ) + "G" + std::to_string( (int)( y()*255.0f ) ) + "B" + std::to_string( (int)( z()*255.0f ) ) + "A" + std::to_string( (int)( w()*255.0f ) );
}

[[maybe_unused]] Vector4f convertStringHexColorInVector4( const std::string& color, int32_t numBits ) {
    auto number = (uint32_t)strtol( color.c_str(), nullptr, 0 );
    return V4fc::ITORGBA(number, numBits );
}
