#pragma once

#include "ui_control.h"

class UiBackground : public UiControl {
public:
	using UiControl::UiControl;
protected:
	void initImpl() override;
};
