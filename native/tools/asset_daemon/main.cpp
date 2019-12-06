#include <stb/stb_image_write.h>

#include <core/http/webclient.h>
#include <core/util.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
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

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/gridfs/downloader.hpp>

#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>
#include <core/image_util.h>
#include <core/profiler.h>


using strview = boost::string_view;

void initDeamon() {
    /* Our process ID and Session ID */
    pid_t pid, sid;

    /* Fork off the parent process */
    pid = fork();
    if ( pid < 0 ) {
        exit( EXIT_FAILURE );
    }
    /* If we got a good PID, then
       we can exit the parent process. */
    if ( pid > 0 ) {
        exit( EXIT_SUCCESS );
    }

    /* Change the file mode mask */
    umask( 0 );

    /* Open any logs here */

    /* Create a new SID for the child process */
    sid = setsid();
    if ( sid < 0 ) {
        /* Log the failure */
        exit( EXIT_FAILURE );
    }
    /* Change the current working directory */
    if (( chdir( "/" )) < 0 ) {
        /* Log the failure */
        exit( EXIT_FAILURE );
    }

    /* Close out the standard file descriptors */
    close( STDIN_FILENO );
    close( STDOUT_FILENO );
    close( STDERR_FILENO );
}

void resize_image( mongocxx::gridfs::bucket& bucket, const char* filename, int desiredWidth, int desiredHeight,
                   strview project,
                   strview uname,
                   strview uemail)
{
    int w, h, n;

    unsigned char* input_data = stbi_load(filename, &w, &h, &n, 0);
    if (!input_data) {
        LOGE( "Material image output file not present: %s", filename );
        return;
    }

    auto output_data = (unsigned char*)malloc(desiredWidth * desiredHeight * n);

    stbir_resize_uint8(input_data, w, h, 0, output_data, desiredWidth, desiredHeight, 0, n);

    auto bm = imageUtil::bufferToPngMemory( desiredWidth, desiredHeight, n, output_data );

    try {
        // GridFS
        auto fname = getFileName( filename );
        auto uploadStream = bucket.open_upload_stream( fname );
        uploadStream.write( bm.first.get(), bm.second );
        auto fid = uploadStream.close();
        LOGRS( "Upload ID = " << fid.id().get_oid().value.to_string() );

        // Entity
        Http::post( Url{ HttpFilePrefix::entities + fname + "/" + fname + "/" + std::string(project) + "/image/" + url_encode( std::string(uname) ) + "/" +
                         url_encode( std::string(uemail) ) }, "Urcarcamagnu" );

    } catch (const std::exception& e) {
        LOGRS( e.what() );
    }

    stbi_image_free(input_data);

    free(output_data);
}

void elaborateMatFile( mongocxx::gridfs::bucket& bucket, const std::string& mainFileName,
                       const std::string& layerName,
                       int size, strview project,
                       strview uname,
                       strview uemail ) {
    auto fileRoot = getDaemonRoot();

    std::string fn = getFileNameOnly( mainFileName );
    std::string fext = ".png";

    std::array< std::string, 6 > outputs {
            "basecolor", "metallic", "ambient_occlusion", "roughness", "height", "normal"
    };

    std::string sizeString = std::to_string( log2( size ));
    std::string sbRender = "/opt/Allegorithmic/Substance_Automation_Toolkit/sbsrender render --inputs "
                           + mainFileName +
                           " --set-value '$outputsize@" + sizeString + "," + sizeString +
                           "' --output-bit-depth \"8\" --png-format-compression best_compression "
                           "--output-name {inputName}_{outputNodeName}";
    for ( const auto& output : outputs ) {
        sbRender.append(" --input-graph-output " + output);
    }
    sbRender.append(" --output-path " + fileRoot);
                           ;
//                           + " && cd " + fileRoot +
//                           " && sips -Z 512 " + fn + "*.png "
//                                                     " && zip -X " + fn + ".zip " + fn + "*.png "
//                                                                                         " && rm " + fn + "*.png ";

    std::system( sbRender.c_str());

    for ( const auto& output : outputs ) {
        auto bfilename = mainFileName;
        bfilename.append("_").append(output).append(".png");
        resize_image( bucket, bfilename.c_str(), 512, 512, project, uname, uemail );
    }

//    auto fb = FM::readLocalFile( bfilename );
//    Http::post( Url{ HttpFilePrefix::entities + "multizip/" + fn + "/material/" + url_encode( uname ) + "/" +
//                     url_encode( uemail ) }, FM::readLocalFile( fileRoot + fn + ".zip" ));
}

uint8_p make_uint8_p( uint64_t _size ) {
    return {  std::make_unique<uint8_t[]>(_size), _size };
}

void elaborateMat( mongocxx::gridfs::bucket& bucket, mongocxx::gridfs::bucket& entity_bucket, bsoncxx::types::value _id,  strview _filename, strview project, strview uname, strview uemail ) {

    Profiler p1{"elaborateMat"};
    auto downloadStream = bucket.open_download_stream(_id);

    auto buffer = make_uint8_p(downloadStream.file_length());
    std::string filename = getDaemonRoot() + std::string{_filename};

    auto ret = downloadStream.read( buffer.first.get(), downloadStream.file_length() );
    if ( ret == downloadStream.file_length() ) {
        FM::writeLocalFile( filename, reinterpret_cast<const char *>(buffer.first.get()), buffer.second, true );
        int size = 512;
        std::string layerName{};
        elaborateMatFile( entity_bucket, filename, layerName, size, project, uname, uemail );
    }

//    FM::readRemoteSimpleCallback( _filename, [project, uname, uemail]( const Http::Result& _res ) {
//        auto fileRoot = getDaemonRoot();
//        std::string filename = getFileName( _res.uri );
//        FM::writeLocalFile( fileRoot + filename, reinterpret_cast<const char *>(_res.buffer.get()), _res.length, true );
//        int size = 2048;
//        std::string layerName;
//        if ( auto p = filename.find( MQSettings::Low ); p != std::string::npos ) {
//            size = 128;
//            layerName = MQSettings::Low;
//        }
//        if ( auto p = filename.find( MQSettings::Hi ); p != std::string::npos ) {
//            size = 2048;
//            layerName = MQSettings::Hi;
//        }
//        if ( auto p = filename.find( MQSettings::UltraHi ); p != std::string::npos ) {
//            size = 4096;
//            layerName = MQSettings::UltraHi;
//        }
//        elaborateMatFile( std::string(fileRoot + filename), layerName, size, project, uname, uemail );
//    } );
}

void elaborateGeom( const std::string& _filename, const std::string& project, const std::string& uname,
                    const std::string& uemail ) {
    FM::readRemoteSimpleCallback( _filename,
                                  [project, uname, uemail]( const Http::Result& _res ) {
                                      std::string dRoot = cacheFolder(); // "/" on linux
                                      std::string filename = getFileName( _res.uri );
                                      std::string mainFileName = dRoot + filename;
                                      FM::writeLocalFile( mainFileName,
                                                          reinterpret_cast<const char *>(_res.buffer.get()),
                                                          _res.length, true );

                                      std::string cmd = "FBX2glTF -b --pbr-metallic-roughness -o " + dRoot +
                                                        getFileNameOnly( filename ) + " " +
                                                        mainFileName;

                                      std::system( cmd.c_str());

                                      std::string filenameglb = getFileNameOnly( filename ) + ".glb";

                                      std::string finalPath = dRoot + filenameglb;

                                      std::string reqParams =
                                              filenameglb + "/" + project + "/" + ResourceGroup::Geom + "/" +
                                              url_encode( uname ) +
                                              "/" + url_encode( uemail );
                                      Http::post( Url{ HttpFilePrefix::entities + reqParams },
                                                  FM::readLocalFile( finalPath ));
                                  } );
}

//int main( [[maybe_unused]] int argc, [[maybe_unused]] char **argv ) {
//
//    LOGRS( "Daemon version 2.5.0" );
//
//    Http::initDaemon();
//
////    initDeamon();
//
//    if ( argc > 1 ) {
//        std::string filename = std::string( argv[1] );
//        elaborateMatFile( filename, "", 2048, "eh_sandbox", "Daemon", "Daemon" );
//        return 0;
//    } else {
//        bool bAwaking = false;
//        std::string reqProject;
//        std::string reqFilename;
//        std::string reqUsername;
//        std::string reqUserEmail;
//
//        Socket::on( "cloudStorageFileToElaborate", [&]( const std::string& msg, SocketCallbackDataType&& _data ) {
//            reqFilename = _data["data"]["name"].GetString();
//            reqProject = _data["data"]["project"].GetString();
//            reqUsername = _data["data"]["uname"].GetString();
//            reqUserEmail = _data["data"]["uemail"].GetString();
//            bAwaking = true;
//            LOGRS( "Daemon will awake from cloudStorageFileToElaborate" );
//        } );
//
//        daemonLoop( 1, bAwaking, [&]() {
//            bAwaking = false;
//            std::string filename = url_decode( reqFilename );
//            Http::login( LoginFields::Daemon( reqProject ), [filename, reqProject, reqUsername, reqUserEmail]() {
//                if ( filename.find( DaemonPaths::upload( ResourceGroup::Material )) != std::string::npos ) {
//                    elaborateMat( filename, reqProject, reqUsername, reqUserEmail );
//                } else if ( filename.find( DaemonPaths::upload( ResourceGroup::Geom )) != std::string::npos ) {
//                    elaborateGeom( filename, reqProject, reqUsername, reqUserEmail );
//                }
//            } );
//        } );
//    }
//
//    return 0;
//}


int main( [[maybe_unused]] int argc, [[maybe_unused]] char **argv ) {

    LOGRS( "Daemon version 3.0.1" );

    Http::useClientCertificate(true);

//    initDeamon();

    mongocxx::instance inst{};
//    mongocxx::client conn{mongocxx::uri{"mongodb://localhost:27011,localhost:27012,localhost:27013/event_horizon?replicaSet=rs0"}};
    mongocxx::client conn{mongocxx::uri{"mongodb://localhost:27017/event_horizon?replicaSet=rs0"}};

    auto collection = conn["event_horizon"]["fs_source_assets.files"];
    auto stream = collection.watch();

    mongocxx::options::gridfs::bucket b1;
    b1.bucket_name("fs_source_assets");
    mongocxx::gridfs::bucket source_assets_bucket = conn["event_horizon"].gridfs_bucket(b1);

    mongocxx::options::gridfs::bucket b2;
    b2.bucket_name("fs_entity_assets");
    mongocxx::gridfs::bucket entity_bucket = conn["event_horizon"].gridfs_bucket(b2);

//    static bool bFisrtTimeEver = true;
    static bool bExit = false;
    while (!bExit) {
        for ( auto change : stream ) {
            auto fulldoc = change["fullDocument"]["metadata"];
            auto filename = change["fullDocument"]["filename"].get_utf8().value;
            auto group = fulldoc["group"].get_utf8().value;
            auto username  = fulldoc["user"]["name"].get_utf8().value;
            auto useremail = fulldoc["user"]["email"].get_utf8().value;
            auto project = fulldoc["user"]["project"].get_utf8().value;

            std::cout << "Group: " << group << std::endl;
            if ( group == ResourceGroup::Material ) {
                elaborateMat( source_assets_bucket, entity_bucket, change["fullDocument"]["_id"].get_value(), filename, project, username, useremail );
            }
//            std::cout << "Document metadata: " << bsoncxx::to_json(fulldoc) << std::endl;
            std::cout << "Something has changed " << bsoncxx::to_json(change) << std::endl;
        }
    };

    return 0;
}
