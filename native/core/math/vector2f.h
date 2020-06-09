#pragma once

#include <cmath>
#include <vector>
#include <ostream>
#include "math_util.h"
#include "vector2i.h"

// SHARED CLASS: Any changes to this, should be integrated to /svn/shared, and a mail should be sent to everyone

using namespace JMATH;

class Vector3f;

class Vector2f {
public:
    friend std::ostream& operator<<( std::ostream& os, const Vector2f& f );

	Vector2f() = default;

	explicit Vector2f( int xy ) {
		mX = xy;
		mY = xy;
	}

	explicit Vector2f( float xy ) {
		mX = xy;
		mY = xy;
	}

	Vector2f( float x, float y ) {
		mX = x;
		mY = y;
	}

	Vector2f( double x, double y ) {
		mX = static_cast<float>( x );
		mY = static_cast<float>( y );
	}

	Vector2f( int x, int y ) {
		mX = static_cast<float>( x );
		mY = static_cast<float>( y );
	}

	explicit Vector2f( const Vector2i& v ) {
		mX = static_cast<float>( v.x() );
		mY = static_cast<float>( v.y() );
	}

    Vector2f( const Vector3f& v );

    template <typename T>
    explicit Vector2f( const std::vector<T> _v ) {
        ASSERT( _v.size() == 2 );
        mX = static_cast<float>(_v[0]);
        mY = static_cast<float>(_v[1]);
    }

	Vector2f( float* _xy ) {
		mX = _xy[0];
		mY = _xy[1];
	}

	float x() const {
		return mX;
	}

	float y() const {
		return mY;
	}

	float xss() const {
		return mX * 2.0f - 1.0f;
	}

	float yss() const {
		return mY * 2.0f - 1.0f;
	}

	Vector2f xx() const {
		return Vector2f( mX, mX );
	}

	Vector2f x0() const {
		return Vector2f( mX, 0.0f );
	}

	Vector2f yy() const {
		return Vector2f( mY, mY );
	}

	Vector2f yx() const {
		return Vector2f( mY, mX );
	}

	Vector2f y0() const {
		return Vector2f( 0.0f, mY );
	}

	void swizzle( uint32_t i1, uint32_t i2 ) {
		ASSERT( i1 < 2 && i2 < 2 );
		float c = ( *this )[i1];
		( *this )[i1] = ( *this )[i2];
		( *this )[i2] = c;
	}

	void fill( float* _set ) const {
		_set[0] = mX;
		_set[1] = mY;
	}

	void set( float x, float y ) {
		mX = x;
		mY = y;
	}

	void setX( float x ) {
		mX = x;
	}

	void setY( float y ) {
		mY = y;
	}

	void incX( float x ) {
		mX += x;
	}

	void incY( float y ) {
		mY += y;
	}

	int32_t size() const {
		return 2;
	}

	int64_t hash() const;

	float operator[]( int element ) const {
		ASSERT( element >= 0 && element < 2 );
		return ( &mX )[element];
	}

	float& operator[]( int element ) {
		ASSERT( element >= 0 && element < 2 );
		return ( &mX )[element];
	}

	bool operator==( const Vector2f& rhs ) const {
		return mX == rhs.mX && mY == rhs.mY;
	}

	bool operator!=( const Vector2f& rhs ) const {
		return mX != rhs.mX || mY != rhs.mY;
	}

	Vector2f operator+( const Vector2f& rhs ) const {
		return Vector2f( mX + rhs.mX, mY + rhs.mY );
	}

	void operator+=( const Vector2f& rhs ) {
		mX += rhs.x();
		mY += rhs.y();
	}

	void operator-=( const Vector2f& rhs ) {
		mX -= rhs.x();
		mY -= rhs.y();
	}

	void operator-=( float s ) {
		mX -= s;
		mY -= s;
	}

	Vector2f operator+( float rhs ) const {
		return Vector2f( mX + rhs, mY + rhs );
	}

	Vector2f operator-( const Vector2f& rhs ) const {
		return Vector2f( mX - rhs.mX, mY - rhs.mY );
	}

	Vector2f operator-() const {
		return Vector2f( -mX, -mY );
	}

	Vector2f operator-( float rhs ) const {
		return Vector2f( mX - rhs, mY - rhs );
	}

	Vector2f operator*( const Vector2f& rhs ) const {
		return Vector2f( mX * rhs.mX, mY * rhs.mY );
	}

	Vector2f operator/( const Vector2f& rhs ) const {
		return Vector2f( mX / rhs.mX, mY / rhs.mY );
	}

	Vector2f operator*( float rhs ) const {
		return Vector2f( mX * rhs, mY * rhs );
	}

	Vector2f operator/( float rhs ) const {
		const float recip = 1.0f / rhs;
		return Vector2f( mX * recip, mY * recip );
	}

    bool operator<=( const Vector2f& rhs ) const {
        return mX <= rhs.mX && mY <= rhs.mY;
    }

    bool operator<( const Vector2f& rhs ) const {
        return mX < rhs.mX && mY < rhs.mY;
    }

    bool operator>=( const Vector2f& rhs ) const {
        return mX >= rhs.mX && mY >= rhs.mY;
    }

    bool operator>( const Vector2f& rhs ) const {
        return mX > rhs.mX && mY > rhs.mY;
    }

    void operator*=( float rhs ) {
		mX *= rhs;
		mY *= rhs;
	}

	void operator*=( const Vector2f& rhs ) {
		mX *= rhs.x();
		mY *= rhs.y();
	}

	void operator+=( float rhs ) {
		mX += rhs;
		mY += rhs;
	}

	void operator/=( float rhs ) {
		mX /= rhs;
		mY /= rhs;
	}

	void operator/=( const Vector2f& rhs ) {
		mX /= rhs.x();
		mY /= rhs.y();
	}

    float cross( const Vector2f& _v ) {
        return mX * _v.y() - mY * _v.x();
    }

    Vector2f ss() const {
		return Vector2f( xss(), yss() );
	}

	Vector2f ssYInv() const {
		return Vector2f( xss(), -yss() );
	}

	Vector2f invert() {
        mX *= -1.0f;
        mY *= -1.0f;
        return *this;
	}

	void invertY() {
		mY *= -1.0f;
	}

    void invertX() {
        mX *= -1.0f;
    }

    Vector2f invX() const {
		return{ -mX, mY };
	}

	Vector2f invY() const {
		return{ mX, -mY };
	}

    void oneMinusX() { mX = 1.0f - mX; }
    void oneMinusY() { mY = 1.0f - mY; }

	Vector2f dominant() const {
		if ( fabs( x() ) >= fabs( y() ) ) {
			return Vector2f( mX, 0.0f );
		}
		return Vector2f( 0.0f, mY );
	}

	Vector2f dominantFill() const {
		if ( fabs( x() ) >= fabs( y() ) ) {
			return Vector2f( mX, mX );
		}
		return Vector2f( mY, mY );
	}

	inline void reciprocal() {
		mX = 1.0f / mX;
		mY = 1.0f / mY;
	}

	inline float ratio() const {
		ASSERT( mY != 0.0f );
		return mX / mY;
	}

	inline Vector2f vectorRatio() const {
		ASSERT( mY != 0.0f && mX != 0.0f);
		if ( mX > mY ) return { mX / mY, 1.0f};
		return { 1.0f, mY / mX };
	}

	int dominantElement() const {
		if ( fabs( x() ) >= fabs( y() ) ) {
			return 0;
		}
		return 1;
	}

	float dominantValue() const {
		if ( fabs( x() ) >= fabs( y() ) ) {
			return mX;
		}
		return mY;
	}

	int leastDominantElement() const {
		if ( fabs( x() ) >= fabs( y() ) ) {
			return 1;
		}
		return 0;
	}

    inline void rotate( float angle ) {
		Vector2f r;
		float cs = cosf( angle );
		float sn = sinf( angle );

		r.setX( mX * cs - mY * sn );
		r.setY( mX * sn + mY * cs );

		mX = r.x();
		mY = r.y();
	}

	inline float sideOfLine( const Vector2f& a, const Vector2f& b ) {
		return sign( ( ( b.x() - a.x() ) * ( mY - a.y() ) ) - ( ( b.y() - a.y() ) * ( mX - a.x() ) ) );
	}

	inline float signOf() const {
		return mX * mY >= 0.0f ? 1.0f : -1.0f;
	}

	inline bool isPositive() const {
		return mX > 0.0f && mY > 0.0f;
	}

	std::string toString() const;
    std::string toStringObj( const std::string& _prefix) const;

    static constexpr int32_t vsize() { return 2; }

private:
	float mX;
	float mY;
};

using V2f = Vector2f;
using Vector2fList = std::vector<V2f>;
using vector2fList = Vector2fList;
using V2fVector         = std::vector<Vector2f>;
using V2fVectorOfVector = std::vector<std::vector<Vector2f>>;

typedef std::pair<V2f, float> NormalDirPair;
typedef std::tuple<V2f, V2f, V2f> Triangle2d;

namespace V2fc {
    static const Vector2f ZERO = Vector2f( 0.0f, 0.0f );
    static const Vector2f X_AXIS = Vector2f( 1.0f, 0.0f );
    static const Vector2f Y_AXIS = Vector2f( 0.0f, 1.0f );
    static const Vector2f X_AXIS_NEG = Vector2f( -1.0f, 0.0f );
    static const Vector2f Y_AXIS_NEG = Vector2f( 0.0f, -1.0f );
    static const Vector2f X_INV{-1.0f, 1.0f};
    static const Vector2f Y_INV = Vector2f( 1.0f, -1.0f );
    static const Vector2f ONE = Vector2f( 1.0f, 1.0f );
    static const Vector2f HUGE_VALUE_POS = Vector2f( std::numeric_limits<float>::max(), std::numeric_limits<float>::max() );
    static const Vector2f HUGE_VALUE_NEG = Vector2f( std::numeric_limits<float>::lowest(),
                                                        std::numeric_limits<float>::lowest() );
}

inline float dot( const Vector2f& a, const Vector2f& b ) {
	return a.x() * b.x() + a.y() * b.y();
}

inline float dotPerp( const Vector2f& a, const Vector2f& b ) {
	return a.x() * b.y() - a.y() * b.x();
}

inline float edge( const Vector2f& a, const Vector2f& b ) {
	return ( b.x() - a.x() ) * ( b.y() - a.y() );
}

inline float dotSigned( const Vector2f& a, const Vector2f& b ) {
	return atan2f( dotPerp( a, b ), dot( a, b ) );
}

inline float angle( const Vector2f& a, const Vector2f& b ) {
	return acosf( dot( a, b ) );
}

inline float lengthSquared( const Vector2f& v ) {
	return v.x() * v.x() + v.y() * v.y();
}

inline float length( const Vector2f& v ) {
	return sqrtf( lengthSquared( v ) );
}

inline float lengthOf( const Vector2f& v ) {
	return sqrtf( lengthSquared( v ) );
}

inline Vector2f rotate90( const Vector2f& v ) {
	return Vector2f( v.y(), -v.x() );
}

inline Vector2f sign( const Vector2f& v ) {
	return Vector2f( v.x() >= 0.0f ? 1.0f : -1.0f, v.y() >= 0.0f ? 1.0f : -1.0f );
}

inline Vector2f clamp( const Vector2f& v, float min, float max ) {
	return Vector2f( clamp( v.x(), min, max ), clamp( v.y(), min, max ) );
}

inline Vector2f normalize( const Vector2f& v ) {
	float lengthValue = length( v );
	return Vector2f( v.x() / lengthValue, v.y() / lengthValue );
}

inline Vector2f perpendicular( const Vector2f& v ) {
	return Vector2f( v.y(), v.x() );
}

inline Vector2f absolute( const Vector2f& v ) {
	return Vector2f( absolute( v.x() ), absolute( v.y() ) );
}

inline Vector2f min( const Vector2f& a, const Vector2f& b ) {
	return Vector2f( min( a.x(), b.x() ), min( a.y(), b.y() ) );
}

inline Vector2f max( const Vector2f& a, const Vector2f& b ) {
	return Vector2f( max( a.x(), b.x() ), max( a.y(), b.y() ) );
}

inline float minElement( const Vector2f & a ) {
	return min( a.x(), a.y() );
}

inline float maxElement( const Vector2f & a ) {
	return max( a.x(), a.y() );
}

inline float sumElements( const Vector2f & v ) {
	return v.x() + v.y();
}

inline Vector2f fma( const Vector2f& a, const Vector2f& b, const Vector2f& c ) {
	return Vector2f( a.x() * b.x() + c.x(), a.y() * b.y() + c.y() );
}

inline Vector2f reciprocal( const Vector2f& a ) {
	return{ 1.0f / a.x(), 1.0f / a.y() };
}

inline float aspect( const Vector2f& v ) {
	return v.x() / v.y();
}

inline Vector2f operator*( float scalar, const Vector2f& rhs ) {
	return Vector2f( scalar * rhs.x(), scalar * rhs.y() );
}

inline Vector2f operator/( float scalar, const Vector2f& rhs ) {
	return Vector2f( scalar / rhs.x(), scalar / rhs.y() );
}

inline Vector2f rotate( const Vector2f& rhs, float angle ) {
	Vector2f r;
	float cs = cosf( angle );
	float sn = sinf( angle );

	r.setX( rhs.x() * cs - rhs.y() * sn );
	r.setY( rhs.x() * sn + rhs.y() * cs );

	return r;
}

inline bool isScalarEqual( float a, float b, const float epsilon = 0.01f ) {
	return fabs( b - a ) < epsilon;
}

inline bool isVerySimilar( const Vector2f& a, const Vector2f& b, const float epsilon = 0.001f ) {
	return isScalarEqual( a.x(), b.x(), epsilon ) && isScalarEqual( a.y(), b.y(), epsilon );
}

bool isCollinear( const Vector2f& a, const Vector2f& b, const Vector2f& c, const float epsilon = 0.00001f );

inline bool isInsideRect( const Vector2f& position, const Vector2f& rectPos, const Vector2f& rectSize ) {
	Vector2f halfSize = rectSize * 0.5f;
	return position.x() >= rectPos.x() - halfSize.x() && position.y() >= rectPos.y() - halfSize.y() && position.x() <= rectPos.x() + halfSize.x()
		&& position.y() <= rectPos.y() + halfSize.y();
}

bool isInsideTriangle( const Vector2f& P, const Vector2f& A, const Vector2f& B, const Vector2f& C );
bool isNearTriangle( const Vector2f& P, float radius, const Vector2f& A, const Vector2f& B, const Vector2f& C );

bool intersection( Vector2f const& p0, Vector2f const& p1, Vector2f const& p2, Vector2f const& p3, Vector2f& i );
bool intersection(const V2f& p, const V2f& p2, const V2f& q, const V2f& q2, float& i );

inline bool isVectorIntersectingWithLine( std::vector<Vector2f> tvr, Vector2f& v1, Vector2f& v2 ) {
	for ( int64_t m = 0; m < static_cast<int64_t>( tvr.size() ); m++ ) {
		int64_t i1 = m == 0 ? tvr.size() - 1 : m - 1;
		Vector2f c1 = tvr[i1];
		Vector2f c2 = tvr[m];
		Vector2f inter;
		if ( intersection( v1, v2, c1, c2, inter ) ) {
			return true;
		}
	}
	return false;
}

inline float minDistanceFromTwoPoints( const Vector2f& p, const Vector2f& p1, const Vector2f& p2 ) {
	Vector2f diff = p2 - p1;
	if ( ( diff.x() == 0.0f ) && ( diff.y() == 0.0f ) ) {
		diff = p - p1;
		return lengthOf( diff );
	}

	float t = ( ( p.x() - p1.x() ) * diff.x() + ( p.y() - p1.y() ) * diff.y() ) / lengthOf( diff );

	if ( t < 0.0f ) { //point is nearest to the first point i.e x1 and y1
		diff = p - p1;
	} else if ( t > 1.0f ) { //point is nearest to the end point i.e x2 and y2
		diff = p - p2;
	} else { //if perpendicular line intersect the line segment.
		diff = p - ( p1 + diff * t );
	}

	//returning shortest distance
	return lengthOf( diff );
}

inline float sideOfLine( const Vector2f& p, const Vector2f& p1, const Vector2f& p2 ) {
	return sign( ( p2.x() - p1.x() ) * ( p.y() - p1.y() ) - ( p2.y() - p1.y() ) * ( p.x() - p1.x() ) );
}
inline int winding( const Vector2f& p, const Vector2f& p1, const Vector2f& p2 ) {
	return sideOfLine( p, p1, p2 ) > 0.0f ? 1 : 0;
}

[[nodiscard]] inline Vector2f oneMinusX( const Vector2f& p ) { return Vector2f{1.0f - p.x(), p.y()}; }
[[nodiscard]] inline Vector2f oneMinusY( const Vector2f& p ) { return Vector2f{p.x(), 1.0f - p.y()}; }

int winding( const std::vector<Vector2f>& points );

float distanceFromLine( const Vector2f& p, const Vector2f& p1, const Vector2f& p2 );
float distanceFromLine( const Vector2f& p, const Vector2f& p1, const Vector2f& p2, Vector2f& pointOfIntersection );
bool distanceFromLineClamped( const Vector2f& p, const Vector2f& p1, const Vector2f& p2, float& ret, Vector2f& pointOfIntersection );
bool distanceFromLineCapsule( const Vector2f& p, const Vector2f& p1, const Vector2f& p2, float radius, float& ret, Vector2f& pointOfIntersection );
bool isPointInsideLineCapsule( const Vector2f& p, const Vector2f& p1, const Vector2f& p2, float radius );
void straightenOnMainAxis( Vector2f& a, Vector2f& b, float straightCoeff );
void removeCollinear( vector2fList& cs, float epsilon, CollinearWrap cw = CollinearWrap::True );
bool pointWithinSegment( const V2f& p, const V2f& p1, const V2f& p2 );
float getAreaOf( const std::vector<Triangle2d>& tris );

using V2fVectorWrap = VectorWrap<V2f>;
using V2fVectorOfVectorWrap = VectorOfVectorWrap<V2f>;
