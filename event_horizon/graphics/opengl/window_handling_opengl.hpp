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
	void preUpdate();
	void resizeWindow( const Vector2i& _newSize );
	void gatherMainScreenInfo();
	void setDropCallback( GLFWdropfun fn );
	void setResizeWindowCallback( GLFWwindowsizefun fn );
	void setResizeFramebufferCallback( GLFWframebuffersizefun fn );

	extern GLFWwindow* window;
	extern bool bUseGLFWPoll;
};

