//
//  profile.cpp
//  6thViewImporter
//
//  Created by Dado on 13/10/2015.
//
//
#include "profile.hpp"

#include "core/math/matrix4f.h"
#include "core/math/rect2f.h"
#include "core/file_manager.h"
#include "core/serializebin.hpp"
#include "core/math/poly_utils.hpp"
#include "core/service_factory.h"
#define NANOSVG_IMPLEMENTATION	// Expands implementation
#include "nanosvg.h"

void Profile::calculatePPP() {
	switch ( mPPP ) {
	case PivotPointPosition::PPP_CENTER:
	mPivotPoint = Vector2f::ZERO;
	break;
	case PivotPointPosition::PPP_BOTTOM_CENTER:
	mPivotPoint = Vector2f( 0.0f, mBBox.y()*-0.5f );
	break;
	case PivotPointPosition::PPP_TOP_CENTER:
	mPivotPoint = Vector2f( 0.0f, mBBox.y()*0.5f );
	break;
	case PivotPointPosition::PPP_LEFT_CENTER:
	mPivotPoint = Vector2f( -mBBox.x(), 0.0f );
	break;
	case PivotPointPosition::PPP_RIGHT_CENTER:
	mPivotPoint = Vector2f( mBBox.x(), 0.0f );
	break;
	case PivotPointPosition::PPP_BOTTOM_RIGHT:
	mPivotPoint = Vector2f( mBBox.x(), 0.0f );
	break;
	case PivotPointPosition::PPP_BOTTOM_LEFT:
	mPivotPoint = Vector2f( -mBBox.x(), 0.0f );
	break;
	case PivotPointPosition::PPP_TOP_LEFT:
	mPivotPoint = Vector2f( mBBox.x()*-0.5f, mBBox.y()*0.5f );
	break;
	case PivotPointPosition::PPP_TOP_RIGHT:
	mPivotPoint = Vector2f( mBBox.x()*0.5f, mBBox.y()*0.5f );
	break;
	case PivotPointPosition::PPP_CUSTOM:
	mPivotPoint = mCustomPivotPoint;
	break;
	default:
	break;
	}
}

std::vector<Vector3f> Profile::Points3d( const Vector3f & /*mainAxis*/ ) const {
	std::vector<Vector3f> ret;
	for ( auto& p : mPoints ) {
		ret.push_back( Vector3f( p.x(), 0.0f, p.y() ) );
	}
	return ret;
}

void Profile::calculatePerimeter() {
	mPerimeter = 0.0f;
	// Calculate perimeter
	// mLengths have got 1 more entry in the array because they need to calculate the last length to avoid wrapping around zero (form n-1 to 0),
	// and they need to have the first index as 0 length always
	mLengths.push_back( mPerimeter );
	for ( uint64_t t = 0; t < mPoints.size(); t++ ) {
		uint64_t nextIndex = t == mPoints.size() - 1 ? 0 : t + 1;
		mPerimeter += JMATH::distance( mPoints[t], mPoints[nextIndex] );
		mLengths.push_back( mPerimeter );
	}

	ASSERT( mPerimeter > 0.0f );
	// Normalise distances
//	for ( auto&& l : mLenghts ) {
//		l /= mPerimeter;
//	}
}

void Profile::createWire( const float radius, int numSubDivs ) {

	int pSize = (numSubDivs+1) * 3;
	float inc = 360.0f / pSize;
	for ( auto t = 0; t < pSize; t++ ) {
		auto angle = degToRad( t * inc );
		mPoints.push_back( Vector2f{ sin(angle), cos(angle)} * radius);
	}

	calculatePerimeter();
	calculatePPP();
}

void Profile::createLine( const Vector2f& a, const Vector2f& b, PivotPointPosition ppp, const Vector2f& customPivotPoint, WindingOrderT wo ) {
	mPPP = ppp;
	mCustomPivotPoint = customPivotPoint;

	if ( wo == WindingOrder::CCW ) {
		mPoints.push_back( a );
		mPoints.push_back( b );
	} else {
		mPoints.push_back( b );
		mPoints.push_back( a );
	}

	calculatePerimeter();
	calculatePPP();
}

void Profile::createRect( const Vector2f& size, PivotPointPosition ppp, const Vector2f& customPivotPoint, WindingOrderT wo ) {
	mPPP = ppp;
	mCustomPivotPoint = customPivotPoint;

	// Counterclockwise
	if ( wo == WindingOrder::CCW ) {
		mPoints.emplace_back( Vector2f( -size.x() * 0.5f, -size.y() * 0.5f ) );
		mPoints.emplace_back( Vector2f( size.x() * 0.5f, -size.y() * 0.5f ) );
		mPoints.emplace_back( Vector2f( size.x() * 0.5f, size.y() * 0.5f ) );
		mPoints.emplace_back( Vector2f( -size.x() * 0.5f, size.y() * 0.5f ) );
	} else {
		mPoints.emplace_back( Vector2f( -size.x() * 0.5f, size.y() * 0.5f ) );
		mPoints.emplace_back( Vector2f( size.x() * 0.5f, size.y() * 0.5f ) );
		mPoints.emplace_back( Vector2f( size.x() * 0.5f, -size.y() * 0.5f ) );
		mPoints.emplace_back( Vector2f( -size.x() * 0.5f, -size.y() * 0.5f ) );
	}
	// In this case the bbox is the same as the rect's size
	mBBox = size;

	calculatePerimeter();

	calculatePPP();
}

void Profile::createRect( const JMATH::Rect2f& _rect, PivotPointPosition ppp /*= PivotPointPosition::PPP_CENTER*/, const Vector2f& customPivotPoint /*= Vector2f::ZERO*/, WindingOrderT /*wo*/ /*= WindingOrder::CCW */ ) {
	mPPP = ppp;
	mCustomPivotPoint = customPivotPoint;

	mPoints.push_back( _rect.topRight() );
	mPoints.push_back( _rect.topLeft() );
	mPoints.push_back( _rect.bottomLeft() );
	mPoints.push_back( _rect.bottomRight() );
	// In this case the bbox is the same as the rect's size
	mBBox = _rect.size();

	calculatePerimeter();

	calculatePPP();
}

void Profile::createArc( float startAngle, float angle, float radius, float numSegments, PivotPointPosition ppp, const Vector2f& customPivotPoint, WindingOrderT /*wo*/ ) {
	mPPP = ppp;
	mCustomPivotPoint = customPivotPoint;

	for ( int t = 0; t < numSegments; t++ ) {
		float delta = ( ( static_cast<float>( t ) / ( numSegments - 1 ) ) * angle ) + startAngle;
		float cosa = cos( delta )*radius;
		float sina = sin( delta )*radius;
		mPoints.emplace_back( cosa, sina );
	}
	mBBox = { radius*2.0f, radius*2.0f };

	calculatePerimeter();
	calculatePPP();
}

void Profile::createArbitrary( const Vector2f& size, const std::vector<Vector2f>& points, PivotPointPosition ppp, const Vector2f& customPivotPoint ) {
	mPPP = ppp;
	mCustomPivotPoint = customPivotPoint;

	mPoints = points;
	// In this case the bbox is the same as the rect's size
	mBBox = size;

	calculatePerimeter();
	calculatePPP();
}

void Profile::raise( const Vector2f& v ) {
	if ( v == Vector2f::ZERO ) return;

	for ( auto& p : mPoints ) {
		p += v;
	}
	//mBBox += Vector2f( 0.0f, y );
}

void Profile::mirror( const Vector2f& axis ) {
	std::vector<Vector2f> pointsCopy;
	for ( auto& v : mPoints ) {
		if ( axis.x() > axis.y() ) {
			v.setX( mBBox.x() - v.x() );
		} else {
			v.setY( mBBox.y() - v.y() );
		}
		pointsCopy.push_back( v );
	}
	mPoints.clear();
	for ( std::vector<Vector2f>::reverse_iterator p = pointsCopy.rbegin(); p != pointsCopy.rend(); ++p ) {
		mPoints.push_back( *p );
	}
}

void Profile::flip( const Vector2f& axis ) {

	if ( axis == Vector2f::ZERO ) return;

	std::vector<Vector2f> pointsCopy;
	for ( auto& v : mPoints ) {
		if ( axis.x() > axis.y() ) {
			v.setX( -v.x() );
		} else {
			v.setY( -v.y() );
		}
		pointsCopy.push_back( v );
	}
	mPoints.clear();
	for ( std::vector<Vector2f>::reverse_iterator p = pointsCopy.rbegin(); p != pointsCopy.rend(); ++p ) {
		mPoints.push_back( *p );
	}
}

void Profile::invertCCW() {
	std::vector<Vector2f> pointsCopy;
	for ( auto& v : mPoints ) {
		pointsCopy.push_back( v );
	}
	mPoints.clear();
	for ( std::vector<Vector2f>::reverse_iterator p = pointsCopy.rbegin(); p != pointsCopy.rend(); ++p ) {
		mPoints.push_back( *p );
	}
}

void Profile::move( const Vector2f& pos ) {
	for ( auto& v : mPoints ) {
		v += pos;
	}
}

void Profile::centered() {
	move( { width()*-0.5f, height()*-0.5f } );
}

std::vector<Vector3f> Profile::rotatePoints( const Vector3f& nx, const Vector3f& ny, const Vector3f& offset ) const {
	std::vector<Vector3f> retArray;

	for ( auto p : mPoints ) {
		Vector3f pn = (nx * p.x()) + (ny * p.y());
		retArray.push_back( pn + offset );
	}
	return retArray;
}

Profile::Profile( [[maybe_unused]] const std::string& _name, uint8_p&& _data ) {
	std::string svgString( reinterpret_cast<const char*>(_data.first.get()), _data.second );
	if ( !svgString.empty() ) {
		auto prof = std::make_unique<char[]>( svgString.length() );
		std::memcpy( prof.get(), svgString.c_str(), svgString.length() );
		NSVGimage* image = nsvgParse( reinterpret_cast<char *>(prof.get()), "pc", 96 );
		mBBox = { image->width, image->height };
		Rect2f lbbox = Rect2f::INVALID;
		std::vector<Vector2f> rawPoints;
		int subDivs = 3;
		for ( auto shape = image->shapes; shape != NULL; shape = shape->next ) {
			if ( shape->next != nullptr ) continue;
			for ( auto path = shape->paths; path != NULL; path = path->next ) {
				for ( auto i = 0; i < path->npts - 1; i += 3 ) {
					float *p = &path->pts[i * 2];
					for ( int s = 0; s < subDivs + 1; s++ ) {
						float t = s / static_cast<float>(subDivs + 1);
						Vector2f pi = interpolateBezier( Vector2f{ p[0], p[1] }, Vector2f{ p[2], p[3] },
														 Vector2f{ p[4], p[5] }, Vector2f{ p[6], p[7] }, t );
						pi -= Vector2f{ path->bounds[0], path->bounds[1] };
						pi *= 0.01f;
						rawPoints.push_back( pi );
						lbbox.expand( pi );
					}
				}
			}
		}
		rawPoints.pop_back();
		nsvgDelete( image );

		sanitizePath( rawPoints, mPoints, true, 0.0001f * 0.0001f );
		ASSERT( mPoints.size() > 2 );
		auto wo = detectWindingOrder( mPoints[0], mPoints[1], mPoints[2] );
		if ( wo == WindingOrder::CCW ) {
			std::reverse( std::begin(mPoints), std::end(mPoints) );
		}
		for ( auto& p : mPoints ) {
			p.setY( lbbox.height() - p.y());
		}
		mBBox = { lbbox.calcWidth(), lbbox.calcHeight() };
	}
	mbForceWindingOrder = true;
	mForcedWindingOrder = WindingOrder::CW;

	calculatePerimeter();
//	mPPP = pb.ppp;
//	mCustomPivotPoint = pb.customPivotPoint;
	calculatePPP();
}

