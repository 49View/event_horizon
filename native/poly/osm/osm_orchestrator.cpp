//
// Created by dado on 15/10/2020.
//

#include "osm_orchestrator.hpp"

#include <core/http/webclient.h>
#include <core/descriptors/osm_bsdata.hpp>
#include <poly/scene_graph.h>
#include <poly/collision_mesh.hpp>
#include <poly/osm/osm_names.hpp>

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
    return ret;
}

void OSMService::loadOSM( float lon, float lat, int zoomLevel ) {

    Http::getNoCache(Url{ "/osm/" + std::to_string(lon) + "/" + std::to_string(lat) + "/" + std::to_string(zoomLevel) },
                     [&]( HttpResponeParams params ) {
                         if ( !params.BufferString().empty() ) {
                             auto newHouseInstance = std::make_shared<OSMData>(params.BufferString());
//                             HOD::resolver<OSMData>(sg, nullptr, [&]() {
//                                 OSMData map{FM::readLocalFileC("../../elements.json")};
//                                 sg.loadCollisionMesh(OSMService::createCollisionMesh(&map));
//                                 sg.setCollisionEnabled(true);
//                                 sg.GB<GT::OSMTile>(&map, V2f{-0.1344f, 51.4892f}, GT::Tag(SHADOW_MAGIC_TAG), GT::Bucket(GTBucket::NearUnsorted), GT::M("city,atlas"), GT::Program(S::SH_CITY_ATLAS));
//                                 sg.GB<GT::OSMBuildings>(&map, V2f{-0.1344f, 51.4892f}, GT::Bucket(GTBucket::Near), GT::M("city,atlas"), GT::Program(S::SH_CITY_ATLAS));
//                             });
                         }
                     });
}
