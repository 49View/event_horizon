//
//  window_handling.hpp
//
//

#pragma once

#include "../text_input.hpp"
#include "../mouse_input.hpp"
#include "../renderer.h"

struct GLFWwindow;
typedef void (* GLFWdropfun)(GLFWwindow*,int,const char**);

namespace WindowHandling {

	void initImGUI();
	void imguiUpdate();
	void imguiUpdateStart();
	void imguiUpdateEnd();
	void gatherMainScreenInfo();
	void setDropCallback( GLFWdropfun fn );

	extern GLFWwindow* window;
	extern bool bUseGLFWPoll;
};

