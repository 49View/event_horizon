#ifndef D_MATH_AABB_H
#define D_MATH_AABB_H

#include "vector3f.h"
#include "matrix4f.h"
#include "rect2f.h"

// SHARED CLASS: Any changes to this, should be integrated to /svn/shared, and a mail should be send to everyone


namespace JMATH {
enum class AABBFeature {
	invalid = 0,
};

class AABB {
public:

	static const AABB IDENTITY;
	static const AABB INVALID;
	static const AABB ZERO;

	static AABB& MIDENTITY() {
		static AABB a( Vector3f( 0.0f ), Vector3f( 1.0f ) );
		return a;
	}

	static AABB MINVALID() {
		return AABB::INVALID;
	}

	static constexpr bool IsSerializable() { return true; }

	AABB() {
	}

	AABB( const Vector3f& minPoint, const Vector3f& maxPoint ) {
		mMinPoint = minPoint;
		mMaxPoint = maxPoint;
	}

	AABB( const std::vector<Vector3f> points ) {
		*this = INVALID;
		for ( auto& p : points ) expand( p );
	}

	bool isValid() const {
		return *this != AABB::INVALID;
	}

	float* rawPtr() {
		return reinterpret_cast<float*>( &mMinPoint[0] );
	}

	void set( const Vector3f& minPoint, const Vector3f& maxPoint ) {
		*this = INVALID;
		expand( minPoint );
		expand( maxPoint );
	}

	void calc( const JMATH::Rect2f& bbox, float height, const Matrix4f& tmat ) {
		Vector3f b3dmin = { bbox.topLeft(), 0.0f };
		Vector3f b3dmax = { bbox.bottomRight(), height };

		b3dmin = tmat.transform( b3dmin );
		b3dmax = tmat.transform( b3dmax );

		set( b3dmin, b3dmax );
	}

	void calc( const std::initializer_list<Vector3f>& ilist, const Matrix4f& tmat ) {
		std::vector<Vector3f> vlist;
		for ( auto& v : ilist ) vlist.push_back( v );
		calc( vlist, tmat );
	}

	void calc( const std::vector<Vector3f>& vlist, const Matrix4f& tmat ) {
		Vector3f b3dmin = Vector3f::HUGE_VALUE_POS;
		Vector3f b3dmax = Vector3f::HUGE_VALUE_NEG;

		for ( auto& v : vlist ) {
			b3dmin = min( b3dmin, v );
			b3dmax = max( b3dmax, v );
		}

		b3dmin = tmat.transform( b3dmin );
		b3dmax = tmat.transform( b3dmax );

		set( b3dmin, b3dmax );
	}

	void setCenterAndSize( const Vector3f& _center, const Vector3f& _size ) {
		mMinPoint = _center;
		mMaxPoint = _center;
		expand( _center + _size*0.5f );
		expand( _center + _size*-0.5f );
	}

	void expand( const Vector3f& p ) {
		if ( p.x() < mMinPoint.x() ) mMinPoint.setX( p.x() );
		if ( p.x() > mMaxPoint.x() ) mMaxPoint.setX( p.x() );

		if ( p.y() < mMinPoint.y() ) mMinPoint.setY( p.y() );
		if ( p.y() > mMaxPoint.y() ) mMaxPoint.setY( p.y() );

		if ( p.z() < mMinPoint.z() ) mMinPoint.setZ( p.z() );
		if ( p.z() > mMaxPoint.z() ) mMaxPoint.setZ( p.z() );
	}

	void expandMin( const Vector3f& p ) {
		if ( p.x() < mMinPoint.x() ) mMinPoint.setX( p.x() );
		if ( p.y() < mMinPoint.y() ) mMinPoint.setY( p.y() );
		if ( p.z() < mMinPoint.z() ) mMinPoint.setZ( p.z() );
	}

	void expandMax( const Vector3f& p ) {
		if ( p.x() > mMaxPoint.x() ) mMaxPoint.setX( p.x() );
		if ( p.y() > mMaxPoint.y() ) mMaxPoint.setY( p.y() );
		if ( p.z() > mMaxPoint.z() ) mMaxPoint.setZ( p.z() );
	}

	void merge( const AABB& val );

	Rect2f demoteTo2d() const;
	int leastDominantAxis() const {
		Vector3f diff = mMaxPoint - mMinPoint;
		return diff.leastDominantElement();
	}

	float calcWidth() const {
		return fabs( mMaxPoint.x() - mMinPoint.x() );
	}

	float calcHeight() const {
		return fabs( mMaxPoint.y() - mMinPoint.y() );
	}

	float calcDepth() const {
		return fabs( mMaxPoint.z() - mMinPoint.z() );
	}

	float calcDiameter() const {
		Vector3f s = size();
		return s[s.dominantElement()];
	}

	float calcRadius() const {
		return calcDiameter() * 0.5f;
	}

	float pivotHeight() const {
		return mMaxPoint.y();
	}

	float pivotWidth() const {
		return mMaxPoint.x();
	}

	Vector3f calcCentre() const {
		Vector3f centre = ( mMinPoint + mMaxPoint ) * 0.5f;
		return centre;
	}

	Vector3f centreTop() const {
		return calcCentre() + Vector3f::Z_AXIS*calcDepth()*0.5f;
	}

	Vector3f centreFront() const {
		return calcCentre() + Vector3f::Y_AXIS*calcHeight()*0.5f;
	}

	Vector3f bottomFront() const {
		return Vector3f{ calcCentre().xy(), minPoint().z() } +Vector3f::Y_AXIS*calcHeight()*0.5f;
	}

	void translate( const Vector3f& offset ) {
		mMinPoint += offset;
		mMaxPoint += offset;
	}

	void scaleX( float x ) {
		mMinPoint.setX( mMinPoint.x()*x );
		mMaxPoint.setX( mMaxPoint.x()*x );
	}

	void scaleY( float x ) {
		mMinPoint.setY( mMinPoint.y()*x );
		mMaxPoint.setY( mMaxPoint.y()*x );
	}

	void scaleZ( float x ) {
		mMinPoint.setZ( mMinPoint.z()*x );
		mMaxPoint.setZ( mMaxPoint.z()*x );
	}

	void scale( float x ) {
		mMinPoint *= 1.0f / x;
		mMaxPoint *= x;
	}

	void transform( const Matrix4f& mat ) {
		Vector3f mi = mat.transform( mMinPoint );
		Vector3f ma = mat.transform( mMaxPoint );

		set( mi, ma );
	}

	AABB rotate( float angle, const Vector3f& axis ) const;

	std::vector<Vector3f> topDownOutline( CompositeWrapping _wrap = CompositeWrapping::NoWrap ) const;

	bool operator==( const AABB& rhs ) const {
		if ( minPoint() != rhs.minPoint() || maxPoint() != rhs.maxPoint() ) return false;
		return true;
	}

	bool operator!=( const AABB& rhs ) const {
		if ( minPoint() != rhs.minPoint() || maxPoint() != rhs.maxPoint() ) return true;
		return false;
	}

	AABB operator-( const Vector3f& rhs ) const {
		return AABB( mMinPoint - rhs, mMaxPoint - rhs );
	}

	AABB operator+( const Vector3f& rhs ) const {
		return AABB( mMinPoint + rhs, mMaxPoint + rhs );
	}

	void operator -=( const Vector3f& rhs ) {
		mMinPoint -= rhs;
		mMaxPoint -= rhs;
	}

	void operator +=( const Vector3f& rhs ) {
		mMinPoint += rhs;
		mMaxPoint += rhs;
	}

	AABB operator*( const Vector3f& rhs ) const {
		AABB r = *this;

		r.scaleX( rhs.x() );
		r.scaleY( rhs.y() );

		return r;
	}

	bool intersectLine( const Vector3f& linePos, const Vector3f& lineDir, float &tNear, float &tFar ) const;

	//std::vector<Vector3f> points3d() const {
	//	std::vector<Vector3f> ret;

	//	ret.push_back(Vector3f(bottomLeft(), z));
	//	ret.push_back(Vector3f(maxPoint(), z));
	//	ret.push_back(Vector3f(topRight(), z));
	//	ret.push_back(Vector3f(minPoint(), z));

	//	return ret;
	//}

	inline Vector3f minPoint() { return mMinPoint; }
	inline Vector3f maxPoint() { return mMaxPoint; }

    inline const Vector3f& minPoint() const { return mMinPoint; }
    inline const Vector3f& maxPoint() const { return mMaxPoint; }

	inline void setMinPoint( const Vector3f& val ) { mMinPoint = val; }
	inline void setMaxPoint( const Vector3f& val ) { mMaxPoint = val; }

	//inline Vector2f topRight() const { return Vector2f(right(), top()); }
	//inline Vector2f bottomLeft() const { return Vector2f(left(), bottom()); }

	//inline Vector2f topLeftYInv() const { return Vector2f(mMinPoint.x(), 1.0f-mMinPoint.y()); }
	//inline Vector2f bottomRightYInv() const { return Vector2f(mMaxPoint.x(), 1.0f-mMaxPoint.y()); }
	//inline Vector2f topRightYInv() const { return Vector2f(right(), 1.0f-top()); }
	//inline Vector2f bottomLeftYInv() const { return Vector2f(left(), 1.0f-bottom()); }

	//// Scalar accessors
	//float left() const { return mMinPoint.x(); }
	//float top() const { return mMinPoint.y(); }
	//float right() const { return mMaxPoint.x(); }
	//float bottom() const { return mMaxPoint.y(); }
	//
	////Screen normalised [-1,+1]
	//float leftSN() const { return mMinPoint.x() * 2.0f - 1.0f; }
	//float topSN() const { return mMinPoint.y() * 2.0f - 1.0f; }
	//float rightSN() const { return mMaxPoint.x() * 2.0f - 1.0f; }
	//float bottomSN() const { return mMaxPoint.y() * 2.0f - 1.0f; }
	//
	//void setLeft(float f) { mMinPoint.setX(f); }
	//void setTop(float f) { mMinPoint.setY(f); }
	//void setRight(float f) { mMaxPoint.setX(f); }
	//void setBottom(float f) { mMaxPoint.setY(f); }

	//void setTopLeft(const Vector2f& v) { mMinPoint = v; }
	//void setTopRight(const Vector2f& v) { setTop(v.y()); setRight(v.x()); }
	//void setBottomLeft(const Vector2f& v) { setBottom(v.y()); setLeft(v.x()); }
	//void setBottomRight(const Vector2f& v) { mMaxPoint = v; }

	//float width() const { return mMaxPoint.x()- mMinPoint.x(); }
	//float height() const { return mMaxPoint.y() - mMinPoint.y(); }
	//float perimeter() const { return width()*2.0f + height()*2.0f; }
	//float area() const { return width() * height(); }
	//float ratio() const { return width()/height(); }
	Vector3f size() const { return mMaxPoint - mMinPoint; }
	Vector3f centre() const { return calcCentre(); }

	//Vector2f centreRight() const { return centre() + Vector2f(width()*0.5f, 0.0f); }
	//Vector2f centreLeft() const { return centre() - Vector2f(width()*0.5f, 0.0f); }
	//Vector2f centreTop() const { return centre() + Vector2f(0.0f, height()*0.5f); }
	//Vector2f centreBottom() const { return centre() - Vector2f(0.0f, height()*0.5f); }

	// DO NOT Change the order of these member variables as they are needed to get proper indices for raw access (performance requirement)
	Vector3f mMinPoint;
	Vector3f mMaxPoint;
};
}



#endif // D_AABB_H
