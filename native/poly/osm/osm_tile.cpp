//
// Created by dado on 13/10/2020.
//

#include "osm_tile.hpp"

#include <core/v_data.hpp>
#include <core/descriptors/osm_bsdata.hpp>
#include <core/math/poly_shapes.hpp>
#include <poly/osm/osm_calc.hpp>
#include <poly/osm/osm_names.hpp>
#include <poly/osm/osm_tree.hpp>
#include <poly/osm/osm_building.hpp>
#include <poly/osm/osm_barrier.hpp>

void addOSMMesh( const std::shared_ptr<VData>& _ret, const std::function<void(Topology&)>& func ) {
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

void addOSMTileTriangles( const std::shared_ptr<VData>& _ret, const std::vector<OSMMesh>& osmMeshes, const V3f& elemCenterProj3d, float globalOSMScale ) {
    for ( const auto& osmMesh : osmMeshes ) {
        addOSMMesh(_ret, [osmMesh, elemCenterProj3d, globalOSMScale]( Topology& mesh ) {
            C4f color = C4fc::XTORGBA(osmMesh.colour);
            for ( const auto& v : osmMesh.vertices ) {
                mesh.addVertexOfTriangle(osmTileProject(v, elemCenterProj3d, globalOSMScale),
                                         V4fc::ZERO, color);
            }
        });
    }
}

void addOSMTile( const std::shared_ptr<VData>& _ret, const OSMData* osm, float globalOSMScale ) {
    std::set<std::string> roadNames;

    addOSMTileBoundaries( _ret, osm->tileBoundary, globalOSMScale );

    for ( const auto& element : osm->elements ) {
        V3f elemCenterProj3d = osmTileDeltaPos(element);
        if ( element.type == OSMElementName::unclassified() ) {
            addOSMTileTriangles( _ret, element.meshes, elemCenterProj3d, globalOSMScale);
        }
        if ( element.type == OSMElementName::road() ) {
            addOSMTileTriangles( _ret, element.meshes, elemCenterProj3d, globalOSMScale);
        }
        if ( element.type == OSMElementName::water() ) {
            addOSMTileTriangles( _ret, element.meshes, elemCenterProj3d, globalOSMScale);
        }
        if ( element.type == OSMElementName::parking() ) {
            addOSMTileTriangles( _ret, element.meshes, elemCenterProj3d, globalOSMScale);
        }
        if ( element.type == OSMElementName::park() ) {
            addOSMTileTriangles( _ret, element.meshes, elemCenterProj3d, globalOSMScale);
        }
    }
}

void addOSMSolid( const std::shared_ptr<VData>& _ret, const OSMData* osm, float globalOSMScale ) {
    auto addToOSMV = [_ret]( const std::vector<PolyStruct>& pss ) {
        for ( const auto& ps : pss ) _ret->fill(ps);
    };

    auto addToOSM = [_ret]( const PolyStruct& ps ) {
        _ret->fill(ps);
    };

    for ( const auto& element : osm->elements ) {
        V3f tilePosDelta = osmTileDeltaPos(element);
        for ( const auto& group : element.meshes ) {
            if ( element.type == OSMElementName::tree() ) {
                addToOSMV(osmCreateTree(tilePosDelta, globalOSMScale ));
            } else if ( element.type == OSMElementName::building() ) {
                addToOSM(osmCreateBuilding(group, tilePosDelta, globalOSMScale));
            } else if ( element.type == OSMElementName::barrier() ) {
                addToOSMV(osmCreateBarrier(group, globalOSMScale));
            }
        }
    }
}
