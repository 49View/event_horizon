#include "window_handling_opengl_glfvr.hpp"
#include <graphics/opengl/gl_headers.hpp>
#include <graphics/window_handling.hpp>

namespace WindowHandling {

    void initializeWindow( [[maybe_unused]] std::optional<std::string> title, [[maybe_unused]] std::optional<std::string> _width, [[maybe_unused]] std::optional<std::string> _height, [[maybe_unused]] uint64_t flags, Renderer& rr ) {
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

    void enableVSync( [[maybe_unused]]  const bool val ) {
    }

    bool isInputEnabled() {
        return true;
    }

    void imRenderLoopStats( const RenderStats& rs ) {
    }

}
