//
// Created by Dado on 2019-01-11.
//

#include "camera_controls.hpp"
#include <core/camera.h>
#include <core/node.hpp>
#include <core/camera_rig.hpp>
#include <core/math/vector_util.hpp>
#include <core/v_data.hpp>
#include <graphics/mouse_input.hpp>
#include <graphics/text_input.hpp>
#include <graphics/renderer.h>
#include <poly/resources/ui_shape_builder.h>
#include <render_scene_graph/render_orchestrator.h>


CameraControl::CameraControl( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg ) :
                              mCameraRig(std::move( cameraRig )), rsg( rsg) {}

void CameraControl::updateFromInputData( const AggregatedInputData& mi ) {

    auto camera = mCameraRig->getMainCamera();
//    if ( !camera->ViewPort().contains( mi.mousePos) ) return;

    updateFromInputDataImpl( camera, mi );
    camera->update();

}

std::shared_ptr<CameraRig> CameraControl::rig() {
    return mCameraRig;
}

std::shared_ptr<Camera> CameraControl::getMainCamera() {
    return mCameraRig->getMainCamera();
}

void CameraControlEditable::togglesUpdate( const AggregatedInputData& mi ) {
    if ( !inputIsBlockedOnSelection() ) {
        ViewportTogglesT cvtTggles = ViewportToggles::None;
        // Keyboards
        if ( mi.ti.checkKeyToggleOn( GMK_1 )) cvtTggles |= ViewportToggles::DrawWireframe;
        if ( mi.ti.checkKeyToggleOn( GMK_G )) cvtTggles |= ViewportToggles::DrawGrid;
        if ( cvtTggles != ViewportToggles::None ) {
            toggle( rig()->Cvt(), cvtTggles );
        }
    }
}

void CameraControlFly::selected( const UUID& _uuid, MatrixAnim& _trs, NodeVariantsSP _node, SelectableFlagT _flags ) {
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

void CameraControlFly::updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) {

    if ( !IsAlreadyInUse() || isWASDActive ) {
        togglesUpdate( mi );

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
        if ( mi.moveDiffSS(TOUCH_ZERO) != Vector2f::ZERO ) {
            _cam->incrementQuatAngles( V3f{ mi.moveDiffSS(TOUCH_ZERO).yx(), 0.0f } );
        }

        if ( !inputIsBlockedOnSelection() && mi.isMouseTouchedDownFirstTime(TOUCH_ZERO) ) {
            unselectAll();
            auto rayPick = _cam->rayViewportPickIntersection( mi.mousePos(TOUCH_ZERO) );
//            bool bHit =
            rsg.SG().rayIntersect( rayPick.rayNear, rayPick.rayFar, [&]( NodeVariantsSP _geom, float _near) {
//                ### REF reimplement selection
//                std::visit( SelectionRecursiveLamba{*this}, _geom );
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
                rsg.SG().removeNode( ui );
                erase_if_it( selectedNodes, ui );
            }
        }

    }

//        ### REF selected needs to be worked out with new assets graphs
//    for ( const auto& [k,v] : selectedNodes ) {
//        rsg.SG().visitNode( k, lambdaUpdateNodeTransform );
//    }
}

void CameraControlFly::renderControls() {
    for ( auto& [k,n] : selectedNodes ) {
        showGizmo( n, getMainCamera() );
    }
}

CameraControlFly::CameraControlFly( const std::shared_ptr<CameraRig>& cameraRig, RenderOrchestrator& rsg )
        : CameraControlEditable( cameraRig, rsg ) {
    toggle( rig()->Cvt(), ViewportToggles::DrawGrid );
    cameraRig->getCamera()->setPosition( Vector3f( 0.0f, 1.0f, 3.0f ) );
}

void CameraControlWalk::updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) {
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
    if ( mi.moveDiffSS(TOUCH_ZERO) != Vector2f::ZERO ) {
        _cam->incrementQuatAngles( V3f{ mi.moveDiffSS(TOUCH_ZERO).yx(), 0.0f } );
    }
}


void CameraControl2d::selected( const UUID& _uuid, MatrixAnim& _trs, NodeVariantsSP _node, SelectableFlagT _flags ) {
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

void CameraControl2d::unselectImpl( const UUID& _uuid, Selectable& _node ) {
    Color4f oldColor{Color4f::WHITE};
    rsg.RR().changeMaterialColorOnUUID( _uuid, _node.oldColor, oldColor );
}

void CameraControl2d::updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) {

    if ( IsAlreadyInUse() ) return;

    togglesUpdate( mi );

    float moveForward = 0.0f;
    float strafe = 0.0f;
    float moveUp = 0.0f;

    if ( mi.isMouseTouchedDown(TOUCH_ONE)) {
        moveUp = mi.moveDiff(TOUCH_ONE).y();
        strafe = mi.moveDiff(TOUCH_ONE).x();
    }
    moveForward = mi.scrollValue; // It's safe to call it every frame as no gesture on wheel/magic mouse
    _cam->moveForward( moveForward );
    _cam->strafe( strafe );
    _cam->moveUp( moveUp );

}

void CameraControl2d::renderControls() {
    for ( auto& [k,n] : selectedNodes ) {
        showGizmo( n, getMainCamera() );
    }
}

CameraControl2d::CameraControl2d( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg )
        : CameraControlEditable( cameraRig, rsg ) {
}


CameraControlWalk::CameraControlWalk( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg )
        : CameraControl( cameraRig, rsg ) {
}
