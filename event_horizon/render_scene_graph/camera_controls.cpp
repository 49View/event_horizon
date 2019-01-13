//
// Created by Dado on 2019-01-11.
//

#include "camera_controls.hpp"
#include <core/camera.h>
#include <graphics/camera_rig.hpp>

CameraControl::CameraControl( const std::shared_ptr<CameraRig>& cameraRig ) : mCameraRig( cameraRig ) {}

void CameraControl::updateFromInputData( const CameraInputData& mi ) {

    auto camera = mCameraRig->getMainCamera();

    if ( !camera->ViewPort().contains( mi.mousePos) ) return;

    if ( mi.cvt != ViewportToggles::None ) {
        toggle( mCameraRig->Cvt(), mi.cvt );
    }

    updateFromInputDataImpl( camera, mi );

//    if ( Mode() == CameraMode::Edit2d ) {
//        if ( mi.isMouseTouchedDown) {
//            pan( Vector3f( mi.moveDiff * Vector2f{-1.0f, 1.0f}, 0.0f ) );
//        }
//        zoom2d(mi.scrollValue); // It's safe to call it every frame as no gesture on wheel/magic mouse
//        // will mean zero value so unchanged
//    }
//
}

std::shared_ptr<CameraRig> CameraControl::rig() {
    return mCameraRig;
}

std::shared_ptr<Camera> CameraControl::getMainCamera() {
    return mCameraRig->getMainCamera();
}

void CameraControlFly::updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const CameraInputData& mi ) {
    _cam->moveForward( mi.moveForward );
    _cam->strafe( mi.strafe );
    _cam->moveUp( mi.moveUp );
    if ( mi.moveDiffSS != Vector2f::ZERO ) {
        _cam->incrementQuatAngles( Vector3f( mi.moveDiffSS.yx(), 0.0f ));
    }
}

CameraControlFly::CameraControlFly( const std::shared_ptr<CameraRig>& cameraRig ) : CameraControl( cameraRig ) {}

std::shared_ptr<CameraControl> CameraControlFactory::make( CameraControls _cc, std::shared_ptr<CameraRig> _cr ) {
    switch ( _cc ) {
        case CameraControls::Fly:
            return std::make_shared<CameraControlFly>(_cr);
        default:
            return nullptr;
    };
}
