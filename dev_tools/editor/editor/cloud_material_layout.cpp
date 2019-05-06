//
// Created by Dado on 2018-10-29.
//

#include "cloud_material_layout.h"
#include "cloud_entities_layout.h"
//#include <render_scene_graph/scene_orchestrator.hpp>

void ImGuiCloudEntitiesMaterials::renderImpl( SceneGraph& _sg, RenderOrchestrator& _rsg, Rect2f& _r ) {
    ImGuiCloudEntities( _sg, _rsg, _r, "Cloud Materials", ResourceGroup::Material );
}
