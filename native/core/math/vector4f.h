#pragma once

#include <cmath>
#include <vector>

#include "vector2f.h"
#include "vector3f.h"

// SHARED CLASS: Any changes to this, should be integrated to /svn/shared, and a mail should be sent to everyone

class Vector4f {
public:
	Vector4f() = default;
	explicit Vector4f( int xyzw );
	explicit Vector4f( float xyzw );
	Vector4f( float x, float y, float z, float w );

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

	[[nodiscard]] float x() const {
		return mX;
	}

	[[nodiscard]] float y() const {
		return mY;
	}

	[[nodiscard]] float z() const {
		return mZ;
	}

	[[nodiscard]] float w() const {
		return mW;
	}

	[[nodiscard]] int size() const {
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

	[[nodiscard]] Vector2f xy() const {
		return Vector2f( mX, mY );
	}

	[[nodiscard]] Vector3f xyz() const {
		return Vector3f( mX, mY, mZ );
	}

	[[nodiscard]] Vector3f xzy() const {
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

    void oneMinusX() { mX = 1.0f - mX; }
    void oneMinusY() { mY = 1.0f - mY; }
    void oneMinusZ() { mZ = 1.0f - mZ; }
    void oneMinusW() { mW = 1.0f - mW; }

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

    Vector4f operator+=( const Vector4f& rhs ) {
        mX += rhs.mX;
        mY += rhs.mY;
        mZ += rhs.mZ;
        mW += rhs.mW;
        return *this;
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

	[[nodiscard]] float dot( const Vector4f& rhs ) const {
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
    [[maybe_unused]] std::string toStringJSONArray() const;
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

	[[nodiscard]] inline std::string toStringColor() const;

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

[[maybe_unused]] inline Vector4f convertStringHexColorInVector4( const std::string& color, int32_t numBits = 32 );

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

namespace V4fc {
    static const Vector4f ZERO = Vector4f( 0.0f, 0.0f, 0.0f, 0.0f );
    static const Vector4f QUAT_UNIT = Vector4f( 0.0f, 0.0f, 0.0f, 1.0f );
    static const Vector4f X_AXIS = Vector4f( 1.0f, 0.0f, 0.0f, 0.0f );
    static const Vector4f Y_AXIS = Vector4f( 0.0f, 1.0f, 0.0f, 0.0f );
    static const Vector4f Z_AXIS = Vector4f( 0.0f, 0.0f, 1.0f, 0.0f );
    static const Vector4f W_AXIS = Vector4f( 0.0f, 0.0f, 0.0f, 1.0f );
    static const Vector4f ONE = Vector4f( 1.0f, 1.0f, 1.0f, 1.0f );
    static const Vector4f HUGE_VALUE_POS = Vector4f( std::numeric_limits<float>::max() );
    static const Vector4f HUGE_VALUE_NEG = Vector4f( std::numeric_limits<float>::lowest() );
    static const Vector4f WHITE = Vector4f( 1.0f, 1.0f, 1.0f, 1.0f );
    static const Vector4f BLACK = Vector4f( 0.0f, 0.0f, 0.0f, 1.0f );
    static const Vector4f YELLOW = Vector4f( 1.0f, 1.0f, 0.0f, 1.0f );
    static const Vector4f RED = Vector4f( 1.0f, 0.0f, 0.0f, 1.0f );
    static const Vector4f GREEN = Vector4f( 0.0f, 1.0f, 0.0f, 1.0f );
    static const Vector4f BLUE = Vector4f( 0.0f, 0.0f, 1.0f, 1.0f );
    static const Vector4f BROWN = Vector4f( 0.6f, 0.2f, 0.2f, 1.0f );
    static const Vector4f CYAN = Vector4f( 0.0f, 1.0f, 1.0f, 1.0f );
    static const Vector4f PURPLE = Vector4f( 1.0f, 0.0f, 1.0f, 1.0f );
    static const Vector4f NORMAL_MAP_COLOR = Vector4f( 0.5f, 0.5f, 1.0f, 1.0f );
    static const Vector4f PASTEL_GRAY = Vector4f( 64.0f / 255.0f, 64.0f / 255.0f, 64.0f / 255.0f, 1.0f );
    static const Vector4f PASTEL_GRAYLIGHT = Vector4f( 128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f );
    static const Vector4f PASTEL_ORANGE = Vector4f( 241.0f / 255.0f, 103.0f / 255.0f, 69.0f / 255.0f, 1.0f );
    static const Vector4f PASTEL_RED = Vector4f( 255.0f / 255.0f, 100.0f / 255.0f, 97.0f / 255.0f, 1.0f );
    static const Vector4f PASTEL_CYAN = Vector4f( 76.0f / 255.0f, 195.0f / 255.0f, 217.0f / 255.0f, 1.0f );
    static const Vector4f PASTEL_GREEN = Vector4f( 123.0f / 255.0f, 200.0f / 255.0f, 164.0f / 255.0f, 1.0f );
    static const Vector4f PASTEL_YELLOW = Vector4f( 255.0f / 255.0f, 198.0f / 255.0f, 93.0f / 255.0f, 1.0f );
    static const Vector4f PASTEL_BROWN = Vector4f( 131.0f / 255.0f, 105.0f / 255.0f, 83.0f / 255.0f, 1.0f );
    static const Vector4f SAND = Vector4f( 70.0f / 255.0f, 70.0f / 255.0f, 50.0f / 255.0f, 1.0f );
    static const Vector4f ORANGE_SCHEME1_1 = Vector4f( 255.0f / 255.0f, 116.0f / 255.0f, 0.0f / 255.0f, 1.0f );
    static const Vector4f ORANGE_SCHEME1_2 = Vector4f( 166.0f / 255.0f, 75.0f / 255.0f, 0.0f / 255.0f, 1.0f );
    static const Vector4f ORANGE_SCHEME1_3 = Vector4f( 255.0f / 255.0f, 178.0f / 115.0f, 0.0f / 255.0f, 1.0f );
    static const Vector4f ACQUA_T = Vector4f( .15f, 0.7f, .95f, 0.5f );
    static const Vector4f BLUE_SHADOW = Vector4f( 0.15294f, 0.0f, 0.53510f, 0.6f );
    static const Vector4f BRIGHT_PINK = Vector4f( 255.0f / 255.0f, 16.0f / 255.0f, 201.0f / 255.0f, 1.0f );
    static const Vector4f DARK_GRAY = Vector4f( 32.0f / 255.0f, 32.0f / 255.0f, 32.0f / 255.0f, 1.0f );
    static const Vector4f DARK_BLUE = Vector4f( 68.0f / 255.0f, 24.0f / 255.0f, 255.0f / 255.0f, 1.0f );
    static const Vector4f DARK_YELLOW = Vector4f( 232.0f / 255.0f, 184.0f / 255.0f, 49.0f / 255.0f, 1.0f );
    static const Vector4f DARK_RED = Vector4f( 173.0f / 255.0f, 22.0f / 255.0f, 41.0f / 255.0f, 1.0f );
    static const Vector4f DARK_GREEN = Vector4f( 44.0f / 255.0f, 82.0f / 255.0f, 5.0f / 255.0f, 1.0f );
    static const Vector4f DARK_BROWN = Vector4f( 59.0f / 255.0f, 28.0f / 255.0f, 12.0f / 255.0f, 1.0f );
    static const Vector4f DARK_PURPLE = Vector4f( 65.0f / 255.0f, 16.0f / 255.0f, 178.0f / 255.0f, 1.0f );
    static const Vector4f INDIAN_RED = Vector4f( 205.0f / 255.0f, 92.0f / 255.0f, 92.0f / 255.0f, 1.0f );
    static const Vector4f LIGHT_CORAL = Vector4f( 240.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f );
    static const Vector4f SALMON = Vector4f( 250.0f / 255.0f, 128.0f / 255.0f, 114.0f / 255.0f, 1.0f );
    static const Vector4f DARK_SALMON = Vector4f( 233.0f / 255.0f, 150.0f / 255.0f, 122.0f / 255.0f, 1.0f );
    static const Vector4f LIGHT_SALMON = Vector4f( 255.0f / 255.0f, 160.0f / 255.0f, 122.0f / 255.0f, 1.0f );
    static const Vector4f CRIMSON = Vector4f( 220.0f / 255.0f, 20.0f / 255.0f, 60.0f / 255.0f, 1.0f );
    static const Vector4f FIRE_BRICK = Vector4f( 178.0f / 255.0f, 30.0f / 255.0f, 30.0f / 255.0f, 1.0f );
    static const Vector4f PINK = Vector4f( 255.0f / 255.0f, 192.0f / 255.0f, 203.0f / 255.0f, 1.0f );
    static const Vector4f HOT_PINK = Vector4f( 255.0f / 255.0f, 105.0f / 255.0f, 180.0f / 255.0f, 1.0f );
    static const Vector4f DEEP_PINK = Vector4f( 255.0f / 255.0f, 20.0f / 255.0f, 150.0f / 255.0f, 1.0f );
    static const Vector4f GOLD = Vector4f( 255.0f / 255.0f, 215.0f / 255.0f, 0.0f / 255.0f, 1.0f );
    static const Vector4f PEACH = Vector4f( 255.0f / 255.0f, 218.0f / 255.0f, 185.0f / 255.0f, 1.0f );
    static const Vector4f LAVANDER = Vector4f( 230.0f / 255.0f, 230.0f / 255.0f, 250.0f / 255.0f, 1.0f );
    static const Vector4f VIOLET = Vector4f( 238.0f / 255.0f, 130.0f / 255.0f, 238.0f / 255.0f, 1.0f );
    static const Vector4f FUCHSIA = Vector4f( 255.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f, 1.0f );
    static const Vector4f DARK_VIOLET = Vector4f( 148.0f / 255.0f, 0.0f / 255.0f, 211.0f / 255.0f, 1.0f );
    static const Vector4f INDINGO = Vector4f( 75.0f / 255.0f, 0.0f / 255.0f, 130.0f / 255.0f, 1.0f );
    static const Vector4f SPRING_GREEN = Vector4f( 0.0f / 255.0f, 255.0f / 255.0f, 127.0f / 255.0f, 1.0f );
    static const Vector4f FOREST_GREEN = Vector4f( 34.0f / 255.0f, 139.0f / 255.0f, 34.0f / 255.0f, 1.0f );
    static const Vector4f OLIVE = Vector4f( 128.0f / 255.0f, 128.0f / 255.0f, 0.0f / 255.0f, 1.0f );
    static const Vector4f DARK_CYAN = Vector4f( 0.0f / 255.0f, 139.0f / 255.0f, 139.0f / 255.0f, 1.0f );
    static const Vector4f AQUAMARINE = Vector4f( 129.0f / 255.0f, 255.0f / 255.0f, 212.0f / 255.0f, 1.0f );
    static const Vector4f STEEL_BLUE = Vector4f( 70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f, 1.0f );
    static const Vector4f SKY_BLUE = Vector4f( 135.0f / 255.0f, 206.0f / 255.0f, 235.0f / 255.0f, 1.0f );
    static const Vector4f DODGER_BLUE = Vector4f( 30.0f / 255.0f, 144.0f / 255.0f, 255.0f / 255.0f, 1.0f );
    static const Vector4f ROYAL_BLUE = Vector4f( 65.0f / 255.0f, 105.0f / 255.0f, 255.0f / 255.0f, 1.0f );
    static const Vector4f NAVY = Vector4f( 0.0f / 255.0f, 0.0f / 255.0f, 128.0f / 255.0f, 1.0f );
    static const Vector4f ALMOND = Vector4f( 255.0f / 255.0f, 235.0f / 255.0f, 205.0f / 255.0f, 1.0f );
    static const Vector4f TAN = Vector4f( 210.0f / 255.0f, 180.0f / 255.0f, 140.0f / 255.0f, 1.0f );
    static const Vector4f SANDY_BROWN = Vector4f( 244.0f / 255.0f, 164.0f / 255.0f, 96.0f / 255.0f, 1.0f );
    static const Vector4f SIENNA = Vector4f( 160.0f / 255.0f, 82.0f / 255.0f, 45.0f / 255.0f, 1.0f );
    static const Vector4f MAROON = Vector4f( 128.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f );
    static const Vector4f SNOW = Vector4f( 255.0f / 255.0f, 250.0f / 255.0f, 250.0f / 255.0f, 1.0f );
    static const Vector4f HONEYDEW = Vector4f( 185.0f / 255.0f, 70.0f / 255.0f, 36.0f / 255.0f, 1.0f );
    static const Vector4f LIGHT_GREY = Vector4f( 211.0f / 255.0f, 211.0f / 255.0f, 211.0f / 255.0f, 1.0f );

    Vector4f XTORGBA( const std::string& _hexString );
    Vector4f ITORGBA( uint32_t number, int32_t numBits = 32 );
    Vector3f ITORGB( unsigned int r, unsigned int g, unsigned int b );
    Vector4f ITORGBA( unsigned int r, unsigned int g, unsigned int b, unsigned int a );
    Vector3f FTORGB( float r, float g, float b );
    Vector4f FTORGBA( float r, float g, float b, float a );
    Vector4f FTORGBA( const Vector4f& _val );
    Vector4f COLORA( const Vector4f& source, float a );
    Vector4f RANDA1();
}

namespace C4fc = V4fc;