//
//  mouse_input.hpp
//
//

#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <core/math/vector3f.h>
#include <core/math/plane3f.h>
#include <core/math/rect2f.h>
#include <core/htypes_shared.hpp>
#include <utility>
#include <core/observable.h>
#include <core/update_signals.hpp>

#include "text_input.hpp"

//#ifdef USE_GLFW
//#include <graphics/opengl/GLFW/mouse_input_glfw.hpp>
//#elif USE_GLFM
//#include <graphics/opengl/GLFM/text_input_glfm.hpp>
//#endif

class Renderer;
class TextInput;
class Camera;

static constexpr size_t MAX_TAPS = 10;

struct MouseButtonDataEntry {
    int action = MB_RELEASE;
    int mods = 0;
    double xpos;
    double ypos;
};

struct MouseButtonData {
    std::array<MouseButtonDataEntry, MAX_TAPS> button;
};

struct TouchStatus {
    bool hasTouchedUp{false};
    bool touchedDown{false};
    bool touchedDownFirstTime{false};
    bool doubleTapEvent{false};
    bool singleTapEvent{false};
    bool bHasMouseMoved{false};
    bool overridedSwipe{false};
    bool canTriggerLongTap{false};

    float gestureTime{0.0f}; // From Touch Down to Up
    std::vector<float> touchupTimeStamps{0.0f};

    Vector2f rawTouchDownPos {V2fc::ZERO};
    Vector2f normTouchDownPos{V2fc::ZERO};

    int32_t touchPressTick{0};

    double xpos    {0.0};
    double ypos    {0.0};
    double xposOld {0.0};
    double yposOld {0.0};

    Vector2f  moveDiff            {V2fc::ZERO};
    Vector3f  moveDiffMousePick   {V3f::ZERO};
    Vector2f  singleTapPos        {V2fc::ZERO};
    Vector2f  gestureTapsFront    {V2fc::ZERO};
    Vector2f  gestureTapsBack     {V2fc::ZERO};
    V2fVector gesturesTaps{};

    MouseButtonStatusValues mouseButtonStatus{MouseButtonStatusValues::UNKNOWN};
};

enum TouchIndex {
    TOUCH_ZERO = 0,
    TOUCH_ONE = 1,
    TOUCH_TWO = 2,
    TOUCH_THREE = 3,
};

struct AggregatedInputData {
    AggregatedInputData( TextInput& ti, float scrollValue, std::array<TouchStatus, MAX_TAPS>  status ) : ti( ti ),
                                                                                                               scrollValue(
                                                                                                                       scrollValue ),
                                                                                                               status(std::move( status )) {}

    [[nodiscard]] bool isMouseTouchedDownFirstTime( int _touchIndex ) const { return !mouseHasBeenEaten && status[_touchIndex].touchedDownFirstTime; }
    [[nodiscard]] bool isMouseTouchedDown( int _touchIndex ) const { return !mouseHasBeenEaten && status[_touchIndex].touchedDown; }
    [[nodiscard]] bool isMouseTouchedDownAndMoving( int _touchIndex ) const { return !mouseHasBeenEaten && status[_touchIndex].touchedDown && hasMouseMoved(_touchIndex); }
    [[nodiscard]] bool isMouseTouchedUp( int _touchIndex ) const { return !mouseHasBeenEaten && status[_touchIndex].hasTouchedUp; }
    [[nodiscard]] bool isMouseSingleTap( int _touchIndex ) const { return !mouseHasBeenEaten && status[_touchIndex].singleTapEvent; }
    [[nodiscard]] bool isMouseDoubleTap( int _touchIndex ) const { return !mouseHasBeenEaten && status[_touchIndex].doubleTapEvent; }
    [[nodiscard]] bool hasMouseMoved( int _touchIndex ) const { return !mouseHasBeenEaten && status[_touchIndex].bHasMouseMoved; }

    [[nodiscard]] V2f  mousePos( int _touchIndex ) const { return V2f{ status[_touchIndex].xpos, status[_touchIndex].ypos }; }
    [[nodiscard]] V2f  moveDiffSS( int _touchIndex ) const { return getCurrMoveDiffNorm(_touchIndex).dominant(); }
    [[nodiscard]] V2f  moveDiff( int _touchIndex ) const { return getCurrMoveDiff( _touchIndex, YGestureInvert::No ).dominant()*0.01f; }
    [[nodiscard]] float getScrollValue() const { return scrollValue; }

    [[nodiscard]] bool isMouseHasBeenEaten() const { return mouseHasBeenEaten; }
    void setMouseHasBeenEaten( bool _mouseHasBeenEaten ) { mouseHasBeenEaten = _mouseHasBeenEaten; }

    [[nodiscard]] V2f  mouseViewportPos( int _touchIndex, std::shared_ptr<Camera> cam ) const;

    [[nodiscard]] bool checkKeyToggleOn( int keyCode, bool overrideTextInput = false ) const;
    [[nodiscard]] TextInput& TI() const { return ti; }
private:
    [[nodiscard]] V2f getCurrMoveDiff( int _touchIndex, YGestureInvert yInv = YGestureInvert::Yes ) const;
    [[nodiscard]] V2f getCurrMoveDiffNorm( int _touchIndex, YGestureInvert yInv = YGestureInvert::Yes ) const;
    [[nodiscard]] V3f getCurrMoveDiffMousePick( int _touchIndex ) const;

private:
    TextInput&                        ti;
    float                             scrollValue = 0.0f;
    std::array<TouchStatus, MAX_TAPS> status;
    bool                              mouseHasBeenEaten = false;
};

class MouseInput : public Observable<MouseInput> {
public:
	MouseInput();
	MouseInput( MouseInput const& ) = delete;
	void operator=( MouseInput const& ) = delete;

public: // these are globals data accessed from low level functions on inputs etc
    void clearTaps();
    void setPaused( bool isPaused );
    inline bool isPaused() { return mPaused; }

	void onTouchDown( int _touchIndex, const Vector2f& pos, UpdateSignals& _updateSignals );
	void onTouchMove( int _touchIndex, const Vector2f& pos, UpdateSignals& _updateSignals );
	void onTouchUp(   int _touchIndex, const Vector2f& pos, UpdateSignals& _updateSignals );

    bool isTouchedDown(int _touchIndex) const;
    bool isTouchedDownFirstTime(int _touchIndex) const;
    bool wasTouchUpSingleEvent(int _touchIndex) const;
	bool wasDoubleTapEvent(int _touchIndex);
    bool hasMouseMoved( int _touchIndex ) const;

    Vector2f getFirstTap( int _touchIndex, YGestureInvert yInv = YGestureInvert::No ) const;
    Vector2f getLastTap( int _touchIndex, YGestureInvert yInv = YGestureInvert::No ) const;

    inline const vector2fList& getGestureTaps( int _touchIndex ) const;

    MouseButtonStatusValues MouseButtonStatus( int _touchIndex ) const;
    void MouseButtonStatus( int _touchIndex, MouseButtonStatusValues val );

    Vector2f getCurrPos( int _touchIndex ) const;
    Vector2f getCurrPosSS( int _touchIndex ) const;

    float checkLinearSwipe( int _touchIndex, const vector2fList& targetSwipes, const vector2fList& playerSwipes );
	SwipeDirection checkSwipe( int _touchIndex );

    void onScroll( float amount, UpdateSignals& _updateSignals );
	inline float getScrollValue() const { return mScrollValue; }

	void setCursorType( MouseCursorType mct );
    void enableMouseCursor( bool val );

    std::array<TouchStatus, MAX_TAPS> Status() const;
	void update( UpdateSignals& _updateSignals );

    [[nodiscard]] bool UseCaptureOnMove() const;
    void UseCaptureOnMove( bool _bUseCaptureOnMove );
private:
    void setWheelScrollcallbackOnce();
    void mouseButtonUpdatePositions( int _touchIndex, double xpos, double ypos );
    void mouseButtonEventsUpdate( int _touchIndex, UpdateSignals& _updateSignals );
    void getCursorPos( double& xpos, double& ypos );
	bool checkLongTapDistance( int _touchIndex );

private:
	bool  mPaused = false;
	bool  bUseCaptureOnMove = false;
    float mCurrTimeStamp = 0.0f;

    std::array<TouchStatus, MAX_TAPS> status;
	float mScrollValue = 0.0f;

public: // these are globals data accessed from low level functions on inputs etc
    static Vector2f GScrollData;
    static MouseButtonData GMouseButtonData;
};
