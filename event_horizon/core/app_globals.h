#pragma once

#include "math/vector2i.h"
#include "math/vector2f.h"
#include "math/rect2f.h"

// Class to contain data which is shared across all instances of the application.
class AppGlobals {
public:
	float getFootageFrameRateNormal() const {
		return mFootageFrameRateNormal;
	}

	void setScreenSizef( const Vector2f& s ) {
		mScreenSizef = s;
#ifdef OSX
        mScreenSizefUI = mScreenSizef*0.5f;
#else
        mScreenSizefUI = mScreenSizef;
#endif
	}

	void setScreenSizei( const Vector2i& s ) {
		mScreenSizei = s;
	}

	const Vector2f& getScreenSizef() const {
		return mScreenSizef;
	}

	const Vector2f& getScreenSizefUI() const {
		return mScreenSizefUI;
	}

	Vector2f getScreenSizeInv() const {
		return reciprocal( mScreenSizef );
	}

	const Vector2i& getScreenSizei() const {
		return mScreenSizei;
	}

	const float getScreenAspectRatio() const {
		return mScreenSizef.x() / mScreenSizef.y();
	}

	Vector2f getScreenAspectRatioVector() const {
		return Vector2f( mScreenSizef.x() / mScreenSizef.y(), 1.0f );
	}

	Vector2f getScreenAspectRatioVectorY() const {
		return Vector2f( mScreenSizef.y() / mScreenSizef.x(), 1.0f );
	}

	Rect2f getScreenRect() const {
		return { Vector2f::ZERO, mScreenSizef };
	}

	Vector2f getAspectRatio2dFromUnnormalizedScreenPos( const Vector2f& _pos ) const {
		Vector2f ret = _pos / getScreenSizef();
		ret.set( ret.x() * getScreenAspectRatio(), ret.y() );
		return ret;
	}

	static AppGlobals& getInstance() {
		static AppGlobals appGlobals;
		return appGlobals;
	}

private:
	Vector2f mScreenSizef=Vector2f::ZERO;
	Vector2i mScreenSizei=Vector2i::ZERO;
    Vector2f mScreenSizefUI=Vector2f::ZERO;
	float	 mFootageFrameRateNormal{0.0f};
};

#define AG AppGlobals::getInstance()

#define getScreenSizef AppGlobals::getInstance().getScreenSizef()
#define getScreenSizefUI AppGlobals::getInstance().getScreenSizefUI()
#define getScreenSizei AppGlobals::getInstance().getScreenSizei()
#define getScreenSizeInv AppGlobals::getInstance().getScreenSizeInv()
#define getScreenAspectRatio AppGlobals::getInstance().getScreenAspectRatio()
#define getScreenAspectRatioVector AppGlobals::getInstance().getScreenAspectRatioVector()
#define getScreenAspectRatioVectorY AppGlobals::getInstance().getScreenAspectRatioVectorY()
#define getScreenRect AppGlobals::getInstance().getScreenRect()
