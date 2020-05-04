#include "vector3f.h"

#include <map>
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

const Vector3f Vector3f::ZERO = Vector3f( 0.0f, 0.0f, 0.0f );
const Vector3f Vector3f::X_AXIS = Vector3f( 1.0f, 0.0f, 0.0f );
const Vector3f Vector3f::Y_AXIS = Vector3f( 0.0f, 1.0f, 0.0f );
const Vector3f Vector3f::UP_AXIS = Vector3f( 0.0f, 1.0f, 0.0f );
const Vector3f Vector3f::UP_AXIS_NEG = Vector3f( 0.0f, -1.0f, 0.0f );
const Vector3f Vector3f::Z_AXIS = Vector3f( 0.0f, 0.0f, 1.0f );
const Vector3f Vector3f::X_AXIS_NEG = Vector3f( -1.0f, 0.0f, 0.0f );
const Vector3f Vector3f::Y_AXIS_NEG = Vector3f( 0.0f, -1.0f, 0.0f );
const Vector3f Vector3f::Z_AXIS_NEG = Vector3f( 0.0f, 0.0f, -1.0f );

const Vector3f Vector3f::X_AXIS_NEG_MASK = Vector3f( -1.0f, 1.0f, 1.0f );
const Vector3f Vector3f::Y_AXIS_NEG_MASK = Vector3f( 1.0f, -1.0f, 1.0f );
const Vector3f Vector3f::Z_AXIS_NEG_MASK = Vector3f( 1.0f, 1.0f, -1.0f );

const Vector3f Vector3f::MASK_X_OUT = Vector3f( 0.0f, 1.0f, 1.0f );
const Vector3f Vector3f::MASK_Y_OUT = Vector3f( 1.0f, 0.0f, 1.0f );
const Vector3f Vector3f::MASK_Z_OUT = Vector3f( 1.0f, 1.0f, 0.0f );
const Vector3f Vector3f::MASK_UP_OUT = Vector3f( 1.0f, 0.0f, 1.0f );

const Vector3f Vector3f::UE4_PROFILE_AXIS = Vector3f( 0.0f, 0.0f, -1.0f );
const Vector3f Vector3f::ONE = Vector3f( 1.0f, 1.0f, 1.0f );
const Vector3f Vector3f::HUGE_VALUE_POS = Vector3f( std::numeric_limits<float>::max() );
const Vector3f Vector3f::HUGE_VALUE_NEG = Vector3f( std::numeric_limits<float>::lowest() );

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
				cc[t] = Vector3f::HUGE_VALUE_POS;
			} else {
				cc[t1] = Vector3f::HUGE_VALUE_POS;
			}
		}
	}

	for ( auto t = 0; t < csize; t++ ) {
		if ( cc[t] == Vector3f::HUGE_VALUE_POS ) cs[t] = cc[t];
	}
	cs.erase( remove_if( cs.begin(), cs.end(), []( Vector3f const& sc ) -> bool { return sc == Vector3f::HUGE_VALUE_POS; } ), cs.end() );
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

Vector3f XZY::C( const Vector3f& _v ) {
	return _v.xzy();
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
	for ( auto& v : _v ) ret.emplace_back(Vector3f{v.x(), _z, v.y() });
	return ret;
}

Vector2f XZY::C2( const Vector3f& v2 ) {
    return V2f{v2.x(), v2.z()};
}
