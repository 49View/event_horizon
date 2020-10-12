//
// Created by dado on 12/10/2020.
//

#include "osm_barrier.hpp"
#include <core/math/poly_shapes.hpp>
#include <core/descriptors/osm_bsdata.hpp>
#include <poly/follower.hpp>

void osmCreateFenceBarrier( std::vector<PolyStruct>& barriers, const std::vector<V3f>& vertices, float globalOSMScale, const C4f& mainColor ) {
    float elemSize = 0.03f;
    float gap = 0.4f;
    float barrierHeight = 1.97f;
    for ( auto t = 0u; t < vertices.size()-1; t++ ) {
        auto p1 = vertices[t];
        auto p2 = vertices[t+1];
        auto delta = p2-p1;
        auto dist = length(delta);
        size_t repeats = (dist - elemSize) / (gap+elemSize);
        float iDelta = 1.0f / (repeats+1.0f);
        for ( auto q = 0u; q <= repeats; q++ ) {
            barriers.emplace_back(
                    createGeomForCylinder( (( lerp(iDelta*q, p1, p2) + V3f::UP_AXIS()*half(barrierHeight)) * globalOSMScale), V2f{elemSize, barrierHeight} * globalOSMScale, 1,
                                           mainColor));
        }
    }
}

void osmCreateWallBarrier( std::vector<PolyStruct>& barriers, const std::vector<V3f>& vertices, float globalOSMScale, const C4f& mainColor ) {
    auto wallSize = V2f{ 0.3f, 1.0f} * globalOSMScale;
    std::vector<V3f> scaledVerts{};
    std::transform( vertices.begin(), vertices.end(), std::back_inserter(scaledVerts), [globalOSMScale, wallSize]( const auto& v) {
       return (v + V3f::UP_AXIS()*half(wallSize.y())) * globalOSMScale;
    });
    barriers.emplace_back(FollowerService::extrudePolyStruct( scaledVerts, *Profile::makeRect(wallSize, V2f{0.0f, half(wallSize.y())}), mainColor, V3fc::UP_AXIS) );
}

void osmCreateBarrier( std::vector<PolyStruct>& barriers, const OSMMesh& group, float globalOSMScale ) {

    auto mainColor = C4fc::XTORGBA(group.colour);
    if ( group.part == "fence" ) {
        osmCreateFenceBarrier( barriers, group.vertices, globalOSMScale, mainColor );
    } else {
        osmCreateWallBarrier( barriers, group.vertices, globalOSMScale, mainColor );
    }
//    if ( group.part == "wall" ) {
//    }

}
