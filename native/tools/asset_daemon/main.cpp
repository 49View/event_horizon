#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <cmath>

#define TINYGLTF_IMPLEMENTATION
#define TINY_DNG_LOADER_IMPLEMENTATION

#include <iostream>
#include <tinygltf/include/tiny_dng_loader.h>
#include <tinygltf/include/tiny_gltf.h>
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.

#include <core/http/webclient.h>
#include <core/util.h>
#include <core/file_manager.h>
#include <core/runloop_core.h>
#include <core/tar_util.h>
#include <core/zlib_util.h>
#include <core/names.hpp>
#include <core/descriptors/archives.h>
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

//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wmissing-noreturn"

// mongod --port 27017 --replSet rs0
// mongod --port 27017 --replSet rs0 --dbpath ~/Documents/mongodata
// sudo /usr/local/etc/nginx
// sudo brew services restart nginx
// /usr/loca/var/log/nginx/ tail -f aacess.log

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

struct DaemonFileStruct {
    MongoBucket bucket;
    std::string filename;
    std::string group;
    strview project;
    strview uname;
    strview uemail;
    std::string thumb64{};
    std::vector<ArchiveDirectoryEntityElement> candidates{};
};

class DaemonException : public std::exception {
public:
    DaemonException( const std::string& msg ) : msg( msg ) {}

    virtual const char *what() const throw() {
        return msg.c_str();
    }

private:
    std::string msg{};
};

static std::vector<std::string> getExtForGroup( const std::string& _group ) {
    std::unordered_map<std::string, std::vector<std::string>> extmap;

    extmap[ResourceGroup::Geom] = { ".fbx", ".glb", ".gltf", ".obj" };
    extmap[ResourceGroup::Material] = { ".sbsar", ".png", ".jpg" };

    if ( auto it = extmap.find( _group ); it != extmap.end()) {
        return it->second;
    }
    return {};
}

void daemonExceptionLog( Mongo& mdb, const std::exception& e ) {
    LOGRS( e.what());
    mdb.insertDaemonCrashLog(e.what());
    exit(1);
}

void daemonWarningLog( const std::string& e ) {
    LOGRS( e );
}

std::string getContentTypeFor( const DaemonFileStruct& dfs ) {
    if ( dfs.group == ResourceGroup::Material ) {
        return HttpContentType::json;
    } else {
        return HttpContentType::octetStream;
    }
}

std::string createThumbnailCommandFor( const DaemonFileStruct& dfs, std::string& thumbnailFileName ) {
    auto dRoot = getDaemonRoot();
    thumbnailFileName = dRoot + dfs.filename + ".png";

    if ( dfs.group == ResourceGroup::Image ) {
        return "convert " + dRoot + dfs.filename + " -resize 128x128/! " + thumbnailFileName;
    } else if ( dfs.group == ResourceGroup::Profile ) {
        return "convert " + dRoot + dfs.filename + " -trim +repage -resize 256x256/! " + thumbnailFileName;
    } else if ( dfs.group == ResourceGroup::Font ) {
        return "convert -font " + dRoot + dfs.filename + " -pointsize 50 label:\"aA\" " + thumbnailFileName;
    }

    daemonWarningLog( "Asset of type " + dfs.group + " went into the wrong pipeline." );

    return {};
}

auto generateThumbnail64( const DaemonFileStruct& dfs ) {
    std::string thumbnailFileName{};
    std::string cmdThumbnail = createThumbnailCommandFor( dfs, thumbnailFileName );
    auto ret = std::system( cmdThumbnail.c_str());
    if ( ret != 0 ) throw std::string("Failed to generate thumbnail for " + thumbnailFileName);
    return bn::encode_b64String( FM::readLocalFileC( thumbnailFileName ));
}

std::optional<MongoFileUpload> upload( DaemonFileStruct dfs, const ResourceEntityHelper& res ) {
    return Mongo::fileUpload( dfs.bucket, dfs.filename, res.sc,
                              Mongo::FSMetadata( dfs.group, dfs.project, dfs.uname, dfs.uemail,
                                                 getContentTypeFor( dfs ), Hashable<>::hashOf( res.sc ), res.thumb,
                                                 res.deps ));
}

std::optional<MongoFileUpload> elaboratePassThrough( DaemonFileStruct dfs ) {
    ResourceEntityHelper res{ FM::readLocalFileC( getDaemonRoot() + dfs.filename ), {}, generateThumbnail64( dfs ) };
    return upload( dfs, res );
}

std::optional<MongoFileUpload> elaborateImage(
        int desiredWidth, int desiredHeight,
        DaemonFileStruct dfs,
        std::string& thumb ) {
    try {
        int w = 0, h = 0, n = 0;
        int thumbSize = 128;

        unsigned char *input_data = stbi_load( dfs.filename.c_str(), &w, &h, &n, 0 );

        auto output_data = make_uint8_p( desiredWidth * desiredHeight * n );
        auto output_data_thumb = make_uint8_p( thumbSize * thumbSize * n );
        stbir_resize_uint8( input_data, w, h, 0, output_data.first.get(), desiredWidth, desiredHeight, 0, n );
        stbir_resize_uint8( output_data.first.get(), desiredWidth, desiredHeight, 0, output_data_thumb.first.get(),
                            thumbSize, thumbSize, 0, n );

        auto bm = imageUtil::bufferToMemoryCompressed( desiredWidth, desiredHeight, n, output_data.first.get(),
                                                       imageUtil::extToMime( getFileNameExtToLower( dfs.filename )));
        auto bm64 = imageUtil::bufferToPng64( thumbSize, thumbSize, n, output_data_thumb.first.get());
        if ( MPBRTextures::isBaseColorTexture( dfs.filename )) {
            thumb = bm64;
        }
        return Mongo::fileUpload( dfs.bucket, getFileName( dfs.filename ), std::move( bm ),
                                  Mongo::FSMetadata( ResourceGroup::Image, dfs.project, dfs.uname, dfs.uemail,
                                                     HttpContentType::octetStream,
                                                     MD5( bm.first.get(), bm.second ).hexdigest(), bm64 ));
    } catch ( const std::exception& e ) {
        LOGRS( e.what());
        return std::nullopt;
    }
}

ArchiveDirectory generateActiveDirectoryFromSBSARTempFiles( const std::string& fn ) {
    ArchiveDirectory ret{ fn };
    auto fileRoot = getDaemonRoot();

    for ( const auto& output : MPBRTextures::SBSARTextureOutputs()) {
        std::string outFilename{};
        outFilename.append( fn ).append( "_" ).append( output ).append( ".png" );
        if ( FM::fileExist( fileRoot + outFilename )) {
            ret.insert( { outFilename, 0, output } );
        }
    }
    return ret;
}

ArchiveDirectory mapActiveDirectoryFilesToPBR( DaemonFileStruct dfs ) {
    ArchiveDirectory ad{ dfs.filename };

    for ( auto&& elem : dfs.candidates ) {
        elem.metaString = MPBRTextures::findTextureInString( elem.name );
        ad.insert( std::move( elem ));
    }
    return ad;
}

ResourceEntityHelper elaborateInternalMaterial(
        const ArchiveDirectory& ad,
        int nominalSize,
        DaemonFileStruct dfs ) {
    ResourceEntityHelper mat{};
    auto fileRoot = getDaemonRoot();

    Material pbrmaterial{ S::SH, ad.Name() };
    dfs.group = ResourceGroup::Image;
    for ( const auto& output : ad ) {
        dfs.filename = output.second.name;
        if ( nominalSize > 0 ) {
            std::string resizeString = std::to_string(nominalSize) + "x" + std::to_string(nominalSize);
            auto cmdThumbnail = "convert " +  getDaemonRoot() + dfs.filename + " -resize " + resizeString + " " +  getDaemonRoot() + dfs.filename;
            auto ret = std::system( cmdThumbnail.c_str());
            if ( ret != 0 ) throw std::string("Failed to scale down image " + getFileName(dfs.filename));
        }

        ResourceEntityHelper res{ FM::readLocalFileC( getDaemonRoot() + dfs.filename ), {}, generateThumbnail64( dfs ) };
        if ( MPBRTextures::isBaseColorTexture( dfs.filename ) ) {
            mat.thumb = res.thumb;
        }
        auto to = upload( dfs, res );
//        auto to = elaborateImage( nominalSize, nominalSize, dfs, mat.thumb );
        if ( to && !output.second.metaString.empty()) {
            auto tid = ( *to ).getStringId();
            mat.deps[ResourceGroup::Image].emplace_back( tid );
            pbrmaterial.Values()->assign( MPBRTextures::mapToTextureUniform( output.second.metaString ), tid );
            if ( MPBRTextures::isMetallicTexture( output.second.metaString )) {
                pbrmaterial.setMetallicValue( 1.0f );
            }
            if ( MPBRTextures::isRoughnessTexture( output.second.metaString )) {
                pbrmaterial.setRoughnessValue( 1.0f );
            }
        }
    }

    // Create PBR material
    mat.sc = pbrmaterial.serialize();
    return mat;
}

ArchiveDirectory elaborateMatSBSAR(
        int size,
        DaemonFileStruct dfs ) {

    auto fileRoot = getDaemonRoot();

    std::string fn = getFileNameOnly( dfs.filename );
    std::string fext = ".png";

    std::string sizeString = std::to_string( log2( size ));
    std::string sbRender = "/opt/Allegorithmic/Substance_Automation_Toolkit/sbsrender render --inputs "
                           + fileRoot + dfs.filename +
                           " --set-value '$outputsize@" + sizeString + "," + sizeString +
                           "' --output-bit-depth \"8\" --png-format-compression best_compression "
                           "--output-name {inputName}_{outputNodeName}";
    for ( const auto& output : MPBRTextures::SBSARTextureOutputs()) {
        sbRender.append( " --input-graph-output " + output );
    }
    sbRender.append( " --output-path " + fileRoot );;

    auto ret = std::system( sbRender.c_str());
    if ( ret != 0 ) throw std::string("Failed to run: " + sbRender);

    return generateActiveDirectoryFromSBSARTempFiles( fn );
}

int resaveGLB( const std::string& filename ) {
    using namespace tinygltf;

    Model model;
    TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ret = loader.LoadBinaryFromFile( &model, &err, &warn, filename.c_str());

    if ( !warn.empty() || !err.empty() || !ret ) {
        if ( !warn.empty()) {
            printf( "Warn: %s\n", warn.c_str());
        }
        if ( !err.empty()) {
            printf( "Err: %s\n", err.c_str());
        }
        if ( !ret ) {
            printf( "Failed to parse glTF\n" );
            return -1;
        }
    }

    for ( auto& it : model.accessors ) {
        if ( string_ends_with( it.name, "_positions" ) && it.componentType == 5126 && it.type == 3 ) {
            auto buffView = model.bufferViews[it.bufferView];
            if ( buffView.target == 34962 ) {
                auto *buffer = &model.buffers[buffView.buffer];
                auto *buffArray = buffer->data.data() + buffView.byteOffset + it.byteOffset;
                V3f *buffV3f = reinterpret_cast<V3f *>(buffArray);
                for ( size_t t = 0; t < it.count; t++ ) {
                    buffV3f[t] *= 0.01f;
                }
            }
            for ( auto& v : it.maxValues ) v *= 0.01f;
            for ( auto& v : it.minValues ) v *= 0.01f;
        }
    }

    loader.WriteGltfSceneToFile( &model, filename, true, true, true, true );

    return 0;
}

void elaborateGeomFBX( DaemonFileStruct dfs ) {
    auto dRoot = getDaemonRoot();
    auto fn = getFileNameOnly( dfs.filename );
    std::string filenameglb = fn + ".glb";

    std::string cmd =
            "cd " + dRoot + " && FBX2glTF -b --pbr-metallic-roughness -o '" + getFileNameOnly( filenameglb ) +
            "' '" +
            getFileName( dfs.filename ) + "'";
    auto ret = std::system( cmd.c_str());
    if ( ret != 0 ) throw std::string{ "FBX elaboration return code: " + std::to_string( ret ) };

    auto fileData = FM::readLocalFile( dRoot + filenameglb );
//        resaveGLB(dRoot + filenameglb);
    auto fileHash = Hashable<>::hashOf( fileData );
    Mongo::fileUpload( dfs.bucket, filenameglb, std::move( fileData ),
                       Mongo::FSMetadata( dfs.group, dfs.project, dfs.uname, dfs.uemail,
                                          HttpContentType::octetStream, fileHash, dfs.thumb64, ResourceDependencyDict{} ));
}

void elaborateGeomObj( DaemonFileStruct dfs ) {
    auto dRoot = getDaemonRoot();
    auto fn = getFileNameOnly( dfs.filename );
    std::string filenameglb = fn + ".glb";

    std::string cmd =
            "cd " + dRoot + " && obj2glTF -i '" + getFileName( dfs.filename ) + "'  -o '" + filenameglb + "'";
    auto ret = std::system( cmd.c_str());
    if ( ret != 0 ) throw std::string{ "OBJ elaboration return code: " + std::to_string( ret ) };

    auto fileData = FM::readLocalFile( dRoot + filenameglb );
    resaveGLB( dRoot + filenameglb );
    fileData = FM::readLocalFile( dRoot + filenameglb );
    auto fileHash = Hashable<>::hashOf( fileData );
    Mongo::fileUpload( dfs.bucket, filenameglb, std::move( fileData ),
                       Mongo::FSMetadata( dfs.group, dfs.project, dfs.uname, dfs.uemail,
                                          HttpContentType::octetStream, fileHash, "", ResourceDependencyDict{} ));
}

void elaborateGeomGLB( DaemonFileStruct dfs ) {
    auto dRoot = getDaemonRoot();
    auto fileData = FM::readLocalFile( dRoot + dfs.filename );
    auto fileHash = Hashable<>::hashOf( fileData );
    Mongo::fileUpload( dfs.bucket, dfs.filename, std::move( fileData ),
                       Mongo::FSMetadata( dfs.group, dfs.project, dfs.uname, dfs.uemail,
                                          HttpContentType::octetStream, fileHash, "", ResourceDependencyDict{} ));
}

void elaborateGeom( DaemonFileStruct dfs ) {
    if ( getFileNameExtToLower( dfs.filename ) == ".fbx" ) {
        elaborateGeomFBX( dfs );
    }
    if ( getFileNameExtToLower( dfs.filename ) == ".obj" ) {
        elaborateGeomObj( dfs );
    }
    if ( getFileNameExtToLower( dfs.filename ) == ".glb" || getFileNameExtToLower( dfs.filename ) == ".gltf" ) {
        elaborateGeomGLB( dfs );
    }
}

void elaborateMaterial( DaemonFileStruct dfs ) {
    ArchiveDirectory ad{ dfs.filename };
    int sensibleRescaleSize = 512;

    if ( getFileNameExtToLower( std::string( dfs.filename )) == ".zip" ) {
        ad = mapActiveDirectoryFilesToPBR( dfs );
        // Right so if it comes from a zip file it's the provider decision to have their exact size
        // so in this case we'll set sensibleRescaleSize to -1 to tell the elaboration not to touch it
        sensibleRescaleSize = -1;
    }
    if ( getFileNameExtToLower( std::string( dfs.filename )) == ".sbsar" ) {
        int nominalSize = 1024;
        sensibleRescaleSize = -1;
        ad = elaborateMatSBSAR( nominalSize, dfs );
    }

    // Gather texture outputs
    ResourceEntityHelper mat = elaborateInternalMaterial( ad, sensibleRescaleSize, dfs );
    Mongo::fileUpload( dfs.bucket, dfs.filename, mat.sc,
                       Mongo::FSMetadata( dfs.group, dfs.project, dfs.uname, dfs.uemail,
                                          HttpContentType::json, Hashable<>::hashOf( mat.sc ), mat.thumb,
                                          mat.deps ));

    // Clean up
//    std::string cleanup = "cd " + fileRoot + " && rm " + fn + "*";
//    std::system( cleanup.c_str());
}

void findCandidatesScreenshotForThumbnail( DaemonFileStruct& dfs, const ArchiveDirectory& ad ) {
    if ( dfs.group == ResourceGroup::Geom ) {
        auto candidateScreenshot = ad.findFilesWithExtension( { ".jpg" } );
        for ( const auto& elem : candidateScreenshot ) {
            if ( elem.name.find( "preview_" ) != std::string::npos ||
                 elem.name.find( "_preview" ) != std::string::npos ) {
                dfs.thumb64 = imageUtil::makeThumbnail64( getDaemonRoot() + elem.name );
            }
        }
    }
}

void geomFilterOutSameAssetDifferentFormatFromBasePriority(
        const std::string& formatPriority,
        std::vector<ArchiveDirectoryEntityElement>& destCandidates ) {

    std::vector<std::string> nameChecks{};
    for ( const auto& elem : destCandidates ) {
        if ( elem.name.find( formatPriority ) != std::string::npos ) {
            nameChecks.emplace_back( elem.name );
        }
    }

    erase_if( destCandidates, [nameChecks, formatPriority]( const auto& elem ) {
        auto fn = getFileNameOnly( elem.name );
        for ( const auto& name : nameChecks ) {
            if ( getFileNameOnly( name ) == fn && elem.name != name ) {
                if ( elem.name.find( formatPriority ) == std::string::npos ) {
                    return true;
                }
            }
        }
        return false;
    } );
}

void geomFilterDesingConnectCrazyRedundancy(
        std::vector<ArchiveDirectoryEntityElement>& destCandidates ) {

    auto removeDCCrazyDoubles = [&]( const std::string& _source, const std::vector<std::string>& _v1 ) {
        erase_if( destCandidates, [_source, _v1, destCandidates]( const auto& us ) {
            bool sfn = us.name.find( _source ) != std::string::npos;
            if ( sfn ) {
                for ( const auto& v : _v1 ) {
                    for ( const auto& elem : destCandidates ) {
                        if ( elem.name.find( v ) != std::string::npos ) return true;
                    }
                }
            }
            return false;
        } );
    };

    removeDCCrazyDoubles( "_fbx_upZ.fbx", { "_fbx_upY.fbx" } );
    removeDCCrazyDoubles( "_obj.obj", { "_fbx_upY.fbx", "_fbx_upZ.fbx" } );
}

void materialFilterNonImageAssets( std::vector<ArchiveDirectoryEntityElement>& destCandidates ) {

    erase_if( destCandidates, []( const auto& us ) {
        return !nameHasImageExtension( us.name );
    } );
}

std::vector<ArchiveDirectoryEntityElement>
filterCandidates( const std::vector<ArchiveDirectoryEntityElement>& candidates, strview group ) {
    auto filteredCandidates = candidates;

    if ( group.to_string() == ResourceGroup::Geom ) {
        geomFilterOutSameAssetDifferentFormatFromBasePriority( ".fbx", filteredCandidates );
        geomFilterDesingConnectCrazyRedundancy( filteredCandidates );
    } else if ( group.to_string() == ResourceGroup::Material ) {
        materialFilterNonImageAssets( filteredCandidates );
    }

    return filteredCandidates;
}

void elaborateAsset( DaemonFileStruct& dfs, const std::string& assetName ) {
    dfs.filename = assetName;
    if ( dfs.group == ResourceGroup::Geom ) {
        elaborateGeom( dfs );
    }
    if ( dfs.group == ResourceGroup::Material ) {
        elaborateMaterial( dfs );
    }
    // These are simply file copies, the only tricky bit is to handle the generation of screenshots which is done internally
    if ( dfs.group == ResourceGroup::Profile || dfs.group == ResourceGroup::Font ||
         dfs.group == ResourceGroup::Image ) {
        elaboratePassThrough( dfs );
    }
}

bool groupIsAchievable( const std::string& sourceName, strview group ) {
    return group.to_string() == ResourceGroup::Material && isFileExtCompressedArchive( sourceName );
}

void elaborateCandidates( DaemonFileStruct& dfs ) {
    if ( groupIsAchievable( dfs.filename, dfs.group )) {
        elaborateAsset( dfs, dfs.filename );
    } else {
        for ( const auto& elem : dfs.candidates ) {
            elaborateAsset( dfs, elem.name );
        }
    }
}

uint64_t chooseMainArchiveFilename( const ArchiveDirectory& ad, DaemonFileStruct& dfs ) {

    findCandidatesScreenshotForThumbnail( dfs, ad );

    auto candidates = ad.findFilesWithExtension( getExtForGroup( dfs.group ));
    dfs.candidates = filterCandidates( candidates, dfs.group );
    elaborateCandidates( dfs );

    return dfs.candidates.size();
}

void parseElaborateStream( mongocxx::change_stream& stream, MongoBucket sourceAssetBucket, MongoBucket entityBucket ) {

    for ( auto change : stream ) {
        auto optType = change["operationType"].get_value().get_utf8().value;
        if ( optType != strview("insert") && optType != strview("update") && optType != strview("replace") ) continue;
        StreamChangeMetadata meta{ change };
        Profiler p1{ "elaborate time" };
        auto filename = std::string( meta.filename );
        auto fileDownloaded = Mongo::fileDownload( sourceAssetBucket,
                                                   meta.id,
                                                   getDaemonRoot() + std::string{ filename } );

        DaemonFileStruct dfs{ entityBucket, getFileName( fileDownloaded ), std::string( meta.group ), meta.project,
                              meta.username,
                              meta.useremail };

        ArchiveDirectory ad{ filename };
        // First unzip all the content if package arrives in a zip file
        if ( isFileExtCompressedArchive( std::string( filename ))) {
            unzipFilesToTempFolder( fileDownloaded, ad );
            auto numElaborated = chooseMainArchiveFilename( ad, dfs );
            if (( dfs.filename.empty() || numElaborated == 0 )) {
                daemonWarningLog( std::string( meta.filename ) + " does not contain any appropriate asset file" );
                continue;
            }
        } else {
            elaborateAsset( dfs, filename );
        }
    }
}

JSONDATA( SocketEntityResponse, entities
)

    std::vector<std::string> entities{};
};

void parseAssetStream( Mongo& mdb, mongocxx::change_stream& stream ) {
//    uint64_t counter = 0;
    for ( auto change : stream ) {
        Profiler p1{ "Database asset operations time" };
        auto optType = change["operationType"].get_value().get_utf8().value;
        if ( optType != strview("insert") && optType != strview("update") && optType != strview("replace") ) continue;
        StreamChangeMetadata meta{ change };
        auto ent = mdb.insertEntityFromAsset( meta );
        LOGRS( ent );
//        Socket::emit( "entityAdded" + std::to_string( counter++ ), ent );
    }
}

int main( int argc, char **argv ) {

    LOGRS( "Daemon version 3.1.0" );

    auto *mongoPath = std::getenv( "EH_MONGO_PATH" );
    auto *mongoDefaultDB = std::getenv( "EH_MONGO_DEFAULT_DB" );
    auto *mongoRs = std::getenv( "EH_MONGO_REPLICA_SET_NAME" );

    if ( !mongoPath || !mongoDefaultDB || !mongoRs ) {
        LOGRS( "MongoDB Environment initialization variables not present." );
        return 1;
    }

    Mongo mdb{{ std::string( mongoPath ), std::string( mongoDefaultDB ), std::string( mongoRs ) }};
    auto sourceAssetBucket = mdb.useBucket( "fs_assets_to_elaborate" );
    auto entityBucket = mdb.useBucket( "fs_entity_assets" );

    auto streamToElaborate = mdb["fs_assets_to_elaborate.files"].watch();
    auto streamAsset = mdb["fs_entity_assets.files"].watch();

    try {
        while ( true ) {
            parseElaborateStream( streamToElaborate, sourceAssetBucket, entityBucket );
            parseAssetStream( mdb, streamAsset );
        }
    } catch ( const std::exception& e ) {
        daemonExceptionLog( mdb, e );
    }

    return 0;
}

//#pragma clang diagnostic pop