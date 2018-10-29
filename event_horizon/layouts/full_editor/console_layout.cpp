//
// Created by Dado on 2018-10-29.
//

#include "console_layout.h"
#include "graphics/ui/imgui_console.h"
#include <render_scene_graph/ui_presenter.hpp>

void ImGuiConsoleLayout( UiPresenter* p, const Rect2f& _r ) {
    p->Console()->Draw( _r );
};
