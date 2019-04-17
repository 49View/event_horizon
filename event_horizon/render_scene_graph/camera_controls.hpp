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
    explicit CameraControl( std::shared_ptr<CameraRig> cameraRig, RenderSceneGraph& rsg );
    virtual ~CameraControl() = default;
    void updateFromInputData( const CameraInputData& mi );
    virtual void updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const CameraInputData& mi ) = 0;
    virtual void renderControls( SceneOrchestrator* _p ) = 0;

    std::shared_ptr<CameraRig> rig();
    std::shared_ptr<Camera> getMainCamera();

protected:
    std::shared_ptr<CameraRig> mCameraRig;
    RenderSceneGraph& rsg;
};

class CameraControlEditable : public CameraControl, public Selection {
protected:
    using CameraControl::CameraControl;
    void togglesUpdate( const CameraInputData& mi );
};

class CameraControlFly : public CameraControlEditable {
public:
    using CameraControlEditable::CameraControlEditable;
    CameraControlFly( const std::shared_ptr<CameraRig>& cameraRig, RenderSceneGraph& rsg );
    ~CameraControlFly() override = default;
    void updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const CameraInputData& mi ) override;
    void renderControls( SceneOrchestrator* _p ) override;
    void selected( const UUID& _uuid, MatrixAnim& _localTransform, NodeVariantsSP _node, SelectableFlagT _flags ) override;

protected:
    void unselectImpl( const UUID& _uuid, Selectable& _node ) override;

protected:
    bool isWASDActive = false;
};

class CameraControlWalk : public CameraControl {
public:
    using CameraControl::CameraControl;
    CameraControlWalk( std::shared_ptr<CameraRig> cameraRig, RenderSceneGraph& rsg );
    ~CameraControlWalk() override = default;
    void updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const CameraInputData& mi ) override;
    void renderControls( SceneOrchestrator* _p ) override {}
protected:
    bool isWASDActive = false;
};

class CameraControl2d : public CameraControlEditable {
public:
    using CameraControlEditable::CameraControlEditable;
    CameraControl2d( std::shared_ptr<CameraRig> cameraRig, RenderSceneGraph& rsg );
    ~CameraControl2d() override = default;
    void updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const CameraInputData& mi ) override;
    void renderControls( SceneOrchestrator* _p ) override;
    void selected( const UUID& _uuid, MatrixAnim& _localTransform, NodeVariantsSP _node, SelectableFlagT _flags ) override;

protected:
    void unselectImpl( const UUID& _uuid, Selectable& _node ) override;
};

class CameraControlFactory {
public:
    static std::shared_ptr<CameraControl> make( CameraControls _cc, std::shared_ptr<CameraRig> _cr, RenderSceneGraph& _rsg );
};
