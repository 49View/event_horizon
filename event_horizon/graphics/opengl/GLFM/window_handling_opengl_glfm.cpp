#include "window_handling_opengl_glfm.hpp"
#include <graphics/opengl/gl_headers.hpp>
#include <graphics/window_handling.hpp>

//#define _USE_IMGUI_

namespace WindowHandling {

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
