//
// Created by Dado on 2018-11-05.
//

#include "callbacks_layout.h"

#include <unordered_map>
#include <core/util.h>
#include <rapidjson/document.h>
#include <core/file_manager.h>
#include <core/http/webclient.h>
#include <core/TTF.h>

#include <poly/resources/font_builder.h>
#include <render_scene_graph/scene_orchestrator.hpp>

#include "geom_layout.h"
#include "image_layout.h"
#include "material_layout.h"

std::unordered_map<std::string, std::function<entityDaemonCallbackFunction>> daemonEntityCallbacks;

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
    daemonEntityCallbacks[EntityGroup::Geom] = callbackGeom;
    daemonEntityCallbacks[EntityGroup::Material] = callbackMaterial;
    Socket::on( "cloudStorageFileUpdate", cloudCallback );
}

void allConversionsDragAndDropCallback( [[maybe_unused]] SceneOrchestrator* p, const std::string& _path ) {
    std::string pathSanitized = url_encode_spacesonly(_path);
    std::string ext = getFileNameExt( pathSanitized );
    std::string extl = toLower(ext);
    std::string finalPath = pathSanitized;

    if ( extl == ".fbx" ) {
        FM::copyLocalToRemote( pathSanitized, DaemonPaths::upload(EntityGroup::Geom) + getFileName(pathSanitized) );
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
        SerializableContainer fileContent;
        FM::readLocalFile( pathSanitized, fileContent );
        callbackImage( pathSanitized, fileContent );
    }
    else if ( extl == ".ttf" ) {
        SerializableContainer fileContent;
        FM::readLocalFile( pathSanitized, fileContent );
        static SerializableContainer fontBufferData = fileContent;
        static std::string fontName = getFileName(pathSanitized);
        SceneOrchestrator::sUpdateCallbacks.emplace_back( [&]( SceneOrchestrator* p ) {
            FontBuilder{p->SG().FM(), fontName}.create( fontBufferData );
        } );
    }
    else if ( extl == ".svg" ) {
        callbackGeomSVG( pathSanitized, FM::readLocalTextFile( pathSanitized ) );
    }
    else if ( extl == ".sbsar" ) {
        FM::copyLocalToRemote(pathSanitized, DaemonPaths::upload(EntityGroup::Material)+ getFileName(pathSanitized));
    } else if ( extl == ".gltf" || extl == ".glb" ) {
        callbackGeomGLTF( finalPath );
    }
}
