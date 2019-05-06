//
// Created by Dado on 2018-10-29.
//

#include "camera_layout.h"

#include <core/camera.h>
#include "core/camera_rig.hpp"
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/render_orchestrator.h>
#include <render_scene_graph/scene_bridge.h>
#include <render_scene_graph/layouts/layout_helper.hpp>

//namespace CameraRigAngles {
//    extern Vector3f Top   ;
//    extern Vector3f Bottom;
//    extern Vector3f Left  ;
//    extern Vector3f Right ;
//    extern Vector3f Front ;
//    extern Vector3f Back  ;
//}

void ImGuiCamera::renderImpl( SceneGraph& _sg, RenderOrchestrator& _rsg, JMATH::Rect2f& f ) {
//    float lTop[3] = { CameraRigAngles::Top.x(), CameraRigAngles::Top.y(), CameraRigAngles::Top.z()};
//    ImGui::BeginGroup();
//        ImGui::InputFloat3("Top", lTop);
//        CameraRigAngles::Top = V3f{lTop};
//    ImGui::EndGroup();
    for ( const auto& v : _sg.M<CameraRig>().list() ) {
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
}
