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
            mesh.addVertexOfTriangle(XZY::C(v * globalOSMScale, 0.0f), V4fc::ZERO, C4fc::XTORGBA("#E8E3EF"));
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

bool checkTagOnElement( const OSMElement& element, const std::string& tag, const std::string& value ) {
    auto it = element.tags.find(tag);
    return it != element.tags.end() && it->second == value;
}

void addOSMTile( const std::shared_ptr<VData>& _ret, const OSMData* osm, float globalOSMScale ) {

    std::array<std::vector<const OSMElement*>,4> sortedRoads;

    addOSMTileBoundaries( _ret, osm->tileBoundary, globalOSMScale );

    for ( const auto& element : osm->elements ) {
        V3f elemCenterProj3d = osmTileDeltaPos(element);
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

    for ( const auto& element : osm->elements ) {
        V3f elemCenterProj3d = osmTileDeltaPos(element);
        if ( element.type == OSMElementName::unclassified() ) {
            addOSMTileTriangles( _ret, element.meshes, elemCenterProj3d, globalOSMScale);
        }
        if ( element.type == OSMElementName::road() ) {
            const auto& it = element.tags.find(OSMElementName::highway());
            if ( it->second == OSMElementName::primary() || it->second == OSMElementName::secondary() ) {
                sortedRoads[3].emplace_back(&element);
            } else if ( it->second == OSMElementName::tertiary() ) {
                sortedRoads[2].emplace_back(&element);
            } else if ( it->second == OSMElementName::residential() ) {
                sortedRoads[1].emplace_back(&element);
            } else {
                sortedRoads[0].emplace_back(&element);
            }
        }
    }

    for ( const auto& sr : sortedRoads ) {
        for ( const auto& element : sr ) {
            V3f elemCenterProj3d = osmTileDeltaPos(*element);
            addOSMTileTriangles( _ret, element->meshes, elemCenterProj3d, globalOSMScale);
        }
    }

}

void addOSMSolid( const std::shared_ptr<VData>& _ret, const OSMData* osm, const OSMAssetMap& assets, float globalOSMScale ) {
    auto addToOSMV = [_ret]( const std::vector<PolyStruct>& pss ) {
        for ( const auto& ps : pss ) _ret->fill(ps);
    };

    auto addToOSM = [_ret]( const PolyStruct& ps ) {
        _ret->fill(ps);
    };

    for ( const auto& element : osm->elements ) {
        V3f tilePosDelta = osmTileDeltaPos(element);
        for ( const auto& group : element.meshes ) {
            if ( element.type == OSMElementName::entity() ) {
                auto randScale = 2.0f + unitRand(1.5f);
                Matrix4f mat{tilePosDelta*globalOSMScale, Quaternion{}, V3f{globalOSMScale*randScale}};

                if ( checkTagOnElement(element, OSMElementName::amenity(), OSMElementName::telephone()) ) {
//                    addToOSMV(osmCreateTree(tilePosDelta, globalOSMScale ));
                }
                if ( checkTagOnElement(element, OSMElementName::historic(), OSMElementName::monument()) ) {
//                    addToOSMV(osmCreateTree(tilePosDelta, globalOSMScale ));
                }
                if ( checkTagOnElement(element, OSMElementName::natural(), OSMElementName::tree()) ) {
                    auto rand = static_cast<int>(unitRand(OSMTreeList().size()));
                    _ret->fill( *assets.find(OSMTreeList()[rand])->second, mat);
                }
                if ( checkTagOnElement(element, OSMElementName::highway(), OSMElementName::bus_stop()) ) {
//                    addToOSMV(osmCreateTree(tilePosDelta, globalOSMScale ));
                }
            } else if ( element.type == OSMElementName::building() ) {
                addToOSM(osmCreateBuilding(group, tilePosDelta, globalOSMScale));
            } else if ( element.type == OSMElementName::barrier() ) {
                addToOSMV(osmCreateBarrier(group, globalOSMScale));
            }
        }
    }
}
