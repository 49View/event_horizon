//
// Created by Dado on 2019-01-11.
//

#pragma once

#include <core/math/vector2f.h>
#include <graphics/graphic_constants.h>
#include <render_scene_graph/selection.hpp>
#include <render_scene_graph/render_scene_graph.h>

class CameraRig;
class Camera;
class RenderSceneGraph;
class TextInput;

enum class CameraControls {
    Edit2d,
    Plan2d,
    Fly,
    Walk
};

struct CameraInputData {
    TextInput& ti;
    Vector2f mousePos = Vector2f::ZERO;
    bool isMouseTouchedDown = false;
    bool isMouseTouchDownFirst = false;
    bool isMouseSingleTap = false;
    float scrollValue = 0.0f;
    Vector2f moveDiff = Vector2f::ZERO;
    Vector2f moveDiffSS = Vector2f::ZERO;
};

class CameraControl {
public:
    explicit CameraControl( const std::shared_ptr<CameraRig>& cameraRig, RenderSceneGraph& rsg );
    virtual ~CameraControl() = default;
    void updateFromInputData( const CameraInputData& mi );
    virtual void updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const CameraInputData& mi ) = 0;
    virtual void renderControls() = 0;
    virtual bool inputIsBlockedOnSelection() const { return false; }

    std::shared_ptr<CameraRig> rig();
    std::shared_ptr<Camera> getMainCamera();

protected:
    std::shared_ptr<CameraRig> mCameraRig;
    RenderSceneGraph& rsg;
};

class CameraControlFly : public CameraControl, public Selection {
public:
    using CameraControl::CameraControl;
    ~CameraControlFly() override = default;
    void updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const CameraInputData& mi ) override;
    void renderControls() override;
    void selected( const UUID& _uuid, MatrixAnim& _localTransform ) override;
    bool inputIsBlockedOnSelection() const override;

protected:
    void unselect( const UUID& _uuid, const Selectable& _node ) override;

protected:
    bool isWASDActive = false;
};

class CameraControlFactory {
public:
    static std::shared_ptr<CameraControl> make( CameraControls _cc, std::shared_ptr<CameraRig> _cr, RenderSceneGraph& _rsg );
};
