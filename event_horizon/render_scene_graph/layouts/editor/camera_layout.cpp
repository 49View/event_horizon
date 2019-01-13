//
// Created by Dado on 2018-10-29.
//

#include "camera_layout.h"

#include <core/camera.h>
#include "graphics/camera_rig.hpp"
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene.hpp>

void ImGuiCamera( Scene* p, const Rect2f& _r ) {
    ImGui::SetNextWindowPos( ImVec2{ _r.origin().x(), _r.origin().y() } );
    ImGui::SetNextWindowSize( ImVec2{ _r.size().x(), _r.size().y() } );
    ImGui::Begin( "Cameras",  nullptr, ImGuiWindowFlags_NoCollapse );

    for ( const auto& [k,v] : p->getRigs() ) {
        auto cam = v->getMainCamera();
        ImGui::BeginGroup();
        ImGui::Text( "Name: %s", cam->Name().c_str());
        ImGui::Text( "Pos: %f, %f, %f", cam->getPosition().x(), cam->getPosition().y(), cam->getPosition().z());
        ImGui::Text( "Angles: %f, %f, %f", cam->quatAngle().x(), cam->quatAngle().y(), cam->quatAngle().z());
        ImGui::Text( "Fov: %f", cam->FoV());
        ImGui::EndGroup();
        ImGui::Separator();
        ImGui::Separator();
    }
    ImGui::End();
}
