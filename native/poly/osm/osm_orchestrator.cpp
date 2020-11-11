//
// Created by dado on 15/10/2020.
//

#include "osm_orchestrator.hpp"

#include <core/http/webclient.h>
#include <core/descriptors/osm_bsdata.hpp>
#include <core/resources/resource_builder.hpp>
#include <poly/scene_graph.h>
#include <poly/collision_mesh.hpp>
#include <poly/osm/osm_names.hpp>
#include <poly/converters/gltf2/gltf2.h>

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

OsmOrchestrator::OsmOrchestrator( SceneGraph& _sg ) : sg(_sg) {

}

void OsmOrchestrator::OSM( std::shared_ptr<OSMData> _data) {
    osm = _data;
}

OSMData *OsmOrchestrator::OSM() {
    return osm.get();
}

std::shared_ptr<CollisionMesh> OSMService::createCollisionMesh( const OSMData *osm ) {
    auto ret = std::make_shared<CollisionMesh>();
    return ret;
}

void OsmOrchestrator::loadOSM( double lon, double lat, double zoomLevel ) {

    Http::getNoCache(Url{ "/osm/" + std::to_string(lon) + "/" + std::to_string(lat) + "/" + std::to_string(zoomLevel) },
                     [&]( HttpResponeParams params ) {
                         if ( !params.BufferString().empty() ) {
                             auto map = std::make_shared<OSMData>();
                             map->elements = deserializeVector<OSMElement>(params.BufferString());
                             OSM(map);
                             HOD::resolver<OSMData>(sg, OSM(), [&]() {
                                 sg.loadCollisionMesh(OSMService::createCollisionMesh(map.get()));
                                 sg.setCollisionEnabled(true);
                                 auto tiles = sg.GB<GT::OSMTile>(OSM(), GT::Tag(SHADOW_MAGIC_TAG), GT::Bucket(GTBucket::NearUnsorted), GT::M("city,atlas"), GT::Program(S::SH_CITY_ATLAS));
                                 auto buildings = sg.GB<GT::OSMBuildings>(OSM(), GT::Bucket(GTBucket::Near), GT::M("city,atlas"), GT::Program(S::SH_CITY_ATLAS));
                                 GLTF2Service::save( sg, buildings );
                             });
                         }
                     });
}


