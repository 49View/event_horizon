//
// Created by Dado on 2019-01-23.
//

#include "taskbar_layout.hpp"
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene.hpp>
#include <render_scene_graph/layouts/layout_helper.hpp>

void ImGuiTaskbar( Scene* p, const Rect2f& _r ) {

    std::string title = "Taskbar";
    ImGui::SetNextWindowPos( ImVec2{ _r.origin().x(), _r.origin().y() } );
    ImGui::SetNextWindowSize( ImVec2{ _r.size().x(), _r.size().y() } );
    ImGui::Begin( title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize );

    if ( ImGui::Button( "Timeline" ) ) {
        LayoutImGuiShowAndHide::bShowTimeline = !LayoutImGuiShowAndHide::bShowTimeline;
    }
    ImGui::SameLine();
    if ( ImGui::Button( "SceneGraph" ) ) {
        LayoutImGuiShowAndHide::bShowSceneGraph = !LayoutImGuiShowAndHide::bShowSceneGraph;
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Camera" ) ) {
        LayoutImGuiShowAndHide::bShowCamera = !LayoutImGuiShowAndHide::bShowCamera;
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Materials" ) ) {
        LayoutImGuiShowAndHide::bShowMaterial = !LayoutImGuiShowAndHide::bShowMaterial;
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Cloud Geom" ) ) {
        LayoutImGuiShowAndHide::bShowCloudGeom = !LayoutImGuiShowAndHide::bShowCloudGeom;
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Cloud Mat" ) ) {
        LayoutImGuiShowAndHide::bShowCloudMaterial = !LayoutImGuiShowAndHide::bShowCloudMaterial;
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Images" ) ) {
        LayoutImGuiShowAndHide::bShowImage = !LayoutImGuiShowAndHide::bShowImage;
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Login" ) ) {
        LayoutImGuiShowAndHide::bShowLogin = !LayoutImGuiShowAndHide::bShowLogin;
    }

    ImGui::End();

}