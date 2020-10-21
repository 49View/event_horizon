//
// Created by dado on 15/10/2020.
//

#include "osm_orchestrator.hpp"
#include <poly/scene_graph.h>
#include <poly/collision_mesh.hpp>
#include "osm_names.hpp"

namespace HOD { // HighOrderDependency

    template<>
    DepRemapsManager resolveDependencies<OSMData>( [[maybe_unused]] const OSMData *_resources, SceneGraph& sg ) {
        DepRemapsManager ret{};

        ret.addDep(sg, ResourceGroup::Material, "city,atlas");
        for ( const auto& elem : OSMGeomEntityList() ) {
            ret.addDep(sg, ResourceGroup::Geom, elem);
        }

        return ret;
    }
}

std::shared_ptr<CollisionMesh> OSMService::createCollisionMesh( const OSMData *osm ) {
    auto ret = std::make_shared<CollisionMesh>();

//    ret->bbox = house->BBox();
//    std::set<int64_t> doorAddedSet{};
//    for ( const auto& f : house->mFloors ) {
//        CollisionGroup cg{};
//        cg.bbox = f->BBox();
//        for ( const auto& r : f->rooms ) {
//            CollisionRoom cr{};
//            cr.bbox = r->BBox();
//            for ( const auto& ls : r->mWallSegmentsSorted ) {
//                if ( !checkBitWiseFlag(ls.tag, WF_IsDoorPart) ) {
//                    cr.collisionCollisionElement.emplace_back(ls.p1, ls.p2, ls.normal);
//                }
//            }
//
//            for ( const auto& doorHash : r->doors ) {
//                for ( const auto& door : f->doors ) {
//                    if ( door->hash == doorHash && doorAddedSet.find(doorHash) == doorAddedSet.end() ) {
//                        cr.collisionCollisionElement.emplace_back(door->us1.points[1], door->us1.points[2],
//                                                                  door->us1.inwardNormals[0]);
//                        cr.collisionCollisionElement.emplace_back(door->us2.points[1], door->us2.points[2],
//                                                                  door->us2.inwardNormals[0]);
//                        if ( door->isMainDoor ) {
//                            cr.collisionCollisionElement.emplace_back(door->us2.middle, door->us1.middle,
//                                                                      door->us1.crossNormals[0]);
//                        }
//                        cr.bbox.merge(door->BBox());
//                        doorAddedSet.insert(doorHash);
//                    }
//                }
//            }
//            cg.collisionRooms.emplace_back(cr);
//        }
//        ret->collisionGroups.emplace_back(cg);
//    }
    return ret;
}
