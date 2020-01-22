/*
 * Poly2Tri Copyright (c) 2009-2010, Poly2Tri Contributors
 * http://code.google.com/p/poly2tri/
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the name of Poly2Tri nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <map>

#include "cdt.h"

namespace p2t {
CDT::CDT( std::vector<Point*> polyline ) {
	init( polyline );
}

CDT::CDT( const std::vector<Vector2f>& points ) {
	std::vector<p2t::Point*> polyline;
	for ( auto& ep : points )
		polyline.push_back( new p2t::Point( ep.x(), ep.y() ) );

	init( polyline );
}

CDT::CDT( const Vector2f* points, int _size ) {
	std::vector<p2t::Point*> polyline;
	for ( int t = 0; t < _size; t++ )
		polyline.push_back( new p2t::Point( points[t].x(), points[t].y() ) );

	init( polyline );
}

void CDT::init( const std::vector<p2t::Point*>& polyline ) {
	std::map<int64_t, p2t::Point*> lhashtable;

	//	for ( auto& v : polyline ) {
	//		lhashtable[Vector2f(v->x, v->y).hash()] = v;
	//	}
	//	std::vector<p2t::Point*> polylineSanitized;
	//	for ( auto& v : lhashtable ) {
	//		polylineSanitized.push_back(v.second);
	//	}

	std::vector<p2t::Point*> polylineSanitized;
	for ( auto& v : polyline ) {
		bool isPresent = false;
		for ( auto& vp : polylineSanitized ) {
			if ( isVerySimilar( V2f{ v->x, v->y }, V2f{ vp->x, vp->y }, 0.000001f ) ) {
				isPresent = true;
				break;
			}
		}
		if ( !isPresent ) {
			polylineSanitized.push_back( v );
		}
	}

	sweep_context_ = new SweepContext( polylineSanitized );
	sweep_ = new Sweep;
}

void CDT::AddHole( std::vector<Point*> polyline ) {
	sweep_context_->AddHole( polyline );
}

void CDT::AddPoint( Point* point ) {
	sweep_context_->AddPoint( point );
}

void CDT::Triangulate() {
	sweep_->Triangulate( *sweep_context_ );
}

std::vector<p2t::Triangle*> CDT::GetTriangles() {
	return sweep_context_->GetTriangles();
}

std::list<p2t::Triangle*> CDT::GetMap() {
	return sweep_context_->GetMap();
}

CDT::~CDT() {
	delete sweep_context_;
	delete sweep_;
}
}