#pragma once

#include <utility>
#include <cstring>
#include <vector>
#include "../util.h"
#include "math_util.h"
#include "vector2f.h"

// SHARED CLASS: Any changes to this, should be integrated to /svn/shared, and a mail should be sent to everyone



using namespace JMATH;

class Vector4f;

class Vector3f {
public:

	static const Vector3f ZERO;
	static const Vector3f X_AXIS;
	static const Vector3f Y_AXIS;
	static const Vector3f Z_AXIS;
	static const Vector3f X_AXIS_NEG;
	static const Vector3f Y_AXIS_NEG;
	static const Vector3f Z_AXIS_NEG;
	static const Vector3f X_AXIS_NEG_MASK;
	static const Vector3f Y_AXIS_NEG_MASK;
	static const Vector3f Z_AXIS_NEG_MASK;
	static const Vector3f MASK_X_OUT;
	static const Vector3f MASK_Y_OUT;
	static const Vector3f MASK_Z_OUT;
	static const Vector3f MASK_UP_OUT;
	static const Vector3f UE4_PROFILE_AXIS;
	static const Vector3f UE4_PROFILE_AXIS_NORM;
	static const Vector3f UP_AXIS;
	static const Vector3f ONE;
	static const Vector3f HUGE_VALUE_POS;
	static const Vector3f HUGE_VALUE_NEG;

	Vector3f() {
	}

	explicit Vector3f( int xyz ) {
		mX = xyz;
		mY = xyz;
		mZ = xyz;
	}

	explicit Vector3f( float xyz ) {
		mX = xyz;
		mY = xyz;
		mZ = xyz;
	}

	explicit Vector3f( double xyz ) {
		mX = static_cast<float>(xyz);
		mY = static_cast<float>(xyz);
		mZ = static_cast<float>(xyz);
	}

	Vector3f( const unsigned char* _mem ) {
		std::memcpy( &mX, _mem, sizeof(float) * 3 );
	}

	template <typename T>
	Vector3f( const std::vector<T> _v ) {
		ASSERT( _v.size() == 3 );
		mX = static_cast<float>(_v[0]);
		mY = static_cast<float>(_v[1]);
		mZ = static_cast<float>(_v[2]);
	}

	Vector3f( float x, float y, float z ) {
		mX = x;
		mY = y;
		mZ = z;
	}

	Vector3f( double x, double y, double z ) {
		mX = static_cast<float>( x );
		mY = static_cast<float>( y );
		mZ = static_cast<float>( z );
	}

	Vector3f( double x, float y, float z ) {
		mX = static_cast<float>( x );
		mY = static_cast<float>( y );
		mZ = static_cast<float>( z );
	}

	Vector3f( float x, double y, float z ) {
		mX = static_cast<float>( x );
		mY = static_cast<float>( y );
		mZ = static_cast<float>( z );
	}

	Vector3f( float x, float y, double z ) {
		mX = static_cast<float>( x );
		mY = static_cast<float>( y );
		mZ = static_cast<float>( z );
	}

	Vector3f( float x, double y, double z ) {
		mX = static_cast<float>( x );
		mY = static_cast<float>( y );
		mZ = static_cast<float>( z );
	}

	Vector3f( double x, float y, double z ) {
		mX = static_cast<float>( x );
		mY = static_cast<float>( y );
		mZ = static_cast<float>( z );
	}

	Vector3f( double x, double y, float z ) {
		mX = static_cast<float>( x );
		mY = static_cast<float>( y );
		mZ = static_cast<float>( z );
	}

	Vector3f( const Vector2f& v2 ) {
		mX = v2.x();
		mY = v2.y();
		mZ = 0.0f;
	}

	Vector3f( const Vector4f& v4 );

	Vector3f( const Vector2f& v2, float z ) {
		mX = v2.x();
		mY = v2.y();
		mZ = z;
	}

	Vector3f( float x, const Vector2f& v2 ) {
		mX = x;
		mY = v2.x();
		mZ = v2.y();
	}

	Vector3f( const float* coords ) {
		mX = coords[0];
		mY = coords[1];
		mZ = coords[2];
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

	void fill( float* _set ) {
		_set[0] = mX;
		_set[1] = mY;
		_set[2] = mZ;
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

	void incX( float x ) {
		mX += x;
	}

	void incY( float y ) {
		mY += y;
	}

	void incZ( float z ) {
		mZ += z;
	}

	Vector2f xy() const {
		return Vector2f( mX, mY );
	}

	Vector2f yx() const {
		return Vector2f( mY, mX );
	}

	Vector2f xz() const {
		return Vector2f( mX, mZ );
	}

	Vector2f zx() const {
		return Vector2f( mZ, mX );
	}

	Vector2f yz() const {
		return Vector2f( mY, mZ );
	}

	Vector2f zy() const {
		return Vector2f( mZ, mY );
	}

	Vector3f xzy() const {
		return{ mX, mZ, mY };
	}

	Vector3f yxz() const {
		return{ mY, mX, mZ };
	}

	Vector3f yzx() const {
		return{ mY, mZ, mX };
	}

	Vector3f zxy() const {
		return{ mZ, mX, mY };
	}

	Vector3f zyx() const {
		return{ mZ, mY, mX };
	}

	void invX() { mX *= -1.0f; }
	void invY() { mY *= -1.0f; }
	void invZ() { mZ *= -1.0f; }

	void swizzle( uint32_t i1, uint32_t i2 ) {
		ASSERT( i1 < 3 && i2 < 3 );
		float c = ( *this )[i1];
		( *this )[i1] = ( *this )[i2];
		( *this )[i2] = c;
	}

	int32_t size() const {
		return 3;
	}

	float operator[]( int element ) const {
		ASSERT( element >= 0 && element < 3 );
		return ( &mX )[element];
	}

	float& operator[]( int element ) {
		ASSERT( element >= 0 && element < 3 );
		return ( &mX )[element];
	}

	bool operator==( const Vector3f& rhs ) const {
		return mX == rhs.mX && mY == rhs.mY && mZ == rhs.mZ;
	}

	bool operator!=( const Vector3f& rhs ) const {
		return mX != rhs.mX || mY != rhs.mY || mZ != rhs.mZ;
	}
	bool operator!=( const float& rhs ) const {
		return mX != rhs || mY != rhs || mZ != rhs;
	}

	Vector3f operator+( const Vector3f& rhs ) const {
		return Vector3f( mX + rhs.mX, mY + rhs.mY, mZ + rhs.mZ );
	}

	Vector3f operator+( float rhs ) const {
		return Vector3f( mX + rhs, mY + rhs, mZ + rhs );
	}

	Vector3f operator-( const Vector3f& rhs ) const {
		return Vector3f( mX - rhs.mX, mY - rhs.mY, mZ - rhs.mZ );
	}

	Vector3f operator-( float rhs ) const {
		return Vector3f( mX - rhs, mY - rhs, mZ - rhs );
	}

	Vector3f operator*( const Vector3f& rhs ) const {
		return Vector3f( mX * rhs.mX, mY * rhs.mY, mZ * rhs.mZ );
	}

	Vector3f operator*( float rhs ) const {
		return Vector3f( mX * rhs, mY * rhs, mZ * rhs );
	}

	Vector3f operator/( float rhs ) const {
		float recip = 1.0f / rhs;
		return Vector3f( mX * recip, mY * recip, mZ * recip );
	}

	Vector3f operator+=( const Vector3f& rhs ) {
		mX += rhs.mX;
		mY += rhs.mY;
		mZ += rhs.mZ;
		return *this;
	}

	Vector3f operator+=( float rhs ) {
		mX += rhs;
		mY += rhs;
		mZ += rhs;
		return *this;
	}

	Vector3f operator-=( const Vector3f& rhs ) {
		mX -= rhs.mX;
		mY -= rhs.mY;
		mZ -= rhs.mZ;
		return *this;
	}

	Vector3f operator-=( float rhs ) {
		mX -= rhs;
		mY -= rhs;
		mZ -= rhs;
		return *this;
	}

	Vector3f operator*=( const Vector3f& rhs ) {
		mX *= rhs.mX;
		mY *= rhs.mY;
		mZ *= rhs.mZ;
		return *this;
	}

	Vector3f operator*=( float rhs ) {
		mX *= rhs;
		mY *= rhs;
		mZ *= rhs;
		return *this;
	}

	Vector3f operator/=( float rhs ) {
		mX /= rhs;
		mY /= rhs;
		mZ /= rhs;
		return *this;
	}

	Vector3f operator-() const {
		return Vector3f( -mX, -mY, -mZ );
	}

	Vector3f dominant() const {
		if ( fabs( x() ) >= fabs( y() ) && fabs( x() ) >= fabs( z() ) ) {
			return Vector3f( mX, 0.0f, 0.0f );
		}
		if ( fabs( y() ) >= fabs( x() ) && fabs( y() ) >= fabs( z() ) ) {
			return Vector3f( 0.0f, mY, 0.0f );
		}
		return Vector3f( 0.0f, 0.0f, mZ );
	}

	int dominantElement() const {
		if ( fabs( x() ) >= fabs( y() ) && fabs( x() ) >= fabs( z() ) ) {
			return 0;
		}
		if ( fabs( y() ) >= fabs( x() ) && fabs( y() ) >= fabs( z() ) ) {
			return 1;
		}
		return 2;
	}

	Vector2f dominantVector2() const {
		if ( fabs( x() ) >= fabs( y() ) && fabs( x() ) >= fabs( z() ) ) {
			return Vector2f{ mY, mZ };
		}
		if ( fabs( y() ) >= fabs( x() ) && fabs( y() ) >= fabs( z() ) ) {
			return Vector2f{ mX, mZ };
		}
		return Vector2f{ mX, mY};
	}

	std::pair<int32_t, int32_t> dominantPair() const {
		if ( fabs( x() ) >= fabs( y() ) && fabs( x() ) >= fabs( z() ) ) {
			return std::make_pair<int32_t, int32_t>( 1, 2 );
		}
		if ( fabs( y() ) >= fabs( x() ) && fabs( y() ) >= fabs( z() ) ) {
			return std::make_pair<int32_t, int32_t>( 0, 2 );
		}
		return std::make_pair<int32_t, int32_t>( 0, 1 );
	}

	std::pair<int32_t, int32_t> leastDominantPair() const {
		if ( fabs( x() ) <= fabs( y() ) && fabs( x() ) <= fabs( z() ) ) {
			return std::make_pair<int32_t, int32_t>( 1, 2 );
		}
		if ( fabs( y() ) <= fabs( x() ) && fabs( y() ) <= fabs( z() ) ) {
			return std::make_pair<int32_t, int32_t>( 0, 2 );
		}
		return std::make_pair<int32_t, int32_t>( 0, 1 );
	}

	Vector2f pairMapped( const std::pair<int32_t, int32_t>& pm ) const {
		return Vector2f{ ( *this )[pm.first], ( *this )[pm.second] };
	}

	int leastDominantElement() const {
		if ( fabs( x() ) <= fabs( y() ) && fabs( x() ) <= fabs( z() ) ) {
			return 0;
		}
		if ( fabs( y() ) <= fabs( x() ) && fabs( y() ) <= fabs( z() ) ) {
			return 1;
		}
		return 2;
	}

	Vector3f sign() const {
		return Vector3f( mX > 0.0f ? 1.0f : -1.0f, mY > 0.0f ? 1.0f : -1.0f, mZ > 0.0f ? 1.0f : -1.0f );
	}

	float signScalar() const {
		return (mX > 0.0f ? 1.0f : -1.0f) * (mY > 0.0f ? 1.0f : -1.0f) * (mZ > 0.0f ? 1.0f : -1.0f);
	}

	float linearProduct() const {
		return mX * mY * mZ;
	}

	bool isTooSteep( float threshold = 1e25 ) {
		if ( fabs( mX ) > threshold ) return true;
		if ( fabs( mY ) > threshold ) return true;
		if ( fabs( mZ ) > threshold ) return true;
		return false;
	}

	size_t hash() const {
		std::hash<float> hasher;
		size_t seed = 0;
		seed ^= hasher( mX ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
		seed ^= hasher( mY ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
		seed ^= hasher( mZ ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
		return seed;
	}

	std::string toString() const;
	std::string toStringValuesOnly() const;
	std::string toStringJSONArray() const;

	void writeTo( std::basic_ostream<char, std::char_traits<char>> &stream, char separator = ',', char endChar = 0 ) const {
		stream << mX << separator << mY << separator << mZ;
		if ( endChar > 0 )
			stream << endChar;
	}

	friend std::ostream& operator<<( std::ostream& o, const Vector3f& val ) {
		o << val.toStringValuesOnly();
		return o;
	}

private:
	float mX;
	float mY;
	float mZ;
};

inline float dot( const Vector3f& a, const Vector3f& b ) {
	return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
}

inline float length( const Vector3f& v ) {
	return length( v.x(), v.y(), v.z() );
}

inline float lengthSquared( const Vector3f& v ) {
	return lengthSquared( v.x(), v.y(), v.z() );
}

inline Vector3f normalize( const Vector3f& v ) {
	float len = length( v );
	ASSERT( isValid( len ) );
	Vector3f result = v * ( 1.0f / len );
	//	ASSERT( isValid( result.x() ) );
	return result;
}

inline Vector3f cross( const Vector3f& a, const Vector3f& b ) {
	return Vector3f( a.y() * b.z() - a.z() * b.y(), a.z() * b.x() - a.x() * b.z(), a.x() * b.y() - a.y() * b.x() );
}

inline Vector3f crossProduct( const Vector3f& v1, const Vector3f& v2, const Vector3f& v3 ) {
	Vector3f a = normalize( v1 - v2 );
	Vector3f b = normalize( v3 - v2 );

	return Vector3f( a.y() * b.z() - a.z() * b.y(), a.z() * b.x() - a.x() * b.z(), a.x() * b.y() - a.y() * b.x() );
}

inline bool isVerySimilar( const Vector3f& a, const Vector3f& b, const float epsilon = 0.001f ) {
	return isScalarEqual( a.x(), b.x(), epsilon ) && isScalarEqual( a.y(), b.y(), epsilon ) && isScalarEqual( a.z(), b.z(), epsilon );
}

inline bool isCollinear( const Vector3f& a, const Vector3f& b, const Vector3f& c,
						 [[maybe_unused]] const float epsilon = 0.000001f ) {
	// Check if two coords are the same, then it's just a line
	if ( a.xy() == b.xy() && a.xy() == c.xy() ) return true;
	if ( a.xz() == b.xz() && a.xz() == c.xz() ) return true;
	if ( a.yz() == b.yz() && a.yz() == c.yz() ) return true;

	if ( a == b || b == c || c == a ) return true;
	Vector3f p1 = a - b;
	Vector3f p2 = c - b;
	if ( p1 == Vector3f::ZERO ) return true;
	if ( p2 == Vector3f::ZERO ) return true;

	p1 = normalize( p1 );
	p2 = normalize( p2 );

	Vector3f cr = Vector3f( p1.y() * p2.z() - p1.z() * p2.y(), p1.z() * p2.x() - p1.x() * p2.z(), p1.x() * p2.y() - p1.y() * p2.x() );

	auto l = length( cr );
	if ( !isValid( l ) ) return true;

	auto nn = normalize( crossProduct( a, b, c ) );
	if ( !isValid( nn.x() ) ) return true;

	return false;
}

inline Vector3f absolute( const Vector3f& v ) {
	return Vector3f( absolute( v.x() ), absolute( v.y() ), absolute( v.z() ) );
}

inline Vector3f saturate( const Vector3f& v ) {
	return Vector3f( saturate( v.x() ), saturate( v.y() ), saturate( v.z() ) );
}

inline static Vector3f min( const Vector3f& a, const Vector3f& b ) {
	return Vector3f( std::min( a.x(), b.x() ), std::min( a.y(), b.y() ), std::min( a.z(), b.z() ) );
}

inline static Vector3f max( const Vector3f& a, const Vector3f& b ) {
	return Vector3f( max( a.x(), b.x() ), max( a.y(), b.y() ), std::max( a.z(), b.z() ) );
}

inline float minElement( const Vector3f & v ) {
	return min3( v.x(), v.y(), v.z() );
}

inline float maxElement( const Vector3f & v ) {
	return max3( v.x(), v.y(), v.z() );
}

inline float sumElements( const Vector3f & v ) {
	return v.x() + v.y() + v.z();
}

inline Vector3f fma( const Vector3f& a, const Vector3f& b, const Vector3f& c ) {
	return Vector3f( a.x() * b.x() + c.x(), a.y() * b.y() + c.y(), a.z() * b.z() + c.z() );
}

inline Vector3f sphericalToCartasian( const Vector3f& spherical ) {
	Vector3f ret;

	ret.setY( cosf( spherical.x() ) * sinf( spherical.y() ) * spherical.z() );
	ret.setX( sinf( spherical.x() ) * sinf( spherical.y() ) * spherical.z() );
	ret.setZ( cosf( spherical.y() ) * spherical.z() );

	return ret;
}

inline Vector3f cartasianToSpherical( const Vector3f& cartesian ) {
	Vector3f ret;

	float r = length( cartesian );
	ret.setX( atan2( cartesian.x(), cartesian.y() ) );
	ret.setY( acos( cartesian.z() / r ) );
	ret.setZ( r );

	return ret;
}

void removeCollinear( std::vector<Vector3f>& cs );

std::vector<Vector3f> extrudePointsWithWidth( const std::vector<Vector3f>& va, float width, bool wrapIt,
							 float rotAngle = 0.0f, float percToBeDrawn = 1.0f );

typedef Vector3f Color3f;
typedef std::vector<Vector3f> vector3fList;
typedef std::tuple<Vector3f, Vector3f, Vector3f> Triangle3d;
using QuadVector3f = std::array<Vector3f, 4>;
using QuadVector3fList = std::vector<QuadVector3f>;

struct XZY {
	static Vector3f C( const Vector3f& _v );
	static Vector3f C( const Vector2f& v2, const float z );
	static Vector3f C( const float x, const float y, const float z );
	static std::vector<Vector3f> C( const std::vector<Vector3f>& _v );
	static std::vector<Vector3f> C( const std::vector<Vector2f>& _v, float _z = 0.0f );
};
