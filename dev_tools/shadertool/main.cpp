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
#include <core/http/basen.hpp>

JSONDATA(ShaderMap, msg, shaders)
    std::string msg = "shaderchange";
    std::vector<std::pair<std::string, std::string>> shaders;
    std::string flattenJsonSpaced() const {
        std::stringstream ret;

        for ( const auto& p : shaders ) {
            ret << " " << p.first << " " << p.second;
        }

        return ret.str();
    }
    size_t count() const { return shaders.size(); }
};

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

    std::stringstream shaderHeader;
    ShaderMap shaderEmit;
    shaderHeader << "static std::unordered_map<std::string, std::string> gShaderInjection{\n";
    std::string line;
    while (std::getline(ifs, line)) {
        std::istringstream iss(line);
        if ( !line.empty() ) {
            std::string fkey = (getFileNameExt(line) == ".glsl") ? getFileNameNoExt(line) : line;
            std::string fileContent = FM::readLocalTextFile( line );
            auto fileContent64 = bn::encode_b64(fileContent);
            shaderHeader << "{ \"" << fkey << "\", \"" << fileContent64 << "\"},\n";
            if ( sm.injectIfChanged(fkey, fileContent64) ) {
                shaderEmit.shaders.emplace_back( fkey, fileContent64 );
            }
        }
    }

    shaderHeader << "};\n";
    std::system( "rm -f files.txt" );

    if ( shaderEmit.count() > 0 && sm.loadShaders() ) {
        FM::writeLocalFile("../shaders.hpp", shaderHeader );

        Http::useLocalHost(true);
        Http::login(LoginFields::Daemon());

        Socket::emit( "shaderchange" + shaderEmit.flattenJsonSpaced() );
    }

    glfwTerminate();

    return ret;
}
