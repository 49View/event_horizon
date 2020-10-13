//
// Created by dado on 13/10/2020.
//

#include "osm_tile_element.hpp"

#include <memory>
#include <core/v_data.hpp>
#include <core/math/poly_shapes.hpp>
#include <poly/osm/osm_calc.hpp>

template<typename F>
[[maybe_unused]] void addOSMMesh( const std::shared_ptr<VData>& _ret, F&& func ) {
    Topology mesh{};
    func(mesh);
    _ret->fill(createGeom(mesh));
}

void addOSMTileBoundaries( const std::shared_ptr<VData>& _ret, const OSMTileBoundary& tileBoundary, float globalOSMScale ) {
    addOSMMesh(_ret, [tileBoundary, globalOSMScale]( Topology& mesh ) {
        for ( const auto& v : tileBoundary.rect.pointsTriangleList() ) {
            mesh.addVertexOfTriangle(XZY::C(v * globalOSMScale, 0.0f), V4fc::ZERO, C4fc::XTORGBA("#fffdd0"));
        }
    });
}

void addOSMTileTriangles( const std::shared_ptr<VData>& _ret, const OSMMesh& osmMesh, const V3f& elemCenterProj3d, float globalOSMScale ) {
    addOSMMesh( _ret, [osmMesh, elemCenterProj3d, globalOSMScale](Topology& mesh) {
        C4f color = C4fc::XTORGBA(osmMesh.colour);
        for ( const auto& v : osmMesh.vertices ) {
            mesh.addVertexOfTriangle(osmTileProject(v, elemCenterProj3d, globalOSMScale),
                                     V4fc::ZERO, color);
        }
    });
}