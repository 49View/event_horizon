
#include "aabb.h"

const JMATH::AABB JMATH::AABB::IDENTITY = AABB( Vector3f::ZERO, Vector3f::ONE );
const JMATH::AABB JMATH::AABB::ZERO = AABB( Vector3f::ZERO, Vector3f::ZERO );

namespace JMATH {
    std::ostream& operator<<( std::ostream& os, const JMATH::AABB& f ) {
        os << "Min: " << f.mMinPoint << " Max: " << f.mMaxPoint;
        return os;
    }
}

void JMATH::AABB::merge( const AABB& val ) {
	expandMax( val.mMaxPoint );
	expandMin( val.mMinPoint );
}

JMATH::Rect2f JMATH::AABB::demoteTo2d() const {
	Vector3f diff = mMaxPoint - mMinPoint;
	if ( diff.leastDominantElement() == 0 ) {
		return Rect2f( mMinPoint.yz(), mMaxPoint.yz() );
	}
	if ( diff.leastDominantElement() == 1 ) {
		return Rect2f( mMinPoint.xz(), mMaxPoint.xz() );
	}
	return Rect2f( mMinPoint.xy(), mMaxPoint.xy() );
}

JMATH::Rect2f JMATH::AABB::topDown() const {
    return Rect2f( mMinPoint.xz(), mMaxPoint.xz() );
}

JMATH::Rect2f JMATH::AABB::front() const {
    return Rect2f( mMinPoint.xy(), mMaxPoint.xy() );
}

JMATH::AABB JMATH::AABB::rotate( const Vector4f& axisAngle ) const {
    return rotate( axisAngle.w(), axisAngle.xyz() );
}

JMATH::AABB JMATH::AABB::rotate( float angle, const Vector3f& axis ) const {
	Matrix3f mat;
	mat.setRotation( angle, axis );


	Vector3f mi = mat * (mMinPoint - centre());
	Vector3f ma = mat * (mMaxPoint - centre());

	mi += centre();
	ma += centre();

	AABB ret;
	ret.set( mi, ma );

	return ret;
}

const JMATH::AABB JMATH::AABB::INVALID = AABB( Vector3f( std::numeric_limits<float>::max() ), Vector3f(
		std::numeric_limits<float>::lowest() ) );

bool AABB::intersectLine( const Vector3f& linePos, const Vector3f& lineDir, float &tNear, float &tFar ) const {
	//assume2(lineDir.IsNormalized(), lineDir, lineDir.LengthSq());
	//assume2(tNear <= tFar && "AABB::IntersectLineAABB: User gave a degenerate line as input for the intersection test!", tNear, tFar);
	// The user should have inputted values for tNear and tFar to specify the desired subrange [tNear, tFar] of the line
	// for this intersection test.
	// For a Line-AABB test, pass in
	//    tNear = -FLOAT_INF;
	//    tFar = FLOAT_INF;
	// For a Ray-AABB test, pass in
	//    tNear = 0.f;
	//    tFar = FLOAT_INF;
	// For a LineSegment-AABB test, pass in
	//    tNear = 0.f;
	//    tFar = LineSegment.Length();

	// Test each cardinal plane (X, Y and Z) in turn.
	if ( !isScalarEqual( lineDir.x(), 0.f ) ) {
		float recipDir = 1.0f / lineDir.x(); //RecipFast(lineDir.x);
		float t1 = ( mMinPoint.x() - linePos.x() ) * recipDir;
		float t2 = ( mMaxPoint.x() - linePos.x() ) * recipDir;

		// tNear tracks distance to intersect (enter) the AABB.
		// tFar tracks the distance to exit the AABB.
		if ( t1 < t2 ) {
			tNear = std::max( t1, tNear );
			tFar = std::min( t2, tFar );
		} else { // Swap t1 and t2.
			tNear = std::max( t2, tNear );
			tFar = std::min( t1, tFar );
		}

		if ( tNear > tFar )
			return false;//false; // Box is missed since we "exit" before entering it.
	} else if ( linePos.x() < mMinPoint.x() || linePos.x() > mMaxPoint.x() )
		return false; // The ray can't possibly enter the box, abort.

	if ( !isScalarEqual( lineDir.y(), 0.f ) ) {
		float recipDir = 1.0f / lineDir.y();
		float t1 = ( mMinPoint.y() - linePos.y() ) * recipDir;
		float t2 = ( mMaxPoint.y() - linePos.y() ) * recipDir;

		if ( t1 < t2 ) {
			tNear = std::max( t1, tNear );
			tFar = std::min( t2, tFar );
		} else {// Swap t1 and t2.
			tNear = std::max( t2, tNear );
			tFar = std::min( t1, tFar );
		}
		if ( tNear > tFar )
			return false; // Box is missed since we "exit" before entering it.
	} else if ( linePos.y() < mMinPoint.y() || linePos.y() > mMaxPoint.y() )
		return false; // The ray can't possibly enter the box, abort.

	if ( !isScalarEqual( lineDir.z(), 0.f ) ) // ray is parallel to plane in question
	{
		float recipDir = 1.0f / lineDir.z();
		float t1 = ( mMinPoint.z() - linePos.z() ) * recipDir;
		float t2 = ( mMaxPoint.z() - linePos.z() ) * recipDir;

		if ( t1 < t2 ) {
			tNear = std::max( t1, tNear );
			tFar = std::min( t2, tFar );
		} else { // Swap t1 and t2.
			tNear = std::max( t2, tNear );
			tFar = std::min( t1, tFar );
		}
	} else if ( linePos.z() < mMinPoint.z() || linePos.z() > mMaxPoint.z() )
		return false; // The ray can't possibly enter the box, abort.

	return tNear <= tFar;
}

std::vector<Vector3f> AABB::topDownOutline( CompositeWrapping _wrap ) const {
    std::vector<Vector3f> ret;
    ret.push_back( mMinPoint.xy() );
    ret.push_back( Vector2f{ mMaxPoint.x(), mMinPoint.y() } );
    ret.push_back( mMaxPoint.xy() );
    ret.push_back( Vector2f{ mMinPoint.x(), mMaxPoint.y() } );
    if ( _wrap == CompositeWrapping::Wrap ) {
        ret.push_back( mMinPoint.xy() );
    }
    return ret;
}

bool AABB::containsXZ( const V2f& _point ) const {
    return _point.x() > minPoint().x() && _point.x() < maxPoint().x() &&
           _point.y() > minPoint().z() && _point.y() < maxPoint().z();
}

bool AABB::containsXY( const V2f& _point ) const {
    return _point.x() > minPoint().x() && _point.x() < maxPoint().x() &&
           _point.y() > minPoint().y() && _point.y() < maxPoint().y();
}

void AABB::identity() {
    *this = MIDENTITY();
}

void AABB::identityCentered() {
    *this = MIDENTITYCENTER();
}
