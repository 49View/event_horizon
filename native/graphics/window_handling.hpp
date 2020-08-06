//
//  window_handling.hpp
//
//

#pragma once

#include <cstdint>
#include <optional>
#include <string>

class TextInput;
class MouseInput;
class Renderer;
class RenderStats;
class Vector2i;

namespace WindowHandling {
	void initializeWindow( std::optional<std::string> title, std::optional<std::string> _width, std::optional<std::string> _height, uint64_t flags, Renderer& rr );
	void reinitializeWindowWithSize( int width, int height );
	bool shouldWindowBeClosed();
	void flush();
	void pollEvents();
	void enableVSync( bool val );
	void enableInputCallbacks();
	void disableInputCallbacks();
    bool isMouseInputActive();
    bool isKeyboardInputActive();
    bool isInputEnabled();
    void enableMouseCursor( bool flag );
    Vector2i captureWindowSize();
    void imRenderLoopStats( const RenderStats& rs );
    void setMultiSampleCount(int count);
    int  getMultiSampleCount();
};

namespace WH = WindowHandling;

#ifdef USE_GLFW
#include <graphics/opengl/GLFW/window_handling_opengl_glfw.hpp>
#elif USE_GLFM
#include <graphics/opengl/mobile/window_handling_opengl_glfm.hpp>
#elif USE_GLFVR
#include <graphics/opengl/mobile_vr/window_handling_opengl_glfvr.hpp>
#endif
