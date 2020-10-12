//
// Created by dado on 12/10/2020.
//

#include "osm_building.hpp"
#include <core/math/vector3f.h>
#include <core/math/poly_shapes.hpp>
#include <core/descriptors/osm_bsdata.hpp>
#include <poly/osm/osm_names.hpp>
#include <poly/osm/osm_calc.hpp>
#include <core/math/obb.hpp>

[[maybe_unused]] void
osmCreateBuilding( Topology& mesh, const OSMMesh& group, const V3f& tilePosDelta, float globalOSMScale ) {
    float facadeMappingScale = 0.1f;
    Rect2f bigBoundary{ Rect2f::INVALID };
    C4f color = C4fc::XTORGBA(group.colour);

    if ( group.part == "roof" ) {
        std::vector<V2f> rootPoints{};
        for ( const auto& triangle : group.vertices ) {
            V3f p = osmTileProject(triangle, tilePosDelta, bigBoundary, globalOSMScale);
            rootPoints.emplace_back(V2f(p.xz()));
        }
        auto gObb = RotatingCalipers::minAreaRect(rootPoints);

        float yOff = 12.0f + static_cast<float>(unitRandI(3));
        V2f tile{ static_cast<float>(unitRandI(15)) / 16.0f, yOff / 16.0f };
        tile = V2fc::ZERO;
        for ( const auto& triangle : group.vertices ) {
            V3f v1 = osmTileProject(triangle, tilePosDelta, bigBoundary, globalOSMScale);
            V2f uv1 = dominantMapping(V3f::UP_AXIS(), v1, V3fc::ONE);
            uv1 *= V2fc::ONE * ( 1.0f / globalOSMScale ) * facadeMappingScale * .25f;
            uv1.rotate(static_cast<float>(gObb.angle_width));
            mesh.addVertexOfTriangle(v1, V4f{ uv1, tile }, color);
        }
    } else if ( group.part == "lateral" ) {
        auto yOff = static_cast<float>(unitRandI(12));
        V2f tile{ static_cast<float>(unitRandI(15)) / 16.0f, yOff / 16.0f };
        tile = V2fc::ZERO;
        float xAcc = 0.0f;
        for ( auto ti = 0u; ti < group.vertices.size(); ti += 6 ) {

            auto v1 = osmTileProject(group.vertices[ti], tilePosDelta, bigBoundary, globalOSMScale);
            auto v2 = osmTileProject(group.vertices[ti + 1], tilePosDelta, bigBoundary, globalOSMScale);
            auto v3 = osmTileProject(group.vertices[ti + 2], tilePosDelta, bigBoundary, globalOSMScale);
            auto v4 = osmTileProject(group.vertices[ti + 3], tilePosDelta, bigBoundary, globalOSMScale);
            auto v5 = osmTileProject(group.vertices[ti + 4], tilePosDelta, bigBoundary, globalOSMScale);
            auto v6 = osmTileProject(group.vertices[ti + 5], tilePosDelta, bigBoundary, globalOSMScale);

            float dist = distance(v1, v2);
            V2f uv1 = V2f{ xAcc, -v1.y() };
            V2f uv2 = V2f{ xAcc + dist, -v2.y() };
            V2f uv3 = V2f{ xAcc + dist, -v3.y() };
            V2f uv4 = V2f{ xAcc, -v4.y() };
            V2f uv5 = V2f{ xAcc + dist, -v5.y() };
            V2f uv6 = V2f{ xAcc, -v6.y() };

            mesh.addVertexOfTriangle(v1, V4f{ uv1 * V2fc::ONE * ( 1.0f / globalOSMScale ) *
                                              facadeMappingScale, tile }, color);
            mesh.addVertexOfTriangle(v2, V4f{ uv2 * V2fc::ONE * ( 1.0f / globalOSMScale ) *
                                              facadeMappingScale, tile }, color);
            mesh.addVertexOfTriangle(v3, V4f{ uv3 * V2fc::ONE * ( 1.0f / globalOSMScale ) *
                                              facadeMappingScale, tile }, color);
            mesh.addVertexOfTriangle(v4, V4f{ uv4 * V2fc::ONE * ( 1.0f / globalOSMScale ) *
                                              facadeMappingScale, tile }, color);
            mesh.addVertexOfTriangle(v5, V4f{ uv5 * V2fc::ONE * ( 1.0f / globalOSMScale ) *
                                              facadeMappingScale, tile }, color);
            mesh.addVertexOfTriangle(v6, V4f{ uv6 * V2fc::ONE * ( 1.0f / globalOSMScale ) *
                                              facadeMappingScale, tile }, color);
            xAcc += dist;
        }
    }
}
