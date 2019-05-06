//
// Created by Dado on 2018-10-29.
//

#include "cloud_geom_layout.h"
#include "cloud_entities_layout.h"
#include <core/v_data.hpp>

void ImGuiCloudEntitiesGeom::renderImpl( SceneGraph& _sg, RenderOrchestrator& _rsg, Rect2f& _r ) {
    ImGuiCloudEntities( _sg, _rsg, _r, "Cloud Geometry", ResourceGroup::Geom );
}
