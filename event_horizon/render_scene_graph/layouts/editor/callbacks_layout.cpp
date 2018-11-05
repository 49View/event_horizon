//
// Created by Dado on 2018-11-05.
//

#include "callbacks_layout.h"

#include <unordered_map>
#include <core/util.h>
#include <rapidjson/document.h>
#include <core/file_manager.h>

#include "geom_layout.h"
#include "material_layout.h"

std::unordered_map<std::string, std::function<entityDaemonCallbackFunction>> daemonEntityCallbacks;

void cloudCallback( const rapidjson::Document& data ) {
    std::string filename = data["name"].GetString();
    for ( const auto& [k,func] : daemonEntityCallbacks ) {
        if ( filename.find(DaemonPaths::store(k)) != std::string::npos ) {
            FM::readRemoteSimpleCallback( filename, [&](const Http::Result& _res) {
                for ( const auto& [k,func] : daemonEntityCallbacks ) {
                    if ( filename.find( DaemonPaths::store( k )) != std::string::npos ) {
                        func( filename, zlibUtil::inflateFromMemory( _res ) );
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

void allConversionsDragAndDropCallback( Scene* p, const std::string& _path ) {
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
    else if ( extl == ".sbsar" ) {
        FM::copyLocalToRemote(pathSanitized, DaemonPaths::upload(EntityGroup::Material)+ getFileName(pathSanitized));
    } else if ( extl == ".gltf" || extl == ".glb" ) {
        callbackGeomGLTF( finalPath );
    }
}