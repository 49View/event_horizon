//
// Created by Dado on 2019-07-12.
//

#include "scene_loader.hpp"
#include <poly/scene_graph.h>

void SceneLoader::loadResCount( HttpResouceCBSign _key ) {
    size_t targetNum =
            materialColorResourceLoad.size() +
            materialResourceLoad.size() +
            profileResourceLoad.size() +
            rawImageResourceLoad.size() +
            fontResourceLoad.size();

    loadedResCounter.emplace_back( _key );
    LOGRS( "[Scene Loader] Resource: " << _key );
    if ( loadedResCounter.size() == targetNum ) {
        loadedResCounter.clear();
        activateGeomLoad();
    }
}

void SceneLoader::loadGeomResCount( HttpResouceCBSign _key ) {
    loadedResCounter.emplace_back( _key );
    LOGRS( "[Scene Loader] Resource: " << _key );
    if ( loadedResCounter.size() == geomResourceLoad.size()) {
        activatePostLoad();
    }
}

void SceneLoader::activateGeomLoad() {
#define LGFUNC std::bind(&SceneLoader::loadGeomResCount, this, std::placeholders::_1)

    if ( geomResourceLoad.empty()) {
        activatePostLoad();
    } else {
        for ( const auto& r : geomResourceLoad ) {
            sgl.load<Geom>( r, LGFUNC );
        }
    }

}

void SceneLoader::loadSceneEntities() {
#define LRFUNC std::bind(&SceneLoader::loadResCount, this, std::placeholders::_1)

    if ( fontResourceLoad.empty() && rawImageResourceLoad.empty() && materialResourceLoad.empty()
         && profileResourceLoad.empty() ) {
        activatePostLoad();
    } else {
        for ( const auto& r : fontResourceLoad ) sgl.load<Font>( r, LRFUNC );
        for ( const auto& r : rawImageResourceLoad ) sgl.load<RawImage>( r, LRFUNC );
        for ( const auto& r : materialResourceLoad ) sgl.load<Material>( r, LRFUNC );
        for ( const auto& r : profileResourceLoad ) sgl.load<Profile>( r, LRFUNC );
    }
}

SceneLoader::SceneLoader( SceneGraph& sg ) : sgl( sg ) {}
