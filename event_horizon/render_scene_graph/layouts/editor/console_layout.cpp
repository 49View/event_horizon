//
// Created by Dado on 2018-10-29.
//

#include "console_layout.h"
#include "graphics/ui/imgui_console.h"
#include <render_scene_graph/scene.hpp>
#include <render_scene_graph/layouts/layout_helper.hpp>

void ImGuiConsoleLayout::renderImpl( Scene* p, Rect2f& _r ) {
    p->Console()->Draw( _r );
};
