//
// Created by Dado on 2019-01-11.
//

#pragma once

#include <core/math/vector2f.h>
#include <graphics/graphic_constants.h>
#include <render_scene_graph/selection.hpp>

class CameraRig;
class Camera;
class RenderOrchestrator;
class TextInput;
struct AggregatedInputData;

enum class CameraControls {
    Edit2d,
    Plan2d,
    Fly,
    Walk
};

class CameraControl {
public:
    explicit CameraControl( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg );
    virtual ~CameraControl() = default;
    void updateFromInputData( const AggregatedInputData& mi );
    virtual void updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) = 0;
    virtual void renderControls() = 0;

    std::shared_ptr<CameraRig> rig();
    std::shared_ptr<Camera> getMainCamera();

protected:
    std::shared_ptr<CameraRig> mCameraRig;
    RenderOrchestrator& rsg;
};

class CameraControlEditable : public CameraControl, public Selection {
protected:
    using CameraControl::CameraControl;
    void togglesUpdate( const AggregatedInputData& mi );
};

class CameraControlOrbit3d : public CameraControlEditable {
public:
    using CameraControlEditable::CameraControlEditable;
    CameraControlOrbit3d( const std::shared_ptr<CameraRig>& cameraRig, RenderOrchestrator& rsg );
    ~CameraControlOrbit3d() override = default;
    void updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) override;
    void renderControls() override {}
    void selected( const UUID& _uuid, MatrixAnim& _localTransform, NodeVariantsSP _node, SelectableFlagT _flags ) override {}

protected:
    void unselectImpl( const UUID& _uuid, Selectable& _node ) override {}
};

class CameraControlFly : public CameraControlEditable {
public:
    using CameraControlEditable::CameraControlEditable;
    CameraControlFly( const std::shared_ptr<CameraRig>& cameraRig, RenderOrchestrator& rsg );
    ~CameraControlFly() override = default;
    void updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) override;
    void renderControls() override;
    void selected( const UUID& _uuid, MatrixAnim& _localTransform, NodeVariantsSP _node, SelectableFlagT _flags ) override;

protected:
    void unselectImpl( const UUID& _uuid, Selectable& _node ) override;

protected:
    bool isWASDActive = false;
};

class CameraControlWalk : public CameraControl {
public:
    using CameraControl::CameraControl;
    CameraControlWalk( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg );
    ~CameraControlWalk() override = default;
    void updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) override;
    void renderControls() override {}
protected:
    bool isWASDActive = false;
};

class CameraControl2d : public CameraControlEditable {
public:
    using CameraControlEditable::CameraControlEditable;
    CameraControl2d( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg );
    ~CameraControl2d() override = default;
    void updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) override;
    void renderControls() override;
    void selected( const UUID& _uuid, MatrixAnim& _localTransform, NodeVariantsSP _node, SelectableFlagT _flags ) override;

protected:
    void unselectImpl( const UUID& _uuid, Selectable& _node ) override;
};