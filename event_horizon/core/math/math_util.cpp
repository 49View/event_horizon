#include "math_util.h"

#include <sstream>
#include <iostream>
#include <iomanip>
#include "matrix2f.h"
#include "rect2f.h"

namespace JMATH {

	std::string floatToFixedDigits( const float number ) {
	std::stringstream stream;
	if ( fabs( number ) >= 10000.0f ) {
		stream << std::fixed << std::setprecision( 0 ) << number;
	} else if ( fabs( number ) >= 1000.0f ) {
		stream << std::fixed << std::setprecision( 1 ) << number;
	} else if ( fabs( number ) >= 100.0f ) {
		stream << std::fixed << std::setprecision( 2 ) << number;
	} else if ( fabs( number ) >= 10.0f ) {
		stream << std::fixed << std::setprecision( 3 ) << number;
	} else {
		stream << std::fixed << std::setprecision( 4 ) << number;
	}

	return stream.str();
}

std::string floatToDistance( float number ) {
	std::stringstream stream;
	if ( number < 1.0f ) {
	    number *= 100.0f;
        stream << std::fixed << std::setprecision( 0 ) << number << "cm";
	} else if ( number < 100.0f ) {
        stream << std::fixed << std::setprecision( 2 ) << number << "m";
	} else if ( number < 1000.0f ) {
        stream << std::fixed << std::setprecision( 1 ) << number << "m";
    } else {
        number /= 1000.0f;
        stream << std::fixed << std::setprecision( 2 ) << number << "km";
	}
	return stream.str();
}

int modulo( int value, int denominator ) {
	/*
		Returns a signed integer modulo according to Euclidean division.
		The result is calculated such that the pattern for negative numbers extends the periodic pattern of modulo on positive numbers.

		For example, given this array of integers from -10 to +9:
		{-10,-9,-8,-7,-6,   -5,-4,-3,-2,-1,
		0, 1, 2, 3, 4,    5, 6, 7, 8, 9 }

		If we take modulo(x,5) to each value we get:
		{  0, 1, 2, 3, 4,    0, 1, 2, 3, 4,
		0, 1, 2, 3, 4,    0, 1, 2, 3, 4 }

		This differs from the C++ modulo operator (%), where the behaviour of negative values is undefined (until C++11)
		or does truncation towards zero (C++11 or later).

		In the above example, the output of built-in modulo operator (x % 5) would be:
		{  ?, ?, ?, ?, ?,    ?, ?, ?, ?, ?,
		0, 1, 2, 3, 4,    0, 1, 2, 3, 4 }
		*/

	if ( value < 0 ) {
		value = -( value + 1 );
		value %= denominator;
		return ( denominator - 1 ) - value;
	} else {
		return value % denominator;
	}
}

int divide( int value, int denominator ) {
	/*
		Returns a signed integer division according to Euclidean division.
		The result is calculated such that the pattern for negative numbers extends the periodic pattern of division on positive numbers.

		For example, given this array of integers from -10 to +9:
		{-10,-9,-8,-7,-6,   -5,-4,-3,-2,-1,
		0, 1, 2, 3, 4,    5, 6, 7, 8, 9 }

		If we take divide(x,5) to each value we get:
		{ -2,-2,-2,-2,-2,   -1,-1,-1,-1,-1,
		0, 0, 0, 0, 0,    1, 1, 1, 1, 1 }

		This differs from the C++ division operator (/), where behaviour of negative values is undefined (until C++11)
		or does truncation towards zero (C++11 or later).

		In the above example, the output of the built-in division operator (x / 5) would be:
		{  ?, ?, ?, ?, ?,    ?, ?, ?, ?, ?,
		0, 0, 0, 0, 0,    1, 1, 1, 1, 1 }
		*/

	value -= modulo( value, denominator );
	return value / denominator;
}

double perlin1D( int x ) {
	x = ( x << 13 ) ^ x;
	return (double)( 1.0 - ( ( x * ( x * x * 15731 + 789221 ) + 1376312589 ) & 0x7fffffff ) / 1073741824.0 );
}

double smoothPerlin1D( int x ) {
	return perlin1D( x ) / 2.0f + perlin1D( x - 1 ) / 4.0f + perlin1D( x + 1 ) / 4.0f;
}

double interpolatedPerlin1D( float x ) {
	int integer_X = roundDownInt( x );
	float fractional_X = x - integer_X;

	double v1 = smoothPerlin1D( integer_X );
	double v2 = smoothPerlin1D( integer_X + 1 );

	return interpolateCosine( v1, v2, fractional_X );
}

float perlinNoise1D( float x, int octaves ) {
	float total = 0.0f;
	float p = 0.25f;

	for ( int i = 0; i < octaves; i++ ) {
		float frequency = powf( 2.0, static_cast<float>( i ) );
		float amplitude = powf( p, static_cast<float>( i ) );

		total = total + static_cast<float>( interpolatedPerlin1D( x * frequency ) * amplitude );
	}

	return total;
}

float degToRad() {
	return ( M_PI / 180.0f );
}

}

void innerRoundCornerInter( std::vector<Vector3f>& ret, const V2f& tl1,  const V2f& tlm,  const V2f& tl2 ) {
    ret.emplace_back( tl1 );
    for ( size_t t = 1; t < 5; t++ ) {
        float delta = (float)t / 5.0f;
        ret.emplace_back( interpolateQuadraticBezier( tl1, tlm, tl2, delta ) );
    }
    ret.emplace_back( tl2 );
}

std::vector<Vector3f> roundedCornerFanFromRect( const JMATH::Rect2f& rect, float cornerAngle ) {
    std::vector<Vector3f> ret{};

    float cornerRatio = cornerAngle;

    V2f tl1 = rect.topLeft() + (V2f::Y_AXIS * cornerRatio);
    V2f tl2 = rect.topLeft() + (V2f::X_AXIS * cornerRatio);

    V2f tr1 = rect.topRight() + (V2f::X_AXIS_NEG * cornerRatio);
    V2f tr2 = rect.topRight() + (V2f::Y_AXIS * cornerRatio);

    V2f br1 = rect.bottomRight() + (V2f::Y_AXIS_NEG * cornerRatio);
    V2f br2 = rect.bottomRight() + (V2f::X_AXIS_NEG * cornerRatio);

    V2f bl1 = rect.bottomLeft() + (V2f::X_AXIS * cornerRatio);
    V2f bl2 = rect.bottomLeft() + (V2f::Y_AXIS_NEG *cornerRatio);

    innerRoundCornerInter( ret, tl1, rect.topLeft(),     tl2 );
    innerRoundCornerInter( ret, tr1, rect.topRight(),    tr2 );
    innerRoundCornerInter( ret, br1, rect.bottomRight(), br2 );
    innerRoundCornerInter( ret, bl1, rect.bottomLeft(),  bl2 );

    return ret;
}
