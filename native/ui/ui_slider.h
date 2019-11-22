#pragma once

#include "ui_control.h"

class UiSlider : public UiControl {
public:
	using UiControl::UiControl;

	void setValuesRange( const int _vstart, const int _vend, const int _curr );
	void setSliderBarTo( float value );

	void setValue( int value );

	void onValueChanged( int value );

private:
	void initImpl() override;

private:
	int mStartValue;
	int mEndValue;
	int mCurrValue;
};
