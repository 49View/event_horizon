#include "poly_utils.hpp"
#include "../serializebin.hpp"

std::vector<polyQuadSub> quadSubDiv( const std::array<Vector3f, 4>& vss,
                                     const std::array<Vector2f, 4>& vtcs,
                                     const std::array<Vector3f, 4>& vns,
                                     subdivisionAccuracy _accuracy,
                                     FlipSubdiv subDivDir ) {
	std::vector<polyQuadSub> ret;
	std::array<Vector3f, 4> vs;
	std::array<Vector2f, 4> vtcsi{};
	std::array<Vector3f, 4> vncsi;

	int i2 = ( subDivDir == FlipSubdiv::True ) ? 3 : 2;
	int i3 = ( subDivDir == FlipSubdiv::True ) ? 2 : 3;

	Vector2i numSubDivs = Vector2i::ONE;
	Vector2f deltaXY = { 1.0f / numSubDivs.x(), 1.0f / numSubDivs.y() };
	if ( _accuracy != accuracyNone ) {
		numSubDivs = Vector2i( static_cast<int>( max( 1, distance( vss[0], vss[1] ) / _accuracy ) ), static_cast<int>( max( 1, distance( vss[0], vss[2] ) / _accuracy ) ) );
		deltaXY = { 1.0f / numSubDivs.x(), 1.0f / numSubDivs.y() };
	}

	for ( int y = 0; y < numSubDivs.y(); y++ ) {
		float deltaY = y / (float)numSubDivs.y();
		float deltaY1 = ( y + 1 ) / (float)numSubDivs.y();

		Vector3f vsY0 = JMATH::lerp( deltaY, vss[0], vss[2] );
		Vector3f vsY1 = JMATH::lerp( deltaY, vss[1], vss[3] );
		Vector3f vsY2 = JMATH::lerp( deltaY1, vss[0], vss[2] );
		Vector3f vsY3 = JMATH::lerp( deltaY1, vss[1], vss[3] );

		Vector2f vtcsY0 = JMATH::lerp( deltaY, vtcs[0], vtcs[2] );
		Vector2f vtcsY1 = JMATH::lerp( deltaY, vtcs[1], vtcs[3] );
		Vector2f vtcsY2 = JMATH::lerp( deltaY1, vtcs[0], vtcs[2] );
		Vector2f vtcsY3 = JMATH::lerp( deltaY1, vtcs[1], vtcs[3] );

		Vector3f vncsY0 = normalize( JMATH::lerp( deltaY,  vns[0], vns[2] ) );
		Vector3f vncsY1 = normalize( JMATH::lerp( deltaY,  vns[1], vns[3] ) );
		Vector3f vncsY2 = normalize( JMATH::lerp( deltaY1, vns[0], vns[2] ) );
		Vector3f vncsY3 = normalize( JMATH::lerp( deltaY1, vns[1], vns[3] ) );

		for ( int x = 0; x < numSubDivs.x(); x++ ) {
			float deltaX = x / (float)numSubDivs.x();
			float deltaX1 = ( x + 1 ) / (float)numSubDivs.x();

			vs[0] = JMATH::lerp( deltaX, vsY0, vsY1 );
			vs[1] = JMATH::lerp( deltaX1, vsY0, vsY1 );
			vs[i2] = JMATH::lerp( deltaX, vsY2, vsY3 );
			vs[i3] = JMATH::lerp( deltaX1, vsY2, vsY3 );

			vtcsi[0] = JMATH::lerp( deltaX, vtcsY0, vtcsY1 );
			vtcsi[1] = JMATH::lerp( deltaX1, vtcsY0, vtcsY1 );
			vtcsi[i2] = JMATH::lerp( deltaX, vtcsY2, vtcsY3 );
			vtcsi[i3] = JMATH::lerp( deltaX1, vtcsY2, vtcsY3 );

			vncsi[0] = normalize( JMATH::lerp( deltaX,  vncsY0, vncsY1 ) );
			vncsi[1] = normalize( JMATH::lerp( deltaX1, vncsY0, vncsY1 ) );
			vncsi[i2] = normalize( JMATH::lerp( deltaX,  vncsY2, vncsY3 ) );
			vncsi[i3] = normalize( JMATH::lerp( deltaX1, vncsY2, vncsY3 ) );

			ret.push_back( { vs, vtcsi, vncsi } );
		}
	}
	return ret;
}

std::vector<polyQuadSub> quadSubDiv( const std::array<Vector3f, 4>& vss,
                                     const std::array<Vector2f, 4>& vtcs,
                                     const Vector3f& nomal,
                                     subdivisionAccuracy _accuracy ) {
	std::array<Vector3f, 4> vns;
	for ( int t = 0; t < 4; t++ ) vns[t] = nomal;
	return quadSubDiv(vss, vtcs, vns, _accuracy, FlipSubdiv::False );
}

std::vector<polyQuadSub> quadSubDivInv( const std::array<Vector3f, 4>& vss,
                                        const std::array<Vector2f, 4>& vtcs,
                                        const Vector3f& nomal,
                                        subdivisionAccuracy _accuracy ) {
    std::array<Vector3f, 4> vns;
    for ( int t = 0; t < 4; t++ ) vns[t] = nomal;
    std::array<Vector3f, 4> vssInv = vss;
    std::swap( vssInv[2], vssInv[3] );
    return quadSubDiv(vssInv, vtcs, vns, _accuracy, FlipSubdiv::True );
}

Vector3f utilMirrorFlip( const Vector3f& v, WindingOrderT wow, WindingOrderT woh, const Rect2f& bbox ) {
	Vector3f ret = v;
	if ( wow == WindingOrder::CW ) ret[0] = bbox.width() - v[0];
	if ( woh == WindingOrder::CW ) ret[1] = bbox.height() - v[1];
	return ret;
}

WindingOrderT detectWindingOrder( const Vector2f& pa, const Vector2f& pb, const Vector2f& pc ) {
	float detleft = ( pa.x() - pc.x() ) * ( pb.y() - pc.y() );
	float detright = ( pa.y() - pc.y() ) * ( pb.x() - pc.x() );
	float val = detleft - detright;

	ASSERT( val != 0.0f ); // Is Collinear

	if ( val > 0.0f ) {
		return WindingOrder::CCW;
	}
	return WindingOrder::CW;
}

WindingOrderT detectWindingOrder( const Vector3f& v1, const Vector3f& v2, const Vector3f& v3 ) {
	Vector3f tentativeNormal = normalize( crossProduct( v1, v2, v3 ) );
	Vector2f w1, w2, w3;
	if ( fabs( tentativeNormal.z() ) > fabs( tentativeNormal.x() ) &&
		 fabs( tentativeNormal.z() ) > fabs( tentativeNormal.y() ) ) {
		w1 = v1.xy();
		w2 = v2.xy();
		w3 = v3.xy();
	} else if ( fabs( tentativeNormal.y() ) > fabs( tentativeNormal.z() ) &&
				fabs( tentativeNormal.y() ) > fabs( tentativeNormal.x() ) ) {
		w1 = v1.xz();
		w2 = v2.xz();
		w3 = v3.xz();
	} else {
		w1 = v1.yz();
		w2 = v2.yz();
		w3 = v3.yz();
	}
	return ( w3.sideOfLine( w1, w2 ) > 0.0f ) ? WindingOrder::CCW : WindingOrder::CW;
}

std::vector<Vector3f> createQuadPoints( const Vector2f& size ) {

	std::vector<Vector3f> lPoints;

	lPoints.emplace_back( Vector2f( -size.x() * 0.5f, size.y() * 0.5f ) );
	lPoints.emplace_back( Vector2f( size.x() * 0.5f, size.y() * 0.5f ) );
	lPoints.emplace_back( Vector2f( size.x() * 0.5f, -size.y() * 0.5f ) );
	lPoints.emplace_back( Vector2f( -size.x() * 0.5f, -size.y() * 0.5f ) );

	return lPoints;
}

WindingOrderT detectWindingOrderOpt2d( const V2fVector& vlist ) {
    if ( vlist.size() < 3 ) return WindingOrder::CCW;

    size_t si = 0;
    std::vector<size_t> ties{};
    float minY = std::numeric_limits<float>::max();
    for ( auto q = 0u; q < vlist.size(); q++ ) {
        if ( vlist[q].y() < minY ) {
            minY = vlist[q].y();
            si = q;
            continue;
        }
        if ( vlist[q].y() == minY ) {
            ties.emplace_back(q);
        }
    }
    if ( !ties.empty() ) {
        ties.push_back(si);
        float maxX = std::numeric_limits<float>::lowest();
        for ( auto q : ties ) {
            if ( vlist[q].x() > maxX ) {
                si = q;
            }
        }
    }

    auto prevIndex = getCircularArrayIndexUnsigned( si -1, vlist.size() );
    auto nextIndex = getCircularArrayIndexUnsigned( si +1, vlist.size() );

    V2f a = vlist[si];
    V2f b = vlist[prevIndex];
    V2f c = vlist[nextIndex];
    V2f ab = normalize(vlist[si] - vlist[prevIndex]);
    V2f ac = normalize(vlist[si] - vlist[nextIndex]);
    float sig = ab.cross(ac);
    double detOrient = (b.x() * c.y() + a.x() * b.y() + a.y() * c.x()) - (a.y() * b.x() + b.y() * c.x() + a.x() * c.y());

    LOGRS( sig << detOrient );
    auto ret = sign(detOrient) > 0.0f ? WindingOrder::CCW : WindingOrder::CW;
    return ret;
}

WindingOrderT detectWindingOrder( const std::vector<V2f>& _input ) {
    size_t i1, i2;
    float area = 0;
    for ( i1 = 0; i1 < _input.size(); i1++ ) {
        i2 = i1 + 1;
        if ( i2 == _input.size() ) i2 = 0;
        area += _input[i1].x() * _input[i2].y() - _input[i1].y() * _input[i2].x();
    }

    if ( area > 0 ) return WindingOrder::CW;
    if ( area < 0 ) return WindingOrder::CCW;
    if ( _input.size() > 2 ) {
        LOGR("[ERROR] cannot get winding order of these points cos area is 0");
    }
    return WindingOrder::CCW;
}

WindingOrderT detectWindingOrder( const std::vector<V3f>& _input, const V3f& axis ) {
    size_t i1, i2;
    float area = 0;
    auto dominantPair = axis.leastDominantPair();
    for ( i1 = 0; i1 < _input.size(); i1++ ) {
        i2 = i1 + 1;
        if ( i2 == _input.size() ) i2 = 0;
        area += _input[i1][dominantPair.second] * _input[i2][dominantPair.first] -
                _input[i1][dominantPair.first] * _input[i2][dominantPair.second];
    }

    if ( area > 0 ) return WindingOrder::CW;
    if ( area < 0 ) return WindingOrder::CCW;
    if ( _input.size() > 2 ) {
        LOGR("[ERROR] cannot get winding order of these points cos area is 0");
    }
    return WindingOrder::CCW;
}
