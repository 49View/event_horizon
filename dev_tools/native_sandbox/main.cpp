#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include <core/file_manager.h>
#include <graphics/platform_graphics.hpp>
#include <graphics/shader_manager.h>

int main( int argc, [[maybe_unused]] char *argv[] ) {

    int ret = 0;

    if ( !glfwInit() ) {
        LOGE( "Could not start GLFW3" );
    }
    initGraphics();
    glfwWindowHint(  GLFW_VISIBLE, GLFW_TRUE );
    auto window = glfwCreateWindow( 640, 480, "Sandbox", NULL, NULL );
    if (!window) {
        ret = -1;
        std::cout << "Cannot initialize GL to compile shaders";
        glfwTerminate();
        return ret;
    }

    initGraphicsExtensions();

    glfwMakeContextCurrent( window );

    checkGlError( "GlewInit", __LINE__, __FILE__ );

    // get version info
    const GLubyte *renderer = glGetString( GL_RENDERER ); // get renderer string
    const GLubyte *version = glGetString( GL_VERSION ); // version as a string
    LOGR( "Renderer: %s", renderer );
    LOGR( "OpenGL version supported %s", version );

    glfwTerminate();

    return ret;
}
