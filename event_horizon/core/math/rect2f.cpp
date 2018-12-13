#ifdef UE4
#include "SixthViewer.h"
#endif
#include "rect2f.h"



const JMATH::Rect2f JMATH::Rect2f::IDENTITY = Rect2f( 0.0f, 0.0f, 1.0f, 1.0f );
const JMATH::Rect2f JMATH::Rect2f::ZERO = Rect2f( 0.0f, 0.0f, 0.0f, 0.0f );
const JMATH::Rect2f JMATH::Rect2f::INVALID = Rect2f( std::numeric_limits<float>::max(),
													 std::numeric_limits<float>::max(),
													 std::numeric_limits<float>::lowest(),
													 std::numeric_limits<float>::lowest() );

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
