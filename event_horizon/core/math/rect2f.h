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

class Rect2f {
public:

	static const Rect2f IDENTITY;
	static const Rect2f INVALID;
	static const Rect2f ZERO;

	static Rect2f& MIDENTITY()  // return reference.
	{
		static Rect2f a( 0.0f, 0.0f, 1.0f, 1.0f );
		return a;
	}

	Rect2f() = default;

	explicit Rect2f( const float _square ) {
		mTopLeft = Vector2f{-_square*0.5f, -_square*0.5f};
		mBottomRight = Vector2f{_square*0.5f, _square*0.5f};
	}

	explicit Rect2f( const Vector2f& topLeft ) {
		mTopLeft = topLeft;
		mBottomRight = Vector2f::ZERO;
	}

	explicit Rect2f( const Vector2f& topLeft, const Vector2f& bottomRight ) {
		mTopLeft = topLeft;
		mBottomRight = bottomRight;
	}

	explicit Rect2f( const Vector2f& topLeft, const Vector2f& bottomRight, bool /*expand*/ ) {
		*this = INVALID;
		expand( topLeft );
		expand( topLeft + bottomRight );
	}

	explicit Rect2f( float left, float top, float right, float bottom ) {
		mTopLeft.set( left, top );
		mBottomRight.set( right, bottom );
	}

	explicit Rect2f( const std::vector<Vector2f>& points ) {
		*this = INVALID;
		for ( auto& p : points ) expand( p );
	}

	explicit Rect2f( const std::initializer_list<Vector2f>& points ) {
		*this = INVALID;
		for ( auto& p : points ) expand( p );
	}

	explicit Rect2f( const std::vector<Vector3f>& points ) {
		*this = INVALID;
		for ( auto& p : points ) expand( p.xy() );
	}

	explicit Rect2f( const Vector3f* points, size_t vsize ) {
		*this = INVALID;
		for ( size_t t = 0; t < vsize; t++ ) expand( points[t].xy() );
	}

	float* rawPtr() {
		return reinterpret_cast<float*>( &mTopLeft[0] );
	}

	Rect2f ss() const {
		Rect2f ret = *this;
		ret.setTopLeft( ret.mTopLeft.ss() );
		ret.setBottomRight( ret.mBottomRight.ss() );

		return ret;
	}

	void set( const Vector2f& topLeft, const Vector2f& bottomRight ) {
		mTopLeft = topLeft;
		mBottomRight = bottomRight;
	}

	void set( float left, float top, float right, float bottom ) {
		mTopLeft.set( left, top );
		mBottomRight.set( right, bottom );
	}

	void setCenterAndSize( const Vector2f& _center, const Vector2f& _size ) {
		setTopLeft( _center );
		setBottomRight( _center );
		expand( _center + _size*0.5f );
		expand( _center + _size*-0.5f );
	}

	bool contains( const Vector2f& pos ) const {
		bool isInside = JMATH::isbetween( pos.x(), mTopLeft.x(), mBottomRight.x() ) && JMATH::isbetween( pos.y(), mTopLeft.y(), mBottomRight.y() );
		return isInside;
	}

	bool contains( float x, float y ) const {
		bool isInside = JMATH::isbetween( x, mTopLeft.x(), mBottomRight.x() ) && JMATH::isbetween( y, mTopLeft.y(), mBottomRight.y() );
		return isInside;
	}

	bool contains( const Rect2f& rect ) const {
		return contains( rect.topLeft() ) && contains( bottomRight() );
	}

	bool intersect( const Rect2f& rect ) const {
		return contains( rect.topLeft() ) || contains( bottomRight() );
	}

	bool isValid() const {
		return !( mTopLeft == Rect2f::INVALID.topLeft() && mBottomRight == Rect2f::INVALID.bottomRight() );
	}

	bool isDegenerated( float epsilon = 0.0001f ) const {
		return isScalarEqual( width(), 0.0F, epsilon ) || isScalarEqual( height(), 0.0F, epsilon );
	}

	bool hasMimimunSize( const Vector2f & _size ) const {
		return ( width() > _size.x() ) && ( height() > _size.y() );
	}

	void expand( const Vector2f& p ) {
		if ( p.x() < mTopLeft.x() ) mTopLeft.setX( p.x() );
		if ( p.x() > mBottomRight.x() ) mBottomRight.setX( p.x() );

		if ( p.y() < mTopLeft.y() ) mTopLeft.setY( p.y() );
		if ( p.y() > mBottomRight.y() ) mBottomRight.setY( p.y() );
	}

	void expand( float x, float y ) {
		expand( Vector2f( x, y ) );
	}

	void merge( const Rect2f& addr ) {
		expand( addr.topLeft() );
		expand( addr.bottomRight() );
	}

	// shrink in units not in percentage
	void shrink( float amount ) {
		mTopLeft += Vector2f( amount );
		mBottomRight -= Vector2f( amount );
	}

	float calcWidth() const {
		return mBottomRight.x() - mTopLeft.x();
	}

	float calcHeight() const {
		return mBottomRight.y() - mTopLeft.y();
	}

	Vector2f calcCentre() const {
		Vector2f centre = ( mTopLeft + mBottomRight ) * 0.5f;
		return centre;
	}

	float dominant() const {
		float w = calcWidth();
		float h = calcHeight();
		
		return ( w > h ) ? w : h;
	}
	
	float aspectRatio() const {
		return width() / height();
	}

	void translate( const Vector2f& offset ) {
		mTopLeft += offset;
		mBottomRight += offset;
	}

	void scaleX( float x ) {
		mTopLeft.setX( mTopLeft.x()*x );
		mBottomRight.setX( mBottomRight.x()*x );
	}

	void scaleY( float x ) {
		mTopLeft.setY( mTopLeft.y()*x );
		mBottomRight.setY( mBottomRight.y()*x );
	}

	void scale( float x ) {
		mTopLeft *= 1.0f / x;
		mBottomRight *= x;
	}

	bool operator==( const Rect2f& rhs ) const {
		if ( bottomLeft() != rhs.bottomLeft() || topRight() != rhs.topRight() ) return false;
		return true;
	}

	bool operator!=( const Rect2f& rhs ) const {
		if ( bottomLeft() == rhs.bottomLeft() && topRight() == rhs.topRight() ) return false;
		return true;
	}

	Rect2f operator*( float rhs ) const {
		return Rect2f( left()*rhs, top()*rhs, right()*rhs, bottom()*rhs );
	}

	void operator*=( float rhs ) {
		mTopLeft *= rhs;
		mBottomRight *= rhs;
	}

	Rect2f operator-( const Vector2f& rhs ) const {
		return Rect2f( mTopLeft - rhs, mBottomRight - rhs );
	}

	Rect2f operator+( const Vector2f& rhs ) const {
		return Rect2f( mTopLeft + rhs, mBottomRight + rhs );
	}

	void operator -=( const Vector2f& rhs ) {
		mTopLeft -= rhs;
		mBottomRight -= rhs;
	}

	void operator +=( const Vector2f& rhs ) {
		mTopLeft += rhs;
		mBottomRight += rhs;
	}

	Rect2f operator*( const Vector2f& rhs ) const {
		Rect2f r = *this;

		r.scaleX( rhs.x() );
		r.scaleY( rhs.y() );

		return r;
	}

	std::vector<Rect2f> booleanDifference( const Rect2f& diffRect ) const;
	bool lineIntersection( const Vector2f& p1, const Vector2f& p2 ) const;

	void floor() {
		mTopLeft.setX(std::floor(mTopLeft.x()));
		mTopLeft.setY(std::floor(mTopLeft.y()));

		mBottomRight.setX(std::floor(mBottomRight.x()));
		mBottomRight.setY(std::floor(mBottomRight.y()));
	}

	void clamp( float minX, float minY, float maxX, float maxY ) {
		if ( mTopLeft.x() < minX ) mTopLeft.setX( minX );
		if ( mTopLeft.y() < minY ) mTopLeft.setY( minY );
		if ( mBottomRight.x() > maxX ) mBottomRight.setX( maxX );
		if ( mBottomRight.y() > maxY ) mBottomRight.setY( maxY );
	}

	void scaleWithClamp( float x, float minX, float minY, float maxX, float maxY ) {
		float increaseX = ( width() * ( x - 1.0f ) );
		float increaseY = ( height() * ( x - 1.0f ) );

		float increase = increaseX > increaseY ? increaseY : increaseX;

		mTopLeft.setX( mTopLeft.x() - increase );
		mTopLeft.setY( mTopLeft.y() - increase );

		mBottomRight.setX( mBottomRight.x() + increase );
		mBottomRight.setY( mBottomRight.y() + increase );

		clamp( minX, minY, maxX, maxY );
	}

	void cropOnMainAxis( const Vector2f& cropPerc ) {
		ASSERT( cropPerc.x() <= 1.0f && cropPerc.y() <= 1.0f );
		Vector2f crop = Vector2f::ONE;

		if ( width() > height() ) {
			crop = Vector2f( ( width() * cropPerc.x() ) * 0.5f, ( height() * cropPerc.y() ) * 0.5f );
		} else {
			crop = Vector2f( ( width() * cropPerc.y() ) * 0.5f, ( height() * cropPerc.x() ) * 0.5f );
		}

		mTopLeft += crop;
		mBottomRight -= crop;
	}

	void scaleWithClampOnMainAxes( float mainA, float secA, float minX, float minY, float maxX, float maxY ) {
		float increaseX;
		float increaseY;
		float ma = mainA - 1.0f;
		float sa = secA - 1.0f;
		
		if ( width() > height() ) {
			increaseY = ( height() * std::abs( sa ) ) * JMATH::sign( sa );
			increaseX = ( width() * std::abs( ma ) ) * JMATH::sign( ma );
		} else {
			increaseY = ( height() * std::abs( ma) ) * JMATH::sign( ma );
			increaseX = ( width() * std::abs( sa ) ) * JMATH::sign( sa );
		}

		mTopLeft.setX( mTopLeft.x() - increaseX );
		mTopLeft.setY( mTopLeft.y() - increaseY );

		mBottomRight.setX( mBottomRight.x() + increaseX );
		mBottomRight.setY( mBottomRight.y() + increaseY );

		clamp( minX, minY, maxX, maxY );
	}

	// This return a vector2f normalized [0...1] against the rect
	// optionally it will scale it by the vscale vector
	Vector2f normalizeWithinRect( const Vector2f& v, const Vector2f& vscale = Vector2f::ONE ) const {
		Vector2f offCenter = v - topLeft();
		return  absolute( ( offCenter / Vector2f( width(), height() ) ) * vscale );
	}

	Vector2f calculateBestFitSize( const Vector2f& fsize ) const {
		Vector2f ret = fsize;
		if ( ratio() > 1.0f ) {
			ret.setY( ret.y() / ratio() );
		} else {
			ret.setX( ret.x() * ratio() );
		}
		return ret;
	}

	void setFeature( JMATH::Rect2fFeatureT feature, const Vector2f& val ) {
		switch ( feature ) {
		case JMATH::Rect2fFeature::left:
		setLeft( val.x() );
		break;
		case JMATH::Rect2fFeature::right:
		setRight( val.x() );
		break;
		case JMATH::Rect2fFeature::top:
		setTop( val.y() );
		break;
		case JMATH::Rect2fFeature::bottom:
		setBottom( val.y() );
		break;
		case JMATH::Rect2fFeature::topLeft:
		setTopLeft( val );
		break;
		case JMATH::Rect2fFeature::topRight:
		setTopRight( val );
		break;
		case JMATH::Rect2fFeature::bottomLeft:
		setBottomLeft( val );
		break;
		case JMATH::Rect2fFeature::bottomRight:
		setBottomRight( val );
		break;
		default:
		break;
		}
	}

	void addFeatureDelta( JMATH::Rect2fFeatureT feature, const Vector2f& val ) {
		switch ( feature ) {
		case JMATH::Rect2fFeature::left:
		setLeft( left() + val.x() );
		break;
		case JMATH::Rect2fFeature::right:
		setRight( right() + val.x() );
		break;
		case JMATH::Rect2fFeature::top:
		setTop( top() + val.y() );
		break;
		case JMATH::Rect2fFeature::bottom:
		setBottom( bottom() + val.y() );
		break;
		case JMATH::Rect2fFeature::topLeft:
		setTopLeft( topLeft() + val );
		break;
		case JMATH::Rect2fFeature::topRight:
		setTopRight( topRight() + val );
		break;
		case JMATH::Rect2fFeature::bottomLeft:
		setBottomLeft( bottomLeft() + val );
		break;
		case JMATH::Rect2fFeature::bottomRight:
		setBottomRight( bottomRight() + val );
		break;
		default:
		break;
		}
	}

	Vector2f coordOfFeature( JMATH::Rect2fFeatureT feature ) {
		switch ( feature ) {
		case JMATH::Rect2fFeature::topLeft:
		return topLeft();
		case JMATH::Rect2fFeature::topRight:
		return topRight();
		case JMATH::Rect2fFeature::bottomLeft:
		return bottomLeft();
		case JMATH::Rect2fFeature::bottomRight:
		return bottomRight();
		default:
		return Vector2f::ZERO;
		}
		return Vector2f::ZERO;
	}

	JMATH::Rect2fFeatureT isPointNearAFeature( const Vector2f& pos, float minDist, float& ret_distance ) {
		if ( ( ret_distance = JMATH::distance( pos, topLeft() ) ) < minDist ) return JMATH::Rect2fFeature::topLeft;
		if ( ( ret_distance = JMATH::distance( pos, topRight() ) ) < minDist ) return JMATH::Rect2fFeature::topRight;
		if ( ( ret_distance = JMATH::distance( pos, bottomLeft() ) ) < minDist ) return JMATH::Rect2fFeature::bottomLeft;
		if ( ( ret_distance = JMATH::distance( pos, bottomRight() ) ) < minDist ) return JMATH::Rect2fFeature::bottomRight;

		float distance1 = 0.0f;
		float distance2 = 0.0f;
		if ( JMATH::distance( pos.x(), left() ) < minDist ) {
			distance1 = JMATH::distance( pos, topLeft() );
			distance2 = JMATH::distance( pos, bottomLeft() );
			ret_distance = min( distance1, distance2 );
			return JMATH::Rect2fFeature::left;
		}
		if ( JMATH::distance( pos.x(), right() ) < minDist ) {
			distance1 = JMATH::distance( pos, bottomRight() );
			distance2 = JMATH::distance( pos, topRight() );
			ret_distance = min( distance1, distance2 );
			return JMATH::Rect2fFeature::right;
		}
		if ( JMATH::distance( pos.y(), top() ) < minDist ) {
			distance1 = JMATH::distance( pos, topLeft() );
			distance2 = JMATH::distance( pos, topRight() );
			ret_distance = min( distance1, distance2 );
			return JMATH::Rect2fFeature::top;
		}
		if ( JMATH::distance( pos.y(), bottom() ) < minDist ) {
			distance1 = JMATH::distance( pos, bottomLeft() );
			distance2 = JMATH::distance( pos, bottomRight() );
			ret_distance = min( distance1, distance2 );
			return JMATH::Rect2fFeature::bottom;
		}

		return JMATH::Rect2fFeature::invalid;
	}

	std::vector<Vector2f> points() const {
		std::vector<Vector2f> ret;

		ret.push_back( bottomLeft() );
		ret.push_back( bottomRight() );
		ret.push_back( topRight() );
		ret.push_back( topLeft() );

		return ret;
	}

	std::vector<Vector2f> pointscw() const {
		std::vector<Vector2f> ret;

		ret.push_back( topLeft() );
		ret.push_back( topRight() );
		ret.push_back( bottomRight() );
		ret.push_back( bottomLeft() );

		return ret;
	}

	std::vector<Vector3f> points3d( float z ) const {
		std::vector<Vector3f> ret;

		ret.emplace_back( bottomLeft(), z );
		ret.emplace_back( bottomRight(), z );
		ret.emplace_back( topRight(), z );
		ret.emplace_back( topLeft(), z );

		return ret;
	}

	std::vector<Vector3f> points3dcw( float z ) const {
		std::vector<Vector3f> ret;

		ret.emplace_back( topLeft(), z );
		ret.emplace_back( topRight(), z );
		ret.emplace_back( bottomRight(), z );
		ret.emplace_back( bottomLeft(), z );

		return ret;
	}

	inline Rect2f squared() const {
		Rect2f ret = *this;
		if ( width() < height() ) {
			ret.expand( centre() - height()*0.5f );
			ret.expand( centre() + height()*0.5f );
		} else {
			ret.expand( centre() - width()*0.5f );
			ret.expand( centre() + width()*0.5f );
		}
		return ret;
	}

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
	void setTopLeft( const Vector2f& v ) { mTopLeft = v; }
	void setTopRight( const Vector2f& v ) { setTop( v.y() ); setRight( v.x() ); }
	void setBottomLeft( const Vector2f& v ) { setBottom( v.y() ); setLeft( v.x() ); }
	void setBottomRight( const Vector2f& v ) { mBottomRight = v; }

	float width() const { return mBottomRight.x() - mTopLeft.x(); }
	float height() const { return mBottomRight.y() - mTopLeft.y(); }
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
