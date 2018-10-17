#include "vector2f.h"
#include "matrix2f.h"
#include "math_util.h"



const Vector2f Vector2f::ZERO = Vector2f( 0.0f, 0.0f );
const Vector2f Vector2f::X_AXIS = Vector2f( 1.0f, 0.0f );
const Vector2f Vector2f::Y_AXIS = Vector2f( 0.0f, 1.0f );
const Vector2f Vector2f::X_AXIS_NEG = Vector2f( -1.0f, 0.0f );
const Vector2f Vector2f::Y_AXIS_NEG = Vector2f( 0.0f, -1.0f );
const Vector2f Vector2f::Y_INV = Vector2f( 1.0f, -1.0f );
const Vector2f Vector2f::X_INV = Vector2f( -1.0f, 1.0f );
const Vector2f Vector2f::ONE = Vector2f( 1.0f, 1.0f );
const Vector2f Vector2f::HUGE_VALUE_POS = Vector2f( std::numeric_limits<float>::max(), std::numeric_limits<float>::max() );
const Vector2f Vector2f::HUGE_VALUE_NEG = Vector2f( std::numeric_limits<float>::lowest(),
													std::numeric_limits<float>::lowest() );

int64_t Vector2f::hash() const {
	size_t h1 = std::hash<float>()( mX );
	size_t h2 = std::hash<float>()( mY );
	return ( h1 ^ ( h2 << 1 ) );
}

std::string Vector2f::toString() const {
	return "X = " + floatToFixedDigits( mX ) + " Y = " + floatToFixedDigits( mY );
}

int winding( const std::vector<Vector2f>& points ) {
	float s = 0.0f;
	int si = static_cast<int>( points.size() );
	for ( int t = 0; t < si; t++ ) {
		auto e1 = points[getCircularArrayIndex( t - 1, si )] - points[t];
		auto e2 = points[t] - points[getCircularArrayIndex( t + 1, si )];
		s += dotPerp( e1, e2 ) / ( length( e1 ) * length( e2 ) );
	}

	return s > 0.0f ? 1 : 0;
}

float distanceFromLine( const Vector2f& p, const Vector2f& p1, const Vector2f& p2 ) {
	Vector2f diff = p2 - p1;
	Matrix2f m( diff, p1 - p );
	float nom = fabs( m.determinant() );
	float den = length( diff );

	//	ASSERT(den != 0.0f);

		// If den == 0.0f then p1==p2 so make it calculate the distance between p and p1 instead
	if ( den != 0.0f ) {
		if ( nom != 0.0f ) {
			return nom / den;
		} else {
			return min( distance( p, p1 ), distance( p, p2 ) );
		}
	} else // Otherwise
		return distance( p, p1 );
}

float distanceFromLine( const Vector2f& p, const Vector2f& p1, const Vector2f& p2, Vector2f& pointOfIntersection ) {
	float ret = distanceFromLine( p, p1, p2 );

	Vector2f perp = normalize( p2 - p1 );
	perp.rotate( M_PI_2 );
	bool isInters = intersection( p1, p2, p - perp*100000.0f, p + perp*100000.0f, pointOfIntersection );

	if ( !isInters ) {
		float d1 = distance( p, p1 );
		float d2 = distance( p, p2 );
		pointOfIntersection = ( d1 <= d2 ) ? p1 : p2;
		ret = ( d1 <= d2 ) ? d1 : d2;
	}

	return ret;
}

bool distanceFromLineClamped( const Vector2f& p, const Vector2f& p1, const Vector2f& p2, float& ret, Vector2f& pointOfIntersection ) {
	ret = distanceFromLine( p, p1, p2 );

	Vector2f perp = normalize( p2 - p1 );
	perp.rotate( M_PI_2 );
	bool isInters = intersection( p1, p2, p - perp*100000.0f, p + perp*100000.0f, pointOfIntersection );

	return isInters;
}

bool distanceFromLineCapsule( const Vector2f& p, const Vector2f& p1, const Vector2f& p2, float radius, float& ret, Vector2f& pointOfIntersection ) {
	// check first if p is within radius of p1 and p2, in that case that will be 100% inside the capsule
	if ( distance( p, p1 ) <= radius ) return true;
	if ( distance( p, p2 ) <= radius ) return true;

	return distanceFromLineClamped( p, p1, p2, ret, pointOfIntersection ) && ret <= radius;
}

bool isPointInsideLineCapsule( const Vector2f& p, const Vector2f& p1, const Vector2f& p2, float radius ) {
	float ret = 0.0f;
	Vector2f pointOfIntersection = Vector2f::ZERO;

	return distanceFromLineCapsule( p, p1, p2, radius, ret, pointOfIntersection );
}

bool isCollinear( const Vector2f& a, const Vector2f& b, const Vector2f& c, const float epsilon /*= 0.01f*/ ) {
	//if ( isVerySimilar( a, b, epsilon ) ) return true;
	//if ( isVerySimilar( c, b, epsilon ) ) return true;
	//if ( isVerySimilar( a, c, epsilon ) ) return true;
	//float cp = fabs( ( b.x() - a.x() )*( c.y() - a.y() ) - ( c.x() - a.x() )*( b.y() - a.y() ) );
	float cp = fabs( a.x() * (b.y() - c.y()) + b.x() * (c.y() - a.y()) + c.x() * (a.y() - b.y()) );
	bool ret = ( cp < epsilon );
	return ret;
}

bool isInsideTriangle( const Vector2f& P, const Vector2f& A, const Vector2f& B, const Vector2f& C ) {
	Vector2f v0 = C - A;
	Vector2f v1 = B - A;
	Vector2f v2 = P - A;

	// Compute dot products
	float dot00 = dot( v0, v0 );
	float dot01 = dot( v0, v1 );
	float dot02 = dot( v0, v2 );
	float dot11 = dot( v1, v1 );
	float dot12 = dot( v1, v2 );

	// Compute barycentric coordinates
	float invDenom = 1.0f / ( dot00 * dot11 - dot01 * dot01 );
	float u = ( dot11 * dot02 - dot01 * dot12 ) * invDenom;
	float v = ( dot00 * dot12 - dot01 * dot02 ) * invDenom;

	// Check if point is in triangle
	return ( u >= 0.0f ) && ( v >= 0.0f ) && ( u + v < 1.0f );
}

void straightenOnMainAxis( Vector2f& a, Vector2f& b, float straightCoeff ) {
	Vector2f vdiff = normalize( a - b );
	float ad = fabs( dot( vdiff, Vector2f( 1.0f, 0.0f ) ) );
	if ( ( ad > straightCoeff && ad < 1.0f ) ) {
		b.setY( a.y() );
	}
	float ad2 = fabs( dot( vdiff, Vector2f( 0.0f, 1.0f ) ) );
	if ( ( ad2 > straightCoeff && ad2 < 1.0f ) ) {
		b.setX( a.x() );
	}
}

void removeCollinear( vector2fList& cs, float epsilon ) {
	int csize = static_cast<int>( cs.size() );
	if ( csize < 3 ) return;

	vector2fList cc = cs;

	for ( auto t = 0; t < csize; t++ ) {
		Vector2f currPoint1 = cs[t];
		int t1 = getCircularArrayIndex( t + 1, csize );
		Vector2f currPoint2 = cs[t1];
		Vector2f currPoint3 = cs[getCircularArrayIndex( t + 2, csize )];
		if ( isCollinear( currPoint1, currPoint2, currPoint3, epsilon ) ) {
			// check if degenerate
			if ( isVerySimilar( currPoint1, currPoint3, epsilon ) ) {
				cc[t] = Vector2f::HUGE_VALUE_POS;
			} else {
				cc[t1] = Vector2f::HUGE_VALUE_POS;
			}
		}
	}

	//int ccTz = 0;
	//for ( auto t = 0; t < csize; t++ ) {
	//	if ( cc[t] == Vector2f::HUGE_VALUE_POS ) ccTz++;
	//}
	//ASSERT( ccTz < csize );

	for ( auto t = 0; t < csize; t++ ) {
		if ( cc[t] == Vector2f::HUGE_VALUE_POS ) cs[t] = cc[t];
	}
	cs.erase( remove_if( cs.begin(), cs.end(), []( Vector2f const& sc ) -> bool { return sc == Vector2f::HUGE_VALUE_POS; } ), cs.end() );
}


