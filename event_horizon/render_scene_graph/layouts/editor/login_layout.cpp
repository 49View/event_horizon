//
// Created by Dado on 2018-10-29.
//

#include "login_layout.h"
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene.hpp>

void ImGuiLogin( Scene* p, const Rect2f& _r ) {
    std::string title = "Login";
    ImGui::SetNextWindowPos( ImVec2{ _r.origin().x(), _r.origin().y() } );
    ImGui::SetNextWindowSize( ImVec2{ _r.size().x(), _r.size().y() } );
    ImGui::Begin( title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse );

    ImGui::PushID( static_cast<int>(std::hash<std::string>{}("Project")));
    static std::string buf;
    buf.reserve(256);
    if ( Http::project().size() < 256 && buf.empty() ) {
        buf = Http::project();
    }
    if ( ImGui::InputText( "Project", buf.data(), 256, ImGuiInputTextFlags_EnterReturnsTrue ) ) {
        Http::login( LoginFields{ Http::cachedLoginFields(), buf } );
    };
    ImGui::PopID();

    ImGui::End();
}
