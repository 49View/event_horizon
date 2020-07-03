#include "../../platform_graphics.hpp"

#include <core/util.h>
#include <graphics/window_handling.hpp>
#include <graphics/opengl/GLFW/window_handling_opengl_glfw.hpp>
#include <graphics/opengl/GLFW/mouse_input_glfw.hpp>
#include <graphics/framebuffer.h>
#include "imgui.h"
#include "../imgui_impl_opengl3.h"

namespace WindowHandling {

    void resizeWindow( const Vector2i& _newSize ) {
        // we do not need to call this as we have the callback active in native/desktop.
        // (we do it in emscripten because it's weird!) :/
//        glfwSetWindowSize( window, _newSize.x(), _newSize.y() );
    }

    void initializeWindow( uint64_t flags, Renderer& rr ) {
        LOGR( "--- Initialising Graphics ---" );

        glfwWindowHint( GLFW_SRGB_CAPABLE, GLFW_TRUE );
        glfwWindowHint( GLFW_SAMPLES, getMultiSampleCount() );

        if ( !glfwInit() ) {
            LOGE( "Could not start GLFW3" );
        }
        initGraphics();

        const GLFWvidmode *mode = glfwGetVideoMode( glfwGetPrimaryMonitor());
        if ( flags & InitializeWindowFlags::FullScreen ) {
            glfwWindowHint( GLFW_RED_BITS, mode->redBits );
            glfwWindowHint( GLFW_GREEN_BITS, mode->greenBits );
            glfwWindowHint( GLFW_BLUE_BITS, mode->blueBits );
            glfwWindowHint( GLFW_REFRESH_RATE, 90 );// mode->refreshRate
            glfwSwapInterval( 1 );
            window = glfwCreateWindow( mode->width, mode->height, "Event Horizon", glfwGetPrimaryMonitor(), NULL );
            AppGlobals::getInstance().setWindowSize(V2f{mode->width, mode->height});
        } else {
            glfwWindowHint( GLFW_REFRESH_RATE, 90 );// mode->refreshRate
            float scaleFactor = 1.0f;
            if ( checkBitWiseFlag( flags, InitializeWindowFlags::HalfSize )) scaleFactor = 2.0f;
            if ( checkBitWiseFlag( flags, InitializeWindowFlags::ThreeQuarter )) scaleFactor = 1.5f;
            float xs = mode->width / scaleFactor;
            float ys = mode->height / scaleFactor;
            xs = 1152*3.0f;
            ys = 768*3.0f;
            window = glfwCreateWindow( static_cast<int>(xs), static_cast<int>(ys), "Event Horizon", NULL, NULL );
            AppGlobals::getInstance().setWindowSize(V2f{xs,ys});
        }
        glfwSwapInterval( -1 );

        if ( flags & InitializeWindowFlags::Minimize )
            glfwIconifyWindow( window );

#ifndef OSX
        if ( flags & InitializeWindowFlags::Maximize )
            glfwMaximizeWindow( window );
#endif

        if ( !window ) {
            LOGE( "Could not open window with GLFW3" );
            glfwTerminate();
        }

        glfwMakeContextCurrent( window );
        //	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

        initGraphicsExtensions();

        checkGlError( "GlewInit", __LINE__, __FILE__ );
        glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
        setTextureAligment( 1 );

        // get version info
        const GLubyte *renderer = glGetString( GL_RENDERER ); // get renderer string
        const GLubyte *version = glGetString( GL_VERSION ); // version as a string
        LOGR( "Renderer: %s", renderer );
        LOGR( "OpenGL version supported %s", version );

        // Get info on default framebuffer:
        gatherMainScreenInfo();
//        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        glfwSetScrollCallback(window, GscrollCallback);
        glfwSetMouseButtonCallback(window, GMouseButtonCallback);

        initImGUI();
    }

}