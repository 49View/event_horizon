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
//    addOSMMesh(_ret, [tileBoundary]( Topology& mesh ) {
//        for ( const auto& v : tileBoundary.rect.pointsTriangleList() ) {
//            mesh.addVertexOfTriangle(XZY::C(v * globalOSMScale, 0.0f), V4fc::ZERO, C4fc::XTORGBA("#E8E3EF"));
//        }
//    });
}

void addOSMTileTriangles( const std::shared_ptr<VData>& _ret, const std::vector<OSMMesh>& osmMeshes, const V3f& elemCenterProj3d, float globalOSMScale ) {
    for ( const auto& osmMesh : osmMeshes ) {
        addOSMMesh(_ret, [osmMesh, elemCenterProj3d, globalOSMScale]( Topology& mesh ) {
            C4f color = C4fc::XTORGBA(osmMesh.colour);
            for ( const auto& vd : osmMesh.vertices ) {
                double lcx = 12709527.581054647;
                double lcy = 2547258.119848553;
                mesh.addVertexOfTriangle(osmTileProject(vd, lcx, lcy),
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


    addOSMTileBoundaries( _ret, osm->tileBoundary, globalOSMScale );

    std::array<std::vector<const OSMElement*>,4> sortedRoads;

    for ( const auto& element : osm->elements ) {
        V3f elemCenterProj3d = osmTileDeltaPos(element);
        if ( element.type == OSMElementName::water() ) {
            addOSMTileTriangles( _ret, element.meshes, elemCenterProj3d, globalOSMScale);
        }
        if ( element.type == OSMElementName::beach() ) {
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

    auto addToOSMVData = [_ret]( const VData& ps, const Matrix4f& mat ) {
        _ret->fill(ps, mat);
    };

    for ( const auto& element : osm->elements ) {
        V3f tilePosDelta = osmTileDeltaPos(element);
//        if ( !osm->tileBoundary.rect.contains( tilePosDelta.xz()) ) {
//            continue;
//        }
        for ( const auto& group : element.meshes ) {
            if ( element.type == OSMElementName::entity() ) {
                if ( checkTagOnElement(element, OSMElementName::amenity(), OSMElementName::telephone()) ) {
                    auto randScale = 1.0f;
                    Matrix4f mat{tilePosDelta*globalOSMScale, Quaternion{}, V3f{globalOSMScale*randScale}};
                    addToOSMVData( *assets.find(OSMElementName::phone_booth())->second, mat);
                }
                if ( checkTagOnElement(element, OSMElementName::highway(), OSMElementName::street_lamp()) ) {
                    auto randScale = 1.0f;
                    Matrix4f mat{tilePosDelta*globalOSMScale, Quaternion{}, V3f{globalOSMScale*randScale}};
                    addToOSMVData( *assets.find(OSMElementName::street_lamp())->second, mat);
                }
                if ( checkTagOnElement(element, OSMElementName::historic(), OSMElementName::monument()) ) {
                    auto randScale = 1.0f;//2.0f + unitRand(1.5f);
                    Matrix4f mat{tilePosDelta*globalOSMScale, Quaternion{}, V3f{globalOSMScale*randScale}};
                    auto rand = static_cast<int>(unitRand(OSMMonumentList().size()));
                    addToOSMVData( *assets.find(OSMMonumentList()[rand])->second, mat);
                }
                if ( checkTagOnElement(element, OSMElementName::natural(), OSMElementName::tree()) ) {
                    auto randScale = 2.0f + unitRand(1.5f);
                    Matrix4f mat{tilePosDelta*globalOSMScale, Quaternion{}, V3f{globalOSMScale*randScale}};
                    auto rand = static_cast<int>(unitRand(OSMTreeList().size()));
                    addToOSMVData( *assets.find(OSMTreeList()[rand])->second, mat);
                }
                if ( checkTagOnElement(element, OSMElementName::highway(), OSMElementName::bus_stop()) ) {
//                    addToOSMV(osmCreateTree(tilePosDelta, globalOSMScale ));
                }
            } else if ( element.type == OSMElementName::building() ) {
                addToOSMV(osmCreateBuilding(group, tilePosDelta, globalOSMScale));
            } else if ( element.type == OSMElementName::barrier() ) {
                addToOSMV(osmCreateBarrier(group, globalOSMScale));
            }
        }
    }

    LOGRS(_ret->getMin() << _ret->getMax() )
}
