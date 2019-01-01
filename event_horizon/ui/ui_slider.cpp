#include "ui_slider.h"

void UiSlider::initImpl() {
	mIsClickable = true;
	mAcceptInputs = true;

	mStartValue = 0;
	mEndValue = 1;
}

void UiSlider::setValuesRange( const int _vstart, const int _vend, const int _curr ) {
	mStartValue = _vstart;
	mEndValue = _vend;
	mCurrValue = _curr;
}

//TouchResult UiSlider::onTouchDown( const Vector2f& pos, ModifiersKey mod ) {
//	UiControl::onTouchDown( pos, mod );
//
//	if ( mTouchDownResult == TOUCH_RESULT_HIT ) {
//		AcceptInputs( false );
//		Vector3f baric = BaricentricHit( pos );
//		if ( isbetween( baric.x(), 0.0f, 1.0f ) ) {
//			onValueChanged( lerp( baric.x(), mStartValue, mEndValue ) );
//		}
//	}
//	return mTouchDownResult;
//}
//
//bool UiSlider::onTouchUp( const Vector2f& /*pos*/, ModifiersKey /*mod*/ ) {
//	if ( !mEnabled ) return false;
//
//	if ( mTouchDownResult == TOUCH_RESULT_HIT ) {
//		AcceptInputs( true );
//		animateTo( mScale, Vector2f::ONE, 0.05f, 0.0f );
//		mTouchDownResult = TOUCH_RESULT_NOT_HIT;
//		if ( mCallbackOnClickAction ) mCallbackOnClickAction();
//		return true;
//	}
//	return false;
//}
//
//bool UiSlider::onTouchMove( const Vector2f& pos, ModifiersKey /*mod*/ ) {
//	Vector3f baric = BaricentricHit( pos );
//	if ( isbetween( baric.x(), 0.0f, 1.0f ) ) {
//		onValueChanged( lerp( baric.x(), mStartValue, mEndValue ) );
//	}
//
//	//	Text(text);
//	return false;
//}

void UiSlider::setValue( int value ) {
	onValueChanged( value );
//	if ( mCallbackOnClickAction ) mCallbackOnClickAction();
}

void UiSlider::onValueChanged( int value ) {
	if ( value == mCurrValue ) return;
	mCurrValue = value;
//	setSliderBarTo( lerpInv( static_cast<float>( value ), static_cast<float>( mStartValue ), static_cast<float>( mEndValue ) ) );
//	if ( mCallbackValueChanged ) mCallbackValueChanged( value );
}
