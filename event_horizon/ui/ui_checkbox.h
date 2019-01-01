#pragma once

#include <string>
#include <vector>

#include "core/math/vector2f.h"
#include "ui_control.h"

class UiCheckBox : public UiControl {
public:
	using UiControl::UiControl;

	int State() const { return mState; }
	void State( int val ) { mState = val; }
	int toggleState();

    float getSliderBallOnLeftValue() const {
        return mSliderBallOnLeftValue;
    }

    float getSliderBallOffRightValue() const {
        return mSliderBallOffRightValue;
    }

protected:
	void initImpl() override;

private:
	float mSliderBallOnLeftValue = 0.0f;
	float mSliderBallOffRightValue = 0.0f;

	int mState = 1;
};
