#include <iostream>
#include <core/util.h>

#include "../../window_handling.hpp"
#include <graphics/opengl/GLFW/mouse_input_glfw.hpp>

namespace WindowHandling {

    static void output_error( int error, const char *msg ) {
        fprintf(stderr, "GLFW Error: %s\n", msg);
    }

    void resizeWindow( const Vector2i& _newSize ) {
        reinitializeWindowWithSize(_newSize.x(), _newSize.y());
    }

    void reinitializeWindowWithSize( int width, int height ) {
//        glfwSetWindowSize( window, width, height );

        glfwDestroyWindow(window);
        window = glfwCreateWindow(width, height, "Sixthview", NULL, NULL);
        glfwMakeContextCurrent(window);
    }

    Vector2i captureWindowSize() {
        double width{ 1280.0 };
        double height{ 720.0 };
        emscripten_get_element_css_size("#WasmCanvas", &width, &height);
        auto pixelRatio = emscripten_get_device_pixel_ratio();

        return Vector2i{ static_cast<int>(width * pixelRatio), static_cast<int>(height * pixelRatio) };
    }

    void initializeWindow( [[maybe_unused]] uint64_t flags, Renderer& rr ) {

        Vector2i windowSize = captureWindowSize();
        AppGlobals::getInstance().setScreenSizei(windowSize);

//        emscripten_set_canvas_element_size( nullptr, int(width), int(height));
        LOGRS("GetWidnowSize " << windowSize);

        LOGR("--- Initialising Graphics ---");

        glfwSetErrorCallback(output_error);

        LOGR("glfwSetErrorCallback");

        if ( !glfwInit() ) {
            fputs("Faileid to initialize GLFW", stderr);
            emscripten_force_exit(EXIT_FAILURE);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_FOCUSED, 1);
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
        glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
        // This is *NOT* supported on WebGL, do not try to enable it, it will just crash
//        glfwWindowHint(GLFW_SAMPLES, getMultiSampleCount());

        LOGR("glfwInit");

        window = glfwCreateWindow(windowSize.x(), windowSize.y(), "EventHorizon", NULL, NULL);

        LOGR("glfwCreateWindow");

        if ( !window ) {
            LOGR("Failed to create GLFW window");
            fputs("Failed to create GLFW window", stderr);
            glfwTerminate();
            emscripten_force_exit(EXIT_FAILURE);
        }

        glfwMakeContextCurrent(window);

        LOGR("glfwMakeContextCurrent");

//        checkGlError( "GlewInit", __LINE__, __FILE__ );
//        setTextureAligment( 1 );

        // get version info
        const GLubyte *renderer = glGetString(GL_RENDERER); // get renderer string
        const GLubyte *version = glGetString(GL_VERSION); // version as a string
        LOGR("Renderer: %s", renderer);
        LOGR("OpenGL version supported %s", version);

        // Get info on default framebuffer:
        gatherMainScreenInfo();

        Framebuffer::checkHDRSupport();

        glfwSetScrollCallback(window, GscrollCallback);
        glfwSetMouseButtonCallback(window, GMouseButtonCallback);

        initImGUI();

        rr.setForcedFrameBufferSize(getScreenSizei);
    }

}