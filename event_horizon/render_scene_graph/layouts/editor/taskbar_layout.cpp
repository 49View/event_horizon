//
// Created by Dado on 2019-01-23.
//

#include "taskbar_layout.hpp"
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene_orchestrator.hpp>
#include <render_scene_graph/layouts/layout_helper.hpp>
#include <event_horizon/render_scene_graph/scene_state_machine.h>

void ImGuiTaskbar::renderImpl( SceneOrchestrator* p, Rect2f& _r ) {

    std::string title = "Taskbar";
    ImGui::SetNextWindowPos( ImVec2{ _r.origin().x(), _r.origin().y() } );
    ImGui::SetNextWindowSize( ImVec2{ _r.size().x(), _r.size().y() } );
    ImGui::Begin( title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize );

    if ( ImGui::Button( "Timeline" ) ) {
        p->StateMachine()->toggleVisible(SceneLayoutDefaultNames::Timeline);
    }
    ImGui::SameLine();
    if ( ImGui::Button( "SceneGraph" ) ) {
        p->StateMachine()->toggleVisible(SceneLayoutDefaultNames::Scene);
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Camera" ) ) {
        p->StateMachine()->toggleVisible(SceneLayoutDefaultNames::Camera);
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Materials" ) ) {
        p->StateMachine()->toggleVisible(SceneLayoutDefaultNames::Material);
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Cloud Geom" ) ) {
        p->StateMachine()->toggleVisible(SceneLayoutDefaultNames::CloudGeom);
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Cloud Mat" ) ) {
        p->StateMachine()->toggleVisible(SceneLayoutDefaultNames::CloudMaterial);
    }
    ImGui::SameLine();
    if ( ImGui::Button( "Images" ) ) {
        p->StateMachine()->toggleVisible(SceneLayoutDefaultNames::Image);
    }

    static bool bFirstGather = true;
    if ( bFirstGather && FM::isPersistentInitialized() ) {
        Http::gatherCachedLogin();
        bFirstGather = false;
    }
    float loginItemWidth = 180.0f;
    float spacing = 10.0f;
    ImVec2 bsize{loginItemWidth, 20.0f};
    ImGui::SameLine(ImGui::GetWindowWidth() - (loginItemWidth + spacing));
    auto loginString = Http::cachedLoginFields().email + " | " + Http::project();
    if ( ImGui::Button( loginString.c_str(), bsize ) ) {
        p->StateMachine()->toggleVisible(SceneLayoutDefaultNames::Login);
    }

    ImGui::End();

}
