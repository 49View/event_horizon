//
// Created by Dado on 2019-04-10.
//

#include "poly_services.hpp"
#include <array>
#include <core/math/vector3f.h>
#include <core/math/quad_vertices.h>
#include <poly/triangulator.hpp>
#include <poly/poly_helper.h>

void pushQuad( VDataSP vdata, const std::array<Vector3f, 4>& vs, const std::array<Vector2f, 4>& vts,
               const Vector3f& vn, GeomMappingData& m );
void pushQuad( VDataSP vdata, const QuadVertices3& vs, const QuadVertices2& vts, GeomMappingData& m );
void pushQuad( VDataSP vdata, const QuadVertices3& vs, const QuadVertices2& vts,
               const QuadVertices3& vns, GeomMappingData& m );

void pushQuadSubDiv( VDataSP vdata, const std::array<Vector3f, 4>& vss, const std::array<Vector2f, 4>& vtcs,
                     GeomMappingData& m,
                     const Vector3f& vn = Vector3f::HUGE_VALUE_NEG );
void pushQuadSubDiv( VDataSP vdata, const std::array<Vector3f, 4>& vss, const std::array<Vector2f, 4>& vtcs,
                     const std::array<Vector3f, 4>& vns, GeomMappingData& m );

void pushTriangleSubDiv( VDataSP vdata, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3,
                         const Vector2f& uv1, const Vector2f& uv2, const Vector2f& uv3,
                         const Vector3f& vn1, const Vector3f& vn2, const Vector3f& vn3, GeomMappingData& m );

void pushTriangle( VDataSP vdata,
                   const Vector3f& v1, const Vector3f& v2, const Vector3f& v3,
                   const Vector2f& uv1, const Vector2f& uv2, const Vector2f& uv3,
                   const Vector3f& vn1, const Vector3f& vn2, const Vector3f& vn3,
                   GeomMappingData& m,
                   const Vector3f& vt1 = Vector3f::ZERO, const Vector3f& vt2 = Vector3f::ZERO,
                   const Vector3f& vt3 = Vector3f::ZERO,
                   const Vector3f& vb1 = Vector3f::ZERO, const Vector3f& vb2 = Vector3f::ZERO,
                   const Vector3f& vb3 = Vector3f::ZERO );
void pushTriangle( VDataSP vdata, const std::vector<Vector3f>& vs, const std::vector<Vector2f>& vuv,
                   const std::vector<Vector3f>& vn, GeomMappingData& m );
void pushTriangleSubDivRec( VDataSP vdata,
                            const Vector3f& v1, const Vector3f& v2, const Vector3f& v3,
                            const Vector2f& uv1, const Vector2f& uv2, const Vector2f& uv3,
                            const Vector3f& vn1, const Vector3f& vn2, const Vector3f& vn3,
                            int triSubDiv, GeomMappingData& m );


Vector3f normalFromPoints( const Vector3f* vs, GeomMappingData& m ) {
    Vector3f normal = m.windingOrder == WindingOrder::CCW ? crossProduct( vs[0], vs[1], vs[2] ) :
                                                           crossProduct( vs[0], vs[2], vs[1] );
    normal = normalize( normal );
    return normal;
}

auto pullWindingOrderCheck( const std::vector<Vector2f>& verts, float zOffset ) {
    std::vector<Vector3f> v3f;
    int w = winding( verts );

    if ( WindingOrder::CW == w ) {
        for ( auto& v : verts ) {
            v3f.emplace_back( v, zOffset );
        }
    } else {
        auto nvSize = verts.size();
        for ( size_t q = 0; q < verts.size(); q++ ) v3f.emplace_back( verts[nvSize - 1 - q], zOffset );
    }

    return v3f;
}

namespace MappingServices {

void doNotScaleMapping( GeomMappingData& m ) {
    m.bDoNotScaleMapping = true;
    m.fuvScale = 1.0f;
    m.uvScale = { m.fuvScale, -m.fuvScale };
    m.uvScaleInv = reciprocal( m.uvScale );
}


void resetMapping( GeomMappingData& m, uint64_t arraySize ) {
    m.wrapMappingCoords.clear();
    // fill the lengths vector with 0 to start with adding +1 extra length for not making it wrap to coordinate 0
    for ( uint64_t l = 0; l < arraySize + 1; l++ ) {
        m.wrapMappingCoords.push_back( Vector2f::ZERO );
    }
}

void resetWrapMapping( GeomMappingData& m, const std::vector<float>& yWrapArray ) {
    m.wrapMappingCoords.clear();
    // fill the lengths vector with 0 to start with adding +1 extra length for not making it wrap to coordinate 0
    for ( auto y : yWrapArray ) {
        m.wrapMappingCoords.emplace_back( m.direction == MappingDirection::X_POS ? 0.0f : y,
                                          m.direction == MappingDirection::X_POS ? y : 0.0f );
    }
}

void propagateWrapMapping( GeomMappingData& m, const VData* source ) {
    ASSERT(0);
//    m.wrapMappingCoords.clear();
//    WrapMappingCoords( source->WrapMappingCoords() );
}

void updateWrapMapping( GeomMappingData& m, Vector3f vs[4], Vector2f vtcs[4], uint64_t mi, uint64_t size ) {
    float lm1 = JMATH::distance( vs[0], vs[1] );
    float lm2 = JMATH::distance( vs[2], vs[3] );

    if ( m.direction == MappingDirection::X_POS ) {
        vtcs[0] = m.wrapMappingCoords[mi];
        vtcs[1] = Vector2f( m.wrapMappingCoords[mi].x() + lm1, m.wrapMappingCoords[mi].y() );
        vtcs[2] = m.wrapMappingCoords[mi + 1];
        vtcs[3] = Vector2f( m.wrapMappingCoords[mi + 1].x() + lm2, m.wrapMappingCoords[mi + 1].y() );
        m.wrapMappingCoords[mi][0] += lm1;
        if ( mi + 1 == size ) m.wrapMappingCoords[mi + 1][0] += lm2;
    } else {
        vtcs[0] = m.wrapMappingCoords[mi];
        vtcs[1] = Vector2f( m.wrapMappingCoords[mi].x(), m.wrapMappingCoords[mi].y() + lm1 );
        vtcs[2] = m.wrapMappingCoords[mi + 1];
        vtcs[3] = Vector2f( m.wrapMappingCoords[mi + 1].x(), m.wrapMappingCoords[mi + 1].y() + lm2 );

        m.wrapMappingCoords[mi][1] += lm1;
        if ( mi + 1 == size ) m.wrapMappingCoords[mi + 1][1] += lm2;
    }
}

template <typename T>
void updatePullMapping( GeomMappingData& m, const std::array<T, 4>& vs, std::array<Vector2f, 4>& vtcs ) {
    float lm1 = JMATH::distance( vs[0], vs[2] );
    float lm2 = JMATH::distance( vs[0], vs[1] );

    // this maps MappingDirection::X_POS
    vtcs[0] = Vector2f( m.pullMappingCoords.x(),       m.pullMappingCoords.y() );
    vtcs[1] = Vector2f( m.pullMappingCoords.x() + lm2, m.pullMappingCoords.y() );
    vtcs[2] = Vector2f( m.pullMappingCoords.x(),       m.pullMappingCoords.y() + lm1 );
    vtcs[3] = Vector2f( m.pullMappingCoords.x() + lm2, m.pullMappingCoords.y() + lm1 );

    if ( m.direction == MappingDirection::Y_POS ) {
        for ( uint64_t t = 0; t < 4; t++ ) vtcs[t].swizzle( 0, 1 );
    }
    m.pullMappingCoords.setX( m.pullMappingCoords.x() + distance( vs[0], vs[1] ) );
}

void planarMapping( GeomMappingData& m, const Vector3f& normal, const Vector3f vs[], Vector2f vtcs[], int numVerts ) {
    IndexPair pairMapping = normal.dominantPair();

    for ( int t = 0; t < numVerts; t++ ) {
        Vector2f tm = vs[t].pairMapped( pairMapping );
        vtcs[t] = tm + m.offset;
    }

    if ( m.direction == MappingDirection::X_POS ) {
        for ( int t = 0; t < numVerts; t++ ) vtcs[t].swizzle( 0, 1 );
    }

    if ( m.bUnitMapping ) {
        float minC = std::numeric_limits<float>::max();
        float maxC = std::numeric_limits<float>::lowest();
        for ( int t = 0; t < numVerts; t++ ) { if ( vtcs[t].x() < minC ) minC = vtcs[t].x(); }
        for ( int t = 0; t < numVerts; t++ ) { if ( vtcs[t].x() > maxC ) maxC = vtcs[t].x(); }
        float mappingLength = maxC - minC;
        for ( int t = 0; t < numVerts; t++ ) vtcs[t].setX( (( vtcs[t].x() - minC ) / mappingLength )*m.uvScaleInv.x());
        minC = std::numeric_limits<float>::max();
        maxC = std::numeric_limits<float>::lowest();
        for ( int t = 0; t < numVerts; t++ ) { if ( vtcs[t].y() < minC ) minC = vtcs[t].y(); }
        for ( int t = 0; t < numVerts; t++ ) { if ( vtcs[t].y() > maxC ) maxC = vtcs[t].y(); }
        mappingLength = maxC - minC;
        for ( int t = 0; t < numVerts; t++ ) vtcs[t].setY( ( ( vtcs[t].y() - minC ) / mappingLength ) * m.uvScaleInv.y
                    () );
    }
}

void calcMirrorUVs( GeomMappingData& m, Vector2f* uvs ) {
    if ( m.mirroring != MappingMirrorE::None ) {
        float minC = std::numeric_limits<float>::max();
        float maxC = std::numeric_limits<float>::lowest();
        switch ( m.mirroring ) {
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

}

void pushTriangleSubDivRec( VDataSP vdata, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3,
                            const Vector2f& uv1, const Vector2f& uv2, const Vector2f& uv3,
                            const Vector3f& vn1, const Vector3f& vn2, const Vector3f& vn3,
                            int triSubDiv, GeomMappingData& m ) {
    if ( triSubDiv == 0 ) {
        pushTriangle( vdata, v1, v2, v3, uv1, uv2, uv3, vn1, vn2, vn3, m );
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

        pushTriangleSubDivRec( vdata, v1, vi1, vi3, uv1, uvi1, uvi3, vn1, vni1, vni3,    triSubDiv - 1, m );
        pushTriangleSubDivRec( vdata, vi1, v2, vi2, uvi1, uv2, uvi2, vni1, vn2, vni2,    triSubDiv - 1, m );
        pushTriangleSubDivRec( vdata, vi3, vi2, v3, uvi3, uvi2, uv3, vni3, vni2, vn3,    triSubDiv - 1, m );
        pushTriangleSubDivRec( vdata, vi1, vi2, vi3, uvi1, uvi2, uvi3, vni1, vni2, vni3, triSubDiv - 1, m );
    }
}

void pushTriangleSubDiv( VDataSP vdata, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3,
                         const Vector2f& uv1, const Vector2f& uv2, const Vector2f& uv3,
                         const Vector3f& vn1, const Vector3f& vn2, const Vector3f& vn3, GeomMappingData& m ) {
    if ( m.subdivAccuracy != accuracyNone ) {
        float d1 = distance( v1, v2 );
        d1 = min( d1, distance( v2, v3 ) );
        d1 = min( d1, distance( v1, v3 ) );

        int triSubDiv = static_cast<int>( max( 1, sqrt( d1 / m.subdivAccuracy ) ) );

        pushTriangleSubDivRec( vdata, v1, v2, v3, uv1, uv2, uv3, vn1, vn2, vn3, triSubDiv, m );
    } else {
        pushTriangle( vdata, v1, v2, v3, uv1, uv2, uv3, vn1, vn2, vn3, m );
    }
}

void pushTriangle( VDataSP vdata, const std::vector<Vector3f>& vs, const std::vector<Vector2f>& vuv,
                   const std::vector<Vector3f>& vn, GeomMappingData& m ) {
    pushTriangle( vdata, vs[0], vs[1], vs[2], vuv[0], vuv[1], vuv[2], vn[0], vn[1], vn[2], m );
}

void pushQuad( VDataSP vdata, const std::array<Vector3f, 4>& vs, const std::array<Vector2f, 4>& vts,
              const std::array<Vector3f, 4>& vns, GeomMappingData& m ) {
    pushTriangle( vdata, vs[0], vs[1], vs[2], vts[0], vts[1], vts[2], vns[0], vns[1], vns[2], m );
    pushTriangle( vdata, vs[2], vs[1], vs[3], vts[2], vts[1], vts[3], vns[2], vns[1], vns[3], m );
}

void pushQuad( VDataSP vdata, const std::array<Vector3f, 4>& vs, const std::array<Vector2f, 4>& vts,
               const Vector3f& vn, GeomMappingData& m ) {
    Vector3f normal = vn;
    pushTriangle( vdata, vs[0], vs[1], vs[2], vts[0], vts[1], vts[2], normal, normal, normal, m );
    pushTriangle( vdata, vs[2], vs[1], vs[3], vts[2], vts[1], vts[3], normal, normal, normal, m );
}

void pushQuad( VDataSP vdata, const QuadVertices3& vs, const QuadVertices2& vts, GeomMappingData& m ) {
    Vector3f normal = m.windingOrder == WindingOrder::CCW ? crossProduct( vs[0], vs[1], vs[2] ) :
                                                            crossProduct( vs[0], vs[2], vs[1] );
    normal = normalize( normal );
    pushTriangle( vdata, vs[0], vs[1], vs[2], vts[0] * m.uvScaleInv, vts[1] * m.uvScaleInv, vts[2] * m.uvScaleInv,
                  normal, normal, normal, m );
    pushTriangle( vdata, vs[2], vs[1], vs[3], vts[2] * m.uvScaleInv, vts[1] * m.uvScaleInv, vts[3] * m.uvScaleInv,
                  normal, normal, normal, m );
}

void pushQuad( VDataSP vdata, const QuadVertices3& vs, const QuadVertices2& vts, const QuadVertices3& vns,
               GeomMappingData& m ) {
    pushTriangle( vdata, vs[0], vs[1], vs[2], vts[0] * m.uvScaleInv, vts[1] * m.uvScaleInv, vts[2] * m.uvScaleInv,
                  vns[0], vns[1], vns[2], m );
    pushTriangle( vdata, vs[2], vs[1], vs[3], vts[2] * m.uvScaleInv, vts[1] * m.uvScaleInv, vts[3] * m.uvScaleInv,
                  vns[2], vns[1], vns[3], m );
}

void pushQuadSubDiv( VDataSP vdata, const std::array<Vector3f, 4>& vss, const std::array<Vector2f, 4>& vtcs,
                     GeomMappingData& m, const Vector3f& vn ) {
    Vector3f normal = vn.x() == std::numeric_limits<float>::lowest() ? normalFromPoints( vss.data(), m ) : vn;
    auto quads = quadSubDiv( vss, vtcs, normal, m.subdivAccuracy );
    for ( const auto& quad : quads ) {
        pushQuad( vdata, quad.vcoords, quad.uvs, normal, m );
    }
}

void pushQuadSubDiv( VDataSP vdata, const std::array<Vector3f, 4>& vss, const std::array<Vector2f, 4>& vtcs,
                     const std::array<Vector3f, 4>& vns, GeomMappingData& m ) {
    auto quads = quadSubDiv( vss, vtcs, vns, m.subdivAccuracy );
    for ( const auto& quad : quads ) {
        pushQuad( vdata, quad.vcoords, quad.uvs, quad.normals, m );
    }
}

void pushTriangle( VDataSP vdata,
                   const Vector3f& v1, const Vector3f& v2, const Vector3f& v3,
                   const Vector2f& uv1, const Vector2f& uv2, const Vector2f& uv3,
                   const Vector3f& vn1, const Vector3f& vn2, const Vector3f& vn3,
                   GeomMappingData& m,
                   const Vector3f& vt1, const Vector3f& vt2, const Vector3f& vt3,
                   const Vector3f& vb1, const Vector3f& vb2, const Vector3f& vb3 ) {
    //	ASSERT(!isCollinear(v1, v2, v3));

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

    if ( m.windingOrder == WindingOrder::CCW ) {
        vco[0] = v1;
        vco[1] = v2;
        vco[2] = v3;

        vuvo[0] = uv1*m.uvScale;
        vuvo[1] = uv2*m.uvScale;
        vuvo[2] = uv3*m.uvScale;

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

        vuvo[0] = uv3*m.uvScale;
        vuvo[1] = uv2*m.uvScale;
        vuvo[2] = uv1*m.uvScale;

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

    MappingServices::calcMirrorUVs( m, vuvo );

    for ( auto t = 0; t < 3; t++ ) {
        auto cv = t == 0 ? Color4f::RED : ( t == 1 ? Color4f::GREEN : Color4f::BLUE );
        vdata->addTriangleVertex( vco[t], vuvo[t], vuvo[t], vno[t], vto[t], vbo[t], cv );
    }
}

namespace PolyServices {

    void addQuad( VDataSP vdata, const std::array<Vector3f, 4>& vs, const std::array<Vector2f, 4>& vts,
                  const std::array<Vector3f, 4>& vns, GeomMappingData& m ) {
        pushQuad( std::move(vdata), vs, vts, vns, m );
    }

    void addFlatPoly( VDataSP vdata, const std::vector<Vector3f>& points, WindingOrderT wo, GeomMappingData& m ) {
        if ( points.size() > 2 ) {
            WindingOrderT oldWindingOrder = m.windingOrder;
            m.windingOrder = wo;
            Vector3f normal = normalFromPoints( points.data(), m );
            addFlatPoly( std::move( vdata ), points.size(), points.data(), normal, m );
            m.windingOrder = oldWindingOrder;
        }
    }

    void addFlatPoly( VDataSP vdata, const std::array<Vector3f, 4>& verts, const Vector3f& normal, GeomMappingData& m,
                      bool reverseIfTriangulated ) {
        addFlatPoly( vdata, 4, verts.data(), normal, m, reverseIfTriangulated );
    }

    void addFlatPoly( VDataSP vdata, size_t vsize, const std::vector<Vector2f>& verts, float z, const Vector3f& normal,
                      GeomMappingData& m, bool reverseIfTriangulated ) {
        std::vector<Vector3f> verts3d;
        verts3d.reserve(verts.size());
        for ( auto& v : verts ) {
            verts3d.emplace_back( v, z );
        }
        addFlatPoly( std::move(vdata), vsize, verts3d.data(), normal, m, reverseIfTriangulated );
    }

    void addFlatPoly( VDataSP vdata, size_t vsize, const Vector3f* verts, const Vector3f& normal, GeomMappingData& m,
                      bool reverseIfTriangulated ) {
        std::array<Vector3f, 4> vs;
        std::array<Vector2f, 4> vtcs{};

        if ( vsize == 4 ) {
            vs[0] = ( verts[0] );
            vs[1] = ( verts[1] );
            vs[2] = ( verts[3] );
            vs[3] = ( verts[2] );
            MappingServices::planarMapping( m, normal, vs.data(), vtcs.data(), 4 );
            pushQuadSubDiv( vdata, vs, vtcs, m, normal );
        } else if ( vsize == 3 ) {
            for ( uint64_t t = 0; t < vsize - 2; t++ ) {
                vs[0] = ( verts[0] );
                vs[1] = ( verts[t + 1] );
                vs[2] = ( verts[t + 2] );
                MappingServices::planarMapping( m, normal, vs.data(), vtcs.data(), 3 );
                pushTriangleSubDiv( vdata, vs[0], vs[1], vs[2], vtcs[0], vtcs[1], vtcs[2], normal, normal, normal, m );
            }
        } else {
            Triangulator tri( verts, vsize, normal, 0.00001f );
            for ( auto& tvs : tri.get3dTrianglesList() ) {
                MappingServices::planarMapping( m, normal, tvs.data(), vtcs.data(), 3 );
                if ( reverseIfTriangulated ) {
                    pushTriangleSubDiv( vdata, tvs[2], tvs[1], tvs[0], vtcs[2], vtcs[1], vtcs[0], normal, normal, normal, m );
                } else {
                    pushTriangleSubDiv( vdata, tvs[0], tvs[1], tvs[2], vtcs[0], vtcs[1], vtcs[2], normal, normal, normal, m );
                }
            }
        }
    }

    void addFlatPolyTriangulated( VDataSP vdata, size_t vsize, const std::vector<Vector2f>& verts, float z,
                                  const Vector3f& normal, GeomMappingData& m, bool reverse ) {
        std::vector<Vector3f> verts3d;
        for ( auto& v : verts ) {
            verts3d.emplace_back( v, z );
        }
        addFlatPolyTriangulated( vdata, vsize, verts3d.data(), normal, m, reverse );
    }

    void addFlatPolyTriangulated( VDataSP vdata, size_t vsize, const Vector3f* verts, const Vector3f& normal,
                                  GeomMappingData& m, bool reverse ) {
        Vector2f vtcs[4];

        Triangulator tri( verts, vsize, normal );
        for ( auto& tvs : tri.get3dTrianglesList() ) {
            MappingServices::planarMapping( m, normal, tvs.data(), vtcs, 3 );
            if ( reverse ) {
                pushTriangleSubDiv( vdata, tvs[2], tvs[1], tvs[0], vtcs[2], vtcs[1], vtcs[0], normal, normal, normal, m );
            } else {
                pushTriangleSubDiv( vdata, tvs[0], tvs[1], tvs[2], vtcs[0], vtcs[1], vtcs[2], normal, normal, normal, m );
            }
        }
    }

    void addFlatPolyWithMapping( VDataSP vdata, const QuadStripUV& qs, GeomMappingData& m ) {
        addFlatPolyWithMapping( vdata, 4, qs.vs().verts(), qs.vts().verts(), m );
    }

    void addFlatPolyWithMapping( VDataSP vdata,  size_t vsize, const Vector3f* vs, const Vector2f* vts,
                                 GeomMappingData& m, const Vector3f* vn ) {
        if ( isCollinear( vs[0], vs[1], vs[2] ) ) return;

        Vector3f normal = vn ? *vn : normalFromPoints( vs, m );
        if ( vsize == 4 ) {
            pushQuadSubDiv( vdata, std::array<Vector3f, 4>{ {vs[0], vs[1], vs[2], vs[3]} },
                            std::array<Vector2f, 4>{ {vts[0], vts[1], vts[2], vts[3]} },
                            m, normal );
        } else {
            for ( uint64_t t = 0; t < vsize - 2; t++ ) {
                pushTriangleSubDiv( vdata, vs[0], vs[t + 1], vs[t + 2], vts[0], vts[t + 1], vts[t + 2],
                                    normal, normal, normal, m );
            }
        }
    }

    void pull( VDataSP vdata, const std::vector<Vector2f>& verts, float height, float zOffset,
               GeomMappingData& m, PullFlags pullFlags ) {
        std::vector<Vector3f> v3f = pullWindingOrderCheck( verts, zOffset );

        pull( vdata, v3f, height, m, pullFlags );
    }

    void pull( VDataSP vdata, const std::vector<Vector3f>& verts, float height,
               GeomMappingData& m, PullFlags pullFlags ) {
        std::array<Vector2f, 4> vtcs{};
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
//        if ( mHasBevel ) {
//            height -= mBevelAmount.z();
//            height = max( 0.00001f, height );
//        }

        Vector3f offset = normal * height;
        // Reset pull mapping
        m.pullMappingCoords = m.offset;
        std::array<Vector3f, 4> vss;

        if ( checkBitWiseFlag( pullFlags, PullFlags::Sides ) ) {
            for ( uint64_t t = 0; t < nvertsSane.size(); t++ ) {
                uint64_t index = t;
                uint32_t nextIndex = static_cast<uint32_t>( getCircularArrayIndexUnsigned( t + 1, nvertsSane.size() ));

                vss[1] = ( vertsSane[index] );
                vss[0] = ( vertsSane[nextIndex] );
                vss[3] = ( vertsSane[index] + offset );
                vss[2] = ( vertsSane[nextIndex] + offset );

                MappingServices::updatePullMapping( m, vss, vtcs );
                pushQuadSubDiv( vdata, vss, vtcs, m );
            }
        }

        if ( checkBitWiseFlag( pullFlags, PullFlags::Tops ) ) {
            int32_t nvSize = static_cast<int32_t>( nvertsSane.size() );
            std::unique_ptr<Vector3f[]> topvertsSane( new Vector3f[nvSize] );
            uint64_t l = 0;
            for ( size_t q = 0; q < static_cast<uint64_t>(nvSize); q++ ) {
                topvertsSane[l++] = nvertsSane[q];
            }
//            if ( mHasBevel ) {
//                // Draw bottom cap
//                addFlatPoly( nvertsSane.size(), topvertsSane.get(), -normal, true );
//
//                // Draw Top
//                for ( size_t q = 0; q < nvertsSane.size(); q++ ) topvertsSane[q] = nvertsSane[q] + offset;
//                std::unique_ptr<Vector3f[]> bevelvertsSane( new Vector3f[nvSize] );
//                for ( int32_t t = 0; t < nvSize; t++ ) {
//                    Vector3f p1 = topvertsSane[getCircularArrayIndex( t - 1, nvSize )];
//                    Vector3f p2 = topvertsSane[getCircularArrayIndex( t, nvSize )];
//                    Vector3f p3 = topvertsSane[getCircularArrayIndex( t + 1, nvSize )];
//                    Vector3f inWardNorm = normalize( normalize( p2 - p1 ) + normalize( p2 - p3 ) );
//                    bevelvertsSane[t] = topvertsSane[t] + ( -inWardNorm * mBevelAmount.x() ) + ( normal * mBevelAmount.z() );
//                }
//                addFlatPoly( nvertsSane.size(), bevelvertsSane.get(), normal );
//                for ( l = 0; l < nvertsSane.size(); l++ ) {
//                    uint64_t l1 = getCircularArrayIndexUnsigned( l + 1, static_cast<uint64_t>( nvertsSane.size() ) );
//                    Vector3f lane1[] = { topvertsSane[l], topvertsSane[l1], bevelvertsSane[l1], bevelvertsSane[l] };
//                    Vector3f slopeNormal = -normalFromPoints( lane1 );
//                    addFlatPoly( 4, lane1, slopeNormal );
//                }
//            } else {
//            }
            // Draw bottom cap
            addFlatPoly( vdata, nvertsSane.size(), topvertsSane.get(), -normal, m, true );
            // Draw top cap
            for ( size_t q = 0; q < nvertsSane.size(); q++ ) topvertsSane[q] = nvertsSane[nvSize -1 - q] + offset;
            addFlatPoly( vdata, nvertsSane.size(), topvertsSane.get(), normal, m );
        }
    }

}
