#include "../../window_handling.hpp"
#include <iostream>
#include "../../platform_graphics.hpp"

#include "../gl_util.h"
#include <core/util.h>

namespace WindowHandling {

    static void output_error( int error, const char *msg ) {
        fprintf( stderr, "GLFW Error: %s\n", msg );
    }

    void resizeWindow( const Vector2i& _newSize ) {
        reinitializeWindowWithSize( _newSize.x(), _newSize.y() );
    }

    void reinitializeWindowWithSize( int width, int height ) {
//        glfwSetWindowSize( window, width, height );

        glfwDestroyWindow( window );
        window = glfwCreateWindow( width, height, "Sixthview", NULL, NULL );
        glfwMakeContextCurrent( window );
    }

    Vector2i captureWindowSize() {
        double width{ 1280.0 };
        double height{ 720.0 };
        emscripten_get_element_css_size( "#WasmCanvas", &width, &height );
        auto pixelRatio = emscripten_get_device_pixel_ratio();

        return Vector2i{ static_cast<int>(width*pixelRatio), static_cast<int>(height*pixelRatio)};
    }

    void initializeWindow( [[maybe_unused]] uint64_t flags, Renderer& rr ) {

        Vector2i windowSize = captureWindowSize();
        AppGlobals::getInstance().setScreenSizei(windowSize);

//        emscripten_set_canvas_element_size( nullptr, int(width), int(height));
        LOGRS( "GetWidnowSize " << windowSize );

        LOGR( "--- Initialising Graphics ---" );

        glfwSetErrorCallback( output_error );

        LOGR( "glfwSetErrorCallback" );


        if ( !glfwInit()) {
            fputs( "Faileid to initialize GLFW", stderr );
            emscripten_force_exit( EXIT_FAILURE );
        }

        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 2 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );
        glfwWindowHint(GLFW_FOCUSED, 1);
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
        glfwWindowHint( GLFW_SRGB_CAPABLE, GLFW_TRUE );

        LOGR( "glfwInit" );

        window = glfwCreateWindow( windowSize.x(), windowSize.y(), "EventHorizon", NULL, NULL );

        LOGR( "glfwCreateWindow" );

        if ( !window ) {
            LOGR( "Failed to create GLFW window" );
            fputs( "Failed to create GLFW window", stderr );
            glfwTerminate();
            emscripten_force_exit( EXIT_FAILURE );
        }

        glfwMakeContextCurrent( window );

        LOGR( "glfwMakeContextCurrent" );

//        checkGlError( "GlewInit", __LINE__, __FILE__ );
//        setTextureAligment( 1 );

        // get version info
        const GLubyte *renderer = glGetString( GL_RENDERER ); // get renderer string
        const GLubyte *version = glGetString( GL_VERSION ); // version as a string
        LOGR( "Renderer: %s", renderer );
        LOGR( "OpenGL version supported %s", version );

        // Get info on default framebuffer:
        gatherMainScreenInfo();

        Framebuffer::checkHDRSupport();
        bool hasFloatingPointTexgtures = emscripten_webgl_enable_extension( emscripten_webgl_get_current_context(), "OES_texture_float");
        bool hasFloatingPointLinearFiltering = emscripten_webgl_enable_extension( emscripten_webgl_get_current_context(), "OES_texture_float_linear");
        bool hasDepthTexture = emscripten_webgl_enable_extension( emscripten_webgl_get_current_context(), "WEBGL_depth_texture");
        bool has_sRGB = emscripten_webgl_enable_extension( emscripten_webgl_get_current_context(), "EXT_sRGB");
        bool hasHalfFloat = emscripten_webgl_enable_extension( emscripten_webgl_get_current_context(), "OES_texture_half_float");
        bool hasHalfFloatLinearFiltering = emscripten_webgl_enable_extension( emscripten_webgl_get_current_context(), "OES_texture_half_float_linear");
        bool hasDrawBuffers = emscripten_webgl_enable_extension( emscripten_webgl_get_current_context(), "WEBGL_draw_buffers");
        bool hasFloatColorBuffer = emscripten_webgl_enable_extension( emscripten_webgl_get_current_context(), "WEBGL_color_buffer_float");
        bool hasVAO = emscripten_webgl_enable_extension( emscripten_webgl_get_current_context(), "OES_vertex_array_object");

        LOGRS( "[WebGL1-EXT] Floating point texture " << boolAlphaBinary(hasFloatingPointTexgtures) );
        LOGRS( "[WebGL1-EXT] Floating point texture filtering " << boolAlphaBinary(hasFloatingPointLinearFiltering) );
        LOGRS( "[WebGL1-EXT] Depth texture " << boolAlphaBinary(hasDepthTexture) );
        LOGRS( "[WebGL1-EXT] sRGB support " << boolAlphaBinary(has_sRGB) );
        LOGRS( "[WebGL1-EXT] Half float " << boolAlphaBinary(hasHalfFloat) );
        LOGRS( "[WebGL1-EXT] Half float linear filtering " << boolAlphaBinary(hasHalfFloatLinearFiltering) );
        LOGRS( "[WebGL1-EXT] DrawBuffers " << boolAlphaBinary(hasDrawBuffers) );
        LOGRS( "[WebGL1-EXT] Floating point color buffer " << boolAlphaBinary(hasFloatColorBuffer) );
        LOGRS( "[WebGL1-EXT] VAO (Vertex Array Object) " << boolAlphaBinary(hasVAO) );

        initImGUI();

        rr.setForcedFrameBufferSize( getScreenSizei );
    }

}