//
// Created by Dado on 2019-01-11.
//

#include "camera_controls.hpp"
#include <core/camera.h>
#include <core/node.hpp>
#include <graphics/camera_rig.hpp>
#include <poly/geom_data.hpp>
#include <poly/ui_shape_builder.h>

CameraControl::CameraControl( const std::shared_ptr<CameraRig>& cameraRig, RenderSceneGraph& rsg ) : mCameraRig( cameraRig ),
                                                                                                     rsg( rsg) {}

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

void CameraControlFly::selected( const UUID& _uuid, MatrixAnim& _trs ) {
    auto sn = selectedNodes.find( _uuid );
    auto selectColor = sn != selectedNodes.end() ? sn->second.oldColor : Color4f::DARK_YELLOW;
    Color4f oldColor{Color4f::WHITE};

    rsg.RR().changeMaterialColorOnUUID( _uuid, selectColor, oldColor );
    if ( sn != selectedNodes.end() ) {
        selectedNodes.erase(sn);
    } else {
        selectedNodes.emplace( _uuid, Selectable{ oldColor, _trs } );
    }
}

void CameraControlFly::updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const CameraInputData& mi ) {
    _cam->moveForward( mi.moveForward );
    _cam->strafe( mi.strafe );
    _cam->moveUp( mi.moveUp );
    if ( mi.moveDiffSS != Vector2f::ZERO ) {
        _cam->incrementQuatAngles( Vector3f( mi.moveDiffSS.yx(), 0.0f ));
    }

    if ( mi.isMouseTouchDownFirst ) {
        Vector3f mRayNear = Vector3f::ZERO;
        Vector3f mRayFar = Vector3f::ZERO;
        _cam->mousePickRay( mi.mousePos, mRayNear, mRayFar );

        rsg.rayIntersect( mRayNear, mRayFar, [&]( const NodeVariants& _geom, float _near) {
            std::visit( SelectionRecursiveLamba{*this}, _geom );
        } );
    }

    for ( const auto& [k,v] : selectedNodes ) {
        rsg.visitNode( k, lambdaUpdateNodeTransform );
    }
}

Matrix4f CameraControlFly::getViewMatrix() {
    return getMainCamera()->getViewMatrix();
}

Matrix4f CameraControlFly::getProjMatrix() {
    return getMainCamera()->getProjectionMatrix();
}

void CameraControlFly::renderControls() {
    for ( const auto& [k,n] : selectedNodes ) {
        showGizmo( n.trs );
    }
}

std::shared_ptr<CameraControl> CameraControlFactory::make( CameraControls _cc, std::shared_ptr<CameraRig> _cr,
                                                           RenderSceneGraph& _rsg) {
    switch ( _cc ) {
        case CameraControls::Fly:
            return std::make_shared<CameraControlFly>(_cr, _rsg);
        default:
            return nullptr;
    };
}
