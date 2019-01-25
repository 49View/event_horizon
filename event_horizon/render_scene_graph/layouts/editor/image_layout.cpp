//
// Created by Dado on 2018-10-29.
//

#include "image_layout.h"
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene.hpp>
#include <render_scene_graph/layouts/layout_helper.hpp>

void ImGuiImages::renderImpl( Scene* p, Rect2f& _r ) {
    int ic = 0;
    for ( const auto& it: p->TM() ) {
        std::string tname = it.first;
        if ( !FBNames::isPartOf( cubeMapTName(tname) ) ) {
            ImGui::BeginGroup();
            ImGui::TextColored( ImVec4{0.0f,1.0f,1.0f,1.0f}, "%s", tname.c_str());
            ImGui::Image( reinterpret_cast<void *>(it.second->getHandle()), ImVec2{ 100, 100 } );
            ImGui::EndGroup();
            if ( ++ic % 6 != 0 ) { ImGui::SameLine(); }
        }
    }
}
