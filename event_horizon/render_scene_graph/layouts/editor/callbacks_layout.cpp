//
// Created by Dado on 2018-11-05.
//

#include "callbacks_layout.h"

#include <tuple>
#include <unordered_map>
#include <core/util.h>
#include <rapidjson/document.h>
#include <core/file_manager.h>
#include <core/http/webclient.h>
#include <core/TTF.h>
#include <core/raw_image.h>

#include <poly/profile.hpp>
#include <poly/resources/resource_builder.hpp>

#include <render_scene_graph/scene_orchestrator.hpp>

#include "geom_layout.h"
#include "image_layout.h"
#include "material_layout.h"

std::unordered_map<std::string, std::function<entityDaemonCallbackFunction>> daemonEntityCallbacks;

std::unordered_map<std::string, std::tuple<std::string, SerializableContainer>> sceneEntityFilesCallbacks;

void cloudCallback( SocketCallbackDataType data ) {
    std::string filename = url_decode( data["name"].GetString() );
    for ( const auto& [k,func] : daemonEntityCallbacks ) {
        if ( filename.find(DaemonPaths::store(k)) != std::string::npos ) {
            FM::readRemoteSimpleCallback( filename, [&](const Http::Result& _res) {
                const auto lFilename = url_decode(_res.uri);
                for ( const auto& [k,func] : daemonEntityCallbacks ) {
                    if ( lFilename.find( DaemonPaths::store( k )) != std::string::npos ) {
                        auto fn = getFileName( url_decode(_res.uri) );
                        func( fn, zlibUtil::inflateFromMemory( _res ) );
                    }
                }
            } );
        }
    }
}

void allCallbacksEntitySetup() {
    daemonEntityCallbacks[ResourceGroup::Geom] = callbackGeom;
    daemonEntityCallbacks[ResourceGroup::Material] = callbackMaterial;
    Socket::on( "cloudStorageFileUpdate", cloudCallback );
}

template <typename T, typename M>
void addFileCallback( const std::string& _path, M& mm ) {
    SerializableContainer fileContent;
    FM::readLocalFile( _path, fileContent );
    sceneEntityFilesCallbacks[T::Prefix()] = { getFileName(_path), fileContent};
    SceneOrchestrator::sUpdateCallbacks.emplace_back( [&]( SceneOrchestrator* p ) {
        auto tr = sceneEntityFilesCallbacks[T::Prefix()];
        p->SG().B<T>(std::get<0>(tr)).create( std::get<1>(tr) );
    } );
}

void allConversionsDragAndDropCallback( [[maybe_unused]] SceneOrchestrator* p, const std::string& _path ) {
    std::string pathSanitized = url_encode_spacesonly(_path);
    std::string ext = getFileNameExt( pathSanitized );
    std::string extl = toLower(ext);
    std::string finalPath = pathSanitized;

    if ( extl == ".fbx" ) {
        FM::copyLocalToRemote( pathSanitized, DaemonPaths::upload(ResourceGroup::Geom) + getFileName(pathSanitized) );
    }
//    else if ( extl == ".obj" ) {
//        // Convert to GLTF
//        finalPath = getFileNamePath(pathSanitized) + "/" + getFileNameOnly(finalPath) + ".gltf";
//        std::string cmd = "cd " + getFileNamePath(pathSanitized) + "\n obj2gltf -i " + pathSanitized +
//                          " -o " + finalPath;
//        std::system( cmd.c_str() );
//    } else if ( extl == ".stl" ) {
//        stl::parse_stl(pathSanitized);
//    }
    else if ( extl == ".jpg" || extl == ".jepg" || extl == ".png" ) {
        addFileCallback<ImageBuilder, ImageManager>( pathSanitized, p->SG().TL() );
    }
    else if ( extl == ".ttf" ) {
        addFileCallback<FontBuilder, FontManager>( pathSanitized, p->SG().FM() );
    }
    else if ( extl == ".svg" ) {
        addFileCallback<ProfileBuilder, ProfileManager>( pathSanitized, p->SG().PL() );
    }
    else if ( extl == ".sbsar" ) {
        FM::copyLocalToRemote(pathSanitized, DaemonPaths::upload(ResourceGroup::Material)+ getFileName(pathSanitized));
    } else if ( extl == ".gltf" || extl == ".glb" ) {
        callbackGeomGLTF( finalPath );
    }
}
