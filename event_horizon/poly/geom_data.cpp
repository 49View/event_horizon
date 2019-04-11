//
//  geom_data.cpp
//  6thViewImporter
//
//  Created by Dado on 15/10/2015.
//
//

#include "geom_data.hpp"

#include <iomanip>

#include "triangulator.hpp"

#include <core/node.hpp>
#include <poly/converters/svg/svgtopoly.hpp>
#include <core/resources/profile.hpp>
#include <poly/follower.hpp>

GeomDataListBuilderRetType GeomDataSVGBuilder::build() {
	auto rawPoints = SVGC::SVGToPoly( svgAscii );

	GeomDataListBuilderRetType logoGeoms{};
	logoGeoms.reserve( rawPoints.size() );
	for ( const auto& points : rawPoints ) {
		auto fb = std::make_shared<GeomDataFollowerBuilder>( mProfile,
															 XZY::C(points.path,0.0f),
															 FollowerFlags::WrapPath );
//        _mat->c( points.strokeColor );
		logoGeoms.emplace_back(fb->build());
	}
	return logoGeoms;
}

std::vector<Vector3f> GeomData::utilGenerateFlatBoxFromRect( const JMATH::Rect2f& bbox, float z ) {
	std::vector<Vector3f> bboxPoints;
	bboxPoints.emplace_back( bbox.bottomLeft(), z );
	bboxPoints.emplace_back( bbox.bottomRight(), z );
	bboxPoints.emplace_back( bbox.topRight(), z );
	bboxPoints.emplace_back( bbox.topLeft(), z );
	return bboxPoints;
}

std::vector<Vector3f> GeomData::utilGenerateFlatBoxFromSize( float width, float height, float z ) {
	std::vector<Vector3f> bboxPoints;
	// Clockwise
	bboxPoints.emplace_back( -width*0.5f, -height*0.5f, z );
	bboxPoints.emplace_back( -width*0.5f, height*0.5f, z );
	bboxPoints.emplace_back( width*0.5f, height*0.5f, z );
	bboxPoints.emplace_back( width*0.5f, -height*0.5f, z );
	return bboxPoints;
}

std::vector<Vector2f> GeomData::utilGenerateFlatRect( const Vector2f& size, const WindingOrderT wo, PivotPointPosition ppp, const Vector2f& /*pivot*/ ) {
	std::vector<Vector2f> fverts;

	if ( wo == WindingOrder::CW ) {
		fverts.emplace_back( size.x(), 0.0f );
		fverts.emplace_back( size.x(), size.y());
		fverts.emplace_back( 0.0f, size.y());
		fverts.push_back( Vector2f::ZERO );
	} else {
		fverts.push_back( Vector2f::ZERO );
		fverts.emplace_back( 0.0f, size.y());
		fverts.emplace_back( size.x(), size.y());
		fverts.emplace_back( size.x(), 0.0f );
	}
	for ( auto& v : fverts ) {
		switch ( ppp ) {
			case PivotPointPosition::PPP_CENTER:
			v -= size * 0.5f;
			break;
			case PivotPointPosition::PPP_BOTTOM_CENTER:
			v -= Vector2f( size.x() * 0.5f, 0.0f );
			break;
			case PivotPointPosition::PPP_TOP_CENTER:
			v -= size * 0.5f;
			break;
			case PivotPointPosition::PPP_LEFT_CENTER:
			v -= size * 0.5f;
			break;
			case PivotPointPosition::PPP_RIGHT_CENTER:
			v -= Vector2f( size.x() * 0.5f, 0.0f );
			break;
			case PivotPointPosition::PPP_BOTTOM_RIGHT:
			v -= size * 0.5f;
			break;
			case PivotPointPosition::PPP_BOTTOM_LEFT:
            v -= Vector2f( size.x() * 0.5f, 0.0f );
			break;
			case PivotPointPosition::PPP_TOP_LEFT:
			break;
			case PivotPointPosition::PPP_TOP_RIGHT:
			v -= size * 0.5f;
			break;
			case PivotPointPosition::PPP_CUSTOM:
			v -= size * 0.5f;
			break;
			default:
			break;
		}
	}

	return fverts;
}

//void GeomData::Bevel( const Vector3f & bevelAmount ) {
//	mBevelAmount = bevelAmount;
//	mHasBevel = true;
//}
//
//Vector3f GeomData::Bevel() const {
//	return mBevelAmount;
//}

//void GeomData::shCoeffBaricentricRedAt( Matrix3f& dest, int index, float bu, float bv ) {
//	float bw = 1.0f - ( bu + bv );
//	Matrix3f shr1 = shCoeffRAt( index );
//	Matrix3f shr2 = shCoeffRAt( index + 1 );
//	Matrix3f shr3 = shCoeffRAt( index + 2 );
//
//	dest = shr1 * bu + shr2 * bv + shr3 * bw;
//}
//
//void GeomData::shCoeffBaricentricGreenAt( Matrix3f& dest, int index, float bu, float bv ) {
//	float bw = 1.0f - ( bu + bv );
//	Matrix3f shr1 = shCoeffGAt( index );
//	Matrix3f shr2 = shCoeffGAt( index + 1 );
//	Matrix3f shr3 = shCoeffGAt( index + 2 );
//
//	dest = shr1 * bu + shr2 * bv + shr3 * bw;
//}
//
//void GeomData::shCoeffBaricentricBlueAt( Matrix3f& dest, int index, float bu, float bv ) {
//	float bw = 1.0f - ( bu + bv );
//	Matrix3f shr1 = shCoeffBAt( index );
//	Matrix3f shr2 = shCoeffBAt( index + 1 );
//	Matrix3f shr3 = shCoeffBAt( index + 2 );
//
//	dest = shr1 * bu + shr2 * bv + shr3 * bw;
//}

//void GeomData::calcSHBounce( const Node* /*dad*/, GeomData* /*dest*/ ) {
	//	if (vertexRayHitMap.size() == 0) return;
	//
	//	//	std::vector<bool>::iterator bvi;
	//	std::vector<DeepIntersectData>::iterator bvi;
	//	float factor = (4.0f*M_PI / SH.NumSamples());
	//
	//	Vector3f albedo = getColor() / M_PI;
	//	perfTimerStart(0);
	//	for (int32_t t = 0; t < numVerts(); t++) {
	//		Matrix3f bSHCoeffsR = Matrix3f::ZERO;
	//		Matrix3f bSHCoeffsG = Matrix3f::ZERO;
	//		Matrix3f bSHCoeffsB = Matrix3f::ZERO;
	//		Vector3f pos = vertexAt(t);
	//		Vector3f n = normalAt(t);
	//
	//		// Add a little bit of space to avoid self intersecting with itself and proximity neighbor that should be pretty much co-planar
	//		pos += n * 0.0075f;
	//
	//		int m = 0;
	//		for (bvi = vertexRayHitMap[t].begin(); bvi != vertexRayHitMap[t].end(); ++bvi, ++m) {
	//			//			if ( *bvi == true ) {
	//			if ((*bvi).minDist > 0.0f) {
	//				SHSample* sample = SH.Sample(m);
	//				float Hs = dot(sample->direction, n);
	//				if (Hs > 0.0f) {
	//					DeepIntersectData did = *bvi;
	//					int32_t    index = 0;
	//					float      u = 0.0f;
	//					float	   v = 0.0f;
	//					did.geomBaked->checkBaricentricCoordsOn(did.intersectPoint, did.indexStart, did.indexEnd, index, u, v);
	//
	//					did.geom->shCoeffBaricentricRedAt(bSHCoeffsR, index, u, v);
	//					did.geom->shCoeffBaricentricGreenAt(bSHCoeffsG, index, u, v);
	//					did.geom->shCoeffBaricentricBlueAt(bSHCoeffsB, index, u, v);
	//					dest->shIncCoeffRAt(t, bSHCoeffsR * albedo.x() * Hs * factor);
	//					dest->shIncCoeffGAt(t, bSHCoeffsG * albedo.y() * Hs * factor);
	//					dest->shIncCoeffBAt(t, bSHCoeffsB * albedo.z() * Hs * factor);
	//				}
	//			}
	//		}
	//	}
//}

