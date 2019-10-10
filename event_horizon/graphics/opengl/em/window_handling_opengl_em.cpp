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

    void initializeWindow( [[maybe_unused]] uint64_t flags, Renderer& rr ) {

        double width{ 1280.0 };
        double height{ 720.0 };
        emscripten_get_element_css_size( "#canvas", &width, &height );
        width*=DevicePixelRatio();
        height*=DevicePixelRatio();
        AppGlobals::getInstance().setScreenSizei( {static_cast<int>(width), static_cast<int>(height)} );

//        emscripten_set_canvas_element_size( nullptr, int(width), int(height));
        LOGR( "GetWidnowSize %f %f", width, height );

        LOGR( "--- Initialising Graphics ---" );

        glfwSetErrorCallback( output_error );

        LOGR( "glfwSetErrorCallback" );

        if ( !glfwInit()) {
            fputs( "Faileid to initialize GLFW", stderr );
            emscripten_force_exit( EXIT_FAILURE );
        }

        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );
        glfwWindowHint(GLFW_FOCUSED, 1);
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

        LOGR( "glfwInit" );

        window = glfwCreateWindow( width, height, "EventHorizon", NULL, NULL );

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

        initImGUI();

        rr.setForcedFrameBufferSize( getScreenSizei );
    }

}