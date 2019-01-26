//
//  window_handling.hpp
//
//

#pragma once

#include <cstdint>

class TextInput;
class MouseInput;
class Renderer;

namespace WindowHandling {
	void initializeWindow( uint64_t flags, Renderer& rr );
	void reinitializeWindowWithSize( int width, int height );
	bool shouldWindowBeClosed();
	void flush();
	void pollEvents();
	void enableVSync( const bool val );
	void enableInputCallbacks();
	void disableInputCallbacks();
	bool isInputEnabled();
};

namespace WH = WindowHandling;

#ifdef _OPENGL
#include "opengl/window_handling_opengl.hpp"
#endif