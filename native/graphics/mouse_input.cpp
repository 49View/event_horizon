//
//  mouse_input.cpp
//

#include "mouse_input.hpp"
#include "window_handling.hpp"
#include <core/app_globals.h>
#include <core/camera.h>
#include <core/math/vector_util.hpp>

Vector2f MouseInput::GScrollData{ V2fc::ZERO };
MouseButtonData MouseInput::GMouseButtonData{};

Vector2f AggregatedInputData::getCurrMoveDiff( int _touchIndex, YGestureInvert yInv ) const {
    Vector2f ret = status[_touchIndex].moveDiff;
    if ( yInv == YGestureInvert::Yes ) ret.invertY();
    return ret;
}

Vector2f AggregatedInputData::getCurrMoveDiffNorm( int _touchIndex, YGestureInvert yInv ) const {
    return ( getCurrMoveDiff(_touchIndex, yInv) / getScreenSizef ) * getScreenAspectRatioVector;
}

Vector3f AggregatedInputData::getCurrMoveDiffMousePick( int _touchIndex ) const {
    return status[_touchIndex].moveDiffMousePick;
}

MouseInput::MouseInput() {
    for ( auto& st : status ) {
        st.gesturesTaps.reserve(100);
    }
}

void MouseInput::onTouchDown( int _touchIndex, const Vector2f& pos, UpdateSignals& _updateSignals ) {
    status[_touchIndex].mouseButtonStatus = MouseButtonStatusValues::DOWN;
    if ( status[_touchIndex].gesturesTaps.size() > 2 ) status[_touchIndex].gesturesTaps.clear();
    status[_touchIndex].gesturesTaps.push_back(pos);
    status[_touchIndex].gestureTime = 0.0f;
    status[_touchIndex].overridedSwipe = false;
    status[_touchIndex].canTriggerLongTap = true;

    status[_touchIndex].hasTouchedUp = false;
    status[_touchIndex].touchedDownFirstTime = ( !status[_touchIndex].touchedDown );
    status[_touchIndex].touchedDown = true;
    status[_touchIndex].moveDiff = V2fc::ZERO;
    status[_touchIndex].rawTouchDownPos = pos;
    status[_touchIndex].normTouchDownPos = pos / getScreenSizef;
    notify(*this, "OnTouchDown");
    _updateSignals.NeedsUpdate(true);
}

void MouseInput::onTouchMove( int _touchIndex, const Vector2f& pos, UpdateSignals& _updateSignals ) {
    if ( bUseCaptureOnMove ) {
        WH::enableMouseCursor(false);
    }
    status[_touchIndex].mouseButtonStatus = MouseButtonStatusValues::MOVING;
    status[_touchIndex].touchedDownFirstTime = false;
    if ( status[_touchIndex].gesturesTaps.empty() ) {
        status[_touchIndex].gesturesTaps.push_back(pos);
    }
    if ( pos != status[_touchIndex].gesturesTaps.back() ) {
        status[_touchIndex].moveDiff = pos - status[_touchIndex].gesturesTaps.back();
        status[_touchIndex].gesturesTaps.push_back(pos);
    } else {
        status[_touchIndex].moveDiff = V2fc::ZERO;
        status[_touchIndex].moveDiffMousePick = Vector3f::ZERO;
    }

    if ( status[_touchIndex].gestureTime > MAX_SWIPE_TIME_LIMIT ) {
        checkSwipe(_touchIndex);
    }
    status[_touchIndex].canTriggerLongTap = checkLongTapDistance(_touchIndex);
    notify(*this, "onTouchMove");
    _updateSignals.NeedsUpdate(true);
}

void MouseInput::onTouchUp( int _touchIndex, const Vector2f& pos, UpdateSignals& _updateSignals ) {
    if ( bUseCaptureOnMove ) {
        WH::enableMouseCursor(true);
    }
    status[_touchIndex].mouseButtonStatus = MouseButtonStatusValues::UP;
    status[_touchIndex].gesturesTaps.push_back(pos);
    status[_touchIndex].canTriggerLongTap = false;
    status[_touchIndex].hasTouchedUp = true;
    status[_touchIndex].touchedDown = false;
    status[_touchIndex].touchedDownFirstTime = false;
    status[_touchIndex].moveDiff = V2fc::ZERO;

    Vector2f xyd = status[_touchIndex].gesturesTaps.back() - status[_touchIndex].gesturesTaps.front();
    // Check if a single tap was performed
    if ( !status[_touchIndex].gesturesTaps.empty() && status[_touchIndex].gestureTime < SINGLE_TAP_TIME_LIMIT ) {
        status[_touchIndex].singleTapEvent = true;
        status[_touchIndex].singleTapPos = status[_touchIndex].gesturesTaps.back();
    }
    // Handle and check if double tap
    status[_touchIndex].touchupTimeStamps.push_back(mCurrTimeStamp);
    if ( status[_touchIndex].touchupTimeStamps.size() > 1 && status[_touchIndex].gesturesTaps.size() > 2 ) {
        float time2 = status[_touchIndex].touchupTimeStamps.back();
        float time1 = status[_touchIndex].touchupTimeStamps[status[_touchIndex].touchupTimeStamps.size() - 2];
        if ( time2 - time1 < DOUBLE_TAP_TIME_LIMIT ) {
            if ( length(xyd) < TAP_AREA ) {
                status[_touchIndex].doubleTapEvent = true;
                status[_touchIndex].touchupTimeStamps.clear();
            }
        }
    }

    if ( status[_touchIndex].doubleTapEvent ) {
        notify(*this, "onDoubleTap");
    } else if ( status[_touchIndex].singleTapEvent ) {
        notify(*this, "onSingleTap");
    } else {
        notify(*this, "onTouchUp");
    }
    _updateSignals.NeedsUpdate(true);
}

void MouseInput::onScroll( float amount, UpdateSignals& _updateSignals ) {
    float scroll_dir = 1.0f;
    mScrollValue = amount * scroll_dir;
    if ( amount != 0.0f ) {
        notify(*this, "onScroll");
        _updateSignals.NeedsUpdate(true);
    }
}

void MouseInput::clearTaps() {
    for ( auto& st : status ) {
        st.gesturesTaps.clear();
    }
}

bool AggregatedInputData::checkKeyToggleOn( int keyCode, bool overrideTextInput ) const {
    return ti.checkKeyToggleOn(keyCode, overrideTextInput);
}

V2f AggregatedInputData::mouseViewportPos( int _touchIndex, std::shared_ptr<Camera> cam ) const {
    auto pickedRay = cam->rayViewportPickIntersection(mousePos(_touchIndex));
    Plane3f zeroPlane{ V3f::UP_AXIS, 0.0f };
    return XZY::C(zeroPlane.intersectLine(pickedRay.rayNear, pickedRay.rayFar));
}

V3f AggregatedInputData::mouseViewportDir( int _touchIndex, std::shared_ptr<Camera> cam ) const {
    auto pickedRay = cam->rayViewportPickIntersection(mousePos(_touchIndex));
    return normalize(pickedRay.rayFar-pickedRay.rayNear);
}

SwipeDirection MouseInput::checkSwipe( int _touchIndex ) {
    // Check Swipe
    if ( status[_touchIndex].overridedSwipe ) {
        return SwipeDirection::INVALID;
    }

    Vector2f xyd = status[_touchIndex].gesturesTaps.back() - status[_touchIndex].gesturesTaps.front();

    const float SWIPE_ANGLE = 0.5f;
    const float SWIPE_LENGTH = 0.05f;

    if ( ( fabs(xyd.x()) < SWIPE_ANGLE || fabs(xyd.y()) < SWIPE_ANGLE ) && ( length(xyd) > SWIPE_LENGTH ) ) {
        status[_touchIndex].overridedSwipe = true;
        if ( fabs(xyd.x()) > fabs(xyd.y()) ) {
            if ( xyd.x() > 0.0f ) {
                return SwipeDirection::LEFT;
            } else {
                return SwipeDirection::RIGHT;
            }
        } else {
            if ( xyd.y() > 0.0f ) {
                return SwipeDirection::UP;
            } else {
                return SwipeDirection::DOWN;
            }
        }
    }
    return SwipeDirection::INVALID;
}

// Check Swipe on an arbitrary direction
// Return deviation from direction
float MouseInput::checkLinearSwipe( int _touchIndex, const vector2fList& targetSwipes,
                                    const vector2fList& playerSwipes ) {
    if ( status[_touchIndex].gesturesTaps.size() > 2 && status[_touchIndex].hasTouchedUp ) {
        float dist1 = distance(targetSwipes.front(), playerSwipes.front());
        float dist2 = distance(targetSwipes.back(), playerSwipes.back());

        float deviation = dist1 + dist2;
        return deviation;
    } else {
        return -1.0f;
    }
}

bool MouseInput::checkLongTapDistance( int _touchIndex ) {
    // Check Swipe
    if ( !status[_touchIndex].canTriggerLongTap ) {
        return false;
    }

    Vector2f xyd = status[_touchIndex].gesturesTaps.back() - status[_touchIndex].gesturesTaps.front();

    return length(xyd) < TAP_AREA;
}

void MouseInput::setPaused( bool isPaused ) {
    mPaused = isPaused;
}

Vector2f MouseInput::getLastTap( int _touchIndex, YGestureInvert yInv /*= YGestureInvert::No*/ ) const {
    Vector2f pos = V2fc::ZERO;
    if ( !status[_touchIndex].gesturesTaps.empty() ) {
        pos = status[_touchIndex].gesturesTaps.back();
        if ( yInv == YGestureInvert::Yes ) pos.invertY();
    }
    return pos;
}

Vector2f MouseInput::getCurrPos( int _touchIndex ) const {
    return Vector2f{ status[_touchIndex].xpos, status[_touchIndex].ypos };
}

Vector2f MouseInput::getCurrPosSS( int _touchIndex ) const {
    return Vector2f{ status[_touchIndex].xpos, getScreenSizef.y() - status[_touchIndex].ypos };
}

bool MouseInput::hasMouseMoved( int _touchIndex ) const {
    return status[_touchIndex].bHasMouseMoved;
}

Vector2f MouseInput::getFirstTap( int _touchIndex, YGestureInvert yInv ) const {
    Vector2f pos = status[_touchIndex].gesturesTaps.front();
    if ( yInv == YGestureInvert::Yes ) pos.invertY();
    return pos;
}

bool MouseInput::isTouchedDown( int _touchIndex ) const {
    return status[_touchIndex].touchedDown;
}

bool MouseInput::isTouchedDownFirstTime( int _touchIndex ) const {
    return status[_touchIndex].touchedDownFirstTime;
}

bool MouseInput::wasTouchUpSingleEvent( int _touchIndex ) const {
    return status[_touchIndex].singleTapEvent && !status[_touchIndex].doubleTapEvent;
}

bool MouseInput::wasDoubleTapEvent( int _touchIndex ) {
    return status[_touchIndex].doubleTapEvent;
}

const vector2fList& MouseInput::getGestureTaps( int _touchIndex ) const {
    return status[_touchIndex].gesturesTaps;
}

MouseButtonStatusValues MouseInput::MouseButtonStatus( int _touchIndex ) const {
    return status[_touchIndex].mouseButtonStatus;
}

void MouseInput::MouseButtonStatus( int _touchIndex, MouseButtonStatusValues val ) {
    status[_touchIndex].mouseButtonStatus = val;
}

void MouseInput::mouseButtonUpdatePositions( int _touchIndex, double xpos, double ypos ) {
    getCursorPos(xpos, ypos); // if doesNotHandleTouchMove use direct call, otherwise do nothing and rely on callback
    status[_touchIndex].xpos = xpos;
    status[_touchIndex].ypos = ypos;
    status[_touchIndex].bHasMouseMoved = ( status[_touchIndex].xpos != status[_touchIndex].xposOld ||
                                           status[_touchIndex].ypos != status[_touchIndex].yposOld );
    status[_touchIndex].xposOld = status[_touchIndex].xpos;
    status[_touchIndex].yposOld = status[_touchIndex].ypos;
#ifdef OSX
    status[_touchIndex].xpos *= 2.0;
    status[_touchIndex].ypos *= 2.0;
#endif
    status[_touchIndex].ypos = ( getScreenSizef.y() - status[_touchIndex].ypos );
}

void MouseInput::mouseButtonEventsUpdate( int _touchIndex, UpdateSignals& _updateSignals ) {
    auto buttonStatus = GMouseButtonData.button[_touchIndex];
    int mouseButtonState = buttonStatus.action;
    mouseButtonUpdatePositions(_touchIndex, buttonStatus.xpos, buttonStatus.ypos);
    if ( mouseButtonState == MB_PRESS ) {
        if ( status[_touchIndex].touchPressTick == 0 ) {
            onTouchDown(_touchIndex, { status[_touchIndex].xpos, status[_touchIndex].ypos }, _updateSignals);
        } else {
            onTouchMove(_touchIndex, { status[_touchIndex].xpos, status[_touchIndex].ypos }, _updateSignals);
        }
        ++status[_touchIndex].touchPressTick;
    } else if ( mouseButtonState == MB_RELEASE ) {
        if ( status[_touchIndex].touchPressTick != 0 ) {
            onTouchUp(_touchIndex, { status[_touchIndex].xpos, status[_touchIndex].ypos }, _updateSignals);
        }
        status[_touchIndex].touchPressTick = 0;
    }
}

std::array<TouchStatus, MAX_TAPS> MouseInput::Status() const {
    return status;
}

void MouseInput::update( UpdateSignals& _updateSignals ) {
    static constexpr size_t NUM_MOUSE_BUTTONS = 3;

    mCurrTimeStamp = GameTime::getCurrTimeStamp();

    if ( !WH::isMouseInputActive() ) return;

    for ( size_t _touchIndex = 0; _touchIndex < NUM_MOUSE_BUTTONS; _touchIndex++ ) {
        status[_touchIndex].gestureTime += GameTime::getCurrTimeStep();
        status[_touchIndex].gestureTapsFront = V2fc::ZERO;
        status[_touchIndex].gestureTapsBack = V2fc::ZERO;
        status[_touchIndex].singleTapEvent = false;
        status[_touchIndex].doubleTapEvent = false;
        status[_touchIndex].hasTouchedUp = false;
        if ( !status[_touchIndex].gesturesTaps.empty() ) {
            status[_touchIndex].gestureTapsFront = status[_touchIndex].gesturesTaps.front();
            status[_touchIndex].gestureTapsBack = status[_touchIndex].gesturesTaps.back();
        }

        if ( status[_touchIndex].canTriggerLongTap && status[_touchIndex].gestureTime > LONG_TAP_TIME_LIMIT ) {
            status[_touchIndex].canTriggerLongTap = false;
        }

        mouseButtonEventsUpdate(_touchIndex, _updateSignals);
    }

    onScroll(GScrollData.y(), _updateSignals);
    GScrollData = { 0.0f, 0.0f };
}
