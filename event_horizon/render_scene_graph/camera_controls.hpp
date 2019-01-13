//
// Created by Dado on 2019-01-11.
//

#pragma once

#include <core/math/vector2f.h>
#include <graphics/graphic_constants.h>

class CameraRig;
class Camera;

enum class CameraControls {
    Edit2d,
    Plan2d,
    Fly,
    Walk
};

struct CameraInputData {
    ViewportTogglesT cvt = ViewportToggles::None;
    Vector2f mousePos = Vector2f::ZERO;
    bool isMouseTouchedDown = false;
    float scrollValue = 0.0f;
    Vector2f moveDiff = Vector2f::ZERO;
    Vector2f moveDiffSS = Vector2f::ZERO;

    float moveForward = 0.0f;
    float strafe = 0.0f;
    float moveUp = 0.0f;
};

class CameraControl {
public:
    explicit CameraControl( const std::shared_ptr<CameraRig>& cameraRig );
    virtual ~CameraControl() = default;
    void updateFromInputData( const CameraInputData& mi );
    virtual void updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const CameraInputData& mi ) = 0;

    std::shared_ptr<CameraRig> rig();
    std::shared_ptr<Camera> getMainCamera();

protected:
    std::shared_ptr<CameraRig> mCameraRig;
};

class CameraControlFly : public CameraControl {
public:
    explicit CameraControlFly( const std::shared_ptr<CameraRig>& cameraRig );
    ~CameraControlFly() override = default;
    void updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const CameraInputData& mi ) override;
};

class CameraControlFactory {
public:
    static std::shared_ptr<CameraControl> make( CameraControls _cc, std::shared_ptr<CameraRig> _cr );
};
