#ifndef D_MATH_RECT2F_H
#define D_MATH_RECT2F_H

#include <memory>
#include "vector3f.h"

// SHARED CLASS: Any changes to this, should be integrated to /svn/shared, and a mail should be send to everyone


namespace JMATH {

struct Rect2fFeature {
	const static uint64_t invalid = 0;
	const static uint64_t left = 1;
	const static uint64_t right = 2;
	const static uint64_t top = 3;
	const static uint64_t bottom = 4;
	const static uint64_t topLeft = 5;
	const static uint64_t topRight = 6;
	const static uint64_t bottomLeft = 7;
	const static uint64_t bottomRight = 8;
};

using Rect2fFeatureT = uint64_t;

enum class EdgeTouchingIsIntersecting {
    Yes,
    No
};

enum class RectV2f {
    Centered
};

class Rect2f {
public:

	static const Rect2f IDENTITY;
	static const Rect2f IDENTITY_CENTERED;
	static const Rect2f INVALID;
	static const Rect2f ZERO;

    friend std::ostream& operator<<( std::ostream& os, const JMATH::Rect2f& f );

    static Rect2f& MIDENTITY()  // return reference.
	{
		static Rect2f a( 0.0f, 0.0f, 1.0f, 1.0f );
		return a;
	}

	Rect2f() = default;

	explicit Rect2f( const int _square );
	explicit Rect2f( const float _square );
	explicit Rect2f( const Vector2f& topLeft );
    Rect2f( const Vector2f& _center, RectV2f rt);
	explicit Rect2f( const Vector2f& topLeft, const Vector2f& bottomRight );
	explicit Rect2f( const Vector2f& topLeft, const Vector2f& bottomRight, bool /*expand*/ );
	explicit Rect2f( float left, float top, float right, float bottom );
	explicit Rect2f( const V2fVector & points );
    explicit Rect2f( const V2fVectorOfVector& points );
	explicit Rect2f( const std::initializer_list<Vector2f>& points );
	explicit Rect2f( const std::vector<Vector3f>& points );
	explicit Rect2f( const Vector3f* points, size_t vsize );

    bool operator==( const Rect2f& rhs ) const;
    bool operator!=( const Rect2f& rhs ) const;
    float operator[]( const unsigned int rhs ) const;
    Rect2f operator*( float rhs ) const;
    void operator*=( float rhs );
    Rect2f operator-( const Vector2f& rhs ) const;
    Rect2f operator+( const Vector2f& rhs ) const;
    void operator -=( const Vector2f& rhs );
    void operator +=( const Vector2f& rhs );
    Rect2f operator*( const Vector2f& rhs ) const;

    void centered( const V2f& _size );
    void centerAroundOrigin();

    float* rawPtr();

	void invalidate();

	Rect2f ss() const;

	void set( const Vector2f& topLeft, const Vector2f& bottomRight );
	void set( float left, float top, float right, float bottom );
	void setCenterAndSize( const Vector2f& _center, const Vector2f& _size );
	bool contains( const Vector2f& pos ) const;
    bool containsEqual( const Vector2f& pos ) const;
    bool contains( float x, float y ) const;
    bool containsEqual( float x, float y ) const;
    bool contains( const Rect2f& rect ) const;
    bool intersect( const Rect2f& rect, float _epsilon = 0.00001f, EdgeTouchingIsIntersecting _eti = EdgeTouchingIsIntersecting::No ) const;
    bool isValid() const;
	bool isDegenerated( float epsilon = 0.0001f ) const;
	bool hasMimimunSize( const Vector2f & _size ) const;

	void expand( const Vector2f& p );
	void expand( float x, float y );
	void merge( const Rect2f& addr );
	void shrink( float amount ); // shrink in units not in percentage

	float calcWidth() const;
	float calcHeight() const;
	Vector2f calcCentre() const;

	float dominant() const;
	float aspectRatio() const;
	void translate( const Vector2f& offset );

	void scaleX( float x );
	void scaleY( float x );
	void scale( float x );
    void floor();
    void clamp( float minX, float minY, float maxX, float maxY );
    void scaleWithClamp( float x, float minX, float minY, float maxX, float maxY );
    void cropOnMainAxis( const Vector2f& cropPerc );
    void scaleWithClampOnMainAxes( float mainA, float secA, float minX, float minY, float maxX, float maxY );
    Vector2f calculateBestFitSize( const Vector2f& fsize ) const;

	[[nodiscard]] Rect2f oneMinusY() const;

	std::vector<Rect2f> booleanDifference( const Rect2f& diffRect ) const;
	bool lineIntersection( const Vector2f& p1, const Vector2f& p2 ) const;

	// This return a vector2f normalized [0...1] against the rect
	// optionally it will scale it by the vscale vector
	Vector2f normalizeWithinRect( const Vector2f& v, const Vector2f& vscale = V2fc::ONE ) const;

	void setFeature( JMATH::Rect2fFeatureT feature, const Vector2f& val );
	void addFeatureDelta( JMATH::Rect2fFeatureT feature, const Vector2f& val );
	Vector2f coordOfFeature( JMATH::Rect2fFeatureT feature );
	JMATH::Rect2fFeatureT isPointNearAFeature( const Vector2f& pos, float minDist, float& ret_distance );

	[[nodiscard]] std::vector<Vector2f> points() const;
    [[nodiscard]] V2fVector pointsStrip() const;
    [[nodiscard]] V2fVector pointsTriangleList() const;
    [[nodiscard]] std::vector<Vector2f> pointscw() const;
	[[nodiscard]] std::vector<Vector3f> points3d( float z = 0.0f ) const;
    [[nodiscard]] std::vector<Vector3f> points3d_xzy() const;
    [[nodiscard]] std::vector<Vector3f> points3dcw_xzy() const;
    [[nodiscard]] std::vector<Vector3f> points3dcw( float z = 0.0f ) const;

	[[nodiscard]] Rect2f squared() const;
    [[nodiscard]] Rect2f squaredBothSides() const;

    inline Vector2f topLeft() const { return mTopLeft; }
	inline Vector2f bottomRight() const { return mBottomRight; }
	inline Vector2f topRight() const { return Vector2f( right(), top() ); }
	inline Vector2f bottomLeft() const { return Vector2f( left(), bottom() ); }

	inline Vector2f topLeftYInv() const { return Vector2f( mTopLeft.x(), 1.0f - mTopLeft.y() ); }
	inline Vector2f bottomRightYInv() const { return Vector2f( mBottomRight.x(), 1.0f - mBottomRight.y() ); }
	inline Vector2f topRightYInv() const { return Vector2f( right(), 1.0f - top() ); }
	inline Vector2f bottomLeftYInv() const { return Vector2f( left(), 1.0f - bottom() ); }

	// Scalar accessors
	float left() const { return mTopLeft.x(); }
	float top() const { return mTopLeft.y(); }
	float right() const { return mBottomRight.x(); }
	float bottom() const { return mBottomRight.y(); }

	//Screen normalised [-1,+1]
	float leftSN() const { return mTopLeft.x() * 2.0f - 1.0f; }
	float topSN() const { return mTopLeft.y() * 2.0f - 1.0f; }
	float rightSN() const { return mBottomRight.x() * 2.0f - 1.0f; }
	float bottomSN() const { return mBottomRight.y() * 2.0f - 1.0f; }

	void setLeft( float f ) { mTopLeft.setX( f ); }
	void setTop( float f ) { mTopLeft.setY( f ); }
	void setRight( float f ) { mBottomRight.setX( f ); }
	void setBottom( float f ) { mBottomRight.setY( f ); }

	void setOrigin( const Vector2f& v ) { mTopLeft = v; }
	void setSize( const Vector2f& v ) { mBottomRight =  mTopLeft + v; }
	void setTopLeft( const Vector2f& v ) { mTopLeft = v; }
	void setTopRight( const Vector2f& v ) { setTop( v.y() ); setRight( v.x() ); }
	void setBottomLeft( const Vector2f& v ) { setBottom( v.y() ); setLeft( v.x() ); }
	void setBottomRight( const Vector2f& v ) { mBottomRight = v; }

	float width() const { return fabs(mBottomRight.x() - mTopLeft.x()); }
	float height() const { return fabs(mBottomRight.y() - mTopLeft.y()); }
	float perimeter() const { return width()*2.0f + height()*2.0f; }
	float area() const { return width() * height(); }
	float ratio() const { return width() / height(); }
	Vector2f origin() const { return mTopLeft; }
	Vector2f size() const { return mBottomRight - mTopLeft; }
	Vector2f centre() const { return calcCentre(); }

	Vector2f centreRight() const { return centre() + Vector2f( width()*0.5f, 0.0f ); }
	Vector2f centreLeft() const { return centre() - Vector2f( width()*0.5f, 0.0f ); }
	Vector2f centreTop() const { return centre() + Vector2f( 0.0f, height()*0.5f ); }
	Vector2f centreBottom() const { return centre() - Vector2f( 0.0f, height()*0.5f ); }

	Rect2f verticalSlice( float delta, float deltan ) const;

	void percentage( const Rect2f _percRect, const Vector2f& _scale );
	static Rect2f percentage( const Rect2f& r1, const Rect2f& r2 );
private:
	// DO NOT Change the order of these member variables as they are needed to get proper indices for raw access (performance requirement)
	Vector2f mTopLeft;
	Vector2f mBottomRight;
};

inline bool isRectCornerFeat( Rect2fFeatureT feature ) {
	return feature == Rect2fFeature::bottomLeft || feature == Rect2fFeature::bottomRight || feature == Rect2fFeature::topLeft || feature == Rect2fFeature::topRight;
}

inline void multipleRectPicking( const Vector2f& vpos, float radius, std::vector<Rect2f>& rects, std::vector<std::pair<uint64_t, JMATH::Rect2fFeatureT> >& ret_rects, bool multipleSelection ) {
	ret_rects.clear();
	std::pair<uint64_t, JMATH::Rect2fFeatureT> currFeat = std::make_pair( -1, JMATH::Rect2fFeature::invalid );
	float dist = 0.0f;
	float mindist = std::numeric_limits<float>::max();
	for ( uint64_t t = 0; t < rects.size(); t++ ) {
		JMATH::Rect2fFeatureT feat = rects[t].isPointNearAFeature( vpos, radius, dist );
		if ( feat != JMATH::Rect2fFeature::invalid ) {
			if ( multipleSelection ) {
				ret_rects.push_back( std::make_pair( t, feat ) );
				continue;
			}
			if ( dist < mindist ) {
				currFeat = std::make_pair( t, feat );
				mindist = dist;
				continue;
			}
		}
	}
	if ( currFeat.second != JMATH::Rect2fFeature::invalid && !multipleSelection ) {
		ret_rects.push_back( currFeat );
	}
}

inline Rect2f getContainingBBox( const std::vector<std::vector<Vector2f> >& points ) {
	ASSERT( points.size() > 0 );
	ASSERT( points[0].size() > 0 );

	Rect2f rect = Rect2f( points[0][0], points[0][0] );

	for ( auto vec : points ) {
		for ( auto point : vec ) {
			rect.expand( point );
		}
	}

	return rect;
}

inline Rect2f getContainingBBox( const std::vector<Vector2f>& points ) {
	ASSERT( points.size() > 0 );

	Rect2f rect = Rect2f( points[0], points[0] );

	for ( auto point : points ) {
		rect.expand( point );
	}

	return rect;
}

inline Rect2f createBBoxFromExpansion( const Vector2f& p1, const Vector2f& p2 ) {
	Rect2f ret = Rect2f::INVALID;
	ret.expand( p1 );
	ret.expand( p2 );
	return ret;
}
}



#endif // D_rect2_H
