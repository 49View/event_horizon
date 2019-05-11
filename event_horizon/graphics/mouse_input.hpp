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
#include <core/observable.h>

#include "text_input.hpp"

#ifdef USE_GLFW
#include <graphics/opengl/GLFW/mouse_input_glfw.hpp>
#elif USE_GLFM
#include <graphics/opengl/GLFW/text_input_glfw.hpp>
#endif

class Renderer;

struct AggregatedInputData {
    TextInput& ti;
    Vector2f mousePos = Vector2f::ZERO;
    bool isMouseTouchedDown = false;
    bool isMouseTouchDownFirst = false;
    bool isMouseSingleTap = false;
    bool hasMouseMoved = false;
    float scrollValue = 0.0f;
    Vector2f moveDiff = Vector2f::ZERO;
    Vector2f moveDiffSS = Vector2f::ZERO;
};

class MouseInput : public Observable<MouseInput> {
public:
	MouseInput();
	MouseInput( MouseInput const& ) = delete;
	void operator=( MouseInput const& ) = delete;

public: // these are globals data accessed from low level functions on inputs etc
	void onTouchDown( const Vector2f& pos, UpdateSignals& _updateSignals );
	void onTouchMove( const Vector2f& pos, UpdateSignals& _updateSignals );
	void onTouchMoveM( float* x, float *y, int size, UpdateSignals& _updateSignals );
	void onTouchUp( const Vector2f& pos, UpdateSignals& _updateSignals );
	void onScroll( float amount, UpdateSignals& _updateSignals );
	void clearTaps();

	bool hasBeenTappedInRect( JMATH::Rect2f& rect );
	bool isRectInput() const { return mIsRectInput; }
	void setRectInput( bool _val );
	void updateRectInput();
	void pushNewRectInput();
	void ToggleRectInput();
	bool CanDrawRectInput() { return mCanDrawRectInput; }
	JMATH::Rect2f CurrRectInput() const;
	const std::vector<Rect2f>& inputRects() const {
		std::lock_guard<std::mutex> lock( mInputRectLock );
		return mInputRects;
	}

	bool wasTouchUpSingleEvent() const { return mSingleTapEvent && !mDoubleTapEvent; }
	bool wasDoubleTapEvent() { return mDoubleTapEvent; }
	float checkLinearSwipe( const vector2fList& targetSwipes, const vector2fList& playerSwipes );
	SwipeDirection checkSwipe();
	void setPaused( bool isPaused );
	inline bool isPaused() { return mPaused; }
	inline bool isTouchedDown() const { return mTouchedDown; }
	inline bool isTouchedDownFirstTime() const { return mTouchedDownFirstTime; }
	inline float getScrollValue() const { return mScrollValue; }
	inline const vector2fList& getGestureTaps() const { return mGesturesTaps; }
	Vector2f getFirstTap( YGestureInvert yInv = YGestureInvert::No ) const {
		Vector2f pos = mGesturesTaps.front();
		if ( yInv == YGestureInvert::Yes ) pos.invertY();
		return pos;
	}
	Vector2f getLastTap( YGestureInvert yInv = YGestureInvert::No ) const;
	Vector2f getCurrMoveDiff( YGestureInvert yInv = YGestureInvert::Yes ) const;
	Vector2f getCurrMoveDiffNorm( YGestureInvert yInv = YGestureInvert::Yes ) const;
	Vector3f getCurrMoveDiffMousePick() const;
	MouseButtonStatusValues MouseButtonStatus() const { return mMouseButtonStatus; }
	void MouseButtonStatus( MouseButtonStatusValues val ) { mMouseButtonStatus = val; }

	void setCursorType( MouseCursorType mct );

	float arrowVelocity() const { return accumulatedArrowVelocity; }
	void leftArrowPressed( float speed = 1.0f );
	void rightArrowPressed( float speed = 1.0f );

	void update( UpdateSignals& _updateSignals );
	void enableMouseCursor( bool val );

	Vector2f getCurrPos() const;
	Vector2f getCurrPosSS() const;
    bool hasMouseMoved() const;

private:
    void setWheelScrollcallbackOnce();
    void mouseButtonEventsUpdate( UpdateSignals& _updateSignals );
    int  getLeftMouseButtonState();
    void getCursorPos( double& xpos, double& ypos );
    void setupRectInputAfterStatusChange();
	void accumulateArrowTouches( float direction );
	bool checkLongTapDistance();

private:
	float mGestureTime = 0.0f; // From Touch Down to Up
	bool  mOverridedSwipe = false;
	bool  mCanTriggerLongTap = false;
	bool  mPaused = false;
	bool  mHasTouchedUp = false;
	bool  mTouchedDown = false;
	bool  mTouchedDownFirstTime = false;
	bool  mDoubleTapEvent = false;
	bool  mSingleTapEvent = false;
	bool  mbHasMouseMoved = false;
	Vector2f mRawTouchDownPos = Vector2f::ZERO;
	Vector2f mNormTouchDownPos = Vector2f::ZERO;
	Plane3f mUIPlane{ Vector3f::Z_AXIS* 1.0f, 0.0f };
	float accumulatedArrowVelocity = 0.0f;
	float mCurrTimeStep = 0;
	int32_t glfwMousePressLeftTick = 0;
	bool   mIsRectInput = false;
	bool   mCanDrawRectInput = false;
	std::vector<Rect2f> mInputRects;
	double xpos = 0.0;
	double ypos = 0.0;
	double xposOld = 0.0;
	double yposOld = 0.0;

	MouseButtonStatusValues mMouseButtonStatus;

	float mScrollValue = 0.0f;
	Vector2f mMoveDiff = Vector2f::ZERO;
	Vector3f mMoveDiffMousePick = Vector3f::ZERO;
	Vector2f mSingleTapPos = Vector2f::ZERO;

	Vector2f mGestureTapsFront = Vector2f::ZERO;
	Vector2f mGestureTapsBack = Vector2f::ZERO;

	vector2fList mGesturesTaps;
	std::vector<float> mTouchupTimeStamps;
	float mCurrTimeStamp = 0.0f;

public: // these are globals data accessed from low level functions on inputs etc
    static Vector2f GScrollData;
	mutable std::mutex mInputRectLock;
};
