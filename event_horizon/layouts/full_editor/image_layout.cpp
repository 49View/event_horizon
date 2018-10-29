//
// Created by Dado on 2018-10-29.
//

#include "image_layout.h"
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/ui_presenter.hpp>

void ImGuiImages( UiPresenter* p, const Rect2f& _r ) {
    ImGui::SetNextWindowPos( ImVec2{ _r.origin().x(), _r.origin().y() } );
    ImGui::SetNextWindowSize( ImVec2{ _r.size().x(), _r.size().y() } );
    ImGui::Begin( "Images",  nullptr, ImGuiWindowFlags_NoCollapse );
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
    ImGui::End();
}
