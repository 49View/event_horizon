//
// Created by Dado on 2019-07-12.
//

#include "scene_loader.hpp"
#include <poly/scene_graph.h>

void ScenePreLoader::loadResCount( HttpResouceCBSign _key ) {
    size_t targetNum = appData.firstTierResourceCount();

    loadedResCounter.emplace_back( _key );
    LOGRS( "[Scene Loader] Resource: " << _key );
    if ( loadedResCounter.size() == targetNum ) {
        loadedResCounter.clear();
        activateGeomLoad();
    }
}

void ScenePreLoader::loadGeomResCount( HttpResouceCBSign _key ) {
    loadedResCounter.emplace_back( _key );
    LOGRS( "[Scene Loader] Resource: " << _key );
    if ( loadedResCounter.size() == appData.secondTierResourceCount()) {
        activatePostLoad();
    }
}

void ScenePreLoader::activateGeomLoad() {
#define LGFUNC std::bind(&ScenePreLoader::loadGeomResCount, this, std::placeholders::_1)

    if ( appData.secondTierResourceCount() == 0) {
        activatePostLoad();
    } else {
        for ( const auto& r : appData.Geoms() ) {
            sgl.load<Geom>( r, LGFUNC );
        }
    }

}

void ScenePreLoader::loadSceneEntities() {
#define LRFUNC std::bind(&ScenePreLoader::loadResCount, this, std::placeholders::_1)

    if ( appData.firstTierResourceCount() == 0 ) {
        activatePostLoad();
    } else {
        for ( const auto& r : appData.Fonts() ) sgl.load<Font>( r, LRFUNC );
        for ( const auto& r : appData.RawImages() ) sgl.load<RawImage>( r, LRFUNC );
        for ( const auto& r : appData.Materials() ) sgl.load<Material>( r, LRFUNC );
        for ( const auto& r : appData.Profiles() ) sgl.load<Profile>( r, LRFUNC );
    }
}

ScenePreLoader::ScenePreLoader( SceneGraph& sg ) : sgl( sg ) {}

//{
//    fontres: [],
//    imageres: [],
//    materialres: [],
//    colorres: [],
//    profileres: [],
//    geomres: []
//}