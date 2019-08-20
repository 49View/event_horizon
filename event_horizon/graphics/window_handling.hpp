//
//  window_handling.hpp
//
//

#pragma once

#include <cstdint>

class TextInput;
class MouseInput;
class Renderer;

struct WHDevice {
    static float devicePixelRatio;
};

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

    static inline float DevicePixelRatio() {
        return WHDevice::devicePixelRatio;
    }
    static inline void DevicePixelRatio(float _pr ) {
        WHDevice::devicePixelRatio = _pr;
    }
};

namespace WH = WindowHandling;

#ifdef USE_GLFW
#include <graphics/opengl/GLFW/window_handling_opengl_glfw.hpp>
#elif USE_GLFM
#include <graphics/opengl/mobile/window_handling_opengl_glfm.hpp>
#elif USE_GLFVR
#include <graphics/opengl/mobile_vr/window_handling_opengl_glfvr.hpp>
#endif
