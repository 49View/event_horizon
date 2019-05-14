#include "window_handling_opengl_glfm.hpp"
#include <graphics/opengl/gl_headers.hpp>
#include <graphics/window_handling.hpp>
#include "glfm.h"

//#define _USE_IMGUI_
GLFMDisplay *glfmdisplay = nullptr;

namespace WindowHandling {

    void initializeWindow( [[maybe_unused]] uint64_t flags, Renderer& rr ) {
        ASSERT( glfmdisplay );
        glfmSetDisplayConfig(glfmdisplay,
                             GLFMRenderingAPIOpenGLES3,
                             GLFMColorFormatRGBA8888,
                             GLFMDepthFormatNone,
                             GLFMStencilFormatNone,
                             GLFMMultisampleNone);
    }

    void gatherMainScreenInfo() {
        int w = 0,h = 0;
        V2i sizei{w,h};
        AppGlobals::getInstance().setScreenSizef( Vector2f{w,h});
        AppGlobals::getInstance().setScreenSizei( sizei );
    }
    
    void preUpdate() {
    }

    void enableInputCallbacks() {
    }

    void disableInputCallbacks() {
    }

    void flush() {
    }

    void pollEvents() {
    }

    void enableVSync( const bool val ) {
    }

    bool isInputEnabled() {
        return true;
    }

}
