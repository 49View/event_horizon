
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
#include <stb/stb_image_write.h>

#include <core/http/webclient.h>
#include <core/util.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <cmath>
#include <core/file_manager.h>
#include <core/runloop_core.h>
#include <core/tar_util.h>
#include <core/zlib_util.h>
#include <core/names.hpp>
#include <core/resources/publisher.hpp>
#include <core/resources/material.h>
#include <core/resources/resource_utils.hpp>
#include <core/descriptors/uniform_names.h>
#include <core/resources/resource_builder.hpp>
#include <core/resources/resource_pipe.hpp>
#include <core/resources/publisher.hpp>

void initDeamon() {
    /* Our process ID and Session ID */
    pid_t pid, sid;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then
       we can exit the parent process. */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    /* Change the file mode mask */
    umask(0);

    /* Open any logs here */

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }
    /* Change the current working directory */
    if ((chdir("/")) < 0) {
        /* Log the failure */
        exit(EXIT_FAILURE);
    }

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

void elaborateMatFile( const std::string& mainFileName, const std::string& layerName = "", int size = 64 ) {
    auto fileRoot = getDaemonRoot();

    std::string fn = getFileNameOnly( mainFileName );
    std::string fext = ".png";

    std::string sizeString = std::to_string(log2(size));
    std::string sbRender = "/opt/Allegorithmic/Substance_Automation_Toolkit/sbsrender render --inputs "
                           + mainFileName +
                           " --set-value '$outputsize@" + sizeString + "," + sizeString + "' "
                                                                                          "--output-name {inputName}_{outputNodeName} "
                                                                                          "--input-graph-output basecolor "
                                                                                          "--input-graph-output metallic --input-graph-output ambient_occlusion "
                                                                                          "--input-graph-output roughness --input-graph-output height --input-graph-output normal "
                                                                                          "--output-path " + fileRoot;

    std::system(sbRender.c_str());

    ResourcePipe rpipe;

    rpipe.pipeFile<RawImage>( fileRoot + fn + "_" + MPBRTextures::basecolorString        + fext );
    rpipe.pipeFile<RawImage>( fileRoot + fn + "_" + MPBRTextures::heightString           + fext );
    rpipe.pipeFile<RawImage>( fileRoot + fn + "_" + MPBRTextures::metallicString         + fext );
    rpipe.pipeFile<RawImage>( fileRoot + fn + "_" + MPBRTextures::roughnessString        + fext );
    rpipe.pipeFile<RawImage>( fileRoot + fn + "_" + MPBRTextures::normalString           + fext );
    rpipe.pipeFile<RawImage>( fileRoot + fn + "_" + MPBRTextures::ambientOcclusionString + fext );
    rpipe.pipeFile<RawImage>( fileRoot + fn + "_" + MPBRTextures::opacityString          + fext );
    rpipe.pipeFile<RawImage>( fileRoot + fn + "_" + MPBRTextures::translucencyString     + fext );

    auto values = std::make_shared<HeterogeneousMap>(S::SH);
    for ( const auto& entry : rpipe.getCatalog() ) {
        values->assign( MPBRTextures::mapToTextureUniform( entry.filename ), entry.hash );
    }

    rpipe.pipe<Material>( fn + layerName, Material{values}.serialize() );

    rpipe.publish();

//    FM::writeRemoteFile( DaemonPaths::store( ResourceGroup::Material, tarname ),
//                         zlibUtil::deflateMemory(tagStream.str() ) );
}

void elaborateMat( const std::string& _filename ) {
    FM::readRemoteSimpleCallback( _filename, [](const Http::Result& _res) {
        auto fileRoot = getDaemonRoot();
        std::string filename =  getFileName(_res.uri);
        FM::writeLocalFile( fileRoot + filename, reinterpret_cast<const char*>(_res.buffer.get()), _res.length, true );
        int size = 512;
        std::string layerName;
        if ( auto p = filename.find(MQSettings::Low); p != std::string::npos ) {
          size = 128;
          layerName = MQSettings::Low;
        }
        if ( auto p = filename.find(MQSettings::Hi); p != std::string::npos ) {
          size = 2048;
          layerName = MQSettings::Hi;
        }
        if ( auto p = filename.find(MQSettings::UltraHi); p != std::string::npos ) {
          size = 4096;
          layerName = MQSettings::UltraHi;
        }
        elaborateMatFile( fileRoot + filename, layerName, size );
    } );
}

void elaborateGeom( const std::string& _filename ) {
    FM::readRemoteSimpleCallback( _filename,
        [](const Http::Result& _res) {
            std::string dRoot = cacheFolder(); // "/" on linux
            std::string filename =  getFileName(_res.uri);
            std::string mainFileName = dRoot + filename;
            FM::writeLocalFile( mainFileName, reinterpret_cast<const char*>(_res.buffer.get()), _res.length, true );

            std::string cmd = "FBX2glTF -b --pbr-metallic-roughness -o " + dRoot + getFileNameOnly(filename) + " " +
                    mainFileName;

            std::system( cmd.c_str() );

            std::string filenameglb = getFileNameOnly(filename) + ".glb";

            std::string finalPath = dRoot + filenameglb;

            FM::writeRemoteFile( DaemonPaths::store( ResourceGroup::Geom, filenameglb ),
                            zlibUtil::deflateMemory(FM::readLocalFile( finalPath )) );
    } );
}

int main( [[maybe_unused]] int argc, [[maybe_unused]] char **argv ) {

    Http::initDaemon();

//    initDeamon();

    if ( argc > 1 ) {
        std::string filename = std::string(argv[1]);
        elaborateMatFile( filename );
        return 0;
    } else {
        bool bAwaking = false;
        std::string reqProject;
        std::string reqFilename;

        Socket::on( "cloudStorageFileUpdate", [&]( SocketCallbackDataTypeConstRef _data ) {
            reqFilename = _data["name"].GetString();
            reqProject = _data["project"].GetString();
            bAwaking = true;
        });

        daemonLoop(1, bAwaking, [&]() {
            bAwaking = false;
            std::string filename = url_decode( reqFilename );
            Http::login( LoginFields::Daemon(reqProject), [filename]() {
                if ( filename.find(DaemonPaths::upload(ResourceGroup::Material)) != std::string::npos ){
                    elaborateMat( filename );
                } else if ( filename.find(DaemonPaths::upload(ResourceGroup::Geom)) != std::string::npos ){
                    elaborateGeom( filename );
                }
            });
        });
    }

    return 0;
}

