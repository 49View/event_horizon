#pragma once

#include "ui_control.h"

class UiInputBox : public UiControl {
public:
	UiInputBox(FontManager& fm) : UiControl( fm ) {};

protected:
	void initImpl() override;
};

