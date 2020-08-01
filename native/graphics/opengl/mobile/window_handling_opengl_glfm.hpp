//
//  window_handling.hpp
//
//

#pragma once

#include <graphics/text_input.hpp>
#include <graphics/mouse_input.hpp>

struct GLFMDisplay;
extern GLFMDisplay *glfmdisplay;

namespace WindowHandling {

	void initImGUI();
	void preUpdate();
	void gatherMainScreenInfo();
};

