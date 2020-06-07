#include "rect2f.h"

const JMATH::Rect2f JMATH::Rect2f::IDENTITY = Rect2f( 0.0f, 0.0f, 1.0f, 1.0f );
const JMATH::Rect2f JMATH::Rect2f::IDENTITY_CENTERED = Rect2f( -0.5f, -0.5f, 0.5f, 0.5f );
const JMATH::Rect2f JMATH::Rect2f::ZERO = Rect2f( 0.0f, 0.0f, 0.0f, 0.0f );
const JMATH::Rect2f JMATH::Rect2f::INVALID = Rect2f( std::numeric_limits<float>::max(),
													 std::numeric_limits<float>::max(),
													 std::numeric_limits<float>::lowest(),
													 std::numeric_limits<float>::lowest() );

namespace JMATH {
    std::ostream& operator<<( std::ostream& os, const JMATH::Rect2f& f ) {
        os << "Min: " << f.topLeft() << " Max: " << f.bottomRight();
        return os;
    }
}

// *********************************************************************************************************************
// Constructors
// *********************************************************************************************************************

Rect2f::Rect2f( const int _square ) {
    mTopLeft = Vector2f{-_square*0.5f, -_square*0.5f};
    mBottomRight = Vector2f{_square*0.5f, _square*0.5f};
}
Rect2f::Rect2f( const float _square ) {
    mTopLeft = Vector2f{-_square*0.5f, -_square*0.5f};
    mBottomRight = Vector2f{_square*0.5f, _square*0.5f};
}
Rect2f::Rect2f( const Vector2f& topLeft ) {
    mTopLeft = topLeft;
    mBottomRight = V2fc::ZERO;
}
Rect2f::Rect2f( const Vector2f& _center, RectV2f rt ) { // = RectV2f::Centered
    centered( _center );
}
Rect2f::Rect2f( const Vector2f& topLeft, const Vector2f& bottomRight ) {
    mTopLeft = topLeft;
    mBottomRight = bottomRight;
}
Rect2f::Rect2f( const Vector2f& topLeft, const Vector2f& bottomRight, bool ) {
    invalidate();
    expand( topLeft );
    expand( topLeft + bottomRight );
}
Rect2f::Rect2f( float left, float top, float right, float bottom ) {
    mTopLeft.set( left, top );
    mBottomRight.set( right, bottom );
}
Rect2f::Rect2f( const V2fVector& points ) {
    invalidate();
    for ( const auto& p : points ) expand( p );
}
Rect2f::Rect2f( const V2fVectorOfVector& points ) {
    invalidate();
    for ( const auto& pl1 : points ) {
        for ( const auto& p : pl1 ) {
            expand( p );
        }
    }
}
Rect2f::Rect2f( const std::initializer_list<Vector2f>& points ) {
    invalidate();
    for ( auto& p : points ) expand( p );
}
Rect2f::Rect2f( const std::vector<Vector3f>& points ) {
    invalidate();
    for ( auto& p : points ) expand( p.xy() );
}
Rect2f::Rect2f( const Vector3f *points, size_t vsize ) {
    invalidate();
    for ( size_t t = 0; t < vsize; t++ ) expand( points[t].xy() );
}

// *********************************************************************************************************************
// Operators
// *********************************************************************************************************************

bool Rect2f::operator==( const Rect2f& rhs ) const {
    return !( bottomLeft() != rhs.bottomLeft() || topRight() != rhs.topRight());
}
bool Rect2f::operator!=( const Rect2f& rhs ) const {
    return !( bottomLeft() == rhs.bottomLeft() && topRight() == rhs.topRight());
}
float Rect2f::operator[]( const unsigned int rhs ) const {
    ASSERT(rhs < 4 );
    if ( rhs == 0 ) return left();
    if ( rhs == 1 ) return right();
    if ( rhs == 2 ) return top();
    if ( rhs == 3 ) return bottom();
    return left();
}
Rect2f Rect2f::operator*( float rhs ) const {
    return Rect2f( left()*rhs, top()*rhs, right()*rhs, bottom()*rhs );
}
void Rect2f::operator*=( float rhs ) {
    mTopLeft *= rhs;
    mBottomRight *= rhs;
}
Rect2f Rect2f::operator-( const Vector2f& rhs ) const {
    return Rect2f( mTopLeft - rhs, mBottomRight - rhs );
}
Rect2f Rect2f::operator+( const Vector2f& rhs ) const {
    return Rect2f( mTopLeft + rhs, mBottomRight + rhs );
}
void Rect2f::operator-=( const Vector2f& rhs ) {
    mTopLeft -= rhs;
    mBottomRight -= rhs;
}
void Rect2f::operator+=( const Vector2f& rhs ) {
    mTopLeft += rhs;
    mBottomRight += rhs;
}
Rect2f Rect2f::operator*( const Vector2f& rhs ) const {
    Rect2f r = *this;

    r.scaleX( rhs.x() );
    r.scaleY( rhs.y() );

    return r;
}


void poly_edge_clip( std::vector<Vector2f>& sub, Vector2f& x0, Vector2f& x1, int left, std::vector<Vector2f>& res ) {
	size_t i;
	int side0, side1;
	Vector2f tmp;
	Vector2f v0 = sub[sub.size() - 1];
	Vector2f v1;
	res.clear();

	side0 = (int)sideOfLine( x0, x1, v0 );
	if ( side0 != -left ) res.push_back( v0 );

	for ( i = 0; i < sub.size(); i++ ) {
		v1 = sub[i];
		side1 = (int)sideOfLine( x0, x1, v1 );
		if ( side0 + side1 == 0 && side0 )
			/* last point and current straddle the edge */
			if ( intersection( x0, x1, v0, v1, tmp ) )
				res.push_back( tmp );
		if ( i == sub.size() - 1 ) break;
		if ( side1 != -left ) res.push_back( v1 );
		v0 = v1;
		side0 = side1;
	}
}

std::vector<Rect2f> Rect2f::booleanDifference( const Rect2f& diffRect ) const {
	std::vector<Rect2f> ret;

	std::vector<Vector2f> clipper;
	std::vector<Vector2f> toClipAgainst;
	std::vector<Vector2f> p1;
	std::vector<Vector2f> p2;
	std::vector<Vector2f> tmp;

	clipper.push_back( topLeft() );
	clipper.push_back( topRight() );
	clipper.push_back( bottomRight() );
	clipper.push_back( bottomLeft() );

	toClipAgainst.push_back( diffRect.topLeft() );
	toClipAgainst.push_back( diffRect.topRight() );
	toClipAgainst.push_back( diffRect.bottomRight() );
	toClipAgainst.push_back( diffRect.bottomLeft() );

	int dir = winding( clipper[0], clipper[1], clipper[2] );
	poly_edge_clip( toClipAgainst, clipper[clipper.size() - 1], clipper[0], dir, p2 );
	for ( size_t i = 0; i < clipper.size() - 1; i++ ) {
		tmp = p2; p2 = p1; p1 = tmp;
		if ( p1.size() == 0 ) {
			p2.clear();
			break;
		}
		poly_edge_clip( p1, clipper[i], clipper[i + 1], dir, p2 );
	}

	ASSERT( p2.size() == 4 );
	Rect2f newRect = Rect2f::INVALID;
	for ( auto& v : p2 ) newRect.expand( v );

	ret.push_back( Rect2f( topLeft(), Vector2f( newRect.left(), newRect.bottom() ) ) );
	ret.push_back( Rect2f( Vector2f( newRect.left(), top() ), Vector2f( right(), newRect.top() ) ) );
	ret.push_back( Rect2f( newRect.topRight(), bottomRight() ) );
	ret.push_back( Rect2f( Vector2f( left(), newRect.bottom() ), Vector2f( newRect.right(), bottom() ) ) );

	return ret;
}

bool Rect2f::lineIntersection( const Vector2f& p1, const Vector2f& p2 ) const {
	float minX = p1.x();
	float maxX = p2.x();

	if ( p1.x() > p2.x() ) {
		minX = p2.x();
		maxX = p1.x();
	}

	if ( maxX > left() + width() )
		maxX = left() + width();

	if ( minX < left() )
		minX = left();

	if ( minX > maxX )
		return false;

	float minY = p1.y();
	float maxY = p2.y();

	float dx = p2.x() - p1.x();

	if ( fabs( dx ) > 0.0000001f ) {
		float a = ( p2.y() - p1.y() ) / dx;
		float b = p1.y() - a * p1.x();
		minY = a * minX + b;
		maxY = a * maxX + b;
	}

	if ( minY > maxY ) {
		float tmp = maxY;
		maxY = minY;
		minY = tmp;
	}

	if ( maxY > top() + height() )
		maxY = top() + height();

	if ( minY < top() )
		minY = top();

	if ( minY > maxY )
		return false;

	return true;
}

Rect2f Rect2f::verticalSlice( float delta, float deltan ) const {
	std::vector<Vector2f> vl;
	vl.reserve(4);
	vl.emplace_back(lerp( delta, bottomLeft(), bottomRight() ));
	vl.emplace_back(lerp( delta, topLeft(), topRight() ));
	vl.emplace_back(lerp( deltan, bottomLeft(), bottomRight() ));
	vl.emplace_back(lerp( deltan, topLeft(), topRight() ));

	return Rect2f( vl );
}

Rect2f Rect2f::percentage( const Rect2f& r1, const Rect2f& r2 ) {
    Rect2f ret{};
    ret.setLeft( r1.left() / r2.size().x() );
    ret.setRight( r1.right() / r2.size().x() );
    ret.setTop( r1.top() / r2.size().y() );
    ret.setBottom( r1.bottom() / r2.size().y() );
    return ret;
}

void Rect2f::percentage( const Rect2f _percRect, const Vector2f& _scale ) {
	setLeft( _percRect.left() * _scale.x() );
	setRight( _percRect.right() * _scale.x() );
	setTop( _percRect.top() * _scale.y() );
	setBottom( _percRect.bottom() * _scale.y() );
}

void Rect2f::centered( const V2f& _size ) {
    mTopLeft = V2f{-_size.x()*0.5f, -_size.y()*0.5f};
    mBottomRight = V2f{_size.x()*0.5f, _size.y()*0.5f};
}

[[nodiscard]] Rect2f Rect2f::oneMinusY() const {
    Rect2f ret{Rect2f::INVALID};

    ret.expand( ::oneMinusY(topLeft()) );
    ret.expand( ::oneMinusY(bottomRight()) );

    return ret;
}

void Rect2f::centerAroundOrigin() {
    centered( size() );
}

bool Rect2f::intersect( const Rect2f& rect, float _epsilon, EdgeTouchingIsIntersecting _eti ) const {
    auto ac = centre();
    auto bc = rect.centre();
    if ( _eti == EdgeTouchingIsIntersecting::Yes ) {
        return (abs(ac.x() - bc.x()) * (2.0f+_epsilon) <= (width() + rect.width())) &&
               (abs(ac.y() - bc.y()) * (2.0f+_epsilon) <= (height() + rect.height()));
    }
    return (abs(ac.x() - bc.x()) * (2.0f+_epsilon) < (width() + rect.width())) &&
           (abs(ac.y() - bc.y()) * (2.0f+_epsilon) < (height() + rect.height()));
}

Rect2f Rect2f::squared() const {
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


Rect2f Rect2f::squaredBothSides() const {
    Rect2f ret = *this;
    if ( width() < height() ) {
        ret.expand( centre() - V2fc::X_AXIS*height() );
        ret.expand( centre() + V2fc::X_AXIS*height() );
    } else {
        ret.expand( centre() - V2fc::Y_AXIS*width() );
        ret.expand( centre() + V2fc::Y_AXIS*width() );
    }
    return ret;
}

float *Rect2f::rawPtr() {
    return reinterpret_cast<float*>( &mTopLeft[0] );
}

void Rect2f::invalidate() {
    mTopLeft = V2f{std::numeric_limits<float>::max()};
    mBottomRight = V2f{std::numeric_limits<float>::lowest()};
}
Rect2f Rect2f::ss() const {
    Rect2f ret = *this;
    ret.setTopLeft( ret.mTopLeft.ss() );
    ret.setBottomRight( ret.mBottomRight.ss() );

    return ret;
}
void Rect2f::set( const Vector2f& topLeft, const Vector2f& bottomRight ) {
    mTopLeft = topLeft;
    mBottomRight = bottomRight;
}
void Rect2f::set( float left, float top, float right, float bottom ) {
    mTopLeft.set( left, top );
    mBottomRight.set( right, bottom );
}
void Rect2f::setCenterAndSize( const Vector2f& _center, const Vector2f& _size ) {
    setTopLeft( _center );
    setBottomRight( _center );
    expand( _center + _size*0.5f );
    expand( _center + _size*-0.5f );
}

// *********************************************************************************************************************
// Checks
// *********************************************************************************************************************

bool Rect2f::contains( const Vector2f& pos ) const {
    bool isInside = JMATH::isbetween( pos.x(), mTopLeft.x(), mBottomRight.x() ) && JMATH::isbetween( pos.y(), mTopLeft.y(), mBottomRight.y() );
    return isInside;
}
bool Rect2f::containsEqual( const Vector2f& pos ) const {
    bool isInside = JMATH::isbetweenEqual( pos.x(), mTopLeft.x(), mBottomRight.x() ) && JMATH::isbetweenEqual( pos.y(), mTopLeft.y(), mBottomRight.y() );
    return isInside;
}
bool Rect2f::contains( float x, float y ) const {
    bool isInside = JMATH::isbetween( x, mTopLeft.x(), mBottomRight.x() ) && JMATH::isbetween( y, mTopLeft.y(), mBottomRight.y() );
    return isInside;
}
bool Rect2f::containsEqual( float x, float y ) const {
    bool isInside = JMATH::isbetweenEqual( x, mTopLeft.x(), mBottomRight.x() ) && JMATH::isbetweenEqual( y, mTopLeft.y(), mBottomRight.y() );
    return isInside;
}
bool Rect2f::contains( const Rect2f& rect ) const {
    return contains( rect.topLeft() ) && contains( bottomRight() );
}
bool Rect2f::isValid() const {
    return !( mTopLeft == Rect2f::INVALID.topLeft() && mBottomRight == Rect2f::INVALID.bottomRight() );
}
bool Rect2f::isDegenerated( float epsilon ) const {
    return isScalarEqual( width(), 0.0F, epsilon ) || isScalarEqual( height(), 0.0F, epsilon );
}
bool Rect2f::hasMimimunSize( const Vector2f& _size ) const {
    return ( width() > _size.x() ) && ( height() > _size.y() );
}

float Rect2f::calcWidth() const {
    return mBottomRight.x() - mTopLeft.x();
}

float Rect2f::calcHeight() const {
    return mBottomRight.y() - mTopLeft.y();
}

Vector2f Rect2f::calcCentre() const {
    Vector2f centre = ( mTopLeft + mBottomRight ) * 0.5f;
    return centre;
}

float Rect2f::dominant() const {
    float w = calcWidth();
    float h = calcHeight();

    return ( w > h ) ? w : h;
}

float Rect2f::aspectRatio() const {
    return width() / height();
}

// *********************************************************************************************************************
// Transforms
// *********************************************************************************************************************

void Rect2f::expand( const Vector2f& p ) {
    if ( p.x() < mTopLeft.x() ) mTopLeft.setX( p.x() );
    if ( p.x() > mBottomRight.x() ) mBottomRight.setX( p.x() );

    if ( p.y() < mTopLeft.y() ) mTopLeft.setY( p.y() );
    if ( p.y() > mBottomRight.y() ) mBottomRight.setY( p.y() );
}
void Rect2f::expand( float x, float y ) {
    expand( Vector2f( x, y ) );
}
void Rect2f::merge( const Rect2f& addr ) {
    if ( addr.isValid() ) {
        expand( addr.topLeft() );
        expand( addr.bottomRight() );
    }
}
void Rect2f::translate( const Vector2f& offset ) {
    mTopLeft += offset;
    mBottomRight += offset;
}

// shrink in units not in percentage
void Rect2f::shrink( float amount ) {
    mTopLeft += Vector2f( amount );
    mBottomRight -= Vector2f( amount );
}

void Rect2f::scaleX( float x ) {
    mTopLeft.setX( mTopLeft.x()*x );
    mBottomRight.setX( mBottomRight.x()*x );
}

void Rect2f::scaleY( float x ) {
    mTopLeft.setY( mTopLeft.y()*x );
    mBottomRight.setY( mBottomRight.y()*x );
}

void Rect2f::scale( float x ) {
    mTopLeft *= 1.0f / x;
    mBottomRight *= x;
}

void Rect2f::floor() {
    mTopLeft.setX(std::floor(mTopLeft.x()));
    mTopLeft.setY(std::floor(mTopLeft.y()));

    mBottomRight.setX(std::floor(mBottomRight.x()));
    mBottomRight.setY(std::floor(mBottomRight.y()));
}

void Rect2f::clamp( float minX, float minY, float maxX, float maxY ) {
    if ( mTopLeft.x() < minX ) mTopLeft.setX( minX );
    if ( mTopLeft.y() < minY ) mTopLeft.setY( minY );
    if ( mBottomRight.x() > maxX ) mBottomRight.setX( maxX );
    if ( mBottomRight.y() > maxY ) mBottomRight.setY( maxY );
}

void Rect2f::scaleWithClamp( float x, float minX, float minY, float maxX, float maxY ) {
    float increaseX = ( width() * ( x - 1.0f ) );
    float increaseY = ( height() * ( x - 1.0f ) );

    float increase = increaseX > increaseY ? increaseY : increaseX;

    mTopLeft.setX( mTopLeft.x() - increase );
    mTopLeft.setY( mTopLeft.y() - increase );

    mBottomRight.setX( mBottomRight.x() + increase );
    mBottomRight.setY( mBottomRight.y() + increase );

    clamp( minX, minY, maxX, maxY );
}

void Rect2f::cropOnMainAxis( const Vector2f& cropPerc ) {
    ASSERT( cropPerc.x() <= 1.0f && cropPerc.y() <= 1.0f );
    Vector2f crop = V2fc::ONE;

    if ( width() > height() ) {
        crop = Vector2f( ( width() * cropPerc.x() ) * 0.5f, ( height() * cropPerc.y() ) * 0.5f );
    } else {
        crop = Vector2f( ( width() * cropPerc.y() ) * 0.5f, ( height() * cropPerc.x() ) * 0.5f );
    }

    mTopLeft += crop;
    mBottomRight -= crop;
}

void Rect2f::scaleWithClampOnMainAxes( float mainA, float secA, float minX, float minY, float maxX, float maxY ) {
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

Vector2f Rect2f::normalizeWithinRect( const Vector2f& v, const Vector2f& vscale ) const {
    Vector2f offCenter = v - topLeft();
    return  absolute( ( offCenter / Vector2f( width(), height() ) ) * vscale );
}

Vector2f Rect2f::calculateBestFitSize( const Vector2f& fsize ) const {
    Vector2f ret = fsize;
    if ( ratio() > 1.0f ) {
        ret.setY( ret.y() / ratio() );
    } else {
        ret.setX( ret.x() * ratio() );
    }
    return ret;
}

// *********************************************************************************************************************
// Features
// *********************************************************************************************************************

void Rect2f::setFeature( JMATH::Rect2fFeatureT feature, const Vector2f& val ) {
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

void Rect2f::addFeatureDelta( JMATH::Rect2fFeatureT feature, const Vector2f& val ) {
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

Vector2f Rect2f::coordOfFeature( JMATH::Rect2fFeatureT feature ) {
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
            return V2fc::ZERO;
    }
    return V2fc::ZERO;
}

JMATH::Rect2fFeatureT Rect2f::isPointNearAFeature( const Vector2f& pos, float minDist, float& ret_distance ) {
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

// *********************************************************************************************************************
// Points
// *********************************************************************************************************************

std::vector<Vector2f> Rect2f::points() const {
    std::vector<Vector2f> ret;

    ret.push_back( bottomLeft() );
    ret.push_back( bottomRight() );
    ret.push_back( topRight() );
    ret.push_back( topLeft() );

    return ret;
}

V2fVector Rect2f::pointsStrip() const {
    V2fVector ret;

    ret.push_back( bottomRight());
    ret.push_back( topRight());
    ret.push_back( bottomLeft());
    ret.push_back( topLeft());

    return ret;
}

std::vector<Vector2f> Rect2f::pointscw() const {
    std::vector<Vector2f> ret;

    ret.push_back( topLeft() );
    ret.push_back( topRight() );
    ret.push_back( bottomRight() );
    ret.push_back( bottomLeft() );

    return ret;
}

std::vector<Vector3f> Rect2f::points3d( float z ) const {
    std::vector<Vector3f> ret;

    ret.emplace_back( bottomLeft(), z );
    ret.emplace_back( bottomRight(), z );
    ret.emplace_back( topRight(), z );
    ret.emplace_back( topLeft(), z );

    return ret;
}

std::vector<Vector3f> Rect2f::points3d_xzy() const {
    return XZY::C(points());
}

std::vector<Vector3f> Rect2f::points3dcw_xzy() const {
    return XZY::C(pointscw());
}

std::vector<Vector3f> Rect2f::points3dcw( float z ) const {
    std::vector<Vector3f> ret;

    ret.emplace_back( topLeft(), z );
    ret.emplace_back( topRight(), z );
    ret.emplace_back( bottomRight(), z );
    ret.emplace_back( bottomLeft(), z );

    return ret;
}
