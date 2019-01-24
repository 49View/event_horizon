//
// Created by Dado on 2018-10-29.
//

#include "cloud_geom_layout.h"
#include "cloud_entities_layout.h"
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene.hpp>
#include <core/node.hpp>
#include <poly/geom_data.hpp>
#include <render_scene_graph/layouts/layout_helper.hpp>

void ImGuiCloudEntitiesGeom( Scene* p, const Rect2f& _r ) {
    LAYOUT_IMGUI_WINDOW_POSSIZE( CloudGeom, _r);
    ImGuiCloudEntities( p, _r, "Cloud Geometry", EntityGroup::Geom, GeomData::Version() );
}
