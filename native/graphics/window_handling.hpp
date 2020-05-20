//
//  window_handling.hpp
//
//

#pragma once

#include <cstdint>

class TextInput;
class MouseInput;
class Renderer;
class RenderStats;
class Vector2i;

namespace WindowHandling {
	void initializeWindow( uint64_t flags, Renderer& rr );
	void reinitializeWindowWithSize( int width, int height );
	bool shouldWindowBeClosed();
	void flush();
	void pollEvents();
	void enableVSync( bool val );
	void enableInputCallbacks();
	void disableInputCallbacks();
	bool isInputEnabled();
    void enableMouseCursor( bool flag );
    Vector2i captureWindowSize();
    void imRenderLoopStats( const RenderStats& rs );
};

namespace WH = WindowHandling;

#ifdef USE_GLFW
#include <graphics/opengl/GLFW/window_handling_opengl_glfw.hpp>
#elif USE_GLFM
#include <graphics/opengl/mobile/window_handling_opengl_glfm.hpp>
#elif USE_GLFVR
#include <graphics/opengl/mobile_vr/window_handling_opengl_glfvr.hpp>
#endif
