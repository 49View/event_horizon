//
// Created by Dado on 2019-01-11.
//

#pragma once

#include <core/math/vector2f.h>
#include <core/camera_utils.hpp>
#include <graphics/graphic_constants.h>
#include <render_scene_graph/selection.hpp>

class CameraRig;
class Camera;
class RenderOrchestrator;
class TextInput;
struct AggregatedInputData;

class CameraControl {
public:
    CameraControl( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg );
    virtual ~CameraControl() = default;
    void updateFromInputData( SceneGraph& sg, const AggregatedInputData& mi );
    auto wasd( const AggregatedInputData& mi );
    auto updateDollyWalkingVerticalMovement();
    virtual void updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) = 0;
    virtual void renderControls() = 0;
    CameraControlType getControlType() const;
    void setControlType( CameraControlType _ct );

    std::shared_ptr<CameraRig> rig();
    std::shared_ptr<Camera> getMainCamera();

protected:
    std::shared_ptr<CameraRig> mCameraRig;
    CameraControlType controlType;
    RenderOrchestrator& rsg;
    bool isWASDActive = false;

    float moveForwardInertia = 0.0f;
    float strafeInertia = 0.0f;
    float moveUpInertia = 0.0f;
    float dollyWalkingVerticalMovement = 0.0f;
    float dollyFrequency = 10.0f;
    float baseVelocity = 0.06f;
    float baseAngularVelocity = 0.4f;
    float currentVelocity = 0.0f;
    V2f currentAngularVelocity = V2fc::ZERO;
    float topVelocity = 0.05f;
    float dampingVelocityFactor = 0.985f;
    float dampingAngularVelocityFactor = 0.985f;
};

class CameraControlEditable : public CameraControl, public Selection {
protected:
    using CameraControl::CameraControl;
    void togglesUpdate( const AggregatedInputData& mi );
};

class CameraControlOrbit3d : public CameraControlEditable {
public:
    using CameraControlEditable::CameraControlEditable;
    CameraControlOrbit3d( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg );
    ~CameraControlOrbit3d() override = default;
    void updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) override;
    void renderControls() override {}
    void
    selected( const UUID& _uuid, MatrixAnim& _localTransform, NodeVariantsSP _node, SelectableFlagT _flags ) override {}

protected:
    void unselectImpl( const UUID& _uuid, Selectable& _node ) override {}
};

class CameraControlFly : public CameraControlEditable {
public:
    using CameraControlEditable::CameraControlEditable;
    CameraControlFly( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg );
    ~CameraControlFly() override = default;
    void updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) override;
    void renderControls() override;
    void
    selected( const UUID& _uuid, MatrixAnim& _localTransform, NodeVariantsSP _node, SelectableFlagT _flags ) override;

protected:
    void unselectImpl( const UUID& _uuid, Selectable& _node ) override;
};

class CameraControlWalk : public CameraControl {
public:
    using CameraControl::CameraControl;
    CameraControlWalk( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg );
    ~CameraControlWalk() override = default;
    void updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) override;
    void renderControls() override {}
};

class CameraControl2d : public CameraControlEditable {
public:
    using CameraControlEditable::CameraControlEditable;
    CameraControl2d( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg );
    ~CameraControl2d() override = default;
    void updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) override;
    void renderControls() override;
    void
    selected( const UUID& _uuid, MatrixAnim& _localTransform, NodeVariantsSP _node, SelectableFlagT _flags ) override;

protected:
    void unselectImpl( const UUID& _uuid, Selectable& _node ) override;
};

namespace CameraControlFactory {
    std::shared_ptr<CameraControl>
    create( CameraControlType _ct, std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg );
}
