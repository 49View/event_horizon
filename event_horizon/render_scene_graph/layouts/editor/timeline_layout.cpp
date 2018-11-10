//
// Created by Dado on 2018-10-29.
//

#include "timeline_layout.h"
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene.hpp>

void ImGuiTimeline( [[maybe_unused]] Scene* p, const Rect2f& _r ) {
    ImGui::SetNextWindowPos( ImVec2{ _r.origin().x(), _r.origin().y() } );
    ImGui::SetNextWindowSize( ImVec2{ _r.size().x(), _r.size().y() } );
    ImGui::Begin( "Timeline",  nullptr, ImGuiWindowFlags_NoCollapse );
    ImGui::End();
}
