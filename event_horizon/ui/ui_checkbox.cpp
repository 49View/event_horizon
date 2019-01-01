#include "ui_checkbox.h"

#include "../vertex_processing.h"
#include "../render_list.h"

void UiCheckBox::initImpl() {
//    auto lTextRect = FontManager::optimalSizeForText( mSize->value, fm(), mText, mFontSize );
//    alignOn( lTextRect );
//
//    mSliderBallOnLeftValue = lTextRect.width() * 1.1f;
//    mSliderBallOffRightValue = mSliderBallOnLeftValue + lTextRect.height() * 0.5f;

    mIsClickable = true;
}

int UiCheckBox::toggleState() {
    mState = !mState;
    return mState;
}

//bool UiCheckBox::onTouchUp( const Vector2f& /*pos*/, ModifiersKey /*mod*/ ) {
//	if ( !mEnabled ) return false;
//
//	if ( mTouchDownResult == TOUCH_RESULT_HIT ) {
//		AcceptInputs( true );
//		animateTo( mScale, Vector2f::ONE, 0.05f, 0.0f );
//		if ( MI.wasTouchUpSingleEvent() && mCallbackOnToggleAction ) {
//			auto bs = toggleState();
//			mCallbackOnToggleAction( bs );
//		}
//		mTouchDownResult = TOUCH_RESULT_NOT_HIT;
//		return true;
//	}
//	return false;
//}
