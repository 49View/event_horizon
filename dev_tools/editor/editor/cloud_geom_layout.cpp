//
// Created by Dado on 2018-10-29.
//

#include "cloud_geom_layout.h"
#include "cloud_entities_layout.h"
#include <core/v_data.hpp>

void ImGuiCloudEntitiesGeom::renderImpl( SceneOrchestrator* p, Rect2f& _r ) {
    ImGuiCloudEntities( p, _r, "Cloud Geometry", ResourceGroup::Geom );
}