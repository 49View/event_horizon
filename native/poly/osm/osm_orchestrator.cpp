//
// Created by dado on 15/10/2020.
//

#include <poly/scene_graph.h>
#include "osm_orchestrator.hpp"

namespace HOD { // HighOrderDependency

    template<>
    DepRemapsManager resolveDependencies<OSMData>( const OSMData *_resources, SceneGraph& sg ) {
        DepRemapsManager ret{};

        ret.addDep(sg, ResourceGroup::Material, "city,atlas");
        ret.addDep(sg, ResourceGroup::Geom, "fir,tree");

        return ret;
    }
}
