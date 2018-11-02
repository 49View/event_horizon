#include "../../window_handling.hpp"
#include <iostream>
#include "../../platform_graphics.hpp"

#include "../gl_util.h"
#include "core/util.h"
#include "core/configuration/app_options.h"

namespace WindowHandling {

    static void output_error( int error, const char *msg ) {
        fprintf( stderr, "GLFW Error: %s\n", msg );
    }

    void initializeWindow( [[maybe_unused]] uint64_t flags, TextInput& ti, MouseInput& mi, Renderer& rr ) {

        double width{ 1920.0 };
        double height{ 1080.0 };
        emscripten_get_element_css_size( "display", &width, &height );
        emscripten_set_canvas_element_size("display", int(width), int(height));
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

        LOGR( "glfwInit" );

        window = glfwCreateWindow( width, height, "Sixthview", NULL, NULL );

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

        initImGUI();

        ShaderAssetBuilder{"shaders"}.build(rr);
    }

}