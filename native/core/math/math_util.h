#pragma once

#include <cmath>
#include <memory>
#include <variant>
#include <vector>
#include <cstdint>
#include <string>
#include <unordered_set>
#include <core/util_logs.hpp>
#include <core/util_array.hpp>

// SHARED CLASS: Any changes to this, should be integrated to /svn/shared, and a mail should be sent to everyone

/*
	FILE SYNOPSIS

	// MIN/MAX
	float min(float a, float b);
	float max(float a, float b);
	T min(const T& a, const T& b);
	T max(const T& a, const T& b);
	T min3(T a, T b, T c);
	T max3(T a, T b, T c);
	T min4(T a, T b, T c, T d);
	T max4(T a, T b, T c, T d);

	// GENERAL
	int absolute(int x);
	float absolute(float x);
	float sqrt(float x);
	float square(float x);
	float sign(float x);
	float fraction(float x);
	double fraction(double x);
	bool isValid(float x);
	bool isValid(double x);
    bool isEven(T n);
    bool isOdd(T n);

	// RANGE CONVERSION AND CLAMPING
	T clamp(T value, T min, T max);
	float saturate(float x);
	float saturateSigned(float x);
	T normalisedSignedToUnsigned(T x)			converts range [-1,+1] to [0,1]
	T normalisedUnsignedToSigned(T x)			converts range [0,1] to [-1,+1]

	// ROUNDING / DIVISION / MODULO
	float roundNearest(float x);
	float roundDown(float x);
	float roundUp(float x);
	float roundZero(float x);
	int roundNearestInt(float x);
	int roundDownInt(float x);
	int roundUpInt(float x);
	int roundZeroInt(float x);
	double roundDown(double x);
	int alignDown(int value, int alignment);
	int alignUp(int value, int alignment);
	int divideRoundDown(int value, int divisor);
	int divideRoundUp(int value, int divisor);
	int modulo(int value, int denominator);
	int divide(int value, int denominator);
	float quantise(float value, float grid);

	// INTERPOLATION
	V interpolate(V a, V b, T t);
	V interpolateInverse(V x0, V x1, V x);
	T mix(T a, T b, float t);
	T average(T a, T b);
	double interpolateCosine(float a, float b, float t);

	// FUSED MULTIPLY-ADD
	T fma(T a, T b, T c);
	float fma(float a, float b, float c);

	// MISCELLANEOUS
	void sortFloats(float & a, float & b);
	int isPowerOfTwo(int x);
	float degToRad(float deg);
	float radToDeg(float rad);

	// LENGTH
	float length(float x);
	float length(float x, float y);
	float length(float x, float y, float z);
	float length(float x, float y, float z, float w);
	float lengthSquared(float x, float y);
	float lengthSquared(float x, float y, float z);
	float lengthSquared(float x, float y, float z, float w);
	float distance(const T & pointA, const T & pointB);
	bool distanceWithin(const T & pointA, const T & pointB, float distanceThreshold);

	// NOISE
	double perlin1D(int x);
	double smoothPerlin1D(float x);
	double interpolatedPerlin1D(float x);
	float perlinNoise1D(float x);

	// PREPROCESSOR MACROS
	TWO_PI
	PI
	HALF_PI
*/

#include <cmath>
#include <cstdlib>
#include <algorithm>

enum class CollinearWrap {
    False = 0,
    True = 1
};

[[maybe_unused]] const float ONE_OVER_60HZ = 0.016666667f;

#undef M_PI
#ifndef M_PI
const float M_PI = 3.14159265358979323846f;
template<typename T>
constexpr T PI = T( 3.14159265358979323846f );
#endif
#undef M_PI_2
#ifndef M_PI_2
constexpr float M_PI_2 = 1.57079632679489661923f;
#endif
#undef M_PI_4
#ifndef M_PI_4
constexpr float M_PI_4 = M_PI_2 / 2.0f;
#endif
#ifndef M_SQRT2
const float M_SQRT2 = 1.41421356237309504880f;
#endif
#ifndef TWO_PI
const float TWO_PI = 6.28318530718f;
#endif
#ifndef TWO_PI_NEG
const float TWO_PI_NEG = -6.28318530718f;
#endif
#ifndef HALF_PI
const float HALF_PI = 1.57079632679f;
#endif
const float RAD = 180.0f / static_cast<float>( M_PI );
#ifndef M_E
const float M_E = 2.71828182845904523536f;
#endif

const float M_100_PERC = 1.0f;
const float SMALL_EPSILON = 0.01f;
const float VERY_SMALL_EPSILON = 0.0001f;

namespace JMATH {
    template<typename T>
    T min( const T& a, const T& b ) {
        return std::min( a, b );
    }

    inline float min( float a, float b ) {
#ifdef D_LANGUAGE_CXX11
        return std::fmin( a, b );
#else
        return std::min( a, b );
#endif
    }

    template<typename T>
    T min3( T a, T b, T c ) {
        return min( min( a, b ), c );
    }

    template<typename T>
    T min4( T a, T b, T c, T d ) {
        return min( min( a, b ), min( c, d ));
    }

    template<typename T>
    T max( const T& a, const T& b ) {
        return std::max( a, b );
    }

    inline float max( float a, float b ) {
#ifdef D_LANGUAGE_CXX11
        return std::fmax( a, b );
#else
        return std::max( a, b );
#endif
    }

    template<typename T>
    T max3( T a, T b, T c ) {
        return max( max( a, b ), c );
    }

    template<typename T>
    T max4( T a, T b, T c, T d ) {
        return max( max( a, b ), max( c, d ));
    }

    template<typename T>
    T clamp( T value, T min_, T max_ ) {
        return max( min( value, max_ ), min_ );
    }

    template<typename T>
    T clamp_neg( T value, T max_, T min_ ) {
        return max( min( value, max_ ), min_ );
    }

    template<typename T>
    T fma( T a, T b, T c ) {
        return a * b + c;
    }

    template <typename T>
    inline T half( const T& v ) {
        return v*0.5f;
    }

    template <typename T>
    inline T quarter( const T& v ) {
        return v*0.25f;
    }

//inline float fma(float a, float b, float c) {
//#ifdef D_LANGUAGE_CXX11
//	return std::fma(a, b, c);
//#else
//	return a * b + c;
//#endif
//}

    inline int absolute( int x ) {
        return abs( x );
    }

    inline float absolute( float x ) {
        // Note: we name this absolute() to avoid name collision with std::abs(int). If we used abs() then unqualified code calling abs()
        // (not jfdp::abs()) and passing a float will accidentally call std::abs(int) with implicit float->int conversion (possibly silent).
        return std::fabs( x );
    }

// For consistency, alias all std::sqrt() overloads into this namespace so they are available alongside our other functions,
// as either jfdp::sqrt() or ::sqrt(), depending on whether the jfdp namespace is being used (/END).
    using std::sort;

    inline float square( float x ) {
        // This 'square' helper function may look trivial but can greatly simplify some expressions.
        return x * x;
    }

    inline float roundNearest( float x ) {
#ifdef D_LANGUAGE_CXX11
        // Note: std::round() is available in C++11 but not earlier versions.
        return std::round( x );
#else
        return floorf( x + 0.5f );
#endif
    }

    inline float roundDown( float x ) {
        return floorf( x );
    }

    inline double roundDown( double x ) {
        return floor( x );
    }

    inline float roundUp( float x ) {
        return ceilf( x );
    }

    inline float roundZero( float x ) {
#ifdef D_LANGUAGE_CXX11
        // Note: std::trunc() is available in C++11 but not earlier versions.
        return std::trunc( x );
#else
        return static_cast<float>( static_cast<int>( x ));
#endif
    }

    inline int roundNearestInt( float x ) {
        return static_cast<int>( roundNearest( x ));
    }

    inline int roundDownInt( float x ) {
        return static_cast<int>( roundDown( x ));
    }

    inline int roundUpInt( float x ) {
        return static_cast<int>( roundUp( x ));
    }

    inline int roundZeroInt( float x ) {
        // Note: trunc/roundZero is not needed; int conversion is defined to be a trunc.
        return static_cast<int>( x );
    }

    inline int alignDown( int value, int alignment ) {
        //	ASSERT(value >= 0);
        //	ASSERT(alignment > 0);
        return value - ( value % alignment );
    }

    inline int alignUp( int value, int alignment ) {
        //	ASSERT(value >= 0);
        //	ASSERT(alignment > 0);
        value += alignment - 1;
        return value - ( value % alignment );
    }

    inline int divideRoundDown( int value, int divisor ) {
        //	ASSERT(value >= 0);
        //	ASSERT(alignment > 0);
        return value / divisor;
    }

    inline int divideRoundUp( int value, int divisor ) {
        //	ASSERT(value >= 0);
        //	ASSERT(alignment > 0);
        return ( value + divisor - 1 ) / divisor;
    }

// Returns a signed integer modulo according to Euclidean division.
// The 'value' parameter may have any value, but 'denominator' must be positive.
// The result is calculated such that the pattern for negative numbers extends the periodic pattern of modulo on positive numbers.
// This differs from the C++ modulo operator (%), where the behaviour of negative values is undefined (until C++11)
// or does truncation towards zero (C++11 or later).
    int modulo( int value, int denominator );

// Returns a signed integer division according to Euclidean division.
// The 'value' parameter may have any value, but 'denominator' must be positive.
// The result is calculated such that the pattern for negative numbers extends the periodic pattern of division on positive numbers.
// This differs from the C++ division operator (/), where behaviour of negative values is undefined (until C++11)
// or does truncation towards zero (C++11 or later).
    int divide( int value, int denominator );

    std::string floatToFixedDigits( const float number );
    std::string floatToDistance( const float number );

    template<typename V>
    inline bool isbetween( V x, V a, V b ) {
        return !( x > a && x > b ) && !( x < a && x < b );
    }

    template<typename V>
    inline bool isbetweenEqual( V x, V a, V b ) {
        return !( x >= a && x >= b ) && !( x <= a && x <= b );
    }

    template<typename V>
    inline bool isbetween( const V& x, const std::pair<V, V>& range ) {
        return ( x > range.first && x < range.second );
    }

    template<typename V>
    inline bool isOverlapping( const std::pair<V, V>& range1, const std::pair<V, V>& range2 ) {
        if ( isbetween( range1.first, range2 )) return true;
        if ( isbetween( range1.second, range2 )) return true;
        if ( isbetween( range2.first, range1 )) return true;
        if ( isbetween( range2.second, range1 )) return true;
        return false;
    }

    template<typename V, typename T>
    inline V interpolate( V a, V b, T t ) {
        return ( b - a ) * t + a;
    }

// Returns the interpolation time t corresponding to value x over the range [x0,x1].
// If interpolate(x0,x1,t) returns x, then interpolateInverse(x0,x1,x) returns t.
// Also useful with more general types. For example, in 2D, if interpolate(topLeft,bottomRight,input)
// returns 'output', then interpolateInverse(topLeft,bottomRight,output) returns 'input'.
    template<typename V>
    inline V interpolateInverse( V x0, V x1, V x ) {
        return ( x - x0 ) / ( x1 - x0 );
    }

    template<typename T>
    T normalisedSignedToUnsigned( T v ) {
        return v * T( 0.5 ) + T( 0.5 );
    }

    template<typename T>
    T normalisedUnsignedToSigned( T v ) {
        return v * T( 2.0 ) - T( 1.0 );
    }

    template<typename T>
    inline T average( T a, T b ) {
        return ( a + b ) / T( 2 );
    }

    inline float average( float a, float b ) {
        return ( a + b ) * 0.5f;
    }

    inline double average( double a, double b ) {
        return ( a + b ) * 0.5;
    }

    inline void sortFloats( float& a, float& b ) {
        if ( a > b ) {
            std::swap( a, b );
        }
    }

    inline int isPowerOfTwo( int x ) {
        return (( x != 0 ) && !( x & ( x - 1 )));
    }

    template<typename T>
    inline T degToRad( T deg ) {
        return deg * ( M_PI / 180.0f );
    }

    float degToRad();

    template<typename T>
    inline T degToRadNorm( T deg ) {
        if ( deg < 0.0f ) {
            deg = 360.0f - fmod( fabs( deg ), 360.0f );
        }
        return deg * ( M_PI / 180.0f );
    }

    inline float radToDeg( float rad ) {
        return rad * ( 180.0f / M_PI );
    }

    inline float quantise( float value, float grid ) {
        value /= grid;
        value = roundNearest( value );
        value *= grid;
        return value;
    }

// For compatibility, provide lerp(t,a,b) used by Paper Camera.
// This is getting a bit silly. We now have:
//	interpolate(a,b,t);
//	mix(a,b,t)
//	lerp(t,a,b

    template<typename T>
    inline T lerp( float t, T a, T b ) {
        return static_cast<T>(( b - a ) * t + a );
    }

    template<typename T, typename S>
    inline T mix( T a, T b, S t ) {
        return a * ( S( 1 ) - t ) + b * t;
    }

    template<typename T>
    inline T lerpInv( T r, T a, T b ) {
        return ( r - a ) / ( b - a );
    }

    template <typename T>
    T remap( T x, T a1, T a2, T b1, T b2 ) {
        return lerp( lerpInv(x, a1, a2), b1, b2 );
    }

    inline float unitRand( float scale = 1.0f ) {
        // Regarding precision: the result will have 24-bit precision (i.e. full float32_t precision) on iOS and Android (where RAND_MAX is 2^31-1).
        // On platforms where RAND_MAX is 2^15-1 (e.g. Windows desktop and mobile) the result will only have 15-bit precision.
        // This is fine for most uses, but if it isn't sufficient then a better random number generator should be used (e.g. see class Random in "random/random.h").

        // We divide by RAND_MAX+1, which is a power of two and therefore precisely representable as a float.
        // This also puts the fraction into the exclusive range [0,1), which is the usual convention for random numbers in 0..1, although due to rounding it may equal 1.0f anyway.
        const int64_t randMaxExclusive = int64_t( RAND_MAX ) + 1;
        float randomFraction = float( rand()) / float( randMaxExclusive );

        return randomFraction * scale;
    }

    inline int unitRandI( int scale ) {
        return static_cast<int>(unitRand( scale ));
    }

    inline float signedUnitRand( float scale = 1.0f ) {
        return normalisedUnsignedToSigned( unitRand()) * scale;
    }

    inline float sign( float x ) {
        if ( x != 0.0f )
            return ( x > 0.0f ) ? +1.0f : -1.0f;
        else
            return 0.0f;
    }

    inline float signPositiveIfZero( float x ) {
        if ( x != 0.0f )
            return ( x > 0.0f ) ? +1.0f : -1.0f;
        else
            return 1.0f;
    }

    inline float lengthSquared( float x, float y ) {
        return x * x + y * y;
    }

    inline float lengthSquared( float x, float y, float z ) {
        return x * x + y * y + z * z;
    }

    inline float lengthSquared( float x, float y, float z, float w ) {
        return x * x + y * y + z * z + w * w;
    }

    inline float length( float x ) {
        // Note: length of a scalar is included for completion. It is equal to the absolute value.
        // Including this allows algorithms to be more general; for example, greyscale image differencing
        // might use length(float) whereas colour image differencing might use length(Vector3f),
        // selected based on a template parameter.
        return absolute( x );
    }

    inline float length( float x, float y ) {
        return sqrt( lengthSquared( x, y ));
    }

    inline float length( float x, float y, float z ) {
        return sqrt( lengthSquared( x, y, z ));
    }

    inline float length( float x, float y, float z, float w ) {
        return sqrt( lengthSquared( x, y, z, w ));
    }

    template<typename T>
    float distance( const T& pointA, const T& pointB ) {
        return length( pointB - pointA );
    }

    template<typename T>
    bool distanceWithin( const T& pointA, const T& pointB, float distanceThreshold ) {
        return lengthSquared( pointB - pointA ) <= square( distanceThreshold );
    }

    template<typename T>
    inline T squareDiff( const T& a, const T& b ) {
        return ( a - b ) * ( a - b );
    }

    inline float fractionf( float x ) {
        return x - roundDown( x );
    }

    inline double fractionf( double x ) {
        return x - roundDown( x );
    }

    inline float saturate( float x ) {
        return clamp( x, 0.0f, 1.0f );
    }

    inline float saturateSigned( float x ) {
        return clamp( x, -1.0f, 1.0f );
    }

    inline bool isValid( float x ) {
        return !std::isinf( x ) && !std::isnan( x );
    }

    inline bool isValid( double x ) {
        return !std::isinf( x ) && !std::isnan( x );
    }

    template<typename T>
    inline bool isEven( T x ) {
        return x % 2 == 0;
    }

    template<typename T>
    inline bool isOdd( T x ) {
        return !isEven(x);
    }

    template<typename T>
    inline float calculatePerimeterOf( const std::vector<std::vector<T>>& cs, bool bWrap = true ) {
        float lPerimeter = 0.0f;
        for ( auto& rws : cs ) {
            int csize = static_cast<int>( rws.size());
            if ( csize > 0 ) {
                for ( int q = 0; q < csize - 1; q++ ) {
                    lPerimeter += distance( rws[q], rws[q + 1] );
                }
                if ( bWrap ) {
                    lPerimeter += distance( rws[0], rws[csize - 1] );
                }
            }
        }
        return lPerimeter;
    }

    inline float mirrorAngleRoundPosNegM_PI_2( float angle ) {
        //if (angle >= M_PI_2) {
        //	angle -= M_PI;
        //}
        //else if (angle <= -M_PI_2) {
        //	angle += M_PI;
        //}

        if ( !( angle < M_PI_2 + VERY_SMALL_EPSILON && angle > -M_PI_2 - VERY_SMALL_EPSILON )) {
            angle += static_cast<float>( M_PI );
        }
        return angle;
    }

    inline float shouldMirrorAngleRoundPosNegM_PI_2( float angle ) {
        if ( !( angle < M_PI_2 + VERY_SMALL_EPSILON && angle > -M_PI_2 - VERY_SMALL_EPSILON )) {
            return true;
        }
        return false;
    }

    template<typename Value>
    Value wrapAround( const Value& t, const Value& minv, const Value& maxv ) {
        if ( t < minv ) {
            Value r = maxv - ( fmodf( t - minv, maxv - minv ));
            return r;
        }
        if ( t > maxv ) {
            Value r = minv + ( fmodf( t - maxv, maxv - minv ));
            return r;
        }
        return t;
    }

    template<typename Value>
    Value interpolateCosine( const Value& a, const Value& b, float t ) {
        float m = ( 1.0f - cos( t * M_PI )) * 0.5f;
        return interpolate( a, b, m );
    }

//P(t) = P0*(1-t)^2 + P1*2*(1-t)*t + P2*t^2
    template<typename Value>
    Value interpolateQuadraticBezier( const Value& y0, const Value& y1,
                                      const Value& y2,
                                      float mu ) {
        float mu2 = mu * mu;
        float onem = ( 1.0f - mu );
        float a0 = onem * onem;
        float a1 = 2.0f * onem * mu;
        float a2 = mu2;

        return ( y0 * a0 + y1 * a1 + y2 * a2 );
    }

    template<typename Value>
    Value interpolateBezier( const Value& a, const Value& b, const Value& c, const Value& d, const float t ) {
        Value dest;

        Value ab, bc, cd, abbc, bccd;
        ab = mix( a, b, t );         // Value between a and b (green)
        bc = mix( b, c, t );         // Value between b and c (green)
        cd = mix( c, d, t );         // Value between c and d (green)
        abbc = mix( ab, bc, t );       // Value between ab and bc (blue)
        bccd = mix( bc, cd, t );       // Value between bc and cd (blue)
        dest = mix( abbc, bccd, t );   // Value on the bezier-curve (black)

        return dest;
    }

    template<typename Value>
    Value interpolateCubic( const Value& y0, const Value& y1,
                            const Value& y2, const Value& y3,
                            float mu ) {
        float mu2 = mu * mu;
        Value a0 = y3 - y2 - y0 + y1;
        Value a1 = y0 - y1 - a0;
        Value a2 = y2 - y0;
        Value a3 = y1;

        return ( a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3 );
    }

    template<typename Value>
    Value interpolateCatmullRom( const Value& y0, const Value& y1,
                                 const Value& y2, const Value& y3,
                                 float mu ) {
        float mu2 = mu * mu;
        Value a0 = y0 * -0.5f + y1 * 1.5f - y2 * 1.5f + y3 * 0.5f;
        Value a1 = y0 - y1 * 2.5f + y2 * 2.0f - y3 * 0.5f;
        Value a2 = y0 * ( -0.5f ) + y2 * 0.5f;
        Value a3 = y1;

        return ( a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3 );
    }

/*
Tension: 1 is high, 0 normal, -1 is low
Bias: 0 is even,
positive is towards first segment,
negative towards the other
*/
    template<typename Value>
    Value interpolateHermite( const Value& y0, const Value& y1,
                              const Value& y2, const Value& y3,
                              float mu,
                              float tension = 0.0f,
                              float bias = 0.0f ) {
        float mu2 = mu * mu;
        float mu3 = mu2 * mu;

        Value m0 = ( y1 - y0 ) * ( 1.0f + bias ) * ( 1.0f - tension ) * 0.5f;
        m0 = m0 + ( y2 - y1 ) * ( 1.0f - bias ) * ( 1.0f - tension ) * 0.5f;
        Value m1 = ( y2 - y1 ) * ( 1.0f + bias ) * ( 1.0f - tension ) * 0.5f;
        m1 = m1 + ( y3 - y2 ) * ( 1.0f - bias ) * ( 1.0f - tension ) * 0.5f;

        float a0 = 2.0f * mu3 - 3.0f * mu2 + 1.0f;
        float a1 = mu3 - 2.0f * mu2 + mu;
        float a2 = mu3 - mu2;
        float a3 = -2.0f * mu3 + 3.0f * mu2;

        return ( y1 * a0 + m0 * a1 + m1 * a2 + y2 * a3 );
    }

// Perlin noise functions
    double perlin1D( int x );
    double smoothPerlin1D( float x );
    double interpolatedPerlin1D( float x );
    float perlinNoise1D( float x, int octaves );

// SnapTo functions
    template<typename T>
    T snapTo( const T& value, const std::vector<T>& snapPoints, float distThreshold ) {
        T mindist = T( std::numeric_limits<float>::max());
        T dist;
        T ret = value;
        for ( auto& p : snapPoints ) {
            for ( int s = 0; s < static_cast<int>( value.size()); s++ ) {
                dist[s] = ( JMATH::distance( value[s], p[s] ));
                if ( dist[s] < distThreshold && dist[s] < mindist[s] ) {
                    mindist[s] = dist[s];
                    ret[s] = p[s];
                }
            }
        }

        if ( !snapPoints.empty() && ret == value && value != snapPoints.back()) {
            T lastLine = value - snapPoints.back();
            constexpr float slopeMinAngle = 0.06f;
            float slope = dot( T::X_AXIS(), normalize( lastLine ));
            if constexpr ( T::vSize() == 3 ) {
                if ( fabs( acos( slope )) < slopeMinAngle ) {
                    ret.setZ( snapPoints.back().z());
                }
                slope = dot( T::Z_AXIS(), normalize( lastLine ));
                if ( fabs( acos( slope )) < slopeMinAngle ) {
                    ret.setX( snapPoints.back().x());
                }
            }
            if constexpr ( T::vSize() == 2 ) {
                if ( fabs( acos( slope )) < slopeMinAngle ) {
                    ret.setY( snapPoints.back().y());
                }
                slope = dot( T::Y_AXIS(), normalize( lastLine ));
                if ( fabs( acos( slope )) < slopeMinAngle ) {
                    ret.setX( snapPoints.back().x());
                }
            }
        }

        return ret;
    }

    template<typename T>
    void alignLineOnMainAxis( const T& p1, T& p2 ) {
        T diff = p2 - p1;
        int dx = diff.leastDominantElement();
        p2[dx] = p1[dx];
    }

    // Use a median when you do not want to sort your input vector
    // Warning this has performance implications as it will require an extra copy.
    template<typename T>
    T median( const std::vector<T>& source ) {
        if ( source.empty() ) {
            ASSERTV( 0, "Requesting median on an empty array");
            return T{};
        }

        auto tempV = source;
        std::sort( tempV.begin(), tempV.end());

        size_t size = tempV.size();
        return ( size % 2 == 0 ) ? lerp<T>( 0.5f, tempV[size / 2 - 1], tempV[size / 2]) : tempV[size / 2];
    }

    // Use a medianSort when you do not want to sort your input vector and obtain the median value.
    template<typename T>
    T medianSort( std::vector<T>& source ) {
        if ( source.empty() ) {
            ASSERTV( 0, "Requesting median on an empty array");
            return T{};
        }

        std::sort( source.begin(), source.end());

        size_t size = source.size();
        return ( size % 2 == 0 ) ? lerp<T>( 0.5f, source[size / 2 - 1], source[size / 2]) : source[size / 2];
    }

    // Use a medianSort when you do not want to sort your input vector and obtain the median value.
    template<typename T>
    std::pair<T, T> medianSortSpread( std::vector<T>& source, unsigned int spread ) {
        if ( source.empty() ) {
            ASSERTV( 0, "Requesting median on an empty array");
            return std::make_pair(T{}, T{});
        }
        if ( source.size() <= spread*2+1 ) {
            ASSERT("Spread of median will be limited to array size");
            spread = 0;
        }

        std::sort( source.begin(), source.end());

        size_t size = source.size();
        return ( size % 2 == 0 ) ? std::make_pair( source[size / 2 - 1 - spread], source[size / 2 + spread]) :
                                   std::make_pair( source[size / 2 - spread], source[size / 2 + spread]);
    }

}

template<typename T>
std::vector<T> sanitizePath( const std::vector<T>& _verts, bool wrapPath = true, float epsilon = 0.001f ) {
    std::vector<T> ret;
    int csize = static_cast<int>( _verts.size());

    if ( csize == 0 ) {
        return ret;
    }
    if ( csize == 1 ) {
        ret.push_back( _verts[0] );
        return ret;
    }
    if ( csize == 2 ) {
        ret.push_back( _verts[0] );
        if ( !isVerySimilar( _verts[0], _verts[1], epsilon )) {
            ret.push_back( _verts[1] );
        }
        return ret;
    }

    std::unordered_set<int> doNotCopyList{};
    int wrappingOff = wrapPath ? 0 : 1;
    for ( int q = wrappingOff; q < csize - wrappingOff; q++ ) {
        int qm1 = cai( q - 1, csize );
        int qp1 = cai( q + 1, csize );
        T p1 = _verts[qm1];
        T p2 = _verts[q];
        T p3 = _verts[qp1];
        if ( isCollinear( p1, p2, p3, epsilon ) &&
             !( wrapPath && q == csize - 1 && doNotCopyList.find( 0 ) != doNotCopyList.end())) {
            doNotCopyList.insert( q );
        }
    }

    for ( size_t t = 0; t < _verts.size(); t++ ) {
        if ( doNotCopyList.find( t ) == doNotCopyList.end()) ret.push_back( _verts[t] );
    }

    return ret;
}

template <typename T>
struct VectorWrap {
    VectorWrap() = default;
    VectorWrap( const std::vector<T>& v, bool wrap ) : v( v ), wrap( wrap ) {}

    std::vector<T> v;
    bool wrap = false;
};

template <typename T, typename M>
struct VectorWrapT2 {
    VectorWrapT2() = default;
    VectorWrapT2( const std::vector<T>& _v, bool wrap ) : v(  _v ), wrap( wrap ) {}
    VectorWrapT2( const std::vector<T>& _v, const std::vector<T>& _m, bool wrap ) : v(  _v ), vm( _m ), wrap( wrap ) {}

    std::vector<T> v;
    std::vector<M> vm;
    bool wrap = false;
};

template <typename T>
using VectorOfVectorWrap = std::vector<VectorWrap<T>>;

template <typename T, typename M>
using VectorOfVectorWrapT2 = std::vector<VectorWrapT2<T, M>>;

class Vector3f;
namespace JMATH { class Rect2f; }

std::vector<Vector3f> roundedCornerFanFromRect( const JMATH::Rect2f& rect, float cornerAngle );