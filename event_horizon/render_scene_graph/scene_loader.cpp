//
// Created by Dado on 2019-07-12.
//

#include "scene_loader.hpp"
#include <poly/scene_graph.h>
#include <render_scene_graph/render_orchestrator.h>

void ScenePreLoader::loadResCount( HttpResouceCBSign _key ) {
    size_t targetNum = appData.firstTierResourceCount();

    loadedResCounter.emplace_back( _key );
    float progress = ( static_cast<float>(loadedResCounter.size()) / static_cast<float>(appData.totalResourceCount()));
    rsgl.RR().setProgressionTiming( progress );
    if ( loadedResCounter.size() == targetNum ) {
        loadedResCounter.clear();
        activateGeomLoad();
    }
}

void ScenePreLoader::loadGeomResCount( HttpResouceCBSign _key ) {
    loadedResCounter.emplace_back( _key );
    float progress = ( static_cast<float>(loadedResCounter.size() + appData.firstTierResourceCount()) /
                       static_cast<float>(appData.totalResourceCount()));
    rsgl.RR().setProgressionTiming( progress );
    if ( loadedResCounter.size() == appData.secondTierResourceCount()) {
        loadedResCounter.clear();
        activatePostLoadInternal();
    }
}

void ScenePreLoader::activateGeomLoad() {
#define LGFUNC std::bind(&ScenePreLoader::loadGeomResCount, this, std::placeholders::_1)

    if ( appData.secondTierResourceCount() == 0 ) {
        activatePostLoadInternal();
    } else {
        for ( const auto& r : appData.Geoms() ) {
            sgl.load<Geom>( r, LGFUNC );
        }
    }

}

void ScenePreLoader::loadSceneEntities() {
#define LRFUNC std::bind(&ScenePreLoader::loadResCount, this, std::placeholders::_1)

    sgl.setMaterialRemap( appData.getMatRemapping() );

    rsgl.RR().setLoadingFlag( true );
    if ( appData.firstTierResourceCount() == 0 ) {
        activatePostLoadInternal();
    } else {
        for ( const auto& r : appData.Fonts() ) sgl.load<Font>( r, LRFUNC );
        for ( const auto& r : appData.RawImages() ) sgl.load<RawImage>( r, LRFUNC );
        for ( const auto& r : appData.Materials() ) sgl.load<Material>( r, LRFUNC );
        for ( const auto& r : appData.Profiles() ) sgl.load<Profile>( r, LRFUNC );
    }
}

ScenePreLoader::ScenePreLoader( SceneGraph& sg, RenderOrchestrator& _rsg ) : sgl( sg ), rsgl(_rsg) {}

void ScenePreLoader::activateFinalLoadInternal() {
    rsgl.RR().setLoadingFlag( false );

    rsgl.RR().setShadowOverBurnCofficient( appData.getRenderSettings().shadowOverBurnCofficient );
    rsgl.RR().setIndoorSceneCoeff(appData.getRenderSettings().indoorSceneCoeff);
    rsgl.RR().setShadowZFightCofficient(appData.getRenderSettings().shadowZFightCofficient);

    activatePostLoad();
}

void ScenePreLoader::loadCustomResCount( HttpResouceCBSign _key ) {

    loadedResCounter.emplace_back( _key );
    float progress = ( static_cast<float>(loadedResCounter.size() + appData.firstTierResourceCount()+ appData.secondTierResourceCount()) /
                       static_cast<float>(appData.totalResourceCount()));
    rsgl.RR().setProgressionTiming( progress );
    if ( loadedResCounter.size() == appData.customTierResourceCount()) {
        activateFinalLoadInternal();
    }
}

void ScenePreLoader::activatePostLoadInternal() {
#define LCFUNC std::bind(&ScenePreLoader::loadCustomResCount, this, std::placeholders::_1)

    if ( appData.customTierResourceCount() > 0 ) {
        std::string ckey = "room_layout";
        for ( const auto& r : appData.Custom(ckey) ) {
            Http::get( Url( HttpFilePrefix::entities + ckey + "/" + url_encode( r ) ),
               [this, ckey](HttpResponeParams _res) {
                   if ( _res.statusCode == 204 ) return; // empty result, handle defaults??
                   auto c = SerializableContainer{_res.buffer.get(), _res.buffer.get()+_res.length};
                   if ( auto it = customResources.find(ckey);  it == customResources.end() ) {
                       std::vector<SerializableContainer> cs{};
                       cs.emplace_back(c);
                       customResources.emplace( ckey, cs );
                   } else {
                       it->second.emplace_back(c);
                   }
                   _res.ccf(ckey);
               },
               nullptr,
               Http::ResponseFlags::None,
               LCFUNC );
        }
    } else {
        activateFinalLoadInternal();
    }
}

const SerializableContainer& ScenePreLoader::getPreloadCustomSerializableContainer( const std::string& _key, size_t _index ) {
    return customResources[_key][_index];
}
