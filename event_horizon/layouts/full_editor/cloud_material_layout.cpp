//
// Created by Dado on 2018-10-29.
//

#include "cloud_material_layout.h"
#include "cloud_entities_layout.h"
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/ui_presenter.hpp>

void ImGuiCloudEntitiesMaterials( UiPresenter* p, const Rect2f& _r ) {
    ImGuiCloudEntities( p, _r, "Cloud Materials", Material::entityGroup() );
}
