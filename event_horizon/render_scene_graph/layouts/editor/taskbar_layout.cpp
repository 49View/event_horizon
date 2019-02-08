//
// Created by Dado on 2019-01-23.
//

#include "taskbar_layout.hpp"
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene.hpp>
#include <render_scene_graph/layouts/layout_helper.hpp>
#include <event_horizon/render_scene_graph/scene_layout.h>

void ImGuiTaskbar::renderImpl( SceneOrchestrator* p, Rect2f& _r ) {

    std::string title = "Taskbar";
    ImGui::SetNextWindowPos( ImVec2{ _r.origin().x(), _r.origin().y() } );
    ImGui::SetNextWindowSize( ImVec2{ _r.size().x(), _r.size().y() } );
    ImGui::Begin( title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize );

    if ( ImGui::Button( "Timeline" ) ) {
        p->Layout()->toggleVisible(SceneLayoutDefaultNames::Timeline);
    }
    ImGui::SameLine();
    if ( ImGui::Button( "SceneGraph" ) ) {
        p->Layout()->toggleVisible(SceneLayoutDefaultNames::Geom);
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Camera" ) ) {
        p->Layout()->toggleVisible(SceneLayoutDefaultNames::Camera);
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Materials" ) ) {
        p->Layout()->toggleVisible(SceneLayoutDefaultNames::Material);
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Cloud Geom" ) ) {
        p->Layout()->toggleVisible(SceneLayoutDefaultNames::CloudGeom);
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Cloud Mat" ) ) {
        p->Layout()->toggleVisible(SceneLayoutDefaultNames::CloudMaterial);
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Images" ) ) {
        p->Layout()->toggleVisible(SceneLayoutDefaultNames::Image);
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Login" ) ) {
        p->Layout()->toggleVisible(SceneLayoutDefaultNames::Login);
    }

    ImGui::End();

}
