#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <fstream>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include <core/file_manager.h>
#include <core/http/webclient.h>
#include <graphics/platform_graphics.hpp>
#include <graphics/shader_manager.h>
#include <core/http/basen.hpp>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

const std::string cachedFileName = ".cachedShaderMap.txt";

JSONDATA(FileCheck, filename, lastWriteTime, size, hash)

    FileCheck( const std::string& filename, uint64_t lastWriteTime, uint64_t size, uint64_t hash ) : filename(
            filename ), lastWriteTime( lastWriteTime ), size( size ), hash( hash ) {}

    std::string filename;
    uint64_t lastWriteTime = 0;
    uint64_t size = 0;
    uint64_t hash = 0;
};

bool operator ==(const FileCheck &a, const FileCheck &b) {
    return ( a.filename == b.filename &&
         a.lastWriteTime == b.lastWriteTime &&
         a.size == b.size &&
         a.hash == b.hash );
}

bool operator !=(const FileCheck &a, const FileCheck &b) {
    return !(a == b);
}

using CacheCheckMap = std::unordered_map<std::string, FileCheck>;

struct FileCheckSort
{
    inline bool operator() (const FileCheck& struct1, const FileCheck& struct2)
    {
        return (struct1.filename < struct2.filename);
    }
};

std::vector<FileCheck> getFilesInFolder( const std::string& _folder ) {

    std::vector<FileCheck> ret;

    path p (_folder);

    try {
        if (exists(p) && is_directory(p) ) {
            for (directory_entry& x : directory_iterator(p)) {
                auto filename = x.path().generic_string();

                if ( x.status().type() == file_type::regular_file && filename.find("./.") == std::string::npos ) {
                    ret.emplace_back(FileCheck{getFileName(filename), static_cast<uint64_t >(last_write_time(x)), file_size(x), 0 });
                }
            }
        } else {
            std::cout << p << " is not a directory\n";
        }
    } catch (const filesystem_error& ex) {
        std::cout << ex.what() << '\n';
    }

    std::sort( ret.begin(), ret.end(), FileCheckSort() );

    return ret;
}

void saveCachedFile( const std::vector<FileCheck>& files ) {
    std::vector<std::string> entries;
    for ( const auto& f : files ) {
        entries.emplace_back( f.serialize() );
    }
    FM::writeLocalFile( cachedFileName, entries );
}

CacheCheckMap getCachedFile( const std::vector<FileCheck>& files ) {
    CacheCheckMap ret{};

    auto ls = FM::readLocalTextFileLineByLine(cachedFileName);

    if ( ls.empty() ) {
        for ( const auto& fc : files ) {
            ret.insert( {fc.filename, fc} );
        }
    } else {
        for ( const auto& fc : ls ) {
            if ( fc.empty() ) continue;
            auto fcs = FileCheck{ fc };
            ret.insert( {fcs.filename, fcs} );
        }
    }

    saveCachedFile( files );

    return ret;
}

bool checkFileChanged( const FileCheck& _fc, const CacheCheckMap& _cache ) {

    if ( const auto& entry = _cache.find(_fc.filename); entry != _cache.end() ) {
        return entry->second != _fc;
    }
    return true;
}

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

    auto files = getFilesInFolder(".");

    if ( files.empty() ) return 1; // what no files???

//    auto filesCached = getCachedFile(files);

    std::stringstream shaderHeader;
    ShaderLiveUpdateMap shaderEmit;
    shaderHeader << "static std::unordered_map<std::string, std::string> gShaderInjection{\n";
    for (const auto& fc : files ) {
        auto line = fc.filename;
        std::string fkey = (getFileNameExt(line) == ".glsl") ? getFileNameNoExt(line) : line;
        std::string fileContent = FM::readLocalTextFile( line );
        auto fileContent64 = bn::encode_b64(fileContent);
        shaderHeader << "{ \"" << fkey << "\", \"" << fileContent64 << "\"},\n";
//        if ( checkFileChanged( fc, filesCached) ) {
            sm.inject(fkey, fileContent64);
            shaderEmit.shaders.emplace_back( fkey, fileContent64 );
//        }
    }

    shaderHeader << "};\n";

    bool performCompilerOnly = true;
    if ( shaderEmit.count() > 0 && sm.loadShaders( performCompilerOnly ) ) {
        FM::writeLocalFile("../shaders.hpp", shaderHeader );
        Http::useLocalHost(true);
        Http::login(LoginFields::Daemon());
        Http::post( Url{HttpFilePrefix::broadcast}, shaderEmit.serialize() );
    }

    glfwTerminate();
    Http::shutDown();

    return ret;
}
