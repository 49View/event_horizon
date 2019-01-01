#pragma once

#include <string>

#include "ui_control.h"

class UiLabel : public UiControl {
public:
	using UiControl::UiControl;

	static Vector3f rectForSizeAndText( const Vector2f& size, const std::string& text );

protected:
	void initImpl() override;
};
