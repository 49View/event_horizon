//
// Created by Dado on 2019-01-12.
//

#pragma once

#include <string>
#include <core/math/vector3f.h>
#include <core/math/rect2f.h>
#include <core/camera_utils.hpp>
#include <core/name_policy.hpp>

class Camera;

enum class CameraRigType {
    Flat,
    Probe360
};

class CameraRig : public NamePolicy<> {
public:
    CameraRig();
    explicit CameraRig( const std::string &_name);
    CameraRig( const std::string &_name, const Rect2f& _viewport );
    virtual ~CameraRig() = default;
protected:

    void init( const Rect2f& _viewport );

public:
    std::shared_ptr<Camera> getCamera();
    std::shared_ptr<Camera> getMainCamera() { return mCamera; }
    std::shared_ptr<Camera> getVRLeftCamera() { return mCameraVRLeftEye; }
    std::shared_ptr<Camera> getVRRightCamera() { return mCameraVRRightEye; }

    const Rect2f& getViewport() const;
    void setViewport( const Rect2f& viewport );

    bool contains( const Vector2f& pos ) const;
    Vector2f mousePickRayOrtho( const Vector2f& _pos );
    bool isActive() const;

    int CurrEye() const { return mCurrEye; }
    void CurrEye( int val ) { mCurrEye = val; }

    CameraMode getMainCameraMode() const;

    ViewportTogglesT getCvt() const { return mCvt; }
    ViewportTogglesT& Cvt() { return mCvt; }

    CameraState Status() const { return mStatus; }
    void Status( CameraState val ) { mStatus = val; }

protected:
    std::shared_ptr<Camera> mCamera;
    std::shared_ptr<Camera> mCameraVRLeftEye;
    std::shared_ptr<Camera> mCameraVRRightEye;

    Rect2f mViewport = Rect2f::MIDENTITY();

    ViewportTogglesT mCvt = ViewportToggles::None;
    CameraState mStatus = CameraState::Active;

    int mCurrEye = 0;

public:
    static uint64_t Version() { return 1000; }
    inline const static std::string EntityGroup() { return ResourceGroup::CameraRig; }
};
