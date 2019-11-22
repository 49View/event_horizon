#include "window_handling_opengl_glfm.hpp"
#include <graphics/opengl/gl_headers.hpp>
#include <graphics/window_handling.hpp>

//#define _USE_IMGUI_

namespace WindowHandling {

    void initializeWindow( [[maybe_unused]] uint64_t flags, Renderer& rr ) {
    }

    bool shouldWindowBeClosed() {
        return false;
    }

    void gatherMainScreenInfo() {
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
