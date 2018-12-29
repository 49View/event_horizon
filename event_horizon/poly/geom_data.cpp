//
//  geom_data.cpp
//  6thViewImporter
//
//  Created by Dado on 15/10/2015.
//
//

#include "geom_data.hpp"

#include <iomanip>

#include "geom_builder.h"
#include "triangulator.hpp"

#include <core/node.hpp>

inline void hash_combine( std::size_t& /*seed*/ ) {}

template <typename T, typename... Rest>
inline void hash_combine( std::size_t& seed, const T& v, Rest... rest ) {
	std::hash<T> hasher;
	seed ^= hasher( v ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
	hash_combine( seed, rest... );
}

GeomData::GeomData() {
	material = std::make_shared<PBRMaterial>();
}

GeomData::GeomData( std::shared_ptr<DeserializeBin> reader ) {
	deserialize( reader );
}

GeomData::GeomData( const ShapeType _st,
					const Vector3f& _pos, [[maybe_unused]] const Vector3f& _axis, const Vector3f& _scale,
					std::shared_ptr<PBRMaterial> _material,
					const GeomMappingData& _mapping ) {
	material = _material;
	setMappingData( _mapping );
	addShape( _st, _pos, _scale, 3 );
}

GeomData::GeomData( const std::vector<PolyOutLine>& verts, std::shared_ptr<PBRMaterial> _material,
					const GeomMappingData& _mapping, PullFlags pullFlags ) {
	material = _material;
	setMappingData( _mapping );
	for ( const auto& ot : verts ) {
		pull( ot.verts, ot.zPull, pullFlags );
	}
}

GeomData::GeomData( const std::vector<PolyLine>& _polyLines, std::shared_ptr<PBRMaterial> _material,
                    const GeomMappingData& _mapping ) {
	material = _material;
	setMappingData( _mapping );

    for ( const auto& poly : _polyLines ) {
        addFlatPolyTriangulated( poly.verts.size(), poly.verts.data(), poly.normal,
								   static_cast<bool>(poly.reverseFlag) );
    }
}

GeomData::GeomData( const QuadVector3fNormalfList& _quads, std::shared_ptr<PBRMaterial> _material,
					const GeomMappingData& _mapping ) {
	material = _material;
	setMappingData( _mapping );
	for ( const auto& q : _quads ) {
		addFlatPoly( q.quad, q.normal );
	}
}

std::shared_ptr<GeomData> GeomDataShapeBuilder::build() {
    return std::make_shared<GeomData>( shapeType, pos, axis, scale, material, mappingData );
}

std::shared_ptr<GeomData> GeomDataOutlineBuilder::build() {
	return std::make_shared<GeomData>( outlineVerts, material, mappingData );
}

std::shared_ptr<GeomData> GeomDataPolyBuilder::build() {
	return std::make_shared<GeomData>( polyLine, material, mappingData );
}

std::shared_ptr<GeomData> GeomDataQuadMeshBuilder::build() {
	return std::make_shared<GeomData>( quads, material, mappingData );
}

std::shared_ptr<GeomData> GeomDataFollowerBuilder::build() {
    ASSERT( !mProfile->Points().empty() );

    Profile lProfile{ *mProfile.get() };
    Vector2f lRaise = mRaise;
    if ( mRaiseEnum != PolyRaise::None ) {
		switch ( mRaiseEnum ) {
			case PolyRaise::None:break;
			case PolyRaise::HorizontalPos:
				lRaise= ( Vector2f::X_AXIS * lProfile.width() );
				break;
			case PolyRaise::HorizontalNeg:
				lRaise= ( Vector2f::X_AXIS_NEG * lProfile.width());
				break;
			case PolyRaise::VerticalPos:
				lRaise= ( Vector2f::Y_AXIS * lProfile.height() );
				break;
			case PolyRaise::VerticalNeg:
				lRaise= ( Vector2f::Y_AXIS_NEG * lProfile.height() );
				break;
		};
    }

	lProfile.raise( lRaise );
	lProfile.flip( mFlipVector );

	auto ret = FollowerService::extrude( mVerts, lProfile, mSuggestedAxis, followersFlags );
	ret->setMaterial(material);

	return ret;
}

void GeomData::serialize( std::shared_ptr<SerializeBin> f ) {
	f->write( mName );
	material->serialize( f );
	f->write( mBBox3d.mMinPoint );
	f->write( mBBox3d.mMaxPoint );
	f->write( mapping.direction );
	f->write( mVdata.vIndices );
	f->write( mVdata.vcoords3d );
	f->write( mVdata.vnormals3d );
	f->write( mVdata.vtangents3d );
	f->write( mVdata.vbinormals3d );
	f->write( mVdata.vUVs );
    f->write( mVdata.vUV2s );
	f->write( mVdata.vColor );
	f->write( wrapMappingCoords );
	f->write( mWindingOrder );
}

void GeomData::serializeSphericalHarmonics( [[maybe_unused]] std::shared_ptr<SerializeBin> writer ) {
//	writer->write( mVdata.vSHCoeffsR );
//	writer->write( mVdata.vSHCoeffsG );
//	writer->write( mVdata.vSHCoeffsB );
}

void GeomData::deserialize( std::shared_ptr<DeserializeBin> reader ) {
	reader->read( mName );
	material = std::make_shared<PBRMaterial>( reader );
	reader->read( mBBox3d.mMinPoint );
	reader->read( mBBox3d.mMaxPoint );
	reader->read( mapping.direction );
	reader->read( mVdata.vIndices );
	reader->read( mVdata.vcoords3d );
	reader->read( mVdata.vnormals3d );
	reader->read( mVdata.vtangents3d );
	reader->read( mVdata.vbinormals3d );
	reader->read( mVdata.vUVs );
	reader->read( mVdata.vUV2s );
	reader->read( mVdata.vColor );
	reader->read( wrapMappingCoords );
	reader->read( mWindingOrder );

//	mVdata.allocateEmptySHData();
}

void GeomData::deserializeSphericalHarmonics( [[maybe_unused]] std::shared_ptr<DeserializeBin> reader ) {
//	reader->read( mVdata.vSHCoeffsR );
//	reader->read( mVdata.vSHCoeffsG );
//	reader->read( mVdata.vSHCoeffsB );
}

void GeomData::serializeDependencies( std::shared_ptr<SerializeBin> writer ) {
	material->serializeDependencies( writer );
}

void GeomData::deserializeDependencies( [[maybe_unused]] std::shared_ptr<SerializeBin> reader ) {

}

void GeomData::reset() {
	mBBox3d = AABB::INVALID;
	mVdata.clear();
	mWindingOrder = WindingOrder::CCW;
}

void GeomData::setMappingData( const GeomMappingData& _mapping ) {
	mapping = _mapping;

	if ( mapping.bDoNotScaleMapping ) doNotScaleMapping();
}

//void GeomData::set( const std::string& uniformName, float data ) {
//	mMaterial->setConstant( uniformName, data );
//}
//
//void GeomData::set( const std::string& uniformName, std::shared_ptr<Texture> data ) {
//	mMaterial->setConstant( uniformName, data );
//	if ( uniformName == UniformNames::colorTexture ) {
//		setTextureCoordsMultiplier();
//	}
//}
//
//void GeomData::setTextureSet( const std::string& baseTextureName ) {
//	std::shared_ptr<Texture> data = TM.T( baseTextureName + ".png" );
//	mMaterial->setConstant( UniformNames::colorTexture, data );
//	setTextureCoordsMultiplier();
//	std::string normalMapName = baseTextureName + "_n.png";
//	mMaterial->setConstant( UniformNames::normalTexture, TM.T( normalMapName ) );
//}
//
//void GeomData::set( const std::string& uniformName, const Vector2f& data ) {
//	mMaterial->setConstant( uniformName, data );
//}
//
//void GeomData::set( const std::string& uniformName, const Vector3f& data ) {
//	mMaterial->setConstant( uniformName, data );
//}
//
//void GeomData::set( const std::string& uniformName, const Vector4f& data ) {
//	mMaterial->setConstant( uniformName, data );
//}
//
//void GeomData::set( const std::string& uniformName, const Matrix4f& data ) {
//	mMaterial->setConstant( uniformName, data );
//}
//
//void GeomData::set( const std::string& uniformName, const Matrix3f& data ) {
//	mMaterial->setConstant( uniformName, data );
//}
//
//void GeomData::setTextureCoordsMultiplier() {
//	if ( bDoNotScaleMapping ) return;
//
//	std::shared_ptr<Texture> colorTexture = ( mMaterial->Constants()->hasTexture( UniformNames::colorTexture ) ) ? getColorTexture() : TM.T( "white.png" );
//
//	float fuvScaleCM = 2048.0f;
//	Vector2f normUVScale = { fuvScaleCM / colorTexture->getHeightf(), fuvScaleCM / colorTexture->getWidthf() };
//	uvScale = { normUVScale.x(), -normUVScale.y() };
//	uvScaleInv = reciprocal( uvScale );
//}
//
//Vector3f GeomData::getColor() const {
//	return mMaterial->Constants()->getVector3f( UniformNames::diffuseColor );
//}
//
//float GeomData::getOpacity() const {
//	float opactity;
//	mMaterial->Constants()->get( UniformNames::opacity, opactity );
//	return opactity;
//}
//
//std::shared_ptr<Texture> GeomData::getColorTexture() const {
//	return mMaterial->Constants()->getTexture( UniformNames::colorTexture );
//}

void GeomData::addShape( ShapeType st, const Vector3f& center, const Vector3f& size, int subDivs ) {
	PolyStruct ps;

	switch ( st ) {
		case ShapeType::Cylinder:
			ps = createGeomForCylinder( center, size.xy(), subDivs );
			break;
		case ShapeType::Sphere:
			ps = createGeomForSphere( center, size.x(), subDivs );
			break;
		case ShapeType::Cube:
			ps = createGeomForCube( center, size );
			break;
		case ShapeType::Pillow:
			ps = createGeomForPillow( center, size, subDivs );
			break;
		default:
			ps = createGeomForSphere( center, size.x(), subDivs );
	}

	mVdata.fill(ps);
}

Vector3f GeomData::normalFromPoints( const Vector3f* vs ) {
	Vector3f normal = mWindingOrder == WindingOrder::CCW ? crossProduct( vs[0], vs[1], vs[2] ) : crossProduct( vs[0], vs[2], vs[1] );
	normal = normalize( normal );
	return normal;
}

void GeomData::addFlatPoly( const std::vector<Vector3f>& points, WindingOrderT wo ) {
	if ( points.size() > 2 ) {
		WindingOrderT oldWindingOrder = mWindingOrder;
		mWindingOrder = wo;
		Vector3f normal = normalFromPoints( points.data() );
		addFlatPoly( points.size(), points.data(), normal );
		//for ( uint64_t t = 0; t < points.size() - 2; t++ ) {
		//	topologyFence();
		//	pushTriangleSubDiv( points[0], points[t + 1], points[t + 2], profile.pointAt( 0 ), profile.pointAt( t + 1 ), profile.pointAt( t + 2 ), normal, normal, normal );
		//	if ( mWindingOrder == WindingOrder::CCW )
		//		mTopology.addPoly( { points[0], points[t + 1], points[t + 2] }, topologyFenceStart(), topologyFenceEnd() );
		//	else
		//		mTopology.addPoly( { points[0], points[t + 2], points[t + 1] }, topologyFenceStart(), topologyFenceEnd() );
		//}
		mWindingOrder = oldWindingOrder;
	}
}

void GeomData::doNotScaleMapping() {
	mapping.bDoNotScaleMapping = true;
	fuvScale = 1.0f;
	uvScale = { fuvScale, -fuvScale };
	uvScaleInv = reciprocal( uvScale );
}

void GeomData::pushTriangleSubDivRec( const Vector3f& v1, const Vector3f& v2, const Vector3f& v3,
									  const Vector2f& uv1, const Vector2f& uv2, const Vector2f& uv3,
									  const Vector3f& vn1, const Vector3f& vn2, const Vector3f& vn3, int triSubDiv ) {
	if ( triSubDiv == 0 ) {
		pushTriangle( v1, v2, v3, uv1, uv2, uv3, vn1, vn2, vn3 );
	} else {
		Vector3f vi1 = JMATH::lerp( 0.5f, v1, v2 );
		Vector3f vi2 = JMATH::lerp( 0.5f, v2, v3 );
		Vector3f vi3 = JMATH::lerp( 0.5f, v3, v1 );

		Vector2f uvi1 = JMATH::lerp( 0.5f, uv1, uv2 );
		Vector2f uvi2 = JMATH::lerp( 0.5f, uv2, uv3 );
		Vector2f uvi3 = JMATH::lerp( 0.5f, uv3, uv1 );

		Vector3f vni1 = JMATH::lerp( 0.5f, vn1, vn2 );
		Vector3f vni2 = JMATH::lerp( 0.5f, vn2, vn3 );
		Vector3f vni3 = JMATH::lerp( 0.5f, vn3, vn1 );
		vni1 = normalize( vni1 );
		vni2 = normalize( vni2 );
		vni3 = normalize( vni3 );

		pushTriangleSubDivRec( v1, vi1, vi3, uv1, uvi1, uvi3, vn1, vni1, vni3, triSubDiv - 1 );
		pushTriangleSubDivRec( vi1, v2, vi2, uvi1, uv2, uvi2, vni1, vn2, vni2, triSubDiv - 1 );
		pushTriangleSubDivRec( vi3, vi2, v3, uvi3, uvi2, uv3, vni3, vni2, vn3, triSubDiv - 1 );
		pushTriangleSubDivRec( vi1, vi2, vi3, uvi1, uvi2, uvi3, vni1, vni2, vni3, triSubDiv - 1 );
	}
}

void GeomData::pushTriangleSubDiv( const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, const Vector2f& uv1, const Vector2f& uv2, const Vector2f& uv3, const Vector3f& vn1, const Vector3f& vn2, const Vector3f& vn3 ) {
	if ( mSubdivAccuracy != accuracyNone ) {
		float d1 = distance( v1, v2 );
		d1 = min( d1, distance( v2, v3 ) );
		d1 = min( d1, distance( v1, v3 ) );

		int triSubDiv = static_cast<int>( max( 1, sqrt( d1 / mSubdivAccuracy ) ) );

		pushTriangleSubDivRec( v1, v2, v3, uv1, uv2, uv3, vn1, vn2, vn3, triSubDiv );
	} else {
		pushTriangle( v1, v2, v3, uv1, uv2, uv3, vn1, vn2, vn3 );
	}
}

void GeomData::pushTriangle( const std::vector<Vector3f>& vs, const std::vector<Vector2f>& vuv, const std::vector<Vector3f>& vn ) {
	pushTriangle( vs[0], vs[1], vs[2], vuv[0], vuv[1], vuv[2], vn[0], vn[1], vn[2] );
}

void GeomData::pushQuad( const std::array<Vector3f, 4>& vs, const std::array<Vector2f, 4>& vts, const Vector3f& vn ) {
	Vector3f normal = vn;
	pushTriangle( vs[0], vs[1], vs[2], vts[0], vts[1], vts[2], normal, normal, normal );
	pushTriangle( vs[2], vs[1], vs[3], vts[2], vts[1], vts[3], normal, normal, normal );
}

void GeomData::pushQuad( const std::array<Vector3f, 4>& vs, const std::array<Vector2f, 4>& vts,
                         const std::array<Vector3f, 4>& vns ) {
    pushTriangle( vs[0], vs[1], vs[2], vts[0], vts[1], vts[2], vns[0], vns[1], vns[2] );
    pushTriangle( vs[2], vs[1], vs[3], vts[2], vts[1], vts[3], vns[2], vns[1], vns[3] );
}

void GeomData::pushQuad( const QuadVertices3& vs, const QuadVertices2& vts ) {
	Vector3f normal = mWindingOrder == WindingOrder::CCW ? crossProduct( vs[0], vs[1], vs[2] ) : crossProduct( vs[0], vs[2], vs[1] );
	normal = normalize( normal );
	pushTriangle( vs[0], vs[1], vs[2], vts[0] * uvScaleInv, vts[1] * uvScaleInv, vts[2] * uvScaleInv, normal, normal, normal );
	pushTriangle( vs[2], vs[1], vs[3], vts[2] * uvScaleInv, vts[1] * uvScaleInv, vts[3] * uvScaleInv, normal, normal, normal );
}

void GeomData::pushQuad( const QuadVertices3& vs, const QuadVertices2& vts, const QuadVertices3& vns ) {
    pushTriangle( vs[0], vs[1], vs[2], vts[0] * uvScaleInv, vts[1] * uvScaleInv, vts[2] * uvScaleInv, vns[0], vns[1],
                  vns[2] );
    pushTriangle( vs[2], vs[1], vs[3], vts[2] * uvScaleInv, vts[1] * uvScaleInv, vts[3] * uvScaleInv, vns[2], vns[1],
                  vns[3] );
}

void GeomData::addFlatPolyWithMapping( const QuadStripUV& qs ) {
	addFlatPolyWithMapping( 4, qs.vs().verts(), qs.vts().verts() );
}

void GeomData::pushQuadSubDiv( const std::array<Vector3f, 4>& vss, const std::array<Vector2f, 4>& vtcs, const Vector3f& vn ) {
	Vector3f normal = vn.x() == std::numeric_limits<float>::lowest() ? normalFromPoints( vss.data() ) : vn;
	auto quads = quadSubDiv( vss, vtcs, normal, mSubdivAccuracy );
	for ( const auto& quad : quads ) {
		pushQuad( quad.vcoords, quad.uvs, normal );
	}
}

void GeomData::pushQuadSubDiv( const std::array<Vector3f, 4>& vss, const std::array<Vector2f, 4>& vtcs,
                               const std::array<Vector3f, 4>& vns ) {
    auto quads = quadSubDiv( vss, vtcs, vns, mSubdivAccuracy );
    for ( const auto& quad : quads ) {
        pushQuad( quad.vcoords, quad.uvs, quad.normals );
    }
}

void GeomData::pushTriangle( const Vector3f& v1, const Vector3f& v2, const Vector3f& v3,
							 const Vector2f& uv1, const Vector2f& uv2, const Vector2f& uv3,
							 const Vector3f& vn1, const Vector3f& vn2, const Vector3f& vn3,
							 const Vector3f& vt1, const Vector3f& vt2, const Vector3f& vt3,
							 const Vector3f& vb1, const Vector3f& vb2, const Vector3f& vb3 ) {
	//	ASSERT(!isCollinear(v1, v2, v3));

	mBBox3d.expand( v1 );
	mBBox3d.expand( v2 );
	mBBox3d.expand( v3 );

	Vector3f tangent1 = vt1;
	Vector3f tangent2 = vt2;
	Vector3f tangent3 = vt3;

	Vector3f bitangent1 = vb1;
	Vector3f bitangent2 = vb2;
	Vector3f bitangent3 = vb3;

	if ( vt1 == Vector3f::ZERO ) { // Needs to calculate tangents and bitangents
		tbCalc( v1, v2, v3, uv1, uv2, uv3, tangent1, tangent2, tangent3, bitangent1, bitangent2, bitangent3 );
	}

	Vector3f vco[3];
	Vector2f vuvo[3];
	Vector3f vno[3];
	Vector3f vto[3];
	Vector3f vbo[3];

	if ( mWindingOrder == WindingOrder::CCW ) {
		vco[0] = v1;
		vco[1] = v2;
		vco[2] = v3;

		vuvo[0] = uv1*uvScale;
		vuvo[1] = uv2*uvScale;
		vuvo[2] = uv3*uvScale;

		vno[0] = vn1;
		vno[1] = vn2;
		vno[2] = vn3;

		vto[0] = tangent1;
		vto[1] = tangent2;
		vto[2] = tangent3;

		vbo[0] = bitangent1;
		vbo[1] = bitangent2;
		vbo[2] = bitangent3;
	} else {
		vco[0] = v3;
		vco[1] = v2;
		vco[2] = v1;

		vuvo[0] = uv3*uvScale;
		vuvo[1] = uv2*uvScale;
		vuvo[2] = uv1*uvScale;

		vno[0] = vn3;
		vno[1] = vn2;
		vno[2] = vn1;

		vto[0] = tangent3;
		vto[1] = tangent2;
		vto[2] = tangent1;

		vbo[0] = bitangent3;
		vbo[1] = bitangent2;
		vbo[2] = bitangent1;
	}

	calcMirrorUVs( vuvo );

	for ( auto t = 0; t < 3; t++ ) {
		auto cv = t == 0 ? Color4f::RED : ( t == 1 ? Color4f::GREEN : Color4f::BLUE );
		mVdata.addTriangleVertex( vco[t], vuvo[t], vuvo[t], vno[t], vto[t], vbo[t], cv );
	}
}

void GeomData::resetMapping( uint64_t arraySize ) {
	wrapMappingCoords.clear();
	// fill the lengths vector with 0 to start with adding +1 extra length for not making it wrap to coordinate 0
	for ( uint64_t l = 0; l < arraySize + 1; l++ ) {
		wrapMappingCoords.push_back( Vector2f::ZERO );
	}
}

void GeomData::resetWrapMapping( const std::vector<float>& yWrapArray ) {
	wrapMappingCoords.clear();
	// fill the lengths vector with 0 to start with adding +1 extra length for not making it wrap to coordinate 0
	for ( auto y : yWrapArray ) {
		wrapMappingCoords.push_back( Vector2f( mapping.direction == MappingDirection::X_POS ? 0.0f : y, mapping.direction == MappingDirection::X_POS ? y : 0.0f ) );
	}
}

void GeomData::propagateWrapMapping( const GeomData* source ) {
	wrapMappingCoords.clear();
	WrapMappingCoords( source->WrapMappingCoords() );
}

void GeomData::updateWrapMapping( Vector3f vs[4], Vector2f vtcs[4], uint64_t m, uint64_t size ) {
	float lm1 = JMATH::distance( vs[0], vs[1] );
	float lm2 = JMATH::distance( vs[2], vs[3] );

	if ( mapping.direction == MappingDirection::X_POS ) {
		vtcs[0] = wrapMappingCoords[m];
		vtcs[1] = Vector2f( wrapMappingCoords[m].x() + lm1, wrapMappingCoords[m].y() );
		vtcs[2] = wrapMappingCoords[m + 1];
		vtcs[3] = Vector2f( wrapMappingCoords[m + 1].x() + lm2, wrapMappingCoords[m + 1].y() );
		wrapMappingCoords[m][0] += lm1;
		if ( m + 1 == size ) wrapMappingCoords[m + 1][0] += lm2;
	} else {
		vtcs[0] = wrapMappingCoords[m];
		vtcs[1] = Vector2f( wrapMappingCoords[m].x(), wrapMappingCoords[m].y() + lm1 );
		vtcs[2] = wrapMappingCoords[m + 1];
		vtcs[3] = Vector2f( wrapMappingCoords[m + 1].x(), wrapMappingCoords[m + 1].y() + lm2 );

		wrapMappingCoords[m][1] += lm1;
		if ( m + 1 == size ) wrapMappingCoords[m + 1][1] += lm2;
	}
}

void GeomData::planarMapping( const Vector3f& normal, const Vector3f vs[], Vector2f vtcs[], int numVerts ) {
	IndexPair pairMapping = normal.dominantPair();

	for ( int t = 0; t < numVerts; t++ ) {
		Vector2f tm = vs[t].pairMapped( pairMapping );
		vtcs[t] = tm + mapping.offset;
	}

	if ( mapping.direction == MappingDirection::X_POS ) {
		for ( int t = 0; t < numVerts; t++ ) vtcs[t].swizzle( 0, 1 );
	}

	if ( UnitMapping() ) {
		float minC = std::numeric_limits<float>::max();
		float maxC = std::numeric_limits<float>::lowest();
		for ( int t = 0; t < numVerts; t++ ) { if ( vtcs[t].x() < minC ) minC = vtcs[t].x(); }
		for ( int t = 0; t < numVerts; t++ ) { if ( vtcs[t].x() > maxC ) maxC = vtcs[t].x(); }
		float mappingLength = maxC - minC;
		for ( int t = 0; t < numVerts; t++ ) vtcs[t].setX( (( vtcs[t].x() - minC ) / mappingLength )*uvScaleInv.x());
		minC = std::numeric_limits<float>::max();
		maxC = std::numeric_limits<float>::lowest();
		for ( int t = 0; t < numVerts; t++ ) { if ( vtcs[t].y() < minC ) minC = vtcs[t].y(); }
		for ( int t = 0; t < numVerts; t++ ) { if ( vtcs[t].y() > maxC ) maxC = vtcs[t].y(); }
		mappingLength = maxC - minC;
		for ( int t = 0; t < numVerts; t++ ) vtcs[t].setY( ( ( vtcs[t].y() - minC ) / mappingLength ) * uvScaleInv.y
		() );
	}
}


void GeomData::calcMirrorUVs( Vector2f* uvs ) {
	if ( mapping.mirroring != MappingMirrorE::None ) {
		float minC = std::numeric_limits<float>::max();
		float maxC = std::numeric_limits<float>::lowest();
		switch ( mapping.mirroring ) {
			case MappingMirrorE::X_Only:
			for ( uint64_t t = 0; t < 3; t++ ) { if ( uvs[t].x() < minC ) minC = uvs[t].x(); }
			for ( uint64_t t = 0; t < 3; t++ ) { if ( uvs[t].x() > maxC ) maxC = uvs[t].x(); }
			for ( uint64_t t = 0; t < 3; t++ ) uvs[t].setX( minC + ( maxC - uvs[t].x() ) );
			break;
			case MappingMirrorE::Y_Only:
			for ( uint64_t t = 0; t < 3; t++ ) { if ( uvs[t].y() < minC ) minC = uvs[t].y(); }
			for ( uint64_t t = 0; t < 3; t++ ) { if ( uvs[t].y() > maxC ) maxC = uvs[t].y(); }
			for ( uint64_t t = 0; t < 3; t++ ) uvs[t].setY( minC + ( maxC - uvs[t].y() ) );
			break;
			case MappingMirrorE::BothWays:
			for ( uint64_t t = 0; t < 3; t++ ) { if ( uvs[t].x() < minC ) minC = uvs[t].x(); }
			for ( uint64_t t = 0; t < 3; t++ ) { if ( uvs[t].x() > maxC ) maxC = uvs[t].x(); }
			for ( uint64_t t = 0; t < 3; t++ ) uvs[t].setX( minC + ( maxC - uvs[t].x() ) );
			minC = std::numeric_limits<float>::max();
			maxC = std::numeric_limits<float>::lowest();
			for ( uint64_t t = 0; t < 3; t++ ) { if ( uvs[t].y() < minC ) minC = uvs[t].y(); }
			for ( uint64_t t = 0; t < 3; t++ ) { if ( uvs[t].y() > maxC ) maxC = uvs[t].y(); }
			for ( uint64_t t = 0; t < 3; t++ ) uvs[t].setY( minC + ( maxC - uvs[t].y() ) );
			break;
			default:
			break;
		}
	}
}

template <typename T>
void GeomData::updatePullMapping( const std::array<T, 4>& vs, std::array<Vector2f, 4>& vtcs ) {
	float lm1 = JMATH::distance( vs[0], vs[2] );
	float lm2 = JMATH::distance( vs[0], vs[1] );

	// this maps MappingDirection::X_POS
	vtcs[0] = Vector2f( pullMappingCoords.x(), pullMappingCoords.y() );
	vtcs[1] = Vector2f( pullMappingCoords.x() + lm2, pullMappingCoords.y() );
	vtcs[2] = Vector2f( pullMappingCoords.x(), pullMappingCoords.y() + lm1 );
	vtcs[3] = Vector2f( pullMappingCoords.x() + lm2, pullMappingCoords.y() + lm1 );

	if ( mapping.direction == MappingDirection::Y_POS ) {
		for ( uint64_t t = 0; t < 4; t++ ) vtcs[t].swizzle( 0, 1 );
	}
	pullMappingCoords.setX( pullMappingCoords.x() + distance( vs[0], vs[1] ) );
}

void GeomData::addFlatPoly( const std::array<Vector3f, 4>& verts, const Vector3f& normal, bool reverseIfTriangulated ) {
	addFlatPoly( 4, verts.data(), normal, reverseIfTriangulated );
}

void GeomData::addFlatPoly( size_t vsize, const std::vector<Vector2f>& verts, float z, const Vector3f& normal, bool reverseIfTriangulated ) {
	std::vector<Vector3f> verts3d;
	for ( auto& v : verts ) {
		verts3d.push_back( { v, z } );
	}
	addFlatPoly( vsize, verts3d.data(), normal, reverseIfTriangulated );
}

void GeomData::addFlatPoly( size_t vsize, const Vector3f* verts, const Vector3f& normal, bool reverseIfTriangulated ) {
	std::array<Vector3f, 4> vs;
	std::array<Vector2f, 4> vtcs;

	if ( vsize == 4 ) {
		vs[0] = ( verts[0] );
		vs[1] = ( verts[1] );
		vs[2] = ( verts[3] );
		vs[3] = ( verts[2] );
		planarMapping( normal, vs.data(), vtcs.data(), 4 );
		pushQuadSubDiv( vs, vtcs, normal );
	} else if ( vsize == 3 ) {
		for ( uint64_t t = 0; t < vsize - 2; t++ ) {
			vs[0] = ( verts[0] );
			vs[1] = ( verts[t + 1] );
			vs[2] = ( verts[t + 2] );
			planarMapping( normal, vs.data(), vtcs.data(), 3 );
			pushTriangleSubDiv( vs[0], vs[1], vs[2], vtcs[0], vtcs[1], vtcs[2], normal, normal, normal );
		}
	} else {
		Triangulator tri( verts, vsize, normal, 0.00001f );
		for ( auto& tvs : tri.get3dTrianglesList() ) {
			planarMapping( normal, tvs.data(), vtcs.data(), 3 );
			if ( reverseIfTriangulated ) {
				pushTriangleSubDiv( tvs[2], tvs[1], tvs[0], vtcs[2], vtcs[1], vtcs[0], normal, normal, normal );
			} else {
				pushTriangleSubDiv( tvs[0], tvs[1], tvs[2], vtcs[0], vtcs[1], vtcs[2], normal, normal, normal );
			}
		}
	}
}

void GeomData::addFlatPolyTriangulated( size_t vsize, const std::vector<Vector2f>& verts, float z, const Vector3f& normal, bool reverse ) {
	std::vector<Vector3f> verts3d;
	for ( auto& v : verts ) {
		verts3d.push_back( { v, z } );
	}
	addFlatPolyTriangulated( vsize, verts3d.data(), normal, reverse );
}

void GeomData::addFlatPolyTriangulated( size_t vsize, const Vector3f* verts, const Vector3f& normal, bool reverse ) {
//	Vector3f vs[4];
	Vector2f vtcs[4];

	Triangulator tri( verts, vsize, normal );
	for ( auto& tvs : tri.get3dTrianglesList() ) {
		planarMapping( normal, tvs.data(), vtcs, 3 );
		if ( reverse ) {
			pushTriangleSubDiv( tvs[2], tvs[1], tvs[0], vtcs[2], vtcs[1], vtcs[0], normal, normal, normal );
		} else {
			pushTriangleSubDiv( tvs[0], tvs[1], tvs[2], vtcs[0], vtcs[1], vtcs[2], normal, normal, normal );
		}
	}
}

void GeomData::addFlatPolyWithMapping( size_t vsize, const Vector3f* vs, const Vector2f* vts, const Vector3f& vn ) {
	if ( isCollinear( vs[0], vs[1], vs[2] ) ) return;

	Vector3f normal = vn.x() == std::numeric_limits<float>::lowest() ? normalFromPoints( vs ) : vn;
	if ( vsize == 4 ) {
		pushQuadSubDiv( std::array<Vector3f, 4>{ {vs[0], vs[1], vs[2], vs[3]} },
					   std::array<Vector2f, 4>{ {vts[0], vts[1], vts[2], vts[3]} },
						normal );
	} else {
		for ( uint64_t t = 0; t < vsize - 2; t++ ) {
			pushTriangleSubDiv( vs[0], vs[t + 1], vs[t + 2], vts[0], vts[t + 1], vts[t + 2], normal, normal, normal );
		}
	}
}

std::vector<Vector3f> GeomData::pullWindingOrderCheck( const std::vector<Vector2f>& verts, float zOffset ) {
	std::vector<Vector3f> v3f;
	int w = winding( verts );

	if ( WindingOrder::CW == w ) {
		for ( auto& v : verts ) {
			v3f.push_back( Vector3f( v, zOffset ) );
		}
	} else {
		auto nvSize = verts.size();
		for ( size_t q = 0; q < verts.size(); q++ ) v3f.push_back( { verts[nvSize - 1 - q], zOffset } );
	}

	return v3f;
}

void GeomData::pull( const std::vector<Vector2f>& verts, float height, float zOffset, PullFlags pullFlags ) {
	std::vector<Vector3f> v3f = pullWindingOrderCheck( verts, zOffset );

	pull( v3f, height, pullFlags );
}

void GeomData::pull( const std::vector<Vector3f>& verts, float height, PullFlags pullFlags ) {
	std::array<Vector2f, 4> vtcs;
	Vector3f normal = Vector3f::Z_AXIS;
	Vector3f posNormal = Vector3f::Z_AXIS;
	Vector3f negNormal = Vector3f::Z_AXIS;

	std::vector<Vector3f> vertsSane = verts;
	removeCollinear( vertsSane );

	if ( vertsSane.size() < 3 ) return;

	int numPos = 0;
	int numNeg = 0;
	size_t vsize = vertsSane.size();
	if ( vertsSane.size() > 2 ) {
		for ( size_t t = 0; t < vsize; t++ ) {
			Vector3f a = vertsSane[t];
			Vector3f b = vertsSane[getCircularArrayIndexUnsigned( t + 1, vsize )];
			Vector3f c = vertsSane[getCircularArrayIndexUnsigned( t + 2, vsize )];
			normal = normalize( crossProduct( a, b, c) );
			if ( dot( normal, Vector3f::Z_AXIS ) > 0.0f ) {
				posNormal = normal;
				numPos++;
			} else {
				negNormal = normal;
				numNeg++;
			}
		}
	} else {
		normal = normalize( crossProduct( vertsSane[0], vertsSane[1], vertsSane[0] + Vector3f{ unitRand(), unitRand(), 0.0f } ) );
	}

	normal = ( numPos > numNeg ) ? posNormal : negNormal;
	ASSERT( isValid(normal.x()));

	std::vector<Vector3f> nvertsSane = vertsSane;
	if ( mHasBevel ) {
		height -= mBevelAmount.z();
		height = max( 0.00001f, height );
	}

	Vector3f offset = normal * height;
	// Reset pull mapping
	pullMappingCoords = mapping.offset;
	std::array<Vector3f, 4> vss;

	if ( checkBitWiseFlag( pullFlags, PullFlags::Sides ) ) {
		for ( uint64_t t = 0; t < nvertsSane.size(); t++ ) {
			uint64_t index = t;
			uint32_t nextIndex = static_cast<uint32_t>( getCircularArrayIndexUnsigned( t + 1, nvertsSane.size() ));

			vss[1] = ( vertsSane[index] );
			vss[0] = ( vertsSane[nextIndex] );
			vss[3] = ( vertsSane[index] + offset );
			vss[2] = ( vertsSane[nextIndex] + offset );

			updatePullMapping( vss, vtcs );
			pushQuadSubDiv( vss, vtcs );
		}
	}

	if ( checkBitWiseFlag( pullFlags, PullFlags::Tops ) ) {
		int32_t nvSize = static_cast<int32_t>( nvertsSane.size() );
		std::unique_ptr<Vector3f[]> topvertsSane( new Vector3f[nvSize] );
		uint64_t l = 0;
		for ( size_t q = 0; q < static_cast<uint64_t>(nvSize); q++ ) {
			topvertsSane[l++] = nvertsSane[q];
		}

		if ( mHasBevel ) {
			// Draw bottom cap
			addFlatPoly( nvertsSane.size(), topvertsSane.get(), -normal, true );

			// Draw Top
			for ( size_t q = 0; q < nvertsSane.size(); q++ ) topvertsSane[q] = nvertsSane[q] + offset;
			std::unique_ptr<Vector3f[]> bevelvertsSane( new Vector3f[nvSize] );
			for ( int32_t t = 0; t < nvSize; t++ ) {
				Vector3f p1 = topvertsSane[getCircularArrayIndex( t - 1, nvSize )];
				Vector3f p2 = topvertsSane[getCircularArrayIndex( t, nvSize )];
				Vector3f p3 = topvertsSane[getCircularArrayIndex( t + 1, nvSize )];
				Vector3f inWardNorm = normalize( normalize( p2 - p1 ) + normalize( p2 - p3 ) );
				bevelvertsSane[t] = topvertsSane[t] + ( -inWardNorm * mBevelAmount.x() ) + ( normal * mBevelAmount.z() );
			}
			addFlatPoly( nvertsSane.size(), bevelvertsSane.get(), normal );
			for ( l = 0; l < nvertsSane.size(); l++ ) {
				uint64_t l1 = getCircularArrayIndexUnsigned( l + 1, static_cast<uint64_t>( nvertsSane.size() ) );
				Vector3f lane1[] = { topvertsSane[l], topvertsSane[l1], bevelvertsSane[l1], bevelvertsSane[l] };
				Vector3f slopeNormal = -normalFromPoints( lane1 );
				addFlatPoly( 4, lane1, slopeNormal );
			}
		} else {
			// Draw bottom cap
			addFlatPoly( nvertsSane.size(), topvertsSane.get(), -normal, true );
			// Draw top cap
			for ( size_t q = 0; q < nvertsSane.size(); q++ ) topvertsSane[q] = nvertsSane[nvSize -1 - q] + offset;
			addFlatPoly( nvertsSane.size(), topvertsSane.get(), normal );
		}
	}
}

void GeomData::transform( const Matrix4f& m ) {
	Matrix4f lRot = m;
	lRot.make3x3NormalizedRotationMatrix();

	for ( auto& v : mVdata.vcoords3d ) {
		v = m * v;
	}
	for ( auto& v : mVdata.vnormals3d ) {
		v = lRot * v;
		v = normalize( v );
	}
	for ( auto& v : mVdata.vtangents3d ) {
		v = lRot * v;
		v = normalize( v );
	}
	for ( auto& v : mVdata.vbinormals3d ) {
		v = lRot * v;
		v = normalize( v );
	}
}

void GeomData::bake( const Matrix4f& m, std::shared_ptr<GeomData>& ret ) {
	if ( ret ) {
		*ret = *this;
	} else {
		ret = std::shared_ptr<GeomData>( new GeomData( *this ) );
	}

	ret->transform( m );
}

void GeomData::scale( float /*factor*/ ) {
}

void GeomData::translate( const Vector3f& pos ) {
	Matrix4f m = Matrix4f::IDENTITY;
	m.translate( pos );

	for ( auto&& v : mVdata.vcoords3d ) {
		v = m.transform( v );
	}
}

void GeomData::rotate( float angle, const Vector3f& axis, const Vector3f& pivot ) {
	Matrix4f m = Matrix4f::IDENTITY;
	Matrix4f mr = Matrix4f::IDENTITY;

	m.translate( -pivot );
	m.rotate( angle, axis );
	mr.rotate( angle, axis );

	Vector4f vr;
	for ( auto&& v : mVdata.vcoords3d ) {
		vr = m.transform( v );
		v = vr.xyz();
		v += pivot;
	}

	for ( auto&& v : mVdata.vnormals3d ) {
		vr = mr.transform( v );
		v = normalize( vr.xyz() );
	}
	for ( auto&& v : mVdata.vtangents3d ) {
		vr = mr.transform( v );
		v = vr.xyz();
	}
	for ( auto&& v : mVdata.vbinormals3d ) {
		vr = mr.transform( v );
		v = vr.xyz();
	}
}

void GeomData::flipNormals() {
	for ( auto& v : mVdata.vnormals3d ) {
		v *= -1.0f;
	}
}

std::vector<Vector3f> GeomData::utilGenerateFlatBoxFromRect( const JMATH::Rect2f& bbox, float z ) {
	std::vector<Vector3f> bboxPoints;
	bboxPoints.push_back( Vector3f( bbox.bottomLeft(), z ) );
	bboxPoints.push_back( Vector3f( bbox.bottomRight(), z ) );
	bboxPoints.push_back( Vector3f( bbox.topRight(), z ) );
	bboxPoints.push_back( Vector3f( bbox.topLeft(), z ) );
	return bboxPoints;
}

std::vector<Vector3f> GeomData::utilGenerateFlatBoxFromSize( float width, float height, float z ) {
	std::vector<Vector3f> bboxPoints;
	// Clockwise
	bboxPoints.push_back( Vector3f( -width*0.5f, -height*0.5f, z ) );
	bboxPoints.push_back( Vector3f( -width*0.5f, height*0.5f, z ) );
	bboxPoints.push_back( Vector3f( width*0.5f, height*0.5f, z ) );
	bboxPoints.push_back( Vector3f( width*0.5f, -height*0.5f, z ) );
	return bboxPoints;
}

std::vector<Vector2f> GeomData::utilGenerateFlatRect( const Vector2f& size, const WindingOrderT wo, PivotPointPosition ppp, const Vector2f& /*pivot*/ ) {
	std::vector<Vector2f> fverts;

	if ( wo == WindingOrder::CW ) {
		fverts.push_back( Vector2f( size.x(), 0.0f ) );
		fverts.push_back( Vector2f( size.x(), size.y() ) );
		fverts.push_back( Vector2f( 0.0f, size.y() ) );
		fverts.push_back( Vector2f::ZERO );
	} else {
		fverts.push_back( Vector2f::ZERO );
		fverts.push_back( Vector2f( 0.0f, size.y() ) );
		fverts.push_back( Vector2f( size.x(), size.y() ) );
		fverts.push_back( Vector2f( size.x(), 0.0f ) );
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

void GeomData::Bevel( const Vector3f & bevelAmount ) {
	mBevelAmount = bevelAmount;
	mHasBevel = true;
}

Vector3f GeomData::Bevel() const {
	return mBevelAmount;
}

void GeomData::checkBaricentricCoordsOn( const Vector3f& i, int32_t pIndexStart, int32_t pIndexEnd, int32_t& pIndex, float& u, float& v ) {
	mVdata.checkBaricentricCoordsOn( i, pIndexStart, pIndexEnd, pIndex, u, v );
}

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

void GeomData::debugPrint() {
	for ( uint64_t t = 0; t < mVdata.vcoords3d.size(); t++ ) {
		LOGI( "V%d: [%.3f, %.3f, %.3f]", t, mVdata.vcoords3d[t].x(), mVdata.vcoords3d[t].y(), mVdata.vcoords3d[t].z() );
	}
}

void GeomData::setWindingOrderFlagOnly( const WindingOrderT _wo ) {
	mWindingOrder = _wo;
}

WindingOrderT GeomData::getWindingOrder() const {
	return mWindingOrder;
}

GeomData::~GeomData() {

}

//void VData::allocateEmptySHData() {
//	vSHCoeffsR = std::vector<Matrix3f>( vcoords3d.size() );
//	vSHCoeffsG = std::vector<Matrix3f>( vcoords3d.size() );
//	vSHCoeffsB = std::vector<Matrix3f>( vcoords3d.size() );
//
//	for ( size_t i = 0; i < vcoords3d.size(); i++ ) {
//		vSHCoeffsR[i] = Matrix3f::ONE;
//		vSHCoeffsG[i] = Matrix3f::ONE;
//		vSHCoeffsB[i] = Matrix3f::ONE;
//	}
//}

void VData::clear() {
	vcoords3d.clear();
	vnormals3d.clear();
	vtangents3d.clear();
	vbinormals3d.clear();
	vIndices.clear();
	vUVs.clear();
	vUV2s.clear();
//	vSHCoeffsR.clear();
//	vSHCoeffsG.clear();
//	vSHCoeffsB.clear();
	vColor.clear();
}

void VData::fill( const PolyStruct& ps ) {

	for ( int q = 0; q < ps.numIndices; q++ ) {
	    auto idx = ps.indices[q];
		vcoords3d.push_back( ps.verts[idx] );
		vUVs.push_back( ps.uvs[q] );
		vUV2s.push_back( ps.uv2s[q] );
		vnormals3d.push_back( ps.normals[q] );
		vtangents3d.push_back( ps.tangents[q] );
		vbinormals3d.push_back( ps.binormals[q] );
//		vSHCoeffsR.push_back( Matrix3f::ONE );
//		vSHCoeffsG.push_back( Matrix3f::ONE );
//		vSHCoeffsB.push_back( Matrix3f::ONE );
		vColor.push_back( ps.colors[q] );
		vIndices.push_back( q );
	}
}

void VData::fillIndices( const std::vector<int>& _indices ) {
	vIndices = _indices;
//	for ( int q = 0; q < _indices.size(); q+=3 ) {
//		vIndices[q+0] = _indices[q+1];
//		vIndices[q+1] = _indices[q+0];
//		vIndices[q+2] = _indices[q+2];
//	}
}

void VData::fillCoors3d( const std::vector<Vector3f>& _verts ) {
	vcoords3d = _verts;
//	vSHCoeffsR.clear();
//	vSHCoeffsG.clear();
//	vSHCoeffsB.clear();
	for ( size_t t = 0; t < _verts.size(); t++ ) {
//		vSHCoeffsR.push_back( Matrix3f::ONE );
//		vSHCoeffsG.push_back( Matrix3f::ONE );
//		vSHCoeffsB.push_back( Matrix3f::ONE );
		vColor.push_back( Vector4f::ONE );
	}
}

void VData::fillUV( const std::vector<Vector2f>& _uvs, uint32_t _index ) {
	ASSERT( _index < 2 );
	if ( _index == 0 ) {
		vUVs = _uvs;
	} else {
		vUV2s = _uvs;
	}
}

void VData::fillNormals( const std::vector<Vector3f>& _normals ) {
	vnormals3d = _normals;
}

void VData::fillTangets( const std::vector<Vector3f>& _tangents ) {
	vtangents3d = _tangents;
}

void VData::fillBinormal( const std::vector<Vector3f>& _binormals ) {
	vbinormals3d = _binormals;
}

void VData::fillColors( const std::vector<Vector4f>& _colors ) {
	vColor = _colors;
}

void VData::changeHandness() {
	for ( size_t i = 0; i < vcoords3d.size(); i++ ) {
		vcoords3d[i].swizzle( 1, 2 );
		vnormals3d[i].swizzle( 1, 2 );
		vtangents3d[i].swizzle( 1, 2 );
		vbinormals3d[i].swizzle( 1, 2 );
	}
}

void VData::sanitizeUVMap() {
	if ( vUVs.size() == 0 ) {
		for ( const auto& v : vcoords3d ) {
			vUVs.emplace_back( v.dominantVector2() );
			vUV2s.emplace_back( v.dominantVector2() );
		}
	}
}

void VData::calcBinormal() {
    if ( vtangents3d.size() == 0 ) {
        vtangents3d = vnormals3d;
        vbinormals3d = vnormals3d;
        return;
    }
	vbinormals3d = vtangents3d;

	ASSERT( primitive == PRIMITIVE_TRIANGLES );
	Vector3f b1 = Vector3f::ZERO;
	Vector3f b2 = Vector3f::ZERO;
	Vector3f b3 = Vector3f::ZERO;
	for ( size_t i = 0; i < vIndices.size(); i+=3 ) {
		auto i0 = vIndices[i+0];
		auto i1 = vIndices[i+1];
		auto i2 = vIndices[i+2];
		tbCalc( vcoords3d[i0], vcoords3d[i1], vcoords3d[i2],
				vUVs[i0], vUVs[i1], vUVs[i2],
				vtangents3d[i0], vtangents3d[i1], vtangents3d[i2],
				b1, b2, b3 );
		vbinormals3d[i0] = b1;
		vbinormals3d[i1] = b2;
		vbinormals3d[i2] = b3;
	}
}

void VData::allocateSpaceForVertices( const int _numVerts ) {
	vIndices.resize( _numVerts );
	vcoords3d.resize( _numVerts );
	vnormals3d.resize( _numVerts );
	vtangents3d.resize( _numVerts );
	vbinormals3d.resize( _numVerts );
	vUVs.resize( _numVerts );
	vUV2s.resize( _numVerts );
//	vSHCoeffsR.resize( _numVerts );
//	vSHCoeffsG.resize( _numVerts );
//	vSHCoeffsB.resize( _numVerts );
}

void VData::changeWindingOrder() {
	ASSERT( vIndices.size() % 3 == 0 );

	for ( unsigned int t = 0; t < vIndices.size(); t += 3 ) {
		std::swap( vIndices[t + 0], vIndices[t + 2] );
		std::swap( vcoords3d[t + 0], vcoords3d[t + 2] );
		std::swap( vnormals3d[t + 0], vnormals3d[t + 2] );
		std::swap( vtangents3d[t + 0], vtangents3d[t + 2] );
		std::swap( vbinormals3d[t + 0], vbinormals3d[t + 2] );
	}
}

void VData::mirrorFlip( WindingOrderT wow, WindingOrderT woh, const Rect2f& bbox ) {
	Vector3f flipXNormal = Vector3f( -1.0f, 1.0f, 1.0f );
	Vector3f flipYNormal = Vector3f( 1.0f, -1.0f, 1.0f );
	for ( unsigned int index = 0; index < vcoords3d.size(); index++ ) {
		if ( wow == WindingOrder::CW ) {
			vcoords3d[index][0] = bbox.width() - vcoords3d[index][0];
			vnormals3d[index] *= flipXNormal;
			vtangents3d[index] *= flipXNormal;
			vbinormals3d[index] *= flipXNormal;
		}
		if ( woh == WindingOrder::CW ) {
			vcoords3d[index][1] = bbox.height() - vcoords3d[index][1];
			vnormals3d[index] *= flipYNormal;
			vtangents3d[index] *= flipYNormal;
			vbinormals3d[index] *= flipYNormal;
		}
	}
	if ( ( wow == WindingOrder::CW || woh == WindingOrder::CW ) && ( wow == WindingOrder::CCW || woh == WindingOrder::CCW ) ) {
		changeWindingOrder();
	}
}

void VData::checkBaricentricCoordsOn( const Vector3f& i, int32_t pIndexStart, int32_t pIndexEnd, int32_t& pIndex,
									  float& u, float& v ) {
	Vector3f a = Vector3f::ZERO;
	Vector3f b = Vector3f::ZERO;
	Vector3f c = Vector3f::ZERO;
	int32_t vi = pIndexStart;
	int32_t possibleIndices[256];
	int32_t pii = 0;
	pIndex = pIndexStart;
	u = 0.0f;
	v = 0.0f;
	int passes = 3;
	if ( pIndexEnd - pIndexStart > 3 ) {
		// Shortest distance from i
		float minD = std::numeric_limits<float>::max();
		float d = 0.0f;
		for ( int q = pIndexStart; q < pIndexEnd; q += 3 ) {
			d = distance( i, vcoords3d[q] );
			if ( d < minD ) {
				pii = 0;
				minD = d;
			} else if ( isScalarEqual( d - minD, 0.0f ) ) {
				possibleIndices[pii++] = q;
			}

			d = distance( i, vcoords3d[q + 1] );
			if ( d < minD ) {
				pii = 0;
				minD = d;
			} else if ( isScalarEqual( d - minD, 0.0f ) ) {
				possibleIndices[pii++] = q;
			}

			d = distance( i, vcoords3d[q + 2] );
			if ( d < minD ) {
				pii = 0;
				minD = d;
			} else if ( isScalarEqual( d - minD, 0.0f ) ) {
				possibleIndices[pii++] = q;
			}
		}

		for ( int h = 0; h < pii; h++ ) {
			vi = possibleIndices[h];
			passes = 0;
			for ( int q = 0; q < 3; q++ ) {
				a = vcoords3d[vi + q];
				b = vcoords3d[q == 2 ? vi : vi + q + 1];
				c = a + vnormals3d[vi + q];
				Plane3f pplane = { a,b,c };
				if ( pplane.checkSide( i ) > 0.0f ) continue;
				++passes;
			}
			if ( passes == 3 ) break;
		}
	}
	if ( passes == 3 ) {
		a = vcoords3d[vi];
		b = vcoords3d[vi + 1];
		c = vcoords3d[vi + 2];

		Vector3f crossCB = cross( c - b, i - b );
		u = length( crossCB );
		Vector3f crossAC = cross( a - c, i - c );
		v = length( crossAC );
		pIndex = vi;
		return;
	}
}

void VData::addTriangleVertex( const Vector3f& _vc, const Vector2f& _uv, const Vector2f& _uv2, const Vector3f& _vn,
							   const Vector3f& _vt, const Vector3f& _vb, const Vector3f& _v8 ) {

	vcoords3d.push_back( _vc );
	vUVs.push_back( _uv );
	vUV2s.push_back( _uv2 );
	vnormals3d.push_back( _vn );
	vtangents3d.push_back( _vt );
	vbinormals3d.push_back( _vb );
//	vSHCoeffsR.push_back( Matrix3f::ONE );
//	vSHCoeffsG.push_back( Matrix3f::ONE );
//	vSHCoeffsB.push_back( Matrix3f::ONE );
    vColor.push_back( _v8 );
	vIndices.push_back( vIndices.size() );
}

void VData::add( int32_t _i, const Vector3f& _v, const Vector3f& _n, const Vector2f& _uv, const Vector2f& _uv2,
				 const Vector3f& _t, const Vector3f& _b, const Vector4f& _c ) {
	vcoords3d.push_back( _v );
	vIndices.push_back( _i );
	vUVs.push_back( _uv );
	vUV2s.push_back( _uv2 );
	vnormals3d.push_back( _n );
	vtangents3d.push_back( _t );
	vbinormals3d.push_back( _b );
//	vSHCoeffsR.push_back( Matrix3f::ONE );
//	vSHCoeffsG.push_back( Matrix3f::ONE );
//	vSHCoeffsB.push_back( Matrix3f::ONE );
    vColor.push_back( _c );
}

void VData::swapIndicesWinding( Primitive _pr ) {
	switch ( _pr ) {
		case PRIMITIVE_TRIANGLES:
			for ( size_t i = 0; i < vIndices.size(); i+=3 ) {
				swapVectorPair( vIndices, i + 1, i + 2 );
			}
			break;
		case PRIMITIVE_TRIANGLE_STRIP:
			break;
		case PRIMITIVE_TRIANGLE_FAN:
			break;
		default:
			ASSERT(0);
	}
}

GeomDeserializeDependencies GeomData::gatherDependencies( std::shared_ptr<DeserializeBin> reader ) {
	uint32_t numEntries = 0;
	uint32_t dependencyTag = 0;
	std::string dependencyName;
	GeomDeserializeDependencies ret;

	reader->read( numEntries );
	while ( numEntries > 0 ) {
		reader->read( dependencyTag );
		for ( uint32_t q = 0; q < numEntries; q++ ) {
			reader->read( dependencyName );
			switch ( dependencyTag ) {
				case dependecyTagTexture:
					ret.textureDeps.emplace_back( dependencyName );
					break;
				case dependecyTagMaterial:
					ret.materialDeps.emplace_back( dependencyName );
					break;
			}
		}
		reader->read( numEntries );
	}

	return ret;
}

uint64_t GeomData::Version() { return NodeVersion(1000); }
