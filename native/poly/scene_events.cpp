//
// Created by Dado on 2018-11-05.
//

#include "scene_events.h"

#include <unordered_map>
#include <core/util.h>
#include <core/file_manager.h>
#include <core/descriptors/uniform_names.h>
#include <core/heterogeneous_map.hpp>
#include <core/http/webclient.h>
#include <core/TTF.h>
#include <core/raw_image.h>
#include <core/resources/profile.hpp>
#include <core/resources/material.h>
#include <core/resources/resource_pipe.hpp>
#include <core/resources/resource_builder.hpp>
#include <poly/scene_graph.h>

std::unordered_map<std::string, std::function<entityDaemonCallbackFunction>> daemonEntityCallbacks;

void cloudCallback( const std::string& msg, SocketCallbackDataType&& data ) {
    std::string filename = url_decode( data["name"].GetString());
    for ( const auto&[k, func] : daemonEntityCallbacks ) {
        if ( filename.find( DaemonPaths::store( k )) != std::string::npos ) {
            FM::readRemoteSimpleCallback( filename, [&]( const Http::Result& _res ) {
                const auto lFilename = url_decode( _res.uri );
                for ( const auto&[k, func] : daemonEntityCallbacks ) {
                    if ( lFilename.find( DaemonPaths::store( k )) != std::string::npos ) {
                        auto fn = getFileName( url_decode( _res.uri ));
                        func( fn, zlibUtil::inflateFromMemory( _res ));
                    }
                }
            } );
        }
    }
}

void allCallbacksEntitySetup() {
//    daemonEntityCallbacks[ResourceGroup::Geom] = callbackGeom;
//    daemonEntityCallbacks[ResourceGroup::Material] = callbackMaterial;
    Socket::on( "cloudStorageFileUpdate", cloudCallback );
    Socket::on( SceneEvents::ReloadLuaScript, SceneGraph::addEventCallback );
    Socket::on( SceneEvents::UpdateEntity, SceneGraph::addEventCallback );
    Socket::on( SceneEvents::AddPlaceHolderEntity, SceneGraph::addEventCallback );
    Socket::on( SceneEvents::LoadGeomAndReset, SceneGraph::addEventCallback );
    Socket::on( SceneEvents::ReplaceMaterialOnCurrentObject, SceneGraph::addEventCallback );
    Socket::on( SceneEvents::ChangeMaterialProperty, SceneGraph::addEventCallback );
}

template<typename T>
void addFileCallback( const std::string& _path ) {
    SerializableContainer fileContent;
    FM::readLocalFile( _path, fileContent );
    SceneGraph::addGenericCallback( ResourceVersioning<T>::Prefix(), { getFileName( _path ), fileContent, _path } );
}

void allConversionsDragAndDropCallbackMultiplePaths( const std::vector<std::string>& _paths ) {
    ResourcePipe rpipe;

    for ( const auto& path : _paths ) {
        rpipe.pipeFile<RawImage>( path );
    }

    auto values = std::make_shared<HeterogeneousMap>( S::SH );
    for ( const auto& entry : rpipe.getCatalog()) {
        values->assign( MPBRTextures::mapToTextureUniform( entry.filename ), entry.hash );
    }

    auto fn = getLastFolderInPath( _paths.back());
    rpipe.pipe<Material>( fn, Material{ values }.serialize());
    rpipe.publish();
}

void allConversionsDragAndDropCallback( std::vector<std::string>& _paths ) {

    if ( _paths.empty()) return;

    if ( _paths.size() == 1 ) {
        auto _path = _paths.back();
        std::string pathSanitized = url_encode_spacesonly( _path );
        std::string ext = getFileNameExt( pathSanitized );
        std::string extl = toLower( ext );

        if ( extl == ".fbx" ) {
            FM::copyLocalToRemote( pathSanitized,
                                   DaemonPaths::upload( ResourceGroup::Geom ) + getFileName( pathSanitized ));
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
        else if ( isFileExtAnImage( extl )) {
            addFileCallback<RawImage>( pathSanitized );
        } else if ( extl == ".ttf" ) {
            addFileCallback<Font>( pathSanitized );
        } else if ( extl == ".svg" ) {
            addFileCallback<Profile>( pathSanitized );
        } else if ( extl == ".sbsar" ) {
            FM::copyLocalToRemote( pathSanitized,
                                   DaemonPaths::upload( ResourceGroup::Material ) + getFileName( pathSanitized ));
        } else if ( extl == ".gltf" || extl == ".glb" ) {
            addFileCallback<Geom>( pathSanitized );
        }
    } else {
        allConversionsDragAndDropCallbackMultiplePaths( _paths );
    }

    _paths.clear();
}
