//
// Created by Dado on 2019-01-12.
//

#include "camera_rig.hpp"
#include <core/camera.h>
#include <core/camera_utils.hpp>

namespace CameraRigAngles {
    Vector3f Top   { M_PI_2,  0.0f,    0.0f };
    Vector3f Bottom{ -M_PI_2, 0.0f,    0.0f };
    Vector3f Left  { 0.0f,    -M_PI_2, 0.0f };
    Vector3f Right { 0.0f,    M_PI_2,  0.0f };
    Vector3f Front { 0.0f,    0.0f,    0.0f };
    Vector3f Back  { 0.0f,    M_PI,    0.0f };
}

std::string cubeRigName( int t, const std::string& _probeName ) {
    return _probeName + "_" + cubemapFaceToString( static_cast<CubemapFaces>(t) );
}

CubeMapRigContainer addCubeMapRig( const std::string& _name,
                                   const Vector3f& _pos,
                                   const Rect2f& _viewPort) {

    static std::vector<Vector3f> camAngles { CameraRigAngles::Right, CameraRigAngles::Left,
                                             CameraRigAngles::Top, CameraRigAngles::Bottom,
                                             CameraRigAngles::Front, CameraRigAngles::Back };

    std::array<std::shared_ptr<CameraRig>, 6> ret;

    float cubeMapFOV = 90.0f;
    for ( int faceIndex = 0; faceIndex < 6; faceIndex++ ) {
        auto rigName = cubeRigName( faceIndex, _name );
        auto c = std::make_shared<CameraRig>( rigName, _viewPort );
        c->getCamera()->setFoV( cubeMapFOV );
        c->getCamera()->setPosition( _pos );
        c->getCamera()->setQuatAngles( camAngles[faceIndex] );
        c->getCamera()->Mode( CameraControlType::Walk );
        c->getCamera()->update();

        ret[faceIndex] = c;
    }

    return ret;
}

CameraRig::CameraRig() {
    Name( Name::Foxtrot );
    init( Rect2f::MIDENTITY() );
}

CameraRig::CameraRig( const std::string& _name ) : NamePolicy(_name) {
    init( Rect2f::MIDENTITY() );
}

CameraRig::CameraRig( const std::string& _name, const Rect2f& _viewport ) {
    Name( _name );
    init( _viewport );
}

void CameraRig::init( const Rect2f& _viewport ) {
    mCamera = std::make_shared<Camera>( Name(), _viewport );
    mCameraVRLeftEye = std::make_shared<Camera>( Name() + "LeftEye", _viewport );
    mCameraVRRightEye = std::make_shared<Camera>( Name() + "RightEye", _viewport );

    mViewport = _viewport;
}

//CameraRig::CameraRig( const std::string& _name, std::shared_ptr<Framebuffer> _fb ) {
//    ASSERT(_fb != nullptr);
//    mName = _name;
//    mViewport = Rect2f{ V2fc::ZERO, {_fb->getWidth(), _fb->getHeight()} };
//    mFramebuffer = _fb;
//
//    mCamera = std::make_shared<Camera>( _name, mViewport );
//    mCameraVRLeftEye = std::make_shared<Camera>( _name + "LeftEye", mViewport );
//    mCameraVRRightEye = std::make_shared<Camera>( _name + "RightEye", mViewport );
//}

//void CameraRig::setFramebuffer( std::shared_ptr<Framebuffer> framebuffer ) {
//    mViewport = Rect2f{ V2fc::ZERO, {framebuffer->getWidth(), framebuffer->getHeight()} };
//    mFramebuffer = framebuffer;
//}

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

CameraControlType CameraRig::getMainCameraMode() const { return mCamera->Mode(); }

bool CameraRig::isActive() const { return Status() == CameraState::Active; }
