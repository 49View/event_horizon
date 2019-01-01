#pragma once

#include "ui_control.h"

class UiButton : public UiControl {
public:
	UiButton(FontManager& fm) : UiControl( fm ) {};

protected:
	void initImpl() override;
};
