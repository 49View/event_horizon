//
// Created by dado on 15/10/2020.
//

#include "osm_orchestrator.hpp"
#include <poly/scene_graph.h>
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
