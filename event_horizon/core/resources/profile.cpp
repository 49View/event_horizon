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
#include "poly/nanosvg.h"

void Profile::bufferDecode( const unsigned char* _buffer, size_t _length ) {
    std::string svgString( reinterpret_cast<const char*>(_buffer), _length );
    if ( !svgString.empty() ) {
        auto prof = std::make_unique<char[]>( svgString.length() );
        std::memcpy( prof.get(), svgString.c_str(), svgString.length() );
        NSVGimage* image = nsvgParse( reinterpret_cast<char *>(prof.get()), "pc", 96 );
        mBBox = { image->width, image->height };
        Rect2f lbbox = Rect2f::INVALID;
        Rect2f lTotalbbox = Rect2f::INVALID;
        std::vector<Vector2f> rawPoints;
        // #### NDDADO: we fix subDivs=0 because it's dangerous to interpolate bezier paths when you need total accuracy
        // on connecting profiles with straight elements (IE think about a flat wall)
        int subDivs = 4;
        for ( auto shape = image->shapes; shape != NULL; shape = shape->next ) {
            for ( auto path = shape->paths; path != NULL; path = path->next ) {
                V2fVector lPath{};
                for ( auto i = 0; i < path->npts - 1; i += 3 ) {
                    float *p = &path->pts[i * 2];
                    int extraPoint = path->npts == 4 ? 1 : 0; // if it's a line or single path then close it, adding a trailing end cp
                    int totalSubDivs = subDivs + 1 + extraPoint;
                    for ( int s = 0; s < totalSubDivs; s++ ) {
                        float t = s / static_cast<float>(totalSubDivs - extraPoint);
                        Vector2f pi = interpolateBezier( V2f{ p[0], p[1] }, V2f{ p[2], p[3] },
                                                         V2f{ p[4], p[5] }, V2f{ p[6], p[7] }, t );
                        lPath.emplace_back(pi);
                        lTotalbbox.expand(pi);
                        // NDDADO in case of profile path just add the last one, we should clearly change it!
                    }
                    if ( shape->next == nullptr ) {
                        V2f pi{ p[0], p[1] };
                        pi -= Vector2f{ path->bounds[0], path->bounds[1] };
                        pi *= 0.01f;
                        rawPoints.push_back( pi );
                        lbbox.expand( pi );
                    }
                }
                if ( path->closed && path->npts % 4 != 0 ) lPath.push_back(lPath.front());
                mPaths.push_back( lPath );
            }
        }
        rawPoints.pop_back();
        nsvgDelete( image );

        for ( auto& ps : mPaths ) {
            for ( auto& pp : ps ) {
                pp -= lTotalbbox.centre();
                pp /= lTotalbbox.size();
            }
        }
        mTotalBBox = lTotalbbox.size();

        mPoints = sanitizePath( rawPoints, true, 0.0001f * 0.0001f );
//        ASSERT( mPoints.size() > 2 );
        mPoints = forceWindingOrder( mPoints, WindingOrder::CCW );
        for ( auto& p : mPoints ) {
            p.setY( lbbox.height() - p.y());
        }
        mBBox = { lbbox.calcWidth(), lbbox.calcHeight() };
    }

    calculatePerimeter();
}

std::vector<Vector3f> Profile::Points3d( const Vector3f & /*mainAxis*/ ) const {
	std::vector<Vector3f> ret;
	for ( auto& p : mPoints ) {
		ret.emplace_back( p.x(), 0.0f, p.y());
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

//	ASSERT( mPerimeter > 0.0f );
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
}

void Profile::createLine( const Vector2f& a, const Vector2f& b, WindingOrderT wo ) {

	if ( wo == WindingOrder::CCW ) {
		mPoints.push_back( a );
		mPoints.push_back( b );
	} else {
		mPoints.push_back( b );
		mPoints.push_back( a );
	}

	calculatePerimeter();
}

void Profile::createRect( const Vector2f& size, WindingOrderT wo ) {

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
}

void Profile::createRect( const JMATH::Rect2f& _rect ) {

	mPoints.push_back( _rect.topRight() );
	mPoints.push_back( _rect.topLeft() );
	mPoints.push_back( _rect.bottomLeft() );
	mPoints.push_back( _rect.bottomRight() );
	// In this case the bbox is the same as the rect's size
	mBBox = _rect.size();

	calculatePerimeter();
}

void Profile::createArc( float startAngle, float angle, float radius, float numSegments ) {

	for ( int t = 0; t < numSegments; t++ ) {
		float delta = (( static_cast<float>( t ) / ( numSegments - 1 )) * angle ) + startAngle;
		float cosa = cos( delta ) * radius;
		float sina = sin( delta ) * radius;
		mPoints.emplace_back( cosa, sina );
	}
	mBBox = { radius * 2.0f, radius * 2.0f };

	calculatePerimeter();
}

void Profile::createArbitrary( const std::vector<Vector2f>& points ) {

	mPoints = points;

	calcBBox();
	calculatePerimeter();
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
	for ( auto p = pointsCopy.rbegin(); p != pointsCopy.rend(); ++p ) {
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
	for ( auto p = pointsCopy.rbegin(); p != pointsCopy.rend(); ++p ) {
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

void Profile::calcBBox() {
	Rect2f lbbox = Rect2f::INVALID;
	for ( const auto& p : mPoints ) {
		lbbox.expand(p);
	}
	mBBox = lbbox.size();
}

std::shared_ptr<Profile> Profile::makeLine( const std::string& _name, const std::vector<Vector2f>& vv2fs,
											[[maybe_unused]] const std::vector<float>& vfs) {

    ASSERT(vv2fs.size() == 2);
    std::shared_ptr<Profile> profile = std::make_shared<Profile>();
    profile->createLine( vv2fs[0], vv2fs[1] );

    return profile;
}

std::shared_ptr<Profile> Profile::makeWire( const std::string& _name,
											[[maybe_unused]] const std::vector<Vector2f>& vv2fs,
											const std::vector<float>& vfs) {

    ASSERT( vfs.size() && vfs.size() < 3);
    auto subdivs = vfs.size() == 1 ? 3 : static_cast<int>(vfs[1]);
    std::shared_ptr<Profile> profile = std::make_shared<Profile>();

    profile->createWire( vfs[0], subdivs );

    return profile;
}

std::shared_ptr<Profile> Profile::fromPoints( const std::string& name,  const std::vector<Vector2f>& points ) {
	std::shared_ptr<Profile> ret = std::make_shared<Profile>();
	ret->createArbitrary( points );

	return ret;

}

Profile::Profile( const Vector2f& a, const Vector2f& b, WindingOrderT wo ) {
    createLine( a, b, wo );
}

void ProfileMaker::add( const V2f& _p ) {
	points.emplace_back( _p * scale );
}

ProfileMaker& ProfileMaker::s( float _s ) {
	scale = _s;
	return *this;
}

int ProfileMaker::setPointerSubdivs( int _sd ) const {
	return _sd == -1 ? gsubdivs : _sd;
}

ProfileMaker& ProfileMaker::ay( float radius, int32_t _subdivs ) {
	auto subdivs = setPointerSubdivs(_subdivs);
	V2f start = pointer();
	for ( int t = 1; t < subdivs; t++ ) {
		float delta = ( static_cast<float>( t ) / static_cast<float>( subdivs - 1 ));
		float angle = JMATH::lerp( delta, M_PI_2, M_PI + M_PI_2 );
		add( start + ( V2f{cosf(angle)*-1.0f, 1.0f-sinf(angle)} * radius ) );
	}
	return *this;
}

ProfileMaker& ProfileMaker::o() {
	add( V2f::ZERO );
	return *this;
}

ProfileMaker& ProfileMaker::l( const V2f& _p1 ) {
	add( _p1 + pointer() );
	return *this;
}

ProfileMaker& ProfileMaker::ly( float _y1 ) {
	add( V2f{ 0.0f, _y1} + pointer() );
	return *this;
}

ProfileMaker& ProfileMaker::lx( float _x1 ) {
	add( V2f{ _x1, 0.0f} + pointer() );
	return *this;
}

ProfileMaker& ProfileMaker::sd( uint32_t _sd ) {
	gsubdivs = static_cast<int32_t >(_sd);
	return *this;
}
