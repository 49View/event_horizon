//
// Created by Dado on 2019-03-31.
//

#include "v_data.hpp"

#include <core/soa_utils.h>
#include <core/math/plane3f.h>

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

void VData::fillIndices( const std::vector<int32_t>& _indices ) {
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

void VData::fillNormals( const std::vector<Vector3f>& _normals, bool _bInvert ) {
    vnormals3d = _normals;
    if ( _bInvert ) {
        for ( auto& v : vnormals3d ) v*=-1.0f;
    }
}

void VData::fillTangets( const std::vector<Vector3f>& _tangents, bool _bInvert ) {
    vtangents3d = _tangents;
    if ( _bInvert ) {
        for ( auto& v : vtangents3d ) v*=-1.0f;
    }
}

void VData::fillBinormal( const std::vector<Vector3f>& _binormals, bool _bInvert ) {
    vbinormals3d = _binormals;
    if ( _bInvert ) {
        for ( auto& v : vbinormals3d ) v*=-1.0f;
    }
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

    BBox3d().expand(_vc);
    vcoords3d.push_back( _vc );
    vUVs.push_back( _uv );
    vUV2s.push_back( _uv2 );
    vnormals3d.push_back( _vn );
    vtangents3d.push_back( _vt );
    vbinormals3d.push_back( _vb );
//	vSHCoeffsR.push_back( Matrix3f::ONE );
//	vSHCoeffsG.push_back( Matrix3f::ONE );
//	vSHCoeffsB.push_back( Matrix3f::ONE );
    vColor.emplace_back( _v8 );
    vIndices.push_back( vIndices.size() );
}

void VData::add( int32_t _i, const Vector3f& _v, const Vector3f& _n, const Vector2f& _uv, const Vector2f& _uv2,
                 const Vector3f& _t, const Vector3f& _b, const Vector4f& _c ) {
    BBox3d().expand(_v);
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

void VData::flipNormals() {
    for ( auto& v : vnormals3d ) {
        v *= -1.0f;
    }
}

size_t VData::numIndices() const { return vIndices.size(); }

size_t VData::numVerts() const { return vcoords3d.size(); }

const std::vector<int32_t>& VData::getVIndices() const {
    return vIndices;
}

const std::vector<Vector3f>& VData::getVcoords3d() const {
    return vcoords3d;
}

const std::vector<Vector3f>& VData::getVnormals3d() const {
    return vnormals3d;
}

const std::vector<Vector3f>& VData::getVtangents3d() const {
    return vtangents3d;
}

const std::vector<Vector3f>& VData::getVbinormals3d() const {
    return vbinormals3d;
}

const std::vector<Vector2f>& VData::getVUVs() const {
    return vUVs;
}

const std::vector<Vector2f>& VData::getVUV2s() const {
    return vUV2s;
}

const std::vector<Vector4f>& VData::getVColor() const {
    return vColor;
}

void VData::setVIndices( const size_t _index, const int32_t& _value ) {
    vIndices[_index] = _value;
}

void VData::setVcoords3d( const size_t _index, const Vector3f& _value ) {
    vcoords3d[_index] = _value;
}

void VData::setVnormals3d( const size_t _index, const Vector3f& _value ) {
    vnormals3d[_index] = _value;
}

void VData::setVtangents3d( const size_t _index, const Vector3f& _value ) {
    vtangents3d[_index] = _value;
}

void VData::setVbinormals3d( const size_t _index, const Vector3f& _value ) {
    vbinormals3d[_index] = _value;
}

void VData::setVUVs( const size_t _index, const Vector2f& _value ) {
    vUVs[_index] = _value;
}

void VData::setVUV2s( const size_t _index, const Vector2f& _value ) {
    vUV2s[_index] = _value;
}

void VData::setVColor( const size_t _index, const Vector4f& _value ) {
    vColor[_index] = _value;
}

const Vector3f& VData::getMin() const {
    return BBox3d().minPoint();
}

void VData::setMin( const Vector3f& min ) {
    BBox3d().setMinPoint(min);
}

const Vector3f& VData::getMax() const {
    return BBox3d().maxPoint();
}

void VData::setMax( const Vector3f& max ) {
    BBox3d().setMaxPoint(max);
}

Primitive VData::getPrimitive() const {
    return primitive;
}

void VData::setPrimitive( Primitive _primitive ) {
    VData::primitive = _primitive;
}
