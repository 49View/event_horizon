#pragma  once

#include "ui_control.h"

class UiSeparator : public UiControl {
public:
	using UiControl::UiControl;
private:
	void initImpl() override;
};
