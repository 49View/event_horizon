//
// Created by dado on 13/10/2020.
//

#include "osm_tree.hpp"
#include <core/math/poly_shapes.hpp>
#include <core/soa_utils.h>
#include <core/descriptors/osm_bsdata.hpp>
#include <poly/osm/osm_tile_element.hpp>

void osmCreateTree( const std::shared_ptr<VData>& _ret, const V3f& treePos, float globalOSMScale ) {
    addOSMPolyStruct( _ret, [treePos, globalOSMScale]() -> std::vector<PolyStruct> {
        std::vector<PolyStruct> trees;
        trees.emplace_back(createGeomForCone( treePos * globalOSMScale, V3f{0.2f, 3.14f, 0.2f}* globalOSMScale, 1, C4fc::PASTEL_BROWN ));
        trees.emplace_back(createGeomForSphere( (treePos + V3fc::UP_AXIS*3.0f) * globalOSMScale, globalOSMScale, 1, C4fc::FOREST_GREEN ));
        return trees;
    });
}

