#pragma once

#include <functional>
#include "core/math/vector4f.h"
#include "core/math/matrix4f.h"
#include "core/math/rect2f.h"
#include "core/math/math_util.h"
#include "../font_manager.h"
#include "core/formatting_utils.h"

class UiControlGroup;
class UiControlFactory;

enum class UiDirections {
	Left,
	Right,
	Top,
	Bottom
};

enum class UiControlPosition : int32_t {
	ContinueX,
	ContinueY,
	Overlap,
	Invalid,
};

enum class UiControlSizeType : int32_t {
	FullSize,
	FullQuad,
	FullWidth,
	FullHeight,
	Explicit,
	Invalid,
};

struct UiControlInitData {
	std::string       control;
	UiControlPosition position = UiControlPosition::Invalid;
	UiControlSizeType sizeType = UiControlSizeType::Invalid;
	Vector2f		  origin = V2fc::ZERO;
	Vector2f		  size = V2fc::ZERO;
	Color4f			  color = V4fc::WHITE;
	Color4f			  backGroundColor = C4fc::HUGE_VALUE_NEG;
	float 			  alpha = 1.0f;
	std::string		  text;
	std::string		  title;
	std::string		  font;
	int				  flags = 0;
	Vector4f		  params = V4fc::ZERO;
	UiAlignElement	  margins = V4fc::ZERO;
	UiAlignElement	  padding = V4fc::ZERO;
};

struct UiControlBuilder {
	const std::string name{};
	Vector3f pos = Vector3f::ZERO;
	Vector2f size = V2fc::ZERO;
	Color4f mainColor = C4fc::ALMOND;
	Color4f backgroundColor = Vector4f::HUGE_VALUE_NEG;
    float alpha = 1.0f;
	std::string cname;
	std::string text;
	std::string title;
	std::string font;
	int flags = 0;
	Vector4f params = { 1.0f, 0.0f, 0.0f, 0.0f};
};

class UiControl {
public:
	UiControl( FontManager& fm );
	virtual void init( const UiControlBuilder& _data );
protected:
	virtual void initImpl() = 0;
public:
	virtual ~UiControl() {}

	uint64_t getId() const {
		return mId;
	}

	bool isVisible() const {
		return mVisible;
	}

	void setVisible( const bool val ) {
		mVisible = val;
	}

	const Vector3f& getPosition() const {
		return mPosition;
	}

	const Color4f& getMainColor() const {
		return mMainColor;
	}

	const Color4f& getBackgroundColor() const {
		return mBackgroundColor;
	}

	const Vector2f& getSize() const {
		return mSize;
	}

	const Vector4f& getParams() const {
        return mParams;
    }

	const std::string& getFontName() const {
		return mFontName;
	}

	const Font& getFont() const {
		return fm[mFontName];
	}

	float getFontSize() const {
		return mFontSize;
	}

    JMATH::Rect2f Rect() const {
		return JMATH::Rect2f( mPosition.xy(), mPosition.xy() + ( mSize*Vector2f::Y_INV ) );
	}

	JMATH::Rect2f Rect2d() const {
		return JMATH::Rect2f( mPosition.xy(), mSize*Vector2f{1.0f, mRectInv}, true );
	}

	TouchResult checkTouchDown( const Vector2f &pos ) const;

	void alignOn( const Rect2f& lTextRect );
	void addFlag( UiControlFlag flag );
	void removeFlag( UiControlFlag flag );
	bool hasFlag( UiControlFlag flag ) const;
	UiControlFlag flags() const;
	void setPosition( const Vector3f& position, bool /*resetTransform*/ = true ) {
		mPosition = position;
	}
	void setCoords( const Vector3f& position, const Vector2f& size, bool resetTransform = true ) {
		setPosition( position, resetTransform );
		mSize = size;
	}
	void setSize( const Vector2f& size ) {
		mSize = size;
	}
	bool isInside( const Vector2f& position ) const {
		return isInsideRect( position, mPosition.xy(), mSize );
	}

	void setEnabled( bool enabled );

	bool isClickable() const {
		return mIsClickable;
	}

	void isClickable( bool val ) {
		mIsClickable = val;
	}

	bool AcceptInputs() const {
		return mAcceptInputs;
	}

	void AcceptInputs( bool val ) {
		mAcceptInputs = val;
	}

	bool isEnabled() const {
		return mEnabled;
	}

	// callbacks
    void setOnValueChange( CommandCallbackFunction callbackFunction ) {
        mCallbackValueChanged = callbackFunction;
    }
    void setOnClickAction( CommandCallbackFunction callbackFunction ) {
		mCallbackOnClickAction = callbackFunction;
	}
	void setOnToggleAction( CommandCallbackFunction callbackFunction ) {
		mCallbackOnToggleAction = callbackFunction;
	}
	void setOnMove( CommandCallbackFunction callbackFunction ) {
		mCallbackOnMove = callbackFunction;
	}
	void setOnHoover( CommandCallbackFunction callbackFunction ) {
		mCallbackOnHoover = callbackFunction;
	}

    void CallbackOnClick( const CommandArgumentsT& args ) {
        if ( mCallbackOnClickAction ) mCallbackOnClickAction( args );
    }

	virtual void touchDownAnimation();
	virtual void touchUpAnimation();

    const std::string& getName() const {
        return mName;
    }

	const std::string& getTitle() const {
		return mTitle;
	}

	const std::string& getText() const {
		return mText;
	}

	void setName( const std::string& _val) {
		mName = _val;
	}

	void setText( const std::string& _val) {
		mText = _val;
	}

	void setTitle( const std::string& _val ) {
		mTitle = _val;
	}

//	void mainColorRenderLambda();
//	void alphaRenderLambda();

	void changeMainColor( const Color4f& colorTo, float time = 0.0f );
	void changeAlpha( const float alphaTo, float time = 0.0f );

	bool Selected() const { return mSelected; }
	void Selected( const bool val ) { mSelected = val; }

	bool IsScreenSpace() const { return mIsScreenSpace; }
	void IsScreenSpace( bool val ) { mIsScreenSpace = val; }

	Vector3f BaricentricHit( const Vector2f& pos ) const;

	bool isOnVR() const { return mbIsOnVR; }
	void isOnVR( const bool val ) { mbIsOnVR = val; }

	bool isOpaque() const;

	static UiControlPosition stringToUiControlPosition( const std::string& _value );
	static UiControlSizeType stringToUiControlSizeType( const std::string& _value );
	static UiControlFlag   stringToUiControlFlag( const std::string& _value );

protected:

	uint64_t mId = 0;
	int mFlags = 0;
	
	// status
	bool mVisible = true;
	bool mEnabled = true;
	bool mSelected = false;
	bool mIsClickable = false;
	bool mAcceptInputs = true;
	bool mbIsOnVR = false;
	bool mIsScreenSpace = true;
	float mEnableAlphaMult = 1.0f;
	float mRectInv = 1.0f; // decides where to swap or not (-1.0f, 1.0f) rect for touch check

	Vector3f mPosition;
	Vector2f mSize;
	Color4f  mMainColor;
	Color4f  mBackgroundColor;
    Vector4f mParams;

	std::string mName;

protected:
    std::string mTitle;
	std::string mText;
	std::string mFontName = defaultFontName;
	float mFontSize = 0.03f;
	FontManager& fm;

    CommandCallbackFunction mCallbackValueChanged = nullptr;
	CommandCallbackFunction mCallbackOnClickAction = nullptr;
	CommandCallbackFunction mCallbackOnToggleAction = nullptr;
	CommandCallbackFunction mCallbackOnMove = nullptr;
	CommandCallbackFunction mCallbackOnHoover = nullptr;

	friend class UiControlGroup;
};
