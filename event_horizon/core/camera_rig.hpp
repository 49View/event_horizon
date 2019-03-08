//
// Created by Dado on 2019-01-12.
//

#pragma once

#include <string>
#include <core/math/vector3f.h>
#include <core/math/rect2f.h>
#include <core/camera_utils.hpp>

class Framebuffer;
class Camera;

enum class CameraRigType {
    Flat,
    Probe360
};

class CameraRig {
public:
    CameraRig( const std::string &_name, const Rect2f& _viewport );
//    CameraRig( const std::string &_name, std::shared_ptr<Framebuffer> _fb );

    std::shared_ptr<Camera> getCamera();
    std::shared_ptr<Camera> getMainCamera() { return mCamera; }
    std::shared_ptr<Camera> getVRLeftCamera() { return mCameraVRLeftEye; }
    std::shared_ptr<Camera> getVRRightCamera() { return mCameraVRRightEye; }

//    std::shared_ptr<Framebuffer> getFramebuffer() { return mFramebuffer; };
//    void setFramebuffer( std::shared_ptr<Framebuffer> framebuffer );
    const Rect2f& getViewport() const;
    void setViewport( const Rect2f& viewport );

    bool contains( const Vector2f& pos ) const;
    Vector2f mousePickRayOrtho( const Vector2f& _pos );
    bool isActive() const;

    int CurrEye() const { return mCurrEye; }
    void CurrEye( int val ) { mCurrEye = val; }

    CameraMode getMainCameraMode() const;
    std::string Name() const { return mName; }

    ViewportTogglesT getCvt() const { return mCvt; }
    ViewportTogglesT& Cvt() { return mCvt; }

    CameraState Status() const { return mStatus; }
    void Status( CameraState val ) { mStatus = val; }

protected:
    std::string mName;
    std::shared_ptr<Camera> mCamera;
    std::shared_ptr<Camera> mCameraVRLeftEye;
    std::shared_ptr<Camera> mCameraVRRightEye;

    Rect2f mViewport;
//    std::shared_ptr<Framebuffer> mFramebuffer;

    ViewportTogglesT mCvt = ViewportToggles::None;
    CameraState mStatus = CameraState::Active;

    int mCurrEye = 0;
};
