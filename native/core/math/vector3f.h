#pragma once

#include <utility>
#include <cstring>
#include <vector>
#include "math_util.h"
#include "vector2f.h"
#include "vector2i.h"

// SHARED CLASS: Any changes to this, should be integrated to /svn/shared, and a mail should be sent to everyone



using namespace JMATH;

class Vector4f;

class Vector3f {
public:

	Vector3f() = default;

	explicit Vector3f( int xyz ) {
		mX = static_cast<float>(xyz);
		mY = static_cast<float>(xyz);
		mZ = static_cast<float>(xyz);
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

	explicit Vector3f( const unsigned char* _mem ) {
		std::memcpy( &mX, _mem, sizeof(float) * 3 );
	}

	template <typename T>
	explicit Vector3f( const std::vector<T> _v ) {
		ASSERT( _v.size() == 3 );
		mX = static_cast<float>(_v[0]);
		mY = static_cast<float>(_v[1]);
		mZ = static_cast<float>(_v[2]);
	}

    Vector3f( float x, float y ) {
        mX = x;
        mY = y;
        mZ = 0.0f;
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

	explicit Vector3f( const Vector2f& v2 ) {
		mX = v2.x();
		mY = v2.y();
		mZ = 0.0f;
	}

	explicit Vector3f( const Vector4f& v4 );
	
    Vector3f( const Vector2f& v2, float z ) {
		mX = v2.x();
		mY = v2.y();
		mZ = z;
	}

	explicit Vector3f( const Vector2i& v2, float z = 0.0f ) {
		mX = static_cast<float>(v2.x());
		mY = static_cast<float>(v2.y());
		mZ = z;
	}

	Vector3f( float x, const Vector2f& v2 ) {
		mX = x;
		mY = v2.x();
		mZ = v2.y();
	}

	explicit Vector3f( const float* coords ) {
		mX = coords[0];
		mY = coords[1];
		mZ = coords[2];
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

    [[nodiscard]] float r() const {
        return mX;
    }

    [[nodiscard]] float g() const {
        return mY;
    }

    [[nodiscard]] float b() const {
        return mZ;
    }

    [[nodiscard]] inline static Vector3f ZERO           () { return  Vector3f( 0.0f, 0.0f, 0.0f ); }
    [[nodiscard]] inline static Vector3f X_AXIS         () { return  Vector3f( 1.0f, 0.0f, 0.0f ); }
    [[nodiscard]] inline static Vector3f Y_AXIS         () { return  Vector3f( 0.0f, 1.0f, 0.0f ); }
    [[nodiscard]] inline static Vector3f UP_AXIS        () { return  Vector3f( 0.0f, 1.0f, 0.0f ); }
    [[nodiscard]] inline static Vector3f UP_AXIS_NEG    () { return  Vector3f( 0.0f, -1.0f, 0.0f ); }
    [[nodiscard]] inline static Vector3f Z_AXIS         () { return  Vector3f( 0.0f, 0.0f, 1.0f ); }
    [[nodiscard, maybe_unused]] inline static Vector3f X_AXIS_NEG     () { return  Vector3f( -1.0f, 0.0f, 0.0f ); }
    [[nodiscard, maybe_unused]] inline static Vector3f Y_AXIS_NEG     () { return  Vector3f( 0.0f, -1.0f, 0.0f ); }
    [[nodiscard, maybe_unused]] inline static Vector3f Z_AXIS_NEG     () { return  Vector3f( 0.0f, 0.0f, -1.0f ); }

    [[nodiscard]] float width() const {
        return mX;
    }

    [[nodiscard]] float height() const {
        return mY;
    }

    [[nodiscard]] float depth() const {
        return mZ;
    }

    [[maybe_unused]] float* rawPtr() {
		return &mX;
	}

	[[nodiscard, maybe_unused]] const float* rawPtr() const {
		return reinterpret_cast<const float*>( &mX );
	}

	void fill( float* _set ) const {
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

    [[maybe_unused]] void incX( float x ) {
		mX += x;
	}

    [[maybe_unused]] void incY( float y ) {
		mY += y;
	}

    [[maybe_unused]] void incZ( float z ) {
		mZ += z;
	}

	[[nodiscard]] Vector2f xy() const {
		return Vector2f( mX, mY );
	}

	[[nodiscard]] Vector2f yx() const {
		return Vector2f( mY, mX );
	}

	[[nodiscard]] Vector2f xz() const {
		return Vector2f( mX, mZ );
	}

	[[nodiscard]] Vector2f zx() const {
		return Vector2f( mZ, mX );
	}

	[[nodiscard]] Vector2f yz() const {
		return Vector2f( mY, mZ );
	}

	[[nodiscard]] Vector2f zy() const {
		return Vector2f( mZ, mY );
	}

	[[nodiscard]] Vector3f xzy() const {
		return{ mX, mZ, mY };
	}

	[[nodiscard]] Vector3f yxz() const {
		return{ mY, mX, mZ };
	}

	[[nodiscard]] Vector3f yzx() const {
		return{ mY, mZ, mX };
	}

	[[nodiscard]] Vector3f zxy() const {
		return{ mZ, mX, mY };
	}

	[[nodiscard]] Vector3f zyx() const {
		return{ mZ, mY, mX };
	}

	void invX() { mX *= -1.0f; }
	void invY() { mY *= -1.0f; }
	void invZ() { mZ *= -1.0f; }

    void oneMinusX() { mX = 1.0f - mX; }
    void oneMinusY() { mY = 1.0f - mY; }
    void oneMinusZ() { mZ = 1.0f - mZ; }

    [[nodiscard]] bool isValid() const;

    void swizzle( uint32_t i1, uint32_t i2 ) {
		ASSERT( i1 < 3 && i2 < 3 );
		float c = ( *this )[i1];
		( *this )[i1] = ( *this )[i2];
		( *this )[i2] = c;
	}

	[[nodiscard]] int size() const {
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

	void fmod( float _value );
	[[nodiscard]] Vector3f fmod( float _value ) const;

	[[nodiscard]] Vector3f dominant() const {
		if ( fabs( x() ) >= fabs( y() ) && fabs( x() ) >= fabs( z() ) ) {
			return Vector3f( mX, 0.0f, 0.0f );
		}
		if ( fabs( y() ) >= fabs( x() ) && fabs( y() ) >= fabs( z() ) ) {
			return Vector3f( 0.0f, mY, 0.0f );
		}
		return Vector3f( 0.0f, 0.0f, mZ );
	}

	[[nodiscard]] int dominantElement() const {
		if ( fabs( x() ) >= fabs( y() ) && fabs( x() ) >= fabs( z() ) ) {
			return 0;
		}
		if ( fabs( y() ) >= fabs( x() ) && fabs( y() ) >= fabs( z() ) ) {
			return 1;
		}
		return 2;
	}

	[[nodiscard]] Vector2f dominantVector2() const {
		if ( fabs( x() ) >= fabs( y() ) && fabs( x() ) >= fabs( z() ) ) {
			return Vector2f{ mY, mZ };
		}
		if ( fabs( y() ) >= fabs( x() ) && fabs( y() ) >= fabs( z() ) ) {
			return Vector2f{ mX, mZ };
		}
		return Vector2f{ mX, mY};
	}

	[[nodiscard]] std::pair<int32_t, int32_t> dominantPair() const {
		if ( fabs( x() ) >= fabs( y() ) && fabs( x() ) >= fabs( z() ) ) {
			return std::make_pair<int32_t, int32_t>( 1, 2 );
		}
		if ( fabs( y() ) >= fabs( x() ) && fabs( y() ) >= fabs( z() ) ) {
			return std::make_pair<int32_t, int32_t>( 0, 2 );
		}
		return std::make_pair<int32_t, int32_t>( 0, 1 );
	}

	[[nodiscard]] std::pair<int32_t, int32_t> leastDominantPair() const;

	[[nodiscard]] Vector2f pairMapped( const std::pair<int32_t, int32_t>& pm ) const {
		return Vector2f{ ( *this )[pm.first], ( *this )[pm.second] };
	}

	[[nodiscard]] int leastDominantElement() const {
		if ( fabs( x() ) <= fabs( y() ) && fabs( x() ) <= fabs( z() ) ) {
			return 0;
		}
		if ( fabs( y() ) <= fabs( x() ) && fabs( y() ) <= fabs( z() ) ) {
			return 1;
		}
		return 2;
	}

	[[nodiscard]] Vector3f sign() const {
		return Vector3f( mX > 0.0f ? 1.0f : -1.0f, mY > 0.0f ? 1.0f : -1.0f, mZ > 0.0f ? 1.0f : -1.0f );
	}

	[[nodiscard]] float signScalar() const {
		return (mX > 0.0f ? 1.0f : -1.0f) * (mY > 0.0f ? 1.0f : -1.0f) * (mZ > 0.0f ? 1.0f : -1.0f);
	}

	[[nodiscard]] float linearProduct() const {
		return mX * mY * mZ;
	}

	[[nodiscard]] float linearSum() const {
		return mX + mY + mZ;
	}

	[[nodiscard]] bool isTooSteep( float threshold = 1e25 ) const {
		if ( fabs( mX ) > threshold ) return true;
		if ( fabs( mY ) > threshold ) return true;
		if ( fabs( mZ ) > threshold ) return true;
		return false;
	}

	[[nodiscard]] size_t hash() const {
		std::hash<float> hasher;
		size_t seed = 0;
		seed ^= hasher( mX ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
		seed ^= hasher( mY ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
		seed ^= hasher( mZ ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
		return seed;
	}

	[[nodiscard]] std::string toString() const;
	[[nodiscard]] std::string toStringValuesOnly() const;
	[[nodiscard]] std::string toStringJSONArray() const;
    [[nodiscard]] std::string toStringObj( const std::string& _prefix) const;

	void writeTo( std::basic_ostream<char, std::char_traits<char>> &stream, char separator = ',', char endChar = 0 ) const {
		stream << mX << separator << mY << separator << mZ;
		if ( endChar > 0 )
			stream << endChar;
	}

	friend std::ostream& operator<<( std::ostream& o, const Vector3f& val ) {
		o << val.toStringValuesOnly();
		return o;
	}

    static constexpr int32_t vSize() { return 3; }

private:
	float mX = 0.0f;
	float mY = 0.0f;
	float mZ = 0.0f;
};

using V3f = Vector3f;
using Color3f = Vector3f;
using Vector3fList = std::vector<Vector3f>;
using V3fVector         = std::vector<Vector3f>;
using V3fVectorOfVector = std::vector<V3fVector>;
using V3fVectorWrap = VectorWrap<V3f>;
using V3fVectorOfVectorWrap = VectorOfVectorWrap<V3f>;
using VTMVectorWrap = VectorWrapT2<V3f, V2f>;
using VTMVectorOfVectorWrap = VectorOfVectorWrapT2<V3f, V2f>;
using Triangle3d   = std::tuple<Vector3f, Vector3f, Vector3f>;
using QuadVector3f = std::array<Vector3f, 4>;
using QuadVector3fList = std::vector<QuadVector3f>;

struct XZY {
    enum Conversion {
        PassThrough = 0,
        Convert = 1
    };

    static Vector3f C( const Vector3f& _v );
    static Vector3f C( const Vector2f& _v );
    static std::vector<Vector3f> C( const std::vector<Vector3f>& _v, XZY::Conversion convertOrPassThrough );
    static Vector3f C( const Vector2f& v2, float z );
    static Vector2f C2( const Vector3f& v2 );
    static Vector3f C( float x, float y, float z );
    static std::vector<Vector3f> C( const std::vector<Vector3f>& _v );
    static std::vector<Vector3f> C( const std::vector<Vector3f>& _v, float _z );
    static std::vector<Vector3f> C( const std::vector<Vector2f>& _v, float _z = 0.0f );
    static std::vector<Vector2f> C2( const std::vector<Vector3f>& _v );
};

namespace V3fc {
    static const Vector3f ZERO = Vector3f( 0.0f, 0.0f, 0.0f );
    static const Vector3f X_AXIS = Vector3f( 1.0f, 0.0f, 0.0f );
    static const Vector3f Y_AXIS = Vector3f( 0.0f, 1.0f, 0.0f );
    static const Vector3f UP_AXIS = Vector3f( 0.0f, 1.0f, 0.0f );
    static const Vector3f UP_AXIS_NEG = Vector3f( 0.0f, -1.0f, 0.0f );
    static const Vector3f Z_AXIS = Vector3f( 0.0f, 0.0f, 1.0f );
    static const Vector3f X_AXIS_NEG = Vector3f( -1.0f, 0.0f, 0.0f );
    static const Vector3f Y_AXIS_NEG = Vector3f( 0.0f, -1.0f, 0.0f );
    static const Vector3f Z_AXIS_NEG = Vector3f( 0.0f, 0.0f, -1.0f );

    static const Vector3f X_AXIS_NEG_MASK = Vector3f( -1.0f, 1.0f, 1.0f );
    static const Vector3f Y_AXIS_NEG_MASK = Vector3f( 1.0f, -1.0f, 1.0f );
    static const Vector3f Z_AXIS_NEG_MASK = Vector3f( 1.0f, 1.0f, -1.0f );

    static const Vector3f MASK_X_OUT = Vector3f( 0.0f, 1.0f, 1.0f );
    static const Vector3f MASK_Y_OUT = Vector3f( 1.0f, 0.0f, 1.0f );
    static const Vector3f MASK_Z_OUT = Vector3f( 1.0f, 1.0f, 0.0f );
    static const Vector3f MASK_UP_OUT = Vector3f( 1.0f, 0.0f, 1.0f );

//    static const Vector3f UE4_PROFILE_AXIS = Vector3f( 0.0f, 0.0f, -1.0f );
    static const Vector3f ONE = Vector3f( 1.0f, 1.0f, 1.0f );
    static const Vector3f HUGE_VALUE_POS = Vector3f( std::numeric_limits<float>::max() );
    static const Vector3f HUGE_VALUE_NEG = Vector3f( std::numeric_limits<float>::lowest() );
}

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

inline Vector2f dominantMapping( const V3f& n, const V3f& v, const V3f& size = V3fc::ONE ) {
    switch ( n.dominantElement() ) {
        case 0:
            return ( n[0] > 0.0f ? v.yz() : -v.yz()) * size.yz();
        case 1:
            return ( n[1] > 0.0f ? v.zx() : -v.zx()) * size.zx();
    }

    return ( n[2] > 0.0f ? v.xy() : -v.xy() ) * size.xy();
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
	if ( p1 == V3fc::ZERO ) return true;
	if ( p2 == V3fc::ZERO ) return true;

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

[[maybe_unused]] inline static Vector3f min( const Vector3f& a, const Vector3f& b ) {
	return Vector3f( std::min( a.x(), b.x() ), std::min( a.y(), b.y() ), std::min( a.z(), b.z() ) );
}

[[maybe_unused]] inline static Vector3f max( const Vector3f& a, const Vector3f& b ) {
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

inline Vector3f sphericalToCartesian( const Vector3f& spherical ) {
	Vector3f ret;

	ret.setY( cosf( spherical.x() ) * sinf( spherical.y() ) * spherical.z() );
	ret.setX( sinf( spherical.x() ) * sinf( spherical.y() ) * spherical.z() );
	ret.setZ( cosf( spherical.y() ) * spherical.z() );

	return ret;
}

inline Vector3f cartesianToSpherical( const Vector3f& cartesian ) {
	Vector3f ret;

	float r = length( cartesian );
	ret.setX( atan2( cartesian.x(), cartesian.y() ) );
	ret.setY( acos( cartesian.z() / r ) );
	ret.setZ( r );

	return ret;
}

void removeCollinear( std::vector<Vector3f>& cs );
void tbCalc( const Vector3f& v1, const Vector3f& v2, const Vector3f& v3,
             const Vector2f& uv1, const Vector2f& uv2, const Vector2f& uv3,
             Vector4f& tangent1, Vector4f& tangent2, Vector4f& tangent3,
             Vector3f& bitangent1, Vector3f& bitangent2, Vector3f& bitangent3 );

struct ExtrudeStrip {};
struct ExtrudeContour {};

template <typename T>
std::vector<Vector3f> extrudePointsWithWidth( const std::vector<Vector3f>& va, float width, bool wrapIt = false )  {
    ASSERT( va.size() > 1 );

    Vector3f v1;
    Vector3f v2;
    std::vector<Vector3f> vList;
    vList.reserve(va.size()*2);

    std::vector<Vector3f> vList1{};
    std::vector<Vector3f> vList2{};
    if constexpr ( std::is_same_v<T, ExtrudeContour> ) {
        vList1.reserve( va.size());
        vList2.reserve( va.size());
    }

    for ( int m = 0; m < static_cast<int>( va.size()); m++ ) {
        Vector3f vLeft = getLeftVectorFromList(va, m, wrapIt );
        Vector3f vRight = getRightVectorFromList(va, m, wrapIt );

        v1 = vLeft - va[m];
        v2 = va[m] - vRight;

        auto v1n = normalize( v1 );
        auto v2n = normalize( v2 );

        v1 = XZY::C(V3f{rotate90( v1n.xz())});
        v2 = XZY::C(V3f{rotate90( v2n.xz())});

        if ( !isValid( v1.x()) || !isValid( v2.x())) {
            continue;
        }

        float nDot = dot(v1, v2 );
        float extWidth = JMATH::lerp(nDot, sqrtf(width * width + width * width ), width );
        Vector3f vn = v1 + v2;  //crossProduct(v1, v2, V3fc::Z_AXIS);
        if ( length( vn ) == 0.0f ) {
            vn = v2;
        }
        vn = normalize( vn );

        v2 = va[m] + ( vn * 0.5f * extWidth );
        v1 = va[m] + ( vn * -0.5f * extWidth );

        if constexpr ( std::is_same_v<T, ExtrudeStrip> ) {
            vList.push_back( v1 );
            vList.push_back( v2 );
        }

        if constexpr ( std::is_same_v<T, ExtrudeContour> ) {
            vList1.push_back( v1 );
            vList2.push_back( v2 );
        }
    }
    if ( wrapIt ) {
        if constexpr ( std::is_same_v<T, ExtrudeStrip> ) {
            vList.push_back( vList.front() );
            vList.push_back( *(vList.begin()+1) );
        }

        if constexpr ( std::is_same_v<T, ExtrudeContour> ) {
            vList1.push_back( vList1.front() );
            vList2.push_back( vList2.front() );
        }
    }

    if constexpr ( std::is_same_v<T, ExtrudeStrip> ) {
        return vList;
    }

    if constexpr ( std::is_same_v<T, ExtrudeContour> ) {
        for ( auto& t : vList2 ) {
            vList.emplace_back( t );
        }
        for ( int t = static_cast<int>(vList1.size() - 1); t >= 0; t-- ) {
            vList.emplace_back( vList1[t] );
        }
        return vList;
    }
}

