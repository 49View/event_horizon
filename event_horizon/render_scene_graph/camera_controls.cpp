//
// Created by Dado on 2019-01-11.
//

#include "camera_controls.hpp"
#include <core/camera.h>
#include <core/node.hpp>
#include <graphics/text_input.hpp>
#include <graphics/camera_rig.hpp>
#include <poly/geom_data.hpp>
#include <poly/ui_shape_builder.h>

CameraControl::CameraControl( const std::shared_ptr<CameraRig>& cameraRig, RenderSceneGraph& rsg ) : mCameraRig( cameraRig ),
                                                                                                     rsg( rsg) {}

void CameraControl::updateFromInputData( const CameraInputData& mi ) {

    auto camera = mCameraRig->getMainCamera();

    if ( !camera->ViewPort().contains( mi.mousePos) ) return;

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

void CameraControlFly::selected( const UUID& _uuid, MatrixAnim& _trs, NodeVariants _node, SelectableFlagT _flags ) {
    auto sn = selectedNodes.find( _uuid );
    auto selectColor = sn != selectedNodes.end() ? sn->second.oldColor : Color4f::DARK_YELLOW;
    Color4f oldColor{Color4f::WHITE};

    if ( checkBitWiseFlag( _flags, SelectableFlag::Highlighted ) ) {
        rsg.RR().changeMaterialColorOnUUID( _uuid, selectColor, oldColor );
    }
    if ( sn != selectedNodes.end() ) {
        selectedNodes.erase(sn);
    } else {
        selectedNodes.emplace( _uuid, Selectable{ oldColor, _trs, _node, _flags } );
    }
}

void CameraControlFly::unselectImpl( const UUID& _uuid, Selectable& _node ) {
    Color4f oldColor{Color4f::WHITE};
    rsg.RR().changeMaterialColorOnUUID( _uuid, _node.oldColor, oldColor );
}

void CameraControlFly::updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const CameraInputData& mi ) {

    if ( !IsAlreadyInUse() || isWASDActive ) {
        if ( !inputIsBlockedOnSelection() ) {
            ViewportTogglesT cvtTggles = ViewportToggles::None;
            // Keyboards
            if ( mi.ti.checkKeyToggleOn( GMK_1 )) cvtTggles |= ViewportToggles::DrawWireframe;
            if ( mi.ti.checkKeyToggleOn( GMK_G )) cvtTggles |= ViewportToggles::DrawGrid;
            if ( cvtTggles != ViewportToggles::None ) {
                toggle( rig()->Cvt(), cvtTggles );
            }
        }

        static float camVelocity = 1.000f;
        static float accumulatedVelocity = .0003f;
        float moveForward = 0.0f;
        float strafe = 0.0f;
        float moveUp = 0.0f;

        isWASDActive = mi.ti.checkWASDPressed() != -1;
        if ( isWASDActive ) {
            float vel = 0.003f*GameTime::getCurrTimeStep();
            camVelocity = vel + accumulatedVelocity;
            if ( mi.ti.checkKeyPressed( GMK_W ) ) moveForward = camVelocity;
            if ( mi.ti.checkKeyPressed( GMK_S ) ) moveForward = -camVelocity;
            if ( mi.ti.checkKeyPressed( GMK_A ) ) strafe = camVelocity;
            if ( mi.ti.checkKeyPressed( GMK_D ) ) strafe = -camVelocity;
            if ( mi.ti.checkKeyPressed( GMK_R ) ) moveUp = -camVelocity;
            if ( mi.ti.checkKeyPressed( GMK_F ) ) moveUp = camVelocity;
            accumulatedVelocity += GameTime::getCurrTimeStep()*0.025f;
            if ( camVelocity > 3.50f ) camVelocity = 3.50f;
        } else {
            accumulatedVelocity = 0.0003f;
        }

        _cam->moveForward( moveForward );
        _cam->strafe( strafe );
        _cam->moveUp( moveUp );
        if ( mi.moveDiffSS != Vector2f::ZERO ) {
            _cam->incrementQuatAngles( Vector3f( mi.moveDiffSS.yx(), 0.0f ));
        }

        if ( !inputIsBlockedOnSelection() && mi.isMouseSingleTap ) {
            unselectAll();
            Vector3f mRayNear = Vector3f::ZERO;
            Vector3f mRayFar = Vector3f::ZERO;
            _cam->mousePickRay( mi.mousePos, mRayNear, mRayFar );
//            bool bHit =
            rsg.rayIntersect( mRayNear, mRayFar, [&]( const NodeVariants& _geom, float _near) {
                std::visit( SelectionRecursiveLamba{*this}, _geom );
            } );
//            if ( !bHit ) {
//            }
        }

        if ( mi.ti.checkKeyToggleOn( GMK_DELETE )) {
            std::vector<UUID> uuids;
            for ( const auto& [k,v] : selectedNodes ) {
                uuids.emplace_back(k);
            }
            for ( const auto& ui : uuids ) {
                rsg.remove( ui );
                erase_if_it( selectedNodes, ui );
            }
        }

    }

    for ( const auto& [k,v] : selectedNodes ) {
        rsg.visitNode( k, lambdaUpdateNodeTransform );
    }
}

void CameraControlFly::renderControls( SceneOrchestrator* _p ) {
    for ( auto& [k,n] : selectedNodes ) {
        showGizmo( n, getMainCamera(), _p );
    }
}

bool CameraControlFly::inputIsBlockedOnSelection() const {
    return IsAlreadyInUse() || isImGuiBusy();
}

CameraControlFly::CameraControlFly( const std::shared_ptr<CameraRig>& cameraRig, RenderSceneGraph& rsg )
        : CameraControl( cameraRig, rsg ) {
    toggle( rig()->Cvt(), ViewportToggles::DrawGrid );
}

void CameraControlWalk::updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const CameraInputData& mi ) {
    _cam->LockAtWalkingHeight(true);

    static float camVelocity = 1.000f;
    static float accumulatedVelocity = .0003f;
    float moveForward = 0.0f;
    float strafe = 0.0f;
    float moveUp = 0.0f;

    isWASDActive = mi.ti.checkWASDPressed() != -1;
    if ( isWASDActive ) {
        float vel = 0.003f*GameTime::getCurrTimeStep();
        camVelocity = vel + accumulatedVelocity;
        if ( mi.ti.checkKeyPressed( GMK_W ) ) moveForward = camVelocity;
        if ( mi.ti.checkKeyPressed( GMK_S ) ) moveForward = -camVelocity;
        if ( mi.ti.checkKeyPressed( GMK_A ) ) strafe = camVelocity;
        if ( mi.ti.checkKeyPressed( GMK_D ) ) strafe = -camVelocity;
        if ( mi.ti.checkKeyPressed( GMK_R ) ) moveUp = -camVelocity;
        if ( mi.ti.checkKeyPressed( GMK_F ) ) moveUp = camVelocity;
        accumulatedVelocity += GameTime::getCurrTimeStep()*0.025f;
        if ( camVelocity > 3.50f ) camVelocity = 3.50f;
    } else {
        accumulatedVelocity = 0.0003f;
    }

    _cam->moveForward( moveForward );
    _cam->strafe( strafe );
    _cam->moveUp( moveUp );
    if ( mi.moveDiffSS != Vector2f::ZERO ) {
        _cam->incrementQuatAngles( Vector3f( mi.moveDiffSS.yx(), 0.0f ));
    }

//    _cam->moveForward( mi.moveForward );
//    _cam->strafe( mi.strafe );
//    _cam->moveUp( mi.moveUp );
//    if ( mi.moveDiffSS != Vector2f::ZERO ) {
//        _cam->incrementQuatAngles( Vector3f( mi.moveDiffSS.yx(), 0.0f ));
//    }
}

std::shared_ptr<CameraControl> CameraControlFactory::make( CameraControls _cc, std::shared_ptr<CameraRig> _cr,
                                                           RenderSceneGraph& _rsg) {
    switch ( _cc ) {
        case CameraControls::Fly:
            return std::make_shared<CameraControlFly>(_cr, _rsg);
        case CameraControls::Walk:
            return std::make_shared<CameraControlWalk>(_cr, _rsg);
        default:
            return nullptr;
    };
}

