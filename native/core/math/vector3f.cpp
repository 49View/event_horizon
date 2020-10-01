#include "vector3f.h"

#include <map>
#include <sstream>
#include "math_util.h"
#include "matrix4f.h"

std::string Vector3f::toString() const {
	return "X = " + floatToFixedDigits( mX ) + " Y = " + floatToFixedDigits( mY ) + " Z = " + floatToFixedDigits( mZ );
}

std::string Vector3f::toStringValuesOnly() const {
	return floatToFixedDigits( mX ) + " " + floatToFixedDigits( mY ) + " " + floatToFixedDigits( mZ );
}

std::string Vector3f::toStringJSONArray() const {
	return "[" + floatToFixedDigits( mX ) + "," + floatToFixedDigits( mY ) + "," + floatToFixedDigits( mZ ) + "]";
}

std::string Vector3f::toStringObj( const std::string& _prefix ) const {
    std::ostringstream ss;
    ss << _prefix << " " << mX << " " << mY << " " << mZ << std::endl;
    return ss.str();
}

float fmodAntiGimbal( float _a, float _value ) {
	float c = fmodf( _a, _value );
	return c == 0.0f && _a > 0.0f ? _value : c;
}

void Vector3f::fmod( float _value ) {
    mX = fmodAntiGimbal( mX, _value );
    mY = fmodAntiGimbal( mY, _value );
    mZ = fmodAntiGimbal( mZ, _value );
}

Vector3f Vector3f::fmod( float _value ) const {
	return {fmodAntiGimbal( mX, _value ), fmodAntiGimbal( mY, _value ), fmodAntiGimbal( mZ, _value )};
}

std::pair<int32_t, int32_t> Vector3f::leastDominantPair() const {
    if ( fabs( x() ) >= fabs( y() ) && fabs( x() ) >= fabs( z() ) ) {
        return std::make_pair<int32_t, int32_t>( 1, 2 );
    }
    if ( fabs( y() ) >= fabs( x() ) && fabs( y() ) >= fabs( z() ) ) {
        return std::make_pair<int32_t, int32_t>( 0, 2 );
    }
    return std::make_pair<int32_t, int32_t>( 0, 1 );
}

bool Vector3f::isValid() const {
    return ::isValid(mX) && ::isValid(mY) && ::isValid(mZ);
}

void removeCollinear( std::vector<Vector3f>& cs ) {
	int csize = static_cast<int>( cs.size() );
	if ( csize < 3 ) return;

	std::vector<Vector3f> cc = cs;

	for ( auto t = 0; t < csize; t++ ) {
		auto currPoint1 = cs[t];
		int t1 = getCircularArrayIndex( t + 1, csize );
		auto currPoint2 = cs[t1];
		auto currPoint3 = cs[getCircularArrayIndex( t + 2, csize )];
		auto cp = crossProduct( currPoint1, currPoint2, currPoint3 );
		if ( isValid(cp.x())) cp = normalize( cp );
		if ( !isValid( cp.x()) ) {
			// check if degenerate
			if ( currPoint1 == currPoint3 ) {
				cc[t] = V3fc::HUGE_VALUE_POS;
			} else {
				cc[t1] = V3fc::HUGE_VALUE_POS;
			}
		}
	}

	for ( auto t = 0; t < csize; t++ ) {
		if ( cc[t] == V3fc::HUGE_VALUE_POS ) cs[t] = cc[t];
	}
	cs.erase( remove_if( cs.begin(), cs.end(), []( Vector3f const& sc ) -> bool { return sc == V3fc::HUGE_VALUE_POS; } ), cs.end() );
}

void tbCalc( const Vector3f& v1, const Vector3f& v2, const Vector3f& v3,
             const Vector2f& uv1, const Vector2f& uv2, const Vector2f& uv3,
             Vector4f& tangent1, Vector4f& tangent2, Vector4f& tangent3,
             Vector3f& bitangent1, Vector3f& bitangent2, Vector3f& bitangent3 ) {

    // Edges of the triangle : position delta
    Vector3f edge1 = v2 - v1;
    Vector3f edge2 = v3 - v1;

    // UV delta
    Vector2f deltaUV1 = uv2 - uv1;
    Vector2f deltaUV2 = uv3 - uv1;

    float den = ( deltaUV1.x() * deltaUV2.y() - deltaUV1.y() * deltaUV2.x() );
    float r = isScalarEqual( den, 0.0f ) ? 1.0f : 1.0f / den;

    Vector3f deltaPosT = ( edge1 * deltaUV2.y() - edge2 * deltaUV1.y() );
    Vector3f deltaPosB = ( edge1 * -deltaUV2.x() + edge2 * deltaUV1.x() );

    tangent1 = normalize( deltaPosT * r );
    tangent2 = tangent1;
    tangent3 = tangent1;

    bitangent1 = normalize( deltaPosB * r );
    bitangent2 = bitangent1;
    bitangent3 = bitangent1;
}

QuadVector3f makeQuadV3f( const Vector3f& p1, const Vector3f& p2, float z2 ) {
    V3f v3 = p1 + V3fc::UP_AXIS * z2;
    V3f v4 = p2 + V3fc::UP_AXIS * z2;
    return QuadVector3f{ { p1, p2, v4, v3 } };
}

Vector3f XZY::C( const Vector3f& _v ) {
	return _v.xzy();
}

Vector3f XZY::C( const Vector2f& _v ) {
    return V3f{_v}.xzy();
}

Vector3f XZY::C( const Vector2f& v2, const float z ) {
	return { v2.x(), z, v2.y() };
}

Vector3f XZY::C( const float x, const float y, const float z ) {
	return { x, z, y };
}

std::vector<Vector3f> XZY::C( const std::vector<Vector3f>& _v ) {
	std::vector<Vector3f> ret;
	for ( auto& v : _v ) ret.emplace_back(v.xzy());
	return ret;
}

std::vector<Vector3f> XZY::C( const std::vector<Vector3f>& _v, float _z ) {
    std::vector<Vector3f> ret;
    for ( auto& v : _v ) ret.emplace_back(v.x(), _z, v.y() );
    return ret;
}

std::vector<Vector3f> XZY::C( const std::vector<Vector3f>& _v, XZY::Conversion convertOrPassThrough ) {
    return convertOrPassThrough == XZY::Conversion::Convert ? XZY::C(_v) : _v;
}

std::vector<Vector2f> XZY::C2( const std::vector<Vector3f>& _v ) {
    std::vector<Vector2f> ret;
    for ( auto& v : _v ) ret.emplace_back(v.xz());
    return ret;
}

std::vector<Vector3f> XZY::C( const std::vector<Vector2f>& _v, float _z ) {
	std::vector<Vector3f> ret;
	for ( auto& v : _v ) ret.emplace_back( v.x(), _z, v.y() );
	return ret;
}

Vector2f XZY::C2( const Vector3f& v2 ) {
    return V2f{v2.x(), v2.z()};
}

