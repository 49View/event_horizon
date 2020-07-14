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
#include <core/descriptors/gltf2utils.h>

#include <database/nosql/mongo/mongo.hpp>

#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>
#include <core/image_util.h>
#include <core/string_util.h>
#include <core/profiler.h>
#include <ostream>
#include <utility>
#include <filesystem>

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

struct EntityMeta {
    EntityMeta( const std::string& name, const std::string& source, const std::string& group,
                const std::string& project, const MongoObjectId& userId ) : name(name), source(source), group(group),
                                                                            project(project), userId(userId) {}
    std::string name;
    std::string source;
    std::string group;
    std::string project;
    MongoObjectId userId;
    std::string contentType;
    std::string hash;
    std::string thumb;
    V3f bboxSize = V3f::ZERO;
    bool isPublic = true;
    bool isRestricted = false;
};

struct DaemonFileStruct2 {
    DaemonFileStruct2( Mongo &mdb, std::string fileRoot, EntityMeta entity ) : mdb( mdb ),
                                                                               fileRoot( std::move( fileRoot )),
                                                                               entity( std::move( entity )) {}

    Mongo &mdb;
    std::string fileRoot;
    std::string thumbHint;
    EntityMeta entity;
    std::vector<ArchiveDirectoryEntityElement> candidates{};

    std::string filePath() {
        return fileRoot + "entities/" + entity.group + "/";
    }
    std::string uploadFilePath() {
        return fileRoot + "uploads/" + entity.group + "/";
    }
};

class DaemonException : public std::exception {
public:
    DaemonException( const std::string &msg ) : msg( msg ) {}

    virtual const char *what() const throw() {
        return msg.c_str();
    }

private:
    std::string msg{};
};

static std::vector<std::string> getExtForGroup( const std::string &_group ) {
    std::unordered_map<std::string, std::vector<std::string>> extmap;

    extmap[ResourceGroup::Geom] = { ".fbx", ".glb", ".gltf", ".obj" };
    extmap[ResourceGroup::Material] = { ".sbsar", ".png", ".jpg" };

    if ( auto it = extmap.find( _group ); it != extmap.end()) {
        return it->second;
    }
    return {};
}

void daemonExceptionLog( Mongo &mdb, const std::exception &e, const std::string &username = {} ) {
    LOGRS( e.what());
    mdb.insertDaemonCrashLog( e.what(), username );
    exit( 1 );
}

void daemonWarningLog( const std::string &e ) {
    LOGRS( e );
}

std::string getContentTypeFor( const DaemonFileStruct &dfs ) {
    if ( dfs.group == ResourceGroup::Material ) {
        return HttpContentType::json;
    } else {
        return HttpContentType::octetStream;
    }
}

std::string createThumbnailCommandFor( DaemonFileStruct2 &dfs ) {

    auto filepath = dfs.filePath();
    std::filesystem::create_directories( std::filesystem::path( filepath ));
    if ( dfs.entity.group == ResourceGroup::Image ) {
        return "convert " + dfs.fileRoot + dfs.entity.source +
               " -thumbnail '128x128^' -gravity center -extent '128x128' -quality 90 " + dfs.entity.thumb;
    } else if ( dfs.entity.group == ResourceGroup::Material ) {
        return "convert " + dfs.fileRoot + getFileNamePath( dfs.entity.source ) + "/" +
               dfs.thumbHint +
               " -thumbnail '128x128^' -gravity center -extent '128x128' -quality 90 " + dfs.entity.thumb;
    } else if ( dfs.entity.group == ResourceGroup::Profile ) {
        return "convert " + dfs.fileRoot + dfs.entity.source +
               " -trim +repage -background white -thumbnail '256x256^' -gravity center -extent '256x256' -quality 90 " +
               dfs.entity.thumb;
    } else if ( dfs.entity.group == ResourceGroup::Font ) {
        return "convert -font " + dfs.fileRoot + dfs.entity.source + " -background white -pointsize 50 label:\"aA\" " +
               dfs.entity.thumb;
    }

    daemonWarningLog( "Asset of type " + dfs.entity.group + " went into the wrong pipeline." );

    return {};
}

void geomFilterOutSameAssetDifferentFormatFromBasePriority(
        const std::string &formatPriority,
        std::vector<ArchiveDirectoryEntityElement> &destCandidates ) {

    std::vector<std::string> nameChecks{};
    for ( const auto &elem : destCandidates ) {
        if ( elem.name.find( formatPriority ) != std::string::npos ) {
            nameChecks.emplace_back( elem.name );
        }
    }

    erase_if( destCandidates, [nameChecks, formatPriority]( const auto &elem ) {
        auto fn = getFileNameOnly( elem.name );
        for ( const auto &name : nameChecks ) {
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
        std::vector<ArchiveDirectoryEntityElement> &destCandidates ) {

    auto removeDCCrazyDoubles = [&]( const std::string &_source, const std::vector<std::string> &_v1 ) {
        erase_if( destCandidates, [_source, _v1, destCandidates]( const auto &us ) {
            bool sfn = us.name.find( _source ) != std::string::npos;
            if ( sfn ) {
                for ( const auto &v : _v1 ) {
                    for ( const auto &elem : destCandidates ) {
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

void materialFilterNonImageAssets( std::vector<ArchiveDirectoryEntityElement> &destCandidates ) {

    erase_if( destCandidates, []( const auto &us ) {
        return !nameHasImageExtension( us.name );
    } );
}

std::vector<ArchiveDirectoryEntityElement>
filterCandidates( const std::vector<ArchiveDirectoryEntityElement> &candidates, const std::string &group ) {
    auto filteredCandidates = candidates;

    if ( group == ResourceGroup::Geom ) {
        geomFilterOutSameAssetDifferentFormatFromBasePriority( ".fbx", filteredCandidates );
        geomFilterDesingConnectCrazyRedundancy( filteredCandidates );
    } else if ( group == ResourceGroup::Material ) {
        materialFilterNonImageAssets( filteredCandidates );
    }

    return filteredCandidates;
}

void generateThumbnail( DaemonFileStruct2 &dfs ) {
    dfs.entity.thumb = dfs.filePath() + dfs.entity.hash + "_thumb.jpg";
    if ( !FM::fileExist( dfs.entity.thumb )) {
        std::string cmdThumbnail = createThumbnailCommandFor( dfs );
        auto ret = std::system( cmdThumbnail.c_str());
        if ( ret != 0 ) {
            throw std::runtime_error( std::string{ "Failed to generate thumbnail for " + dfs.entity.thumb } );
        }
        dfs.entity.thumb = getFileName( dfs.entity.thumb );
    }
}

//std::optional<MongoFileUpload> upload( DaemonFileStruct2 dfs ) {
//    return Mongo::fileUpload( dfs.bucket, dfs.entity.source, res.sc,
//                              Mongo::FSMetadata( dfs.group, dfs.project, dfs.uname, dfs.uemail,
//                                                 getContentTypeFor( dfs ), Hashable<>::hashOf( res.sc ), res.thumb,
//                                                 res.deps ));
//}

void elaboratePassThrough( DaemonFileStruct2 &dfs, const SerializableContainer &filedata ) {
    dfs.entity.hash = Hashable<>::hashOf( filedata );
    if ( dfs.entity.group == ResourceGroup::Profile || dfs.entity.group == ResourceGroup::Font ||
         dfs.entity.group == ResourceGroup::Image, dfs.entity.group == ResourceGroup::Material ) {
        generateThumbnail( dfs );
    }
    dfs.entity.contentType = HttpContentType::octetStream;
    if ( dfs.entity.group == ResourceGroup::Geom ) {
        dfs.entity.bboxSize = GLTF2Service::GLTFSize( dfs.entity.hash, "", filedata);
    }

    auto filePath = dfs.filePath() + dfs.entity.hash;
    auto writeRes = FM::writeLocalFile( filePath, filedata );
    if (!writeRes) {
        throw std::runtime_error( std::string{"Can't write file: " + filePath});
    }
    auto entityId = dfs.mdb.upsertEntity( dfs.entity );
    dfs.mdb.updateUploads( dfs.entity, entityId );

//    ResourceEntityHelper res{ FM::readLocalFileC( getDaemonRoot() + dfs.entity.source ), {}, generateThumbnail( dfs2 ) };
//    return upload( dfs, res );
}

//std::optional<MongoFileUpload> elaborateImage(
//        int desiredWidth, int desiredHeight,
//        DaemonFileStruct dfs,
//        std::string& thumb ) {
//    try {
//        int w = 0, h = 0, n = 0;
//        int thumbSize = 128;
//
//        unsigned char *input_data = stbi_load( dfs.entity.source.c_str(), &w, &h, &n, 0 );
//
//        auto output_data = make_uint8_p( desiredWidth * desiredHeight * n );
//        auto output_data_thumb = make_uint8_p( thumbSize * thumbSize * n );
//        stbir_resize_uint8( input_data, w, h, 0, output_data.first.get(), desiredWidth, desiredHeight, 0, n );
//        stbir_resize_uint8( output_data.first.get(), desiredWidth, desiredHeight, 0, output_data_thumb.first.get(),
//                            thumbSize, thumbSize, 0, n );
//
//        auto bm = imageUtil::bufferToMemoryCompressed( desiredWidth, desiredHeight, n, output_data.first.get(),
//                                                       imageUtil::extToMime( getFileNameExtToLower( dfs.entity.source )));
//        auto bm64 = imageUtil::bufferToPng64( thumbSize, thumbSize, n, output_data_thumb.first.get());
//        if ( MPBRTextures::isBaseColorTexture( dfs.entity.source )) {
//            thumb = bm64;
//        }
//        return Mongo::fileUpload( dfs.bucket, getFileName( dfs.entity.source ), std::move( bm ),
//                                  Mongo::FSMetadata( ResourceGroup::Image, dfs.project, dfs.uname, dfs.uemail,
//                                                     HttpContentType::octetStream,
//                                                     MD5( bm.first.get(), bm.second ).hexdigest(), bm64 ));
//    } catch ( const std::exception& e ) {
//        LOGRS( e.what());
//        return std::nullopt;
//    }
//}

ArchiveDirectory generateActiveDirectoryFromSBSARTempFiles( const std::string &fileRoot, const std::string &fn ) {
    ArchiveDirectory ret{ fn };

    for ( const auto &output : MPBRTextures::SBSARTextureOutputs()) {
        std::string outFilename{};
        outFilename.append( fn ).append( "_" ).append( output ).append( ".png" );
        if ( FM::fileExist( fileRoot + outFilename )) {
            ret.insert( { outFilename, 0, output } );
        }
    }
    return ret;
}

ArchiveDirectory mapActiveDirectoryFilesToPBR( DaemonFileStruct2 dfs ) {
    ArchiveDirectory ad{ getFileNameKey( dfs.entity.source ) };

    for ( auto &&elem : dfs.candidates ) {
        elem.metaString = MPBRTextures::findTextureInString( elem.name );
        ad.insert( std::move( elem ));
    }
    return ad;
}

std::string elaborateInternalMaterial( const ArchiveDirectory &ad, int nominalSize, DaemonFileStruct2 &dfs ) {

    Material pbrmaterial{ S::SH, ad.Name() };
    for ( const auto &output : ad ) {
        if ( !output.second.metaString.empty()) {
            auto tid = output.second.name;
            pbrmaterial.Values()->assign( MPBRTextures::mapToTextureUniform( output.second.metaString ), tid );
            if ( MPBRTextures::isMetallicTexture( output.second.metaString )) {
                pbrmaterial.setMetallicValue( 1.0f );
            }
            if ( MPBRTextures::isRoughnessTexture( output.second.metaString )) {
                pbrmaterial.setRoughnessValue( 1.0f );
            }
            if ( MPBRTextures::isBaseColorTexture( output.second.metaString )) {
                dfs.thumbHint = tid;
            }
        }
    }

    auto filePath = dfs.fileRoot + getFileNamePath( dfs.entity.source ) + "/" + ad.Name() + ".mat";
    FM::writeLocalFile( filePath, pbrmaterial.serialize());

    return filePath;
}

ArchiveDirectory elaborateMatSBSAR(
        int size,
        DaemonFileStruct2 &dfs ) {

    auto fileRoot = dfs.fileRoot;
    auto outputPath = fileRoot + getFileNamePath( dfs.entity.source ) + "/";

    std::string fn = getFileNameOnly( dfs.entity.source );
    std::string fext = ".png";

    std::string sizeString = std::to_string( log2( size ));
    std::string sbRender = "/opt/Allegorithmic/Substance_Automation_Toolkit/sbsrender render --inputs "
                           + fileRoot + dfs.entity.source +
                           " --set-value '$outputsize@" + sizeString + "," + sizeString +
                           "' --output-bit-depth \"8\" --png-format-compression best_compression "
                           "--output-name {inputName}_{outputNodeName}";
    for ( const auto &output : MPBRTextures::SBSARTextureOutputs()) {
        sbRender.append( " --input-graph-output " + output );
    }
    sbRender.append( " --output-path " + outputPath );

    auto ret = std::system( sbRender.c_str());
    if ( ret != 0 ) {
        throw std::runtime_error( std::string( "Failed to run: " + sbRender ));
    }

    return generateActiveDirectoryFromSBSARTempFiles( outputPath, fn );
}

int resaveGLB( const std::string &filename ) {
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

    for ( auto &it : model.accessors ) {
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
            for ( auto &v : it.maxValues ) v *= 0.01f;
            for ( auto &v : it.minValues ) v *= 0.01f;
        }
    }

    loader.WriteGltfSceneToFile( &model, filename, true, true, true, true );

    return 0;
}

std::string
getGeomElaborateCommand( DaemonFileStruct2 &dfs, const std::string &filenameglb, const std::string &ftype ) {

    if ( ftype == ".fbx" ) {
        return
                "cd " + dfs.uploadFilePath() +
                " && FBX2glTF -b --pbr-metallic-roughness -o '" + filenameglb +
                "' '" +
                getFileName( dfs.entity.source ) + "'";
    }
    if ( ftype == ".obj" ) {
        return
                "cd " + dfs.uploadFilePath() + " && obj2glTF -i '" +
                getFileName( dfs.entity.source ) + "'  -o '" + filenameglb + "'";
    }

    return {};
}

SerializableContainer
elaborateGeomFile( DaemonFileStruct2 &dfs, const SerializableContainer &filedata, const std::string &ftype ) {
    std::string filenameglb = dfs.fileRoot + getFileNameOnly( dfs.entity.source ) + ".glb";

    std::string cmd = getGeomElaborateCommand( dfs, filenameglb, ftype );
    auto ret = std::system( cmd.c_str());
    if ( ret != 0 ) {
        throw std::runtime_error( std::string{ ftype + " elaboration return code: " + std::to_string( ret ) } );
    };

    return FM::readLocalFileC( filenameglb );
}

SerializableContainer elaborateGeom( DaemonFileStruct2 &dfs, const SerializableContainer &filedata ) {

    auto fext = getFileNameExtToLower( dfs.entity.source );
    if ( fext == ".zip" ) {
        ArchiveDirectory ad{ dfs.entity.source };
        auto filePath = dfs.fileRoot + getFileNamePath( dfs.entity.source );
        unzipFilesToTempFolder( dfs.fileRoot + dfs.entity.source, ad, filePath );
        auto candidates = ad.findFilesWithExtension( getExtForGroup( dfs.entity.group ));
        dfs.candidates = filterCandidates( candidates, dfs.entity.group );
        return {};
    } else if ( fext == ".fbx" || fext == ".obj" ) {
        return elaborateGeomFile( dfs, filedata, fext );
    }
    // ".glb" || ".gltf"
    return filedata;
}

SerializableContainer elaborateMaterial( DaemonFileStruct2& dfs, const SerializableContainer &filedata ) {
    ArchiveDirectory ad{ dfs.entity.source };
    int sensibleRescaleSize = 512;
    auto filePath = dfs.fileRoot + getFileNamePath( dfs.entity.source );

    if ( getFileNameExtToLower( std::string( dfs.entity.source )) == ".zip" ) {
        unzipFilesToTempFolder( dfs.fileRoot + dfs.entity.source, ad, filePath );
        auto candidates = ad.findFilesWithExtension( getExtForGroup( dfs.entity.group ));
        dfs.candidates = filterCandidates( candidates, dfs.entity.group );
        ad = mapActiveDirectoryFilesToPBR( dfs );
        // Right so if it comes from a zip file it's the provider decision to have their exact size
        // so in this case we'll set sensibleRescaleSize to -1 to tell the elaboration not to touch it
        sensibleRescaleSize = -1;
    } else if ( getFileNameExtToLower( std::string( dfs.entity.source )) == ".sbsar" ) {
        int nominalSize = 512;
        sensibleRescaleSize = -1;
        ad = elaborateMatSBSAR( nominalSize, dfs );
    }

    // Gather texture outputs
    auto pbrFile = elaborateInternalMaterial( ad, sensibleRescaleSize, dfs );

    auto tarname = ad.Name() + ".tar";
    std::string cmd = "cd " + filePath + " && tar cvf " + tarname + " " + getFileName( pbrFile ) + " ";
    for ( const auto &output : ad ) {
        cmd += output.second.name + " ";
    }
    auto ret = std::system( cmd.c_str());
    if ( ret != 0 ) {
        throw std::runtime_error( std::string{ "Tarring material elaboration return code: " + std::to_string( ret ) } );
    };

    return FM::readLocalFileC( filePath + "/" + tarname );

    //        ResourceEntityHelper res{ FM::readLocalFileC( getDaemonRoot() + dfs.entity.source ), {}, generateThumbnail( dfs ) };
//        if ( MPBRTextures::isBaseColorTexture( output.second.name ) ) {
//            generateThumbnail(dfs);
//        }
//        auto to = upload( dfs, res );

//    Mongo::fileUpload( dfs.bucket, dfs.entity.source, mat.sc,
//                       Mongo::FSMetadata( dfs.group, dfs.project, dfs.uname, dfs.uemail,
//                                          HttpContentType::json, Hashable<>::hashOf( mat.sc ), mat.thumb,
//                                          mat.deps ));

    // Clean up
//    std::string cleanup = "cd " + fileRoot + " && rm " + fn + "*";
//    std::system( cleanup.c_str());
}

void findCandidatesScreenshotForThumbnail( DaemonFileStruct &dfs, const ArchiveDirectory &ad ) {
    if ( dfs.group == ResourceGroup::Geom ) {
        auto candidateScreenshot = ad.findFilesWithExtension( { ".jpg" } );
        for ( const auto &elem : candidateScreenshot ) {
            if ( elem.name.find( "preview_" ) != std::string::npos ||
                 elem.name.find( "_preview" ) != std::string::npos ) {
                dfs.thumb64 = imageUtil::makeThumbnail64( getDaemonRoot() + elem.name );
            }
        }
    }
}


void elaborateAsset( DaemonFileStruct2 &dfs, const std::string &assetName, const SerializableContainer &filedata ) {
    dfs.entity.source = assetName;
    if ( dfs.entity.group == ResourceGroup::Geom ) {
        auto candidate = elaborateGeom( dfs, filedata );
        if ( !candidate.empty() ) {
            elaboratePassThrough( dfs, candidate );
        } else {
            if ( !dfs.candidates.empty() ) {
                for ( const auto& candidateFile : dfs.candidates ) {
                    dfs.entity.source = candidateFile.name;
                    dfs.entity.name = candidateFile.name;
                    elaboratePassThrough( dfs, elaborateGeom( dfs, FM::readLocalFileC( dfs.uploadFilePath() + candidateFile.name ) ));
                }
            }
        }
    } else if ( dfs.entity.group == ResourceGroup::Material ) {
        elaboratePassThrough( dfs, elaborateMaterial( dfs, filedata ));
    } else {
        // These are simply file copies, the only tricky bit is to handle the generation of screenshots which is done internally
        elaboratePassThrough( dfs, filedata );
    }
}

bool groupIsAchievable( const std::string &sourceName, strview group ) {
    return group.to_string() == ResourceGroup::Material && isFileExtCompressedArchive( sourceName );
}

void elaborateCandidates( DaemonFileStruct &dfs ) {
//    if ( groupIsAchievable( dfs.entity.source, dfs.group )) {
//        elaborateAsset( dfs, dfs.entity.source );
//    } else {
//        for ( const auto& elem : dfs.candidates ) {
//            elaborateAsset( dfs, elem.name );
//        }
//    }
}

uint64_t chooseMainArchiveFilename( const ArchiveDirectory &ad, DaemonFileStruct &dfs ) {

//    findCandidatesScreenshotForThumbnail( dfs, ad );

    auto candidates = ad.findFilesWithExtension( getExtForGroup( dfs.group ));
    dfs.candidates = filterCandidates( candidates, dfs.group );
    elaborateCandidates( dfs );

    return dfs.candidates.size();
}

void parseUploadStream( Mongo &mdb, mongocxx::change_stream &stream, const std::string &fileRoot ) {

    for ( auto change : stream ) {
        auto optType = change["operationType"].get_value().get_utf8().value;
        if ( optType != strview( "insert" ))
            continue;

        auto doc = change["fullDocument"];
        auto filename = std::string{ doc["filename"].get_utf8().value };
        auto group = std::string{ doc["group"].get_utf8().value };
        auto project = std::string{ doc["project"].get_utf8().value };
        auto username = std::string{ doc["username"].get_utf8().value };
        auto userId = MongoObjectId{ doc["userId"].get_value() };

        auto fileDownloaded = FM::readLocalFileC( fileRoot + filename );

        DaemonFileStruct2 dfs{ mdb, fileRoot, EntityMeta{ getFileName( filename ), filename, group, project, userId }};
        try {
            elaborateAsset( dfs, filename, fileDownloaded );
        } catch ( const std::exception &e ) {
            daemonExceptionLog( mdb, e, username );
        }
    }
}

void updateGeomsBBox(Mongo &mdb, const std::string &fileRoot) {
    auto cursor = mdb.find( "entities", std::vector<std::string>{"group", ResourceGroup::Geom} );
    for(auto doc : cursor) {
        auto filename = doc["hash"].get_utf8().value;
        auto size = GLTF2Service::GLTFSize(std::string{filename}, std::string{fileRoot}+"entities/" + ResourceGroup::Geom + "/" + std::string{filename}, {});
        mdb.upsertBBox(doc, size);
        LOGRS(size);
    }
}

int main( int argc, char **argv ) {

    LOGRS( "Daemon version 4.0.4" );

    auto *mongoPath = std::getenv( "EH_MONGO_PATH" );
    auto *mongoDefaultDB = std::getenv( "EH_MONGO_DEFAULT_DB" );
    auto *mongoRs = std::getenv( "EH_MONGO_REPLICA_SET_NAME" );
    auto *fileRoot = std::getenv( "EH_FILE_ROOT" );

    if ( !mongoPath || !mongoDefaultDB || !mongoRs ) {
        LOGRS( "MongoDB Environment initialization variables not present." );
        return 1;
    }

    Mongo mdb{{ std::string( mongoPath ), std::string( mongoDefaultDB ), std::string( mongoRs ) }};
    auto uploadStream = mdb["uploads"].watch();

    try {
        while ( true ) {
            parseUploadStream( mdb, uploadStream, fileRoot );
        }
    } catch ( const std::exception &e ) {
        daemonExceptionLog( mdb, e );
    }

    return 0;
}

//#pragma clang diagnostic pop