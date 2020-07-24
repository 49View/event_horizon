//
// Created by Dado on 2019-01-11.
//

#include "camera_controls.hpp"
#include <core/util.h>
#include <core/camera.h>
#include <core/node.hpp>
#include <core/camera_rig.hpp>
#include <core/math/vector_util.hpp>
#include <core/v_data.hpp>
#include <graphics/mouse_input.hpp>
#include <graphics/text_input.hpp>
#include <graphics/renderer.h>
#include <poly/scene_graph.h>
#include <render_scene_graph/render_orchestrator.h>


CameraControl::CameraControl( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg ) :
        mCameraRig(std::move(cameraRig)), rsg(rsg) {
    rsg.setMICursorCapture(false);
}

auto CameraControl::updateDollyWalkingVerticalMovement() {
    dollyWalkingVerticalMovement += GameTime::getCurrTimeStep() * dollyFrequency;
}

auto CameraControl::wasd( const AggregatedInputData& mi ) {
    float moveForward = 0.0f;
    float scrollWheelMult = 35.0f;
    float strafe = 0.0f;
    float moveUp = 0.0f;
    float velRatio = ( GameTime::getCurrTimeStep() / ONE_OVER_60HZ );
    dampingVelocityFactor = 0.866667f - ( ( velRatio - 1.0f ) * 0.01f );
    dampingAngularVelocityFactor = 0.866667f - ( ( velRatio - 1.0f ) * 0.03f );
    dampingVelocityFactor = clamp(dampingVelocityFactor, 0.0f, 1.0f);
    dampingAngularVelocityFactor = clamp(dampingAngularVelocityFactor, 0.0f, 1.0f);
    baseVelocity = 0.04f * GameTime::getCurrTimeStep();

    isWASDActive = mi.TI().checkWASDPressed() != -1;
    if ( isWASDActive ) {
        currentVelocity = log(1.0f + currentVelocity + baseVelocity);
//        currentVelocity = currentVelocity + baseVelocity;
        if ( mi.TI().checkKeyPressed(GMK_R) || mi.TI().checkKeyPressed(GMK_PAGE_UP) ) moveUpInertia -= currentVelocity;
        if ( mi.TI().checkKeyPressed(GMK_F) || mi.TI().checkKeyPressed(GMK_PAGE_DOWN) )
            moveUpInertia += currentVelocity;
        if ( mi.TI().checkKeyPressed(GMK_W) || mi.TI().checkKeyPressed(GMK_UP) ) {
            moveForwardInertia += currentVelocity;
            updateDollyWalkingVerticalMovement();
        }
        if ( mi.TI().checkKeyPressed(GMK_S) || mi.TI().checkKeyPressed(GMK_DOWN) ) {
            moveForwardInertia -= currentVelocity;
            updateDollyWalkingVerticalMovement();
        }
        if ( mi.TI().checkKeyPressed(GMK_A) || mi.TI().checkKeyPressed(GMK_LEFT) ) {
            strafeInertia += currentVelocity;
            updateDollyWalkingVerticalMovement();
        }
        if ( mi.TI().checkKeyPressed(GMK_D) || mi.TI().checkKeyPressed(GMK_RIGHT) ) {
            strafeInertia -= currentVelocity;
            updateDollyWalkingVerticalMovement();
        }
    }
    if ( !getMainCamera()->areScrollWheelMovementsLocked() ) {
        moveForwardInertia += baseVelocity * sign(mi.getScrollValue()) * scrollWheelMult;
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

void CameraControl::updateFromInputData( SceneGraph& sg, const AggregatedInputData& mi ) {

    auto camera = mCameraRig->getMainCamera();
//    if ( !camera->ViewPort().contains( mi.mousePos) ) return;

    if ( camera->areInputsEnabled() ) {
        updateFromInputDataImpl(camera, mi);
    }
    if ( sg.cameraCollisionDetection(camera) > 0.0f ) {
        currentVelocity = 0.0f;
    }
    camera->update();

}

std::shared_ptr<CameraRig> CameraControl::rig() {
    return mCameraRig;
}

std::shared_ptr<Camera> CameraControl::getMainCamera() {
    return mCameraRig->getMainCamera();
}

CameraControlType CameraControl::getControlType() const {
    return controlType;
}

void CameraControl::setControlType( CameraControlType _ct ) {
    controlType = _ct;
    mCameraRig->getMainCamera()->Mode(_ct);
}

void CameraControlEditable::togglesUpdate( const AggregatedInputData& _aid ) {
//        ViewportTogglesT cvtTggles = ViewportToggles::None;
//        // Keyboards
//        if ( _aid.checkKeyToggleOn(GMK_0) ) cvtTggles |= ViewportToggles::DrawWireframe;
////        if ( _aid.checkKeyToggleOn(GMK_G) ) cvtTggles |= ViewportToggles::DrawGrid;
//        if ( cvtTggles != ViewportToggles::None ) {
//            toggle(rig()->Cvt(), cvtTggles);
//        }
}

void CameraControlFly::updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) {
    if ( isWASDActive ) {
        togglesUpdate(mi);

        auto[moveForward, strafe, moveUp]  = wasd(mi);
        _cam->moveForward(moveForward*3.0f);
        _cam->strafe(strafe*3.0f);
        _cam->moveUp(moveUp*3.0f);
        if ( mi.moveDiffSS(TOUCH_ZERO) != V2fc::ZERO ) {
            auto quatAngles = V3f{ mi.moveDiffSS(TOUCH_ZERO).yx(), 0.0f }*3.0f;

            Quaternion qy( quatAngles.y(), Vector3f::Y_AXIS );
            Quaternion qx( quatAngles.x(), Vector3f::X_AXIS );
            Quaternion qSequence = _cam->quatAngle() * qy;
            qSequence = qx * qSequence;
            qSequence.normalise();
            _cam->setQuat( qSequence );
        }
    }
}

CameraControlFly::CameraControlFly( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg )
        : CameraControlEditable(cameraRig, rsg) {
    cameraRig->getCamera()->Mode(CameraControlType::Walk);
    cameraRig->getCamera()->LockAtWalkingHeight(false);
    rsg.setMICursorCapture(true);
    controlType = CameraControlType::Fly;
}

void CameraControlWalk::updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) {
    _cam->LockAtWalkingHeight(true);

    auto[moveForward, strafe, moveUp]  = wasd(mi);

    if ( isTouchBased() ) {
        if ( mi.moveDiffSS(TOUCH_ZERO) != V2fc::ZERO &&
             mi.moveDiffSS(TOUCH_ONE) != V2fc::ZERO &&
             mi.isMouseTouchedDown(TOUCH_ZERO) &&
             mi.isMouseTouchedDown(TOUCH_ONE) ) {
            moveForward += mi.moveDiffSS(TOUCH_ONE).y() * -4.0f;
            strafe += mi.moveDiffSS(TOUCH_ONE).x() * -4.0f;
        }
    }
    float headJogging = sin(dollyWalkingVerticalMovement) * ( currentVelocity * 0.3f );
    _cam->moveForward(moveForward);
    _cam->strafe(strafe);
    _cam->moveUp(moveUp + headJogging);
    bool isControlKeyDown =
            mi.TI().checkModKeyPressed(GMK_LEFT_CONTROL) || mi.TI().checkModKeyPressed(GMK_RIGHT_CONTROL);

    auto mdss = isControlKeyDown ? V2fc::ZERO : mi.moveDiffSS(TOUCH_ZERO);
    auto mdss1 = isControlKeyDown ? V2fc::ZERO : mi.moveDiffSS(TOUCH_ONE);
    float currAngularVelocity = baseAngularVelocity;// * ( GameTime::getCurrTimeStep() / ONE_OVER_60HZ );
    if ( mdss != V2fc::ZERO && mdss1 == V2fc::ZERO ) {
        auto angledd = isTouchBased() ? mdss.yx() * V2fc::Y_INV : mdss.yx();
        currentAngularVelocity += V2f{ angledd.x() * log10(1.0f + currAngularVelocity),
                                       angledd.y() * log10(1.0f + currAngularVelocity) };
    }
    Quaternion qy( currentAngularVelocity.y(), Vector3f::Y_AXIS );
    Quaternion qx( currentAngularVelocity.x(), Vector3f::X_AXIS );
    Quaternion qSequence = _cam->quatAngle() * qy;
    qSequence = qx * qSequence;
    qSequence.normalise();
    _cam->setQuat( qSequence );
    currentAngularVelocity *= dampingAngularVelocityFactor;
}

void CameraControl2d::updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) {

    togglesUpdate(mi);

    float moveForward = 0.0f;
    float strafe = 0.0f;
    float moveUp = 0.0f;
    constexpr float xzPlaneDistanceClamp = 0.1f;
    constexpr float turboBoostScrollFactor = 10.0f;

    if ( mi.isMouseTouchedDown(TOUCH_ONE) ) {
        moveUp = mi.moveDiff(TOUCH_ONE).y();
        strafe = mi.moveDiff(TOUCH_ONE).x();
    }

    if ( mi.isMouseTouchedDown(TOUCH_ZERO) ) {
//        auto pickedRay = _cam->rayViewportPickIntersection(mi.mousePos(TOUCH_ZERO));
//        Plane3f zeroPlane{V3f::UP_AXIS, 0.0f};
//        auto is = zeroPlane.intersectLine(pickedRay.rayNear, pickedRay.rayFar) * V3f{1.0f, 1.0f, -1.0f};
//        LOGRS("Mouse Position on the screen near: " << pickedRay.rayNear );
//        LOGRS("Mouse Position on the screen far: " << pickedRay.rayFar );
//        LOGRS("Intersection with screen: " << is );
    }

    moveForward = mi.getScrollValue(); // It's safe to call it every frame as no gesture on wheel/magic mouse
    // Turbo boost scrolling here
    if ( mi.TI().checkModKeyPressed(GMK_LEFT_CONTROL) ) moveForward *= turboBoostScrollFactor;
    _cam->moveForward(moveForward);

    // Clamp the Y position to a decent distance from the plane, sanity first
    if ( _cam->getPosition().y() < xzPlaneDistanceClamp ) _cam->setPosition({_cam->getPosition().x(), xzPlaneDistanceClamp, _cam->getPosition().z()});

    _cam->strafe(strafe);
    _cam->moveUp(moveUp);

}

CameraControl2d::CameraControl2d( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg )
        : CameraControlEditable(cameraRig, rsg) {
    controlType = CameraControlType::Edit2d;
    rsg.setMICursorCapture(false);
}


CameraControlWalk::CameraControlWalk( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg )
        : CameraControl(cameraRig, rsg) {
    cameraRig->getCamera()->Mode(CameraControlType::Walk);
    cameraRig->getCamera()->LockAtWalkingHeight(true);
    rsg.setMICursorCapture(true);
    controlType = CameraControlType::Walk;
}

CameraControlOrbit3d::CameraControlOrbit3d( std::shared_ptr<CameraRig> cameraRig, RenderOrchestrator& rsg )
        : CameraControlEditable(cameraRig, rsg) {
    cameraRig->getCamera()->Mode(CameraControlType::Orbit);
    rsg.setMICursorCapture(true);
    controlType = CameraControlType::Orbit;
}

void CameraControlOrbit3d::updateFromInputDataImpl( std::shared_ptr<Camera> _cam, const AggregatedInputData& mi ) {
    if ( mi.getScrollValue() != 0.0f ) {
        _cam->incrementOrbitDistance(-mi.getScrollValue());
    }

    if ( mi.moveDiffSS(TOUCH_ZERO) != V2fc::ZERO ) {
        _cam->incrementSphericalAngles(mi.moveDiffSS(TOUCH_ZERO));
    }
    if ( mi.moveDiffSS(TOUCH_ONE) != V2fc::ZERO ) {
        _cam->strafe(mi.moveDiff(TOUCH_ONE).x());
        _cam->moveUp(mi.moveDiff(TOUCH_ONE).y());
    }

}

std::shared_ptr<CameraControl>
CameraControlFactory::create( CameraControlType _ct, std::shared_ptr<CameraRig> cameraRig,
                              RenderOrchestrator& rsg ) {

    switch ( _ct ) {
        case CameraControlType::Edit2d:
            return std::make_shared<CameraControl2d>( cameraRig, rsg );
        case CameraControlType::Orbit:
            return std::make_shared<CameraControlOrbit3d>( cameraRig, rsg );
        case CameraControlType::Fly:
            return std::make_shared<CameraControlFly>( cameraRig, rsg );
        case CameraControlType::Walk:
            return std::make_shared<CameraControlWalk>( cameraRig, rsg );
        default:
            return nullptr;
    }
}
