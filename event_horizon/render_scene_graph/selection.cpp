//
// Created by Dado on 2019-01-13.
//

#include "selection.hpp"
#include <core/app_globals.h>
#include <core/camera.h>
#include <core/geom.hpp>
#include <graphics/imgui/imgui.h>
#include <graphics/imgui/ImGuizmo.h>
#include <render_scene_graph/scene_orchestrator.hpp>

void Selection::showGizmo( Selectable& _node, std::shared_ptr<Camera> _cam ) {

    const Matrix4f& _view = _cam->getViewMatrix();
    const Matrix4f& _proj = _cam->getProjectionMatrix();
    const Rect2f& _viewport = _cam->ViewPort();

    if ( !checkBitWiseFlag(_node.flags, SelectableFlag::Selected) ) return;

    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
    static bool useSnap = false;
    static float snap[3] = { 1.f, 1.f, 1.f };
    static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
    static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
    static bool boundSizing = false;
    static bool boundSizingSnap = false;

    float retinaMadness = AG.pixelDensity();
    Rect2f lViewport = _viewport;
    MatrixAnim& _trs = _node.trs;

    float rtop = AG.getScreenSizefUI.y() - ( (lViewport.top() + lViewport.height()) * retinaMadness);
    float rleft = (lViewport.origin().x() + lViewport.size().x()) * retinaMadness;
    ImGui::SetNextWindowPos( ImVec2{ rleft, rtop } );
    ImGui::SetNextWindowSize( ImVec2{ std::max(350.0f, AG.getScreenSizefUI.y()-rleft), 230.0f } );
    ImGui::Begin("Transform");

    ImGuizmo::BeginFrame();

    bIsOver = ImGuizmo::IsOver();
    bIsSelected = ImGuizmo::IsUsing();

    if (ImGui::IsKeyPressed(90))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(88))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(67)) // r Key
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Tr", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rt", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Sc", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;

    float matrixTranslation[3];
    float matrixRotation[3];
    float matrixRotationDeg[3];
    float matrixScale[3];

//    ImGuizmo::DecomposeMatrixToComponents( matrix, matrixTranslation, matrixRotation, matrixScale );

    _trs.Pos().fill( matrixTranslation );
    _trs.Euler().fill( matrixRotation );
    for ( auto q = 0; q < 3; q++ ) matrixRotationDeg[q] = radToDeg(matrixRotation[q]);
    _trs.Scale().fill( matrixScale );

    ImGui::InputFloat3("Tr", matrixTranslation, 3);
    ImGui::InputFloat3("Rt", matrixRotationDeg, 3);
    ImGui::InputFloat3("Sc", matrixScale, 3);

    V3f mtt = V3f{matrixTranslation};
    for ( auto q = 0; q < 3; q++ ) matrixRotation[q] = degToRad(matrixRotationDeg[q]);
    V3f mtr = V3f{matrixRotation};
    V3f mts = V3f{matrixScale};
    _trs.set( mtt, mtr, mts );

//    ImGuizmo::RecomposeMatrixFromComponents( matrixTranslation, matrixRotation, matrixScale, matrix );

    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }
    if (ImGui::IsKeyPressed(86))
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

    if ( ImGui::Button("Set Key") ) {
//        ### REF re-enable set key callback
//        std::visit( SelectionAddToKeyFrame{ LayoutMediator::Timeline::TimeLineName(),
//                                            LayoutMediator::Timeline::CurrentTime() }, _node.node );
    }
    ImGui::SameLine();

    static float matrix2[16];
    static V3f oldScaleDelta{1.0f};
    static float oldRotationAngle = 0.0f;
    Matrix4f localTransform = Matrix4f{ _trs };
    float* matrix = localTransform.rawPtr();

    Vector2f rorign{lViewport.origin().x() * retinaMadness, rtop };
    Vector2f rsize{lViewport.size().x() * retinaMadness, lViewport.size().y() * retinaMadness};
    ImGuizmo::SetRect( rorign.x(), rorign.y(), rsize.x(), rsize.y() );

    ImGuizmo::Manipulate( _view.rawPtr(), _proj.rawPtr(), mCurrentGizmoOperation, mCurrentGizmoMode, matrix, matrix2,
                          useSnap ? &snap[0] : NULL, boundSizing?bounds:NULL, boundSizingSnap?boundsSnap:NULL);

    float matrixTranslationDelta[3]{0.0f, 0.0f, 0.0f};
    float matrixRotationDelta[3]{0.0f, 0.0f, 0.0f};
    float matrixScaleDelta[3]{0.0f, 0.0f, 0.0f};

    ImGuizmo::DecomposeMatrixToComponents( matrix2, matrixTranslationDelta, matrixRotationDelta, matrixScaleDelta );
    ImGuizmo::DecomposeMatrixToComponents( matrix, matrixTranslation, matrixRotation, matrixScaleDelta );

    switch (mCurrentGizmoOperation)
    {
        case ImGuizmo::TRANSLATE:
            _trs.set( mtt + V3f{matrixTranslationDelta}, mtr, mts );
            break;
        case ImGuizmo::ROTATE: {
            if ( ImGuizmo::IsUsing() ) {
                auto rt = ImGuizmo::getRotationType();
                auto ra = ( ImGuizmo::getRotationAngle() - oldRotationAngle );
                V3f rot{};
                switch (rt) {
                    case ImGuizmo::MOVETYPE::ROTATE_X:
                        rot = V3f::X_AXIS * ra;
                        break;
                    case ImGuizmo::MOVETYPE::ROTATE_Y:
                        rot = V3f::Y_AXIS * ra;
                        break;
                    case ImGuizmo::MOVETYPE::ROTATE_Z:
                        rot = V3f::Z_AXIS * ra;
                        break;
                    default:
                        rot = Vector3f::ZERO;
                        break;
                }
                _trs.set( mtt, mtr + rot*-1.0f, mts );
                oldRotationAngle = ImGuizmo::getRotationAngle();
            }
        }
            break;
        case ImGuizmo::SCALE: {
            V3f newScaleDelta = V3f{ matrixScaleDelta };
            auto mtsdelta = mts + ( newScaleDelta - oldScaleDelta );
            _trs.set( mtt, mtr, max( mtsdelta, Vector3f{0.0001f} ) );
            oldScaleDelta = newScaleDelta;
        }
            break;
        default:
            break;
    }

    ImGui::End();
}

bool Selection::IsSelected() const {
    return bIsSelected;
}

void Selection::IsSelected( bool bIsSelected ) {
    Selection::bIsSelected = bIsSelected;
}

bool Selection::IsOver() const {
    return bIsOver;
}

void Selection::IsOver( bool bIsOver ) {
    Selection::bIsOver = bIsOver;
}

bool Selection::IsAlreadyInUse() const {
    return IsOver() || IsSelected();
}

void Selection::unselectAll() {
    for ( auto& [k,v] : selectedNodes ) {
        unselectImpl( k, v );
    }

    selectedNodes.clear();
}

bool Selection::isImGuiBusy() const {
#ifdef _USE_IMGUI_
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureKeyboard || io.WantCaptureMouse;
#else
    return false;
#endif
}

void Selection::unselect( const UUID& _uuid, Selectable& _node ) {
    unselectImpl( _uuid, _node );
    erase_if_it( selectedNodes, _uuid );
}

bool Selection::inputIsBlockedOnSelection() const {
    return IsAlreadyInUse() || isImGuiBusy();
}

void SelectionAddToKeyFrame::operator()( GeomSP arg ) const {
//        ### REF selected needs to be worked out with new assets graphs (UUID)
// ### DUMMY REMOVE THIS LINE LOGRS(time);
LOGRS(time);
//    arg->addKeyFrame( timelineName, time );
}
