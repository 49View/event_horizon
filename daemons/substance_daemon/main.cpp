
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

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
#include <core/descriptors/material.h>

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

void elaborateMat( const std::string& _filename ) {
    FM::readRemoteSimpleCallback( _filename,
    [](const Http::Result& _res) {
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
        std::string mainFileName = fileRoot + filename;
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

        std::stringstream tagStream;
        tarUtil::TarWrite tar{ tagStream };

        std::string tarname = fn + layerName + ".tar";
        std::string fileb = fn + "_" + MPBRTextures::basecolorString + fext;
        std::string fileh = fn + "_" + MPBRTextures::heightString + fext;
        std::string filem = fn + "_" + MPBRTextures::metallicString + fext;
        std::string filer = fn + "_" + MPBRTextures::roughnessString + fext;
        std::string filen = fn + "_" + MPBRTextures::normalString + fext;
        std::string filea = fn + "_" + MPBRTextures::ambientOcclusionString + fext;

        tar.putFile( ( fileRoot + fileb).c_str(), fileb.c_str() );
        tar.putFile( ( fileRoot + fileh).c_str(), fileh.c_str() );
        tar.putFile( ( fileRoot + filem).c_str(), filem.c_str() );
        tar.putFile( ( fileRoot + filer).c_str(), filer.c_str() );
        tar.putFile( ( fileRoot + filen).c_str(), filen.c_str() );
        tar.putFile( ( fileRoot + filea).c_str(), filea.c_str() );
        tar.finish();

        FM::writeRemoteFile( DaemonPaths::store( EntityGroup::Material, tarname ),
                        zlibUtil::deflateMemory(tagStream.str() ) );
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

            FM::writeRemoteFile( DaemonPaths::store( EntityGroup::Geom, filenameglb ),
                            zlibUtil::deflateMemory(FM::readLocalFile( finalPath )) );
    } );
}

int main( [[maybe_unused]] int argc, [[maybe_unused]] char **argv ) {

    Http::initDaemon();

//    initDeamon();
    Socket::on( "cloudStorageFileUpdate", []( SocketCallbackDataType data ) {
        std::string filename = url_decode( data["name"].GetString() );
        if ( filename.find(DaemonPaths::upload(EntityGroup::Material)) != std::string::npos ){
            elaborateMat( filename );
        } else if ( filename.find(DaemonPaths::upload(EntityGroup::Geom)) != std::string::npos ){
            elaborateGeom( filename );
        }
    } );

//    std::string filename = "paint_glossy.sbsar";
//    elaborate( filename );

    daemonLoop(1);

    return 0;
}

