//
// Created by Dado on 2019-01-13.
//

#include "selection.hpp"
#include <graphics/imgui/imgui.h>
#include <graphics/imgui/ImGuizmo.h>

void showTransformGizmo(const float *cameraView, const float *cameraProjection, MatrixAnim& _trs )
{
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
    static bool useSnap = false;
    static float snap[3] = { 1.f, 1.f, 1.f };
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    ImGui::Begin("TransformGizmo");
    ImGuizmo::BeginFrame();

    if (ImGui::IsKeyPressed(90))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(69))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(82)) // r Key
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;

    float matrixTranslation[3];
    float matrixRotation[3];
    float matrixScale[3];
    _trs.Pos().fill( matrixTranslation );
    _trs.Euler().fill( matrixRotation );
    _trs.Scale().fill( matrixScale );

    ImGui::InputFloat3("Tr", matrixTranslation, 3);
    ImGui::InputFloat3("Rt", matrixRotation, 3);
    ImGui::InputFloat3("Sc", matrixScale, 3);

    _trs.set( V3f{matrixTranslation},  V3f{matrixRotation},  V3f{matrixScale} );
    Matrix4f localTransform = Matrix4f{ _trs };
    float* matrix = localTransform.rawPtr();

    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }
    if (ImGui::IsKeyPressed(83))
        useSnap = !useSnap;
    ImGui::Checkbox("", &useSnap);
    ImGui::SameLine();

    switch (mCurrentGizmoOperation)
    {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Snap", &snap[0]);
            break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &snap[0]);
            break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &snap[0]);
            break;
        default:
            break;
    }
    ImGui::Checkbox("Bound Sizing", &boundSizing);
    if (boundSizing)
    {
        ImGui::PushID(3);
        ImGui::Checkbox("", &boundSizingSnap);
        ImGui::SameLine();
        ImGui::InputFloat3("Snap", boundsSnap);
        ImGui::PopID();
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    ImGuizmo::Manipulate( cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL,
                          useSnap ? &snap[0] : NULL, boundSizing?bounds:NULL, boundSizingSnap?boundsSnap:NULL);
    _trs.set( localTransform.getPosition3(),  V3f{matrixRotation},  V3f{matrixScale} );

    ImGui::End();
}

void Selection::showTransform( MatrixAnim& _trs, const Matrix4f& _view, const Matrix4f& _proj ) {
    showTransformGizmo( _view.rawPtr(), _proj.rawPtr(), _trs );
}
