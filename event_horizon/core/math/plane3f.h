//
//  plane3f.h
//  SixthViewer
//
//  Created by Dado on 12/11/2015.
//  Copyright © 2015 EpicGames. All rights reserved.
//

#ifndef plane3f_h
#define plane3f_h

#include "vector3f.h"

struct Plane3f {
	Vector3f n; // normal
	float d = 0.0f; // distance from origin

	Plane3f() {} // default constructor

	Plane3f( const Vector3f& _n, float _d ) {
		n = _n;
		d = _d;
	}

	Plane3f( const Vector3f& _n, const Vector3f& _p ) {
		n = _n;
		d = dot( _p, n );
	}

	Plane3f( const Vector3f& a, const Vector3f& b, const Vector3f& c ) {
		set( a, b, c );
	}

	inline void set( const Vector3f& a, const Vector3f& b, const Vector3f& c ) {
		n = normalize( crossProduct( a, b, c ) );
		d = dot( a, n );
	}

	Vector3f intersectLine( const Vector3f& a, const Vector3f& b ) const {
		Vector3f ba = b - a;
		float nDotA = dot( n, a );
		float nDotBA = dot( n, ba );

		return a + ( ba*( ( d - nDotA ) / nDotBA ) );
	}

	inline float checkSide( const Vector3f& i ) const {
		return dot( i, n ) - d;
	}

	Vector3f intersectRay( const Vector3f& p0, const Vector3f& v, bool& inters ) const {
		float vdotn = dot( v, n );
		if ( fabs( vdotn ) == 0.0f ) {
			inters = false;
			return p0;
		}
		float pdotn = dot( p0, n ) - d;
		if ( pdotn < 0.0f ) {
			inters = false;
			return p0;
		}
		if ( vdotn > 0.0f ) {
			inters = false;
			return p0;
		}
		inters = true;
		float t = -( pdotn ) / vdotn;
		return p0 + v*t;
	}

	Vector3f intersectLineGrace( const Vector3f& a, const Vector3f& b ) const {
		Vector3f ba = b - a;
		float nDotA = dot( n, a );
		float nDotBA = dot( n, ba );

		if ( nDotBA == 0.0f ) return a;

		return a + ( ba*( ( d - nDotA ) / nDotBA ) );
	}

	bool intersectRayOnTriangle( const Vector3f& p0, const Vector3f& v, const Vector3f& a, const Vector3f& b, const Vector3f& c, Vector3f& i, float& bu, float& bv ) const {
		bool inters = false;
		i = intersectRay( p0, v, inters );
		if ( inters ) {
			//			Vector3f v0v1 = b - a;
			//			Vector3f v0v2 = c - a;
			//			// no need to normalize
			//			Vector3f N = cross(v0v1, v0v2); // N
			//			float area2 = length(n);
			//
			//			if ( dot(v,N) < 0.0f ) return false;

			Vector3f crossBA = cross( b - a, i - a );
			float d1 = dot( n, crossBA );
			if ( d1 < 0.0f ) return false;

			Vector3f crossCB = cross( c - b, i - b );
			float d2 = dot( n, crossCB );
			if ( d2 < 0.0f ) return false;
			bu = length( crossCB );

			Vector3f crossAC = cross( a - c, i - c );
			float d3 = dot( n, crossAC );
			if ( d3 < 0.0f ) return false;
			bv = length( crossAC );

			return true;
		}
		return inters;
	}
};

#endif /* plane3f_h */
