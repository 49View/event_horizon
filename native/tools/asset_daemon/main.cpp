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

#include <database/nosql/mongo/mongo.hpp>

#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>
#include <core/image_util.h>
#include <core/profiler.h>
#include <ostream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

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

std::optional<MongoFileUpload> saveImageToGridFS( MongoBucket bucket, const char *filename,
                                                  int desiredWidth, int desiredHeight,
                                                  strview project,
                                                  strview uname,
                                                  strview uemail,
                                                  std::vector<std::string>& thumbs ) {
    try {
        int w = 0, h = 0, n = 0;
        int thumbSize = 128;

        unsigned char *input_data = stbi_load( filename, &w, &h, &n, 0 );

        auto output_data = make_uint8_p( desiredWidth * desiredHeight * n );
        auto output_data_thumb = make_uint8_p( thumbSize * thumbSize * n );
        stbir_resize_uint8( input_data, w, h, 0, output_data.first.get(), desiredWidth, desiredHeight, 0, n );
        stbir_resize_uint8( output_data.first.get(), desiredWidth, desiredHeight, 0, output_data_thumb.first.get(),
                            thumbSize, thumbSize, 0, n );

        auto bm = imageUtil::bufferToPngMemory( desiredWidth, desiredHeight, n, output_data.first.get());
        auto bm64 = imageUtil::bufferToPng64( thumbSize, thumbSize, n, output_data_thumb.first.get());
        thumbs.emplace_back( bm64 );

        return Mongo::fileUpload( bucket, getFileName( filename ), std::move( bm ),
                                  Mongo::FSMetadata( ResourceGroup::Image, project, uname, uemail,
                                                     HttpContentType::octetStream,
                                                     MD5( bm.first.get(), bm.second ).hexdigest(), bm64 ));
    } catch ( const std::exception& e ) {
        LOGRS( e.what());
        return std::nullopt;
    }
}

void elaborateMatSBSAR( MongoBucket entity_bucket,
                        const std::string& mainFileName,
                        const std::string& layerName,
                        int size, strview project,
                        strview uname,
                        strview uemail ) {

    if ( mainFileName.empty()) return;
    auto fileRoot = getDaemonRoot();

    std::string fn = getFileNameOnly( mainFileName );
    std::string fext = ".png";
    int nominalSize = 512;

    std::string sizeString = std::to_string( log2( size ));
    std::string sbRender = "/opt/Allegorithmic/Substance_Automation_Toolkit/sbsrender render --inputs "
                           + mainFileName +
                           " --set-value '$outputsize@" + sizeString + "," + sizeString +
                           "' --output-bit-depth \"8\" --png-format-compression best_compression "
                           "--output-name {inputName}_{outputNodeName}";
    for ( const auto& output : MPBRTextures::SBSARTextureOutputs()) {
        sbRender.append( " --input-graph-output " + output );
    }
    sbRender.append( " --output-path " + fileRoot );;

    std::system( sbRender.c_str());

    // Gather texture outputs
    ResourceDependencyDict deps;
    std::vector<std::string> thumbs;
    Material pbrmat{ S::SH, fn };
    for ( const auto& output : MPBRTextures::SBSARTextureOutputs()) {
        auto bfilename = fileRoot + fn;
        bfilename.append( "_" ).append( output ).append( ".png" );
        if ( FM::fileExist( bfilename )) {
            auto to = saveImageToGridFS( entity_bucket, bfilename.c_str(), nominalSize, nominalSize, project, uname,
                                         uemail, thumbs );
            if ( to ) {
                auto tid = ( *to ).getStringId();
                deps[ResourceGroup::Image].emplace_back( tid );
                pbrmat.Values()->assign( MPBRTextures::mapToTextureUniform( output ), tid );
            }
        }
    }

    // Create PBR material
    auto matSer = pbrmat.serialize();
    Mongo::fileUpload( entity_bucket, fn, matSer,
                       Mongo::FSMetadata( ResourceGroup::Material, project, uname, uemail,
                                          HttpContentType::json, Hashable<>::hashOf( matSer ), thumbs[0], deps ));

    // Clean up
    std::string cleanup = "cd " + fileRoot + " && rm " + fn + "*";
    std::system( cleanup.c_str());
}

void elaborateGeomFBX( MongoBucket entity_bucket, const std::string& _filename, strview project,
                       strview uname,
                       strview uemail ) {
    auto dRoot = getDaemonRoot();
    auto fn = getFileNameOnly( _filename );

    std::string cmd = "FBX2glTF -b --pbr-metallic-roughness -o " + dRoot + fn + " " + _filename;
    std::system( cmd.c_str());

    std::string filenameglb = fn + ".glb";

    auto fileData = FM::readLocalFile( dRoot + filenameglb );
    auto fileHash = Hashable<>::hashOf( fileData );
    Mongo::fileUpload( entity_bucket, filenameglb, std::move( fileData ),
                       Mongo::FSMetadata( ResourceGroup::Geom, project, uname, uemail,
                                          HttpContentType::json, fileHash, "", ResourceDependencyDict{} ));
}

void parseElaborateStream( mongocxx::change_stream& stream, MongoBucket sourceAssetBucket, MongoBucket entityBucket ) {
    for ( auto change : stream ) {
        StreamChangeMetadata meta{ change };
        Profiler p1{ "elaborate time:" };
        auto fileDownloaded = Mongo::fileDownload( sourceAssetBucket,
                                                   meta.id,
                                                   getDaemonRoot() + std::string{ meta.filename } );
        if ( meta.group == ResourceGroup::Material ) {
            if ( getFileNameExt( std::string( meta.filename )) == ".sbsar" ) {
                elaborateMatSBSAR( entityBucket, fileDownloaded, {}, 512, meta.project,
                                   meta.username,
                                   meta.useremail );
            }
        } else if ( meta.group == ResourceGroup::Geom ) {
            if ( getFileNameExt( std::string( meta.filename )) == ".fbx" ) {
                elaborateGeomFBX( entityBucket, fileDownloaded, meta.project, meta.username, meta.useremail );
            }
        }
    }
}

void parseAssetStream( Mongo& mdb, mongocxx::change_stream& stream ) {
    for ( auto change : stream ) {
        StreamChangeMetadata meta{ change };
        mdb.insertEntityFromAsset( meta );
    }
}

int main( [[maybe_unused]] int argc, [[maybe_unused]] char **argv ) {

    LOGRS( "Daemon version 3.0.2" );

    if ( !Http::useClientCertificate( true,
                                      "EH_DEAMON_CERT_KEY_PATH", "EH_DEAMON_CERT_CRT_PATH" )) {
        LOGRS( "Daemon certificate and key environment variables needs to be present as"
               "\n$EH_DEAMON_CERT_KEY_PATH\n$EH_DEAMON_CERT_CRT_PATH" );
        return 1;
    }

//    initDeamon();

    Mongo mdb{ "event_horizon" };
    auto sourceAssetBucket = mdb.useBucket( "fs_assets_to_elaborate" );
    auto entityBucket = mdb.useBucket( "fs_entity_assets" );

//    sourceAssetBucket.deleteAll();
//    entityBucket.deleteAll();
//    mdb["entities"]().delete_many({});

    auto streamToElaborate = mdb["fs_assets_to_elaborate.files"].watch();
    auto streamAsset = mdb["fs_entity_assets.files"].watch();

    while ( true ) {
        parseElaborateStream( streamToElaborate, sourceAssetBucket, entityBucket );
        parseAssetStream( mdb, streamAsset );
    }

    return 0;
}

#pragma clang diagnostic pop