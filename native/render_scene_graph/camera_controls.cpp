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
#include <render_scene_graph/render_orchestrator.h>


CameraControl::CameraControl( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg ) :
        mCameraRig(std::move(cameraRig)), rsg(rsg) {}

auto CameraControl::wasd( const AggregatedInputData& mi ) {
    float moveForward = 0.0f;
    float strafe = 0.0f;
    float moveUp = 0.0f;
    float dollyFrequency = 10.0f;
    dampingVelocityFactor = 52.0f*GameTime::getCurrTimeStep();
    dampingVelocityFactor = clamp(dampingVelocityFactor, 0.0f, 1.0f);
    baseVelocity = 0.35f;

    isWASDActive = mi.TI().checkWASDPressed() != -1;
    if ( isWASDActive ) {
        currentVelocity = log10( 1.0f + currentVelocity + baseVelocity * GameTime::getCurrTimeStep() );
        if ( mi.TI().checkKeyPressed(GMK_R) || mi.TI().checkKeyPressed(GMK_PAGE_UP) ) moveUpInertia -= currentVelocity;
        if ( mi.TI().checkKeyPressed(GMK_F) || mi.TI().checkKeyPressed(GMK_PAGE_DOWN) )
            moveUpInertia += currentVelocity;
        if ( mi.TI().checkKeyPressed(GMK_W) || mi.TI().checkKeyPressed(GMK_UP) ) {
            moveForwardInertia += currentVelocity;
            dollyWalkingVerticalMovement += GameTime::getCurrTimeStep() * dollyFrequency;
        }
        if ( mi.TI().checkKeyPressed(GMK_S) || mi.TI().checkKeyPressed(GMK_DOWN) ) {
            moveForwardInertia -= currentVelocity;
            dollyWalkingVerticalMovement += GameTime::getCurrTimeStep() * dollyFrequency;
        }
        if ( mi.TI().checkKeyPressed(GMK_A) || mi.TI().checkKeyPressed(GMK_LEFT) ) strafeInertia += currentVelocity;
        if ( mi.TI().checkKeyPressed(GMK_D) || mi.TI().checkKeyPressed(GMK_RIGHT) ) strafeInertia -= currentVelocity;
        if ( mi.TI().checkKeyPressed(GMK_COMMA) ) baseVelocity += 0.001f;
        if ( mi.TI().checkKeyPressed(GMK_PERIOD) ) baseVelocity -= 0.001f;
    }
    moveForward = moveForwardInertia;
    strafe = strafeInertia;
    moveUp = moveUpInertia;

    currentVelocity *= dampingVelocityFactor;
    moveForwardInertia *= dampingVelocityFactor;
    strafeInertia *= dampingVelocityFactor;
    moveUpInertia *= dampingVelocityFactor;

    return std::tuple<float, float, float>(moveForward, strafe, moveUp);
}

void CameraControl::updateFromInputData( const AggregatedInputData& mi ) {

    auto camera = mCameraRig->getMainCamera();
//    if ( !camera->ViewPort().contains( mi.mousePos) ) return;

    updateFromInputDataImpl(camera, mi);
    camera->update();

}

std::shared_ptr<CameraRig> CameraControl::rig() {
    return mCameraRig;
}

std::shared_ptr<Camera> CameraControl::getMainCamera() {
    return mCameraRig->getMainCamera();
}

void CameraControlEditable::togglesUpdate( const AggregatedInputData& _aid ) {
    if ( !inputIsBlockedOnSelection() ) {
        ViewportTogglesT cvtTggles = ViewportToggles::None;
        // Keyboards
        if ( _aid.checkKeyToggleOn(GMK_1) ) cvtTggles |= ViewportToggles::DrawWireframe;
        if ( _aid.checkKeyToggleOn(GMK_G) ) cvtTggles |= ViewportToggles::DrawGrid;
        if ( cvtTggles != ViewportToggles::None ) {
            toggle(rig()->Cvt(), cvtTggles);
        }
    }
}

void CameraControlFly::selected( const UUID& _uuid, MatrixAnim& _trs, NodeVariantsSP _node, SelectableFlagT _flags ) {
    auto sn = selectedNodes.find(_uuid);
    auto selectColor = sn != selectedNodes.end() ? sn->second.oldColor : Color4f::DARK_YELLOW;
    Color4f oldColor{ Color4f::WHITE };

    if ( checkBitWiseFlag(_flags, SelectableFlag::Highlighted) ) {
        rsg.RR().changeMaterialColorOnUUID(_uuid, selectColor, oldColor);
    }
    if ( sn != selectedNodes.end() ) {
        selectedNodes.erase(sn);
    } else {
        selectedNodes.emplace(_uuid, Selectable{ oldColor, _trs, _node, _flags });
    }
}

void CameraControlFly::unselectImpl( const UUID& _uuid, Selectable& _node ) {
    Color4f oldColor{ Color4f::WHITE };
    rsg.RR().changeMaterialColorOnUUID(_uuid, _node.oldColor, oldColor);
}

void CameraControlFly::updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) {

    if ( !IsAlreadyInUse() || isWASDActive ) {
        togglesUpdate(mi);

        auto[moveForward, strafe, moveUp]  = wasd(mi);
        _cam->moveForward(moveForward);
        _cam->strafe(strafe);
        _cam->moveUp(moveUp);
        if ( mi.moveDiffSS(TOUCH_ZERO) != Vector2f::ZERO ) {
            auto quatAngles = V3f{ mi.moveDiffSS(TOUCH_ZERO).yx(), 0.0f };
            _cam->incrementQuatAngles(quatAngles);
        }

//        if ( !inputIsBlockedOnSelection() && mi.isMouseTouchedDownFirstTime(TOUCH_ZERO) ) {
//            unselectAll();
//            auto rayPick = _cam->rayViewportPickIntersection( mi.mousePos(TOUCH_ZERO) );
////            bool bHit =
//            rsg.SG().rayIntersect( rayPick.rayNear, rayPick.rayFar, [&]( NodeVariantsSP _geom, float _near) {
////                ### REF reimplement selection
////                std::visit( SelectionRecursiveLamba{*this}, _geom );
//            } );
////            if ( !bHit ) {
////            }
//        }
//
//        if ( mi.TI().checkKeyToggleOn( GMK_DELETE )) {
//            std::vector<UUID> uuids;
//            for ( const auto& [k,v] : selectedNodes ) {
//                uuids.emplace_back(k);
//            }
//            for ( const auto& ui : uuids ) {
//                rsg.SG().removeNode( ui );
//                erase_if_it( selectedNodes, ui );
//            }
//        }

    }

//        ### REF selected needs to be worked out with new assets graphs
//    for ( const auto& [k,v] : selectedNodes ) {
//        rsg.SG().visitNode( k, lambdaUpdateNodeTransform );
//    }
}

void CameraControlFly::renderControls() {
    for ( auto&[k, n] : selectedNodes ) {
        showGizmo(n, getMainCamera());
    }
}

CameraControlFly::CameraControlFly( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg )
        : CameraControlEditable(cameraRig, rsg) {
    cameraRig->getCamera()->Mode(CameraMode::Doom);
    cameraRig->getCamera()->LockAtWalkingHeight(false);
}

void CameraControlWalk::updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) {
    _cam->LockAtWalkingHeight(true);

    auto[moveForward, strafe, moveUp]  = wasd(mi);
    if ( isTouchBased() ) {
        if ( mi.moveDiffSS(TOUCH_ZERO) != Vector2f::ZERO &&
             mi.moveDiffSS(TOUCH_ONE) != Vector2f::ZERO &&
             mi.isMouseTouchedDown(TOUCH_ZERO) &&
             mi.isMouseTouchedDown(TOUCH_ONE) ) {
            moveForward += mi.moveDiffSS(TOUCH_ONE).y() * -4.0f;
            strafe += mi.moveDiffSS(TOUCH_ONE).x() * -4.0f;
        }
    }
    float headJogging = sin( dollyWalkingVerticalMovement ) * (currentVelocity*0.3f);
    _cam->moveForward(moveForward);
    _cam->strafe(strafe);
    _cam->moveUp(moveUp + headJogging);
    auto mdss = mi.moveDiffSS(TOUCH_ZERO);
    auto mdss1 = mi.moveDiffSS(TOUCH_ONE);
    baseAngularVelocity = 60.0f;
    if ( mdss != Vector2f::ZERO && mdss1 == V2f::ZERO ) {
        auto angledd = isTouchBased() ? mdss.yx() * V2f::Y_INV : mdss.yx();
        currentAngularVelocity += V2f{ angledd.x() * log10( 1.0f + baseAngularVelocity * GameTime::getCurrTimeStep() ),
                                      angledd.y() * log10( 1.0f + baseAngularVelocity * GameTime::getCurrTimeStep() )};
    }
    _cam->incrementQuatAngles(V3f{ currentAngularVelocity, 0.0f });
    currentAngularVelocity *= dampingVelocityFactor;
}

void CameraControl2d::selected( const UUID& _uuid, MatrixAnim& _trs, NodeVariantsSP _node, SelectableFlagT _flags ) {
    auto sn = selectedNodes.find(_uuid);
    auto selectColor = sn != selectedNodes.end() ? sn->second.oldColor : Color4f::DARK_YELLOW;
    Color4f oldColor{ Color4f::WHITE };

    if ( checkBitWiseFlag(_flags, SelectableFlag::Highlighted) ) {
        rsg.RR().changeMaterialColorOnUUID(_uuid, selectColor, oldColor);
    }
    if ( sn != selectedNodes.end() ) {
        selectedNodes.erase(sn);
    } else {
        selectedNodes.emplace(_uuid, Selectable{ oldColor, _trs, _node, _flags });
    }
}

void CameraControl2d::unselectImpl( const UUID& _uuid, Selectable& _node ) {
    Color4f oldColor{ Color4f::WHITE };
    rsg.RR().changeMaterialColorOnUUID(_uuid, _node.oldColor, oldColor);
}

void CameraControl2d::updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) {

    if ( IsAlreadyInUse() ) return;

    togglesUpdate(mi);

    float moveForward = 0.0f;
    float strafe = 0.0f;
    float moveUp = 0.0f;

    if ( mi.isMouseTouchedDown(TOUCH_ONE) ) {
        moveUp = mi.moveDiff(TOUCH_ONE).y();
        strafe = mi.moveDiff(TOUCH_ONE).x();
    }
    moveForward = mi.getScrollValue(); // It's safe to call it every frame as no gesture on wheel/magic mouse
    _cam->moveForward(moveForward);
    _cam->strafe(strafe);
    _cam->moveUp(moveUp);

}

void CameraControl2d::renderControls() {
    for ( auto&[k, n] : selectedNodes ) {
        showGizmo(n, getMainCamera());
    }
}

CameraControl2d::CameraControl2d( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg )
        : CameraControlEditable(cameraRig, rsg) {
}


CameraControlWalk::CameraControlWalk( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg )
        : CameraControl(cameraRig, rsg) {
    cameraRig->getCamera()->Mode(CameraMode::Doom);
    cameraRig->getCamera()->LockAtWalkingHeight(true);
}

CameraControlOrbit3d::CameraControlOrbit3d( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg )
        : CameraControlEditable(cameraRig, rsg) {
    cameraRig->getCamera()->Mode(CameraMode::Orbit);
}

void CameraControlOrbit3d::updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) {
    if ( mi.getScrollValue() != 0.0f ) {
        _cam->incrementOrbitDistance(-mi.getScrollValue());
    }

    if ( mi.moveDiffSS(TOUCH_ZERO) != Vector2f::ZERO ) {
        _cam->incrementSphericalAngles(mi.moveDiffSS(TOUCH_ZERO));
    }
    if ( mi.moveDiffSS(TOUCH_ONE) != Vector2f::ZERO ) {
        _cam->strafe(mi.moveDiff(TOUCH_ONE).x());
        _cam->moveUp(mi.moveDiff(TOUCH_ONE).y());
    }

}
