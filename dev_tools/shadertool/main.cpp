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
    auto window = glfwCreateWindow( 100, 100, "Sixth view", NULL, NULL );
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

    ShaderManager sm;

    std::system( "ls * > files.txt" );
    std::ifstream ifs( "files.txt" );

    if ( !ifs.is_open()) throw "Error opening file list";

    std::string line;
    while (std::getline(ifs, line))
    {
        std::istringstream iss(line);
        if ( !line.empty() ) {
            std::string fkey = (getFileNameExt(line) == ".glsl") ? getFileNameNoExt(line) : line;
            std::string urca = FM::readLocalTextFile( line );
            sm.createInjection( fkey, urca );
        }
    }
    std::system( "rm -f files.txt" );

    if ( sm.loadShaders() ) {
        std::string tarname = "shaders.tar";
        std::string matname = "shaders.shd";

        std::string tarcmd = "export COPYFILE_DISABLE=true\n tar -cvf " + tarname + " *.*";

        std::system( tarcmd.c_str() );

        std::string ucompressorcmd = "ucompressoor < " + tarname + " > " +  matname;
        std::system( ucompressorcmd.c_str() );

        FM::copyLocalToRemote( matname, "shaders/" + matname );

        // Remove temp files
        std::string removeTarCmd = "rm -f " + tarname + " " + matname;
        std::system( removeTarCmd.c_str() );
    }

    glfwTerminate();

    return ret;
}
