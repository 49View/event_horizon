#pragma once

#include <math.h>
#include <vector>

#include "vector2f.h"
#include "vector3f.h"



// SHARED CLASS: Any changes to this, should be integrated to /svn/shared, and a mail should be sent to everyone

class Vector4f {
public:

	static const Vector4f ZERO;
	static const Vector4f QUAT_UNIT;
	static const Vector4f X_AXIS;
	static const Vector4f Y_AXIS;
	static const Vector4f Z_AXIS;
	static const Vector4f W_AXIS;
	static const Vector4f ONE;
	static const Vector4f HUGE_VALUE_POS;
	static const Vector4f HUGE_VALUE_NEG;

	static const Vector4f WHITE;
	static const Vector4f BLACK;
	static const Vector4f YELLOW;
	static const Vector4f RED;
	static const Vector4f GREEN;
	static const Vector4f BLUE;
	static const Vector4f BROWN;
	static const Vector4f CYAN;
	static const Vector4f PURPLE;
	static const Vector4f PASTEL_GRAY;
	static const Vector4f PASTEL_GRAYLIGHT;
	static const Vector4f PASTEL_ORANGE;
	static const Vector4f PASTEL_RED;
	static const Vector4f PASTEL_CYAN;
	static const Vector4f PASTEL_GREEN;
	static const Vector4f PASTEL_YELLOW;
	static const Vector4f PASTEL_BROWN;
	static const Vector4f ACQUA_T;

	static const Vector4f SAND;

	static const Vector4f ORANGE_SCHEME1_1;
	static const Vector4f ORANGE_SCHEME1_2;
	static const Vector4f ORANGE_SCHEME1_3;
	static const Vector4f BLUE_SHADOW;

	static const Vector4f BRIGHT_PINK;
	static const Vector4f DARK_GRAY;
	static const Vector4f DARK_BLUE;
	static const Vector4f DARK_YELLOW;
	static const Vector4f DARK_RED;
	static const Vector4f DARK_GREEN;
	static const Vector4f DARK_BROWN;
	static const Vector4f DARK_PURPLE;
	static const Vector4f INDIAN_RED;
	static const Vector4f LIGHT_CORAL;
	static const Vector4f SALMON;
	static const Vector4f DARK_SALMON;
	static const Vector4f LIGHT_SALMON;
	static const Vector4f CRIMSON;
	static const Vector4f FIRE_BRICK;
	static const Vector4f PINK;
	static const Vector4f HOT_PINK;
	static const Vector4f DEEP_PINK;
	static const Vector4f GOLD;
	static const Vector4f PEACH;
	static const Vector4f LAVANDER;
	static const Vector4f VIOLET;
	static const Vector4f FUCHSIA;
	static const Vector4f DARK_VIOLET;
	static const Vector4f INDINGO;
	static const Vector4f SPRING_GREEN;
	static const Vector4f FOREST_GREEN;
	static const Vector4f OLIVE;
	static const Vector4f DARK_CYAN;
	static const Vector4f AQUAMARINE;
	static const Vector4f STEEL_BLUE;
	static const Vector4f SKY_BLUE;
	static const Vector4f DODGER_BLUE;
	static const Vector4f ROYAL_BLUE;
	static const Vector4f NAVY;
	static const Vector4f ALMOND;
	static const Vector4f TAN;
	static const Vector4f SANDY_BROWN;
	static const Vector4f SIENNA;
	static const Vector4f MAROON;
	static const Vector4f SNOW;
	static const Vector4f HONEYDEW;
	static const Vector4f LIGHT_GREY;

	static Vector4f XTORGBA( const std::string& _hexstring );
	static Vector4f ITORGBA( uint32_t number, int32_t numbits = 32 );
	static Vector3f ITORGB( const unsigned int r, const unsigned int g, const unsigned int b );
	static Vector4f ITORGBA( const unsigned int r, const unsigned int g, const unsigned int b, const unsigned int a );
	static Vector3f FTORGB( const float r, const float g, const float b );
	static Vector4f FTORGBA( const float r, const float g, const float b, const float a );
	static Vector4f FTORGBA( const Vector4f& _val );
	static Vector4f COLORA( const Vector4f& source, const float a );

	Vector4f() {
	}

	explicit Vector4f( int xyzw ) {
		mX = xyzw;
		mY = xyzw;
		mZ = xyzw;
		mW = xyzw;
	}

	explicit Vector4f( float xyzw ) {
		mX = xyzw;
		mY = xyzw;
		mZ = xyzw;
		mW = xyzw;
	}

	Vector4f( float x, float y, float z, float w ) {
		mX = x;
		mY = y;
		mZ = z;
		mW = w;
	}

	Vector4f( double x, double y, double z, double w ) {
		mX = static_cast<float>( x );
		mY = static_cast<float>( y );
		mZ = static_cast<float>( z );
		mW = static_cast<float>( w );
	}

	Vector4f( const Vector2f& v, float z, float w ) {
		mX = v.x();
		mY = v.y();
		mZ = z;
		mW = w;
	}

	Vector4f( const Vector3f& v, float w = 1.0f ) {
		mX = v.x();
		mY = v.y();
		mZ = v.z();
		mW = w;
	}

	Vector4f( const Vector2f& v, const Vector2f& v2 ) {
		mX = v.x();
		mY = v.y();
		mZ = v2.x();
		mW = v2.y();
	}

	template <typename T>
	Vector4f( const std::vector<T> _v ) {
		ASSERT( _v.size() == 4 );
		mX = static_cast<float>(_v[0]);
		mY = static_cast<float>(_v[1]);
		mZ = static_cast<float>(_v[2]);
		mW = static_cast<float>(_v[3]);
	}

	float x() const {
		return mX;
	}

	float y() const {
		return mY;
	}

	float z() const {
		return mZ;
	}

	float w() const {
		return mW;
	}

	int32_t size() const {
		return 4;
	}

	float operator[]( int element ) const {
		ASSERT( element >= 0 && element < 4 );
		return ( &mX )[element];
	}

	float& operator[]( int element ) {
		ASSERT( element >= 0 && element < 4 );
		return ( &mX )[element];
	}

	Vector2f xy() const {
		return Vector2f( mX, mY );
	}

	Vector3f xyz() const {
		return Vector3f( mX, mY, mZ );
	}

	Vector3f xzy() const {
		return{ mX, mZ, mY };
	}

	void swizzle( uint32_t i1, uint32_t i2 ) {
		ASSERT( i1 < 4 && i2 < 4 );
		float c = ( *this )[i1];
		( *this )[i1] = ( *this )[i2];
		( *this )[i2] = c;
	}

	void set( float x, float y, float z, float w ) {
		mX = x;
		mY = y;
		mZ = z;
		mW = w;
	}

	void set( const Vector2f& xy, float z, float w ) {
		mX = xy.x();
		mY = xy.y();
		mZ = z;
		mW = w;
	}

	void set( const Vector3f& xyz, float w ) {
		mX = xyz.x();
		mY = xyz.y();
		mZ = xyz.z();
		mW = w;
	}

	void setX( float x ) {
		mX = x;
	}

	void setY( float y ) {
		mY = y;
	}

	void setZ( float z ) {
		mZ = z;
	}

	void setW( float w ) {
		mW = w;
	}

	void invY() { mY *= -1.0f; }

	bool operator==( const Vector4f& rhs ) const {
		return mX == rhs.mX && mY == rhs.mY && mZ == rhs.mZ && mW == rhs.mW;
	}

	bool operator!=( const Vector4f& rhs ) const {
		return mX != rhs.mX || mY != rhs.mY || mZ != rhs.mZ || mW != rhs.mW;
	}

	Vector4f operator-() const {
		return Vector4f( -mX, -mY, -mZ, -mW );
	}

	Vector4f operator+( const Vector4f& rhs ) const {
		return Vector4f( mX + rhs.mX, mY + rhs.mY, mZ + rhs.mZ, mW + rhs.mW );
	}

	Vector4f operator+( float rhs ) const {
		return Vector4f( mX + rhs, mY + rhs, mZ + rhs, mW + rhs );
	}

	Vector4f operator-( const Vector4f& rhs ) const {
		return Vector4f( mX - rhs.mX, mY - rhs.mY, mZ - rhs.mZ, mW - rhs.mW );
	}

	Vector4f operator-( const float rhs ) const {
		return Vector4f( mX - rhs, mY - rhs, mZ - rhs, mW - rhs );
	}

	Vector4f operator*( const Vector4f& rhs ) const {
		return Vector4f( mX * rhs.mX, mY * rhs.mY, mZ * rhs.mZ, mW * rhs.mW );
	}

	Vector4f operator*( const float rhs ) const {
		return Vector4f( mX * rhs, mY * rhs, mZ * rhs, mW * rhs );
	}

	float dot( const Vector4f& rhs ) const {
		return mX * rhs.mX + mY * rhs.mY + mZ * rhs.mZ + mW * rhs.mW;
	}

	Vector4f operator/( const float rhs ) const {
		const float recip = 1.0f / rhs;
		return Vector4f( mX * recip, mY * recip, mZ * recip, mW * recip );
	}

	void operator *=( float f ) {
		mX *= f;
		mY *= f;
		mZ *= f;
		mW *= f;
	}

    Vector4f operator*=( const Vector4f& rhs ) {
        mX *= rhs.mX;
        mY *= rhs.mY;
        mZ *= rhs.mZ;
        mW *= rhs.mW;
        return *this;
    }

	void operator -=( float f ) {
		mX -= f;
		mY -= f;
		mZ -= f;
		mW -= f;
	}

	std::string toString() const;
	std::string toStringCommaSeparated() const;
	std::string toStringJSONArray() const;
    std::string toStringObj( const std::string& _prefix) const;

	size_t hash() const {
		std::hash<float> hasher;
		size_t seed = 0;
		seed ^= hasher( mX ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
		seed ^= hasher( mY ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
		seed ^= hasher( mZ ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
		seed ^= hasher( mW ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
		return seed;
	}

	inline std::string toStringColor() const {
		if ( *this == Vector4f::WHITE ) return "white";
		if ( *this == Vector4f::BLACK ) return "black";
		if ( *this == Vector4f::YELLOW ) return "yellow";
		if ( *this == Vector4f::RED ) return "red";
		if ( *this == Vector4f::GREEN ) return "green";
		if ( *this == Vector4f::BLUE ) return "blue";
		if ( *this == Vector4f::BROWN ) return "brown";
		if ( *this == Vector4f::CYAN ) return "cyan";
		if ( *this == Vector4f::PURPLE ) return "purple";

		return "R" + std::to_string( (int)( x()*255.0f ) ) + "G" + std::to_string( (int)( y()*255.0f ) ) + "B" + std::to_string( (int)( z()*255.0f ) ) + "A" + std::to_string( (int)( w()*255.0f ) );
	}

	inline Vector4f A( const float _alpha ) const {
		return{ mX, mY, mZ, _alpha };
	}

	inline	bool isSimilarTo( const Vector4f& v ) const {
		float epsilon = 0.00000001f;

		if ( fabs( mX - v.mX ) > epsilon )
			return false;

		if ( fabs( mY - v.mY ) > epsilon )
			return false;

		if ( fabs( mZ - v.mZ ) > epsilon )
			return false;

		if ( fabs( mW - v.mW ) > epsilon )
			return false;

		return true;
	}

	uint32_t RGBATOI() const {
		return static_cast<uint8_t>(mX*255.0f) | ( static_cast<uint8_t>(mY*255.0f) << 8 ) |
			   ( static_cast<uint8_t>(mZ*255.0f) << 16 ) | ( static_cast<uint8_t>(mW*255.0f) << 24 );
	}

	uint32_t ARGBTOI() const {
		return static_cast<uint8_t>(mW*255.0f) | ( static_cast<uint8_t>(mX*255.0f) << 8 ) |
			   ( static_cast<uint8_t>(mY*255.0f) << 16 ) | ( static_cast<uint8_t>(mZ*255.0f) << 24 );
	}

	uint32_t ABGRTOI() const {
		return static_cast<uint8_t>(mW*255.0f) | ( static_cast<uint8_t>(mZ*255.0f) << 8 ) |
			   ( static_cast<uint8_t>(mY*255.0f) << 16 ) | ( static_cast<uint8_t>(mX*255.0f) << 24 );
	}

	uint32_t BGRTOI() const {
		return static_cast<uint8_t>(mZ*255.0f) | ( static_cast<uint8_t>(mY*255.0f) << 8 ) |
			   ( static_cast<uint8_t>(mX*255.0f) << 16 );
	}

	uint32_t RGBTOI() const {
		return static_cast<uint8_t>(mX*255.0f) | ( static_cast<uint8_t>(mY*255.0f) << 8 ) |
			   ( static_cast<uint8_t>(mZ*255.0f) << 16 );
	}

    friend std::ostream& operator<<( std::ostream& o, const Vector4f& val ) {
        o << val.toString();
        return o;
    }

    static constexpr int32_t vsize() { return 4; }

private:
	// Note: these elements should be declared contiguously and ordered X/Y/W/Z
	// due to a memory-layout assumption in operator[].
	float mX;
	float mY;
	float mZ;
	float mW;
};

inline float dot( const Vector3f& a, const Vector4f& b ) {
	return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
}

inline float length( const Vector4f& v ) {
	return length( v.x(), v.y(), v.z(), v.w() );
}

inline Vector4f absolute( const Vector4f& v ) {
	return Vector4f( absolute( v.x() ), absolute( v.y() ), absolute( v.z() ), absolute( v.w() ) );
}

inline Vector4f saturate( const Vector4f& v ) {
	return Vector4f( saturate( v.x() ), saturate( v.y() ), saturate( v.z() ), saturate( v.w() ) );
}

inline Vector4f min( const Vector4f& a, const Vector4f& b ) {
	return Vector4f(
		min( a.x(), b.x() ),
		min( a.y(), b.y() ),
		min( a.z(), b.z() ),
		min( a.w(), b.w() )
	);
}

inline Vector4f max( const Vector4f& a, const Vector4f& b ) {
	return Vector4f(
		max( a.x(), b.x() ),
		max( a.y(), b.y() ),
		max( a.z(), b.z() ),
		max( a.w(), b.w() )
	);
}

inline float minElement( const Vector4f & v ) {
	return min4( v.x(), v.y(), v.z(), v.w() );
}

inline float maxElement( const Vector4f & v ) {
	return max4( v.x(), v.y(), v.z(), v.w() );
}

inline float sumElements( const Vector4f & v ) {
	return v.x() + v.y() + v.z() + v.w();
}

inline Vector4f fma( const Vector4f& a, const Vector4f& b, const Vector4f& c ) {
	return Vector4f(
		a.x() * b.x() + c.x(),
		a.y() * b.y() + c.y(),
		a.z() * b.z() + c.z(),
		a.w() * b.w() + c.w()
	);
}

inline Vector4f convertStringHexColorInVector4( const std::string& color, int32_t numbits = 32 ) {
	auto number = (uint32_t)strtol( color.c_str(), nullptr, 0 );
	return Vector4f::ITORGBA( number, numbits );
}

inline Vector4f color4fAlpha( const float alpha ) {
	return Vector4f( 1.0f, 1.0f, 1.0f, alpha );
}

using Color4f = Vector4f;
using C4f = Vector4f;
using V4f = Vector4f;
using V4fVector         = std::vector<Vector4f>;
using V4fVectorOfVector = std::vector<V4fVector>;

using V4fVectorWrap = VectorWrap<V4f>;
using V4fVectorOfVectorWrap = VectorOfVectorWrap<V4f>;
