//
// Created by Dado on 2019-01-12.
//

#include "camera_rig.hpp"
#include <core/camera.h>
#include <graphics/framebuffer.h>

CameraRig::CameraRig( const std::string& _name, const Rect2f& _viewport ) {
    mName = _name;
    mCamera = std::make_shared<Camera>( _name, _viewport );
    mCameraVRLeftEye = std::make_shared<Camera>( _name + "LeftEye", _viewport );
    mCameraVRRightEye = std::make_shared<Camera>( _name + "RightEye", _viewport );

    mViewport = _viewport;
}

CameraRig::CameraRig( const std::string& _name, std::shared_ptr<Framebuffer> _fb ) {
    ASSERT(_fb != nullptr);
    mName = _name;
    mViewport = Rect2f{ Vector2f::ZERO, {_fb->getWidth(), _fb->getHeight()} };
    mFramebuffer = _fb;

    mCamera = std::make_shared<Camera>( _name, mViewport );
    mCameraVRLeftEye = std::make_shared<Camera>( _name + "LeftEye", mViewport );
    mCameraVRRightEye = std::make_shared<Camera>( _name + "RightEye", mViewport );
}

void CameraRig::setFramebuffer( std::shared_ptr<Framebuffer> framebuffer ) {
    mViewport = Rect2f{ Vector2f::ZERO, {framebuffer->getWidth(), framebuffer->getHeight()} };
    mFramebuffer = framebuffer;

//    mCamera->ViewPort( mViewport );
//    mCameraVRLeftEye->ViewPort( mViewport );
//    mCameraVRRightEye->ViewPort( mViewport );
}

std::shared_ptr<Camera> CameraRig::getCamera() {
//    if ( VRM.IsOn()) {
//        return CurrEye() == 0 ? mCameraVRLeftEye : mCameraVRRightEye;
//    }
    return mCamera;
}

bool CameraRig::contains( const Vector2f& pos ) const {
    return mViewport.contains( pos );
}

Vector2f CameraRig::mousePickRayOrtho( const Vector2f& _pos ) {
    return getMainCamera()->mousePickRayOrtho( _pos );
}

void CameraRig::setViewport( const Rect2f& viewport ) {
    mViewport = viewport;
    mCamera->ViewPort( viewport );
    mCameraVRLeftEye->ViewPort( viewport );
    mCameraVRRightEye->ViewPort( viewport );
}

const Rect2f& CameraRig::getViewport() const {
    return mViewport;
}

CameraMode CameraRig::getMainCameraMode() const { return mCamera->Mode(); }

bool CameraRig::isActive() const { return Status() == CameraState::Active; }
