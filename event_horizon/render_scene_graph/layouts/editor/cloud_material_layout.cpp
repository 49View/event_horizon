//
// Created by Dado on 2018-10-29.
//

#include "cloud_material_layout.h"
#include "cloud_entities_layout.h"
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene.hpp>

void ImGuiCloudEntitiesMaterials( Scene* p, const Rect2f& _r ) {
    ImGuiCloudEntities( p, _r, "Cloud Materials", EntityGroup::Material );
}
