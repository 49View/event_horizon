#pragma  once

#include "ui_control.h"

class UiImage : public UiControl {
public:
	using UiControl::UiControl;
private:
	void initImpl() override;
};
