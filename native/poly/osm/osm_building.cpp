//
// Created by dado on 12/10/2020.
//

#include "osm_building.hpp"
#include <core/math/vector3f.h>
#include <core/soa_utils.h>
#include <core/math/poly_shapes.hpp>
#include <core/descriptors/osm_bsdata.hpp>
#include <poly/osm/osm_names.hpp>
#include <poly/osm/osm_calc.hpp>
#include <core/math/obb.hpp>
#include <poly/follower.hpp>
#include <core/math/triangulator.hpp>

template<typename T>
T bLerp( const T& c00, const T& c10, const T& c01, const T& c11, float tx, float ty ) {
    return mix(mix(c00, c10, tx), mix(c01, c11, tx), ty);
}

double lcx = 12709527.581054647;
double lcy = 2547258.119848553;

std::vector<PolyStruct> osmCreateBuilding( const OSMMesh& group, const V3f& tilePosDelta, float globalOSMScale ) {
    std::vector<PolyStruct> parts{};
    Topology mesh;
    float facadeMappingScale = 0.1f;
    float latScale = ( 1.0f / globalOSMScale ) * facadeMappingScale;
    C4f color = C4fc::XTORGBA(group.colour);

    if ( group.part == "roof" ) {
        std::vector<V2f> rootPoints{};
        for ( const auto& triangle : group.vertices ) {
            V3f p = osmTileProject(triangle, lcx, lcy);
            rootPoints.emplace_back(V2f(p.xz()));
        }
        auto gObb = RotatingCalipers::minAreaRect(rootPoints);

        float yOff = 12.0f + static_cast<float>(unitRandI(3));
        V2f tile{ static_cast<float>(unitRandI(15)) / 16.0f, yOff / 16.0f };
        tile = V2fc::ZERO;
        for ( const auto& triangle : group.vertices ) {
            V3f v1 = osmTileProject(triangle, lcx, lcy);
            V2f uv1 = dominantMapping(V3f::UP_AXIS(), v1, V3fc::ONE);
            uv1 *= V2fc::ONE * ( 1.0f / globalOSMScale ) * facadeMappingScale * .25f;
            uv1.rotate(static_cast<float>(gObb.angle_width));
            mesh.addVertexOfTriangle(v1, V4f{ uv1, tile }, color);
        }
    } else if ( group.part == "lateral" ) {
        auto yOff = static_cast<float>(unitRandI(12));
        V2f tile{ static_cast<float>(unitRandI(15)) / 16.0f, yOff / 16.0f };
        std::vector<V3f> topVerts{};
//        tile = V2fc::ZERO;
        float xAcc = 0.0f;

        auto minHeight = group.minHeight;
        auto maxHeight = group.maxHeight;
        for ( auto ti = 0u; ti < group.vertices.size(); ti++ ) {
            auto tiNext = cai(ti + 1, group.vertices.size());
            auto pCurr = group.vertices[ti];
            auto pNext = group.vertices[tiNext];

            std::vector<double> bottomLeft  = { pCurr[0], pCurr[1], minHeight };
            std::vector<double> bottomRight = { pNext[0], pNext[1], minHeight };
            std::vector<double> topLeft     = { pCurr[0], pCurr[1], maxHeight };
            std::vector<double> topRight    = { pNext[0], pNext[1], maxHeight };

            auto v1 = osmTileProject(bottomLeft , lcx, lcy);
            auto v2 = osmTileProject(bottomRight, lcx, lcy);
            auto v3 = osmTileProject(topRight   , lcx, lcy);
            auto v4 = osmTileProject(topLeft    , lcx, lcy);

            float dist = distance(v1, v2);
            V4f uv1 = V4f{ V2f{ xAcc, -v1.y() }* latScale, tile };
            V4f uv2 = V4f{ V2f{ xAcc + dist, -v2.y() }* latScale, tile };
            V4f uv3 = V4f{ V2f{ xAcc + dist, -v3.y() }* latScale, tile };
            V4f uv4 = V4f{ V2f{ xAcc, -v4.y() }* latScale, tile };

//            auto center = bLerp( v1, v2, v3, v4, 0.5f, 0.5f );
//            auto centerUV = bLerp( uv1, uv2, uv3, uv4, 0.5f, 0.5f );

//            V2fVectorOfVector holes{};
//            auto silhouette = Triangulator::execute2dList({v1.xy(), v2.xy(), v3.xy(), v4.xy()}, holes, 0.000001f);

//            mesh.addVertexOfTriangle( v1, uv1, color );
//            mesh.addVertexOfTriangle( v2, uv2, color );
//            mesh.addVertexOfTriangle( center, centerUV, color );

            mesh.addQuad({ v1, v2, v3, v4 },{ uv1, uv2, uv3, uv4 },{ color, color, color, color });

            topVerts.emplace_back(v4);
            xAcc += dist;
        }
        parts.emplace_back(
                FollowerService::extrudePolyStruct(topVerts, *Profile::makeRect(V2f{ 0.03f }, V2fc::ZERO), color,
                                                   V3fc::UP_AXIS, FollowerFlags::WrapPath));
    }

    parts.emplace_back(createGeom(mesh));
    return parts;
}
