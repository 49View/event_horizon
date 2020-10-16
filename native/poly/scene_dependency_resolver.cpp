//
// Created by Dado on 2019-07-12.
//

#include "scene_dependency_resolver.hpp"
#include <poly/scene_graph.h>

void SceneDependencyResolver::loadResCount( HttpResourceCBSign _key ) {
    size_t targetNum = firstTierResourceCount();

    loadedResCounter.emplace_back( _key );
    float progress = ( static_cast<float>(loadedResCounter.size()) / static_cast<float>(totalResourceCount()));
    LOGRS("Progress: "<< progress);
    sgl.preloadProgressSignal( progress );
    if ( loadedResCounter.size() == targetNum ) {
        loadedResCounter.clear();
        activateGeomLoad();
    }
}

void SceneDependencyResolver::loadGeomResCount( HttpResourceCBSign _key ) {
    loadedResCounter.emplace_back( _key );
    float progress = ( static_cast<float>(loadedResCounter.size() + firstTierResourceCount()) /
                       static_cast<float>(totalResourceCount()));
    LOGRS("Progress: "<< progress);
    sgl.preloadProgressSignal( progress );
    if ( loadedResCounter.size() == secondTierResourceCount()) {
        loadedResCounter.clear();
        activatePostLoadInternal();
    }
}

void SceneDependencyResolver::activateGeomLoad() {
#define LGFUNC std::bind(&SceneDependencyResolver::loadGeomResCount, this, std::placeholders::_1)

    if ( secondTierResourceCount() == 0 ) {
        activatePostLoadInternal();
    } else {
        for ( const auto& r : Geoms() ) {
            sgl.load<Geom>( r, LGFUNC );
        }
        for ( const auto& r : UIs() ) {
            sgl.load<UIContainer>( r, LGFUNC );
        }
    }

}

void SceneDependencyResolver::resolve() {
#define LRFUNC std::bind(&SceneDependencyResolver::loadResCount, this, std::placeholders::_1)

    if ( firstTierResourceCount() == 0 ) {
        activateGeomLoad();
    } else {
        for ( const auto& r : Fonts()     ) sgl.load<Font>( r, LRFUNC );
        for ( const auto& r : RawImages() ) sgl.load<RawImage>( r, LRFUNC );
        for ( const auto& r : Materials() ) sgl.load<Material>( r, LRFUNC );
        for ( const auto& r : Profiles()  ) sgl.load<Profile>( r, LRFUNC );
        for ( const auto& r : Lights()  )   sgl.load<Light>( r, LRFUNC );
    }
}

SceneDependencyResolver::SceneDependencyResolver( SceneGraph& sg ) : sgl( sg ) {}

void SceneDependencyResolver::activateFinalLoadInternal() {
    if ( activatePostLoad ) activatePostLoad();
}

void SceneDependencyResolver::activatePostLoadInternal() {
    activateFinalLoadInternal();
}

void SceneDependencyResolver::addResolveCallback( std::function<void()> _activatePostLoad ) {
    activatePostLoad = std::move(_activatePostLoad);
}

void SceneDependencyResolver::addDeps( const DependencyList& _deps ) {
    for ( const auto& d : _deps ) {
        addDep(d);
    }
}

void SceneDependencyResolver::addDep( const StringsPair& d ) {
    if ( d.first == ResourceGroup::Material ){
        addMaterial( d.second );
    } else if ( d.first == ResourceGroup::VData ){
//            addVData( d.second );
    } else if ( d.first == ResourceGroup::Geom ){
        addGeom( d.second );
    } else if ( d.first == ResourceGroup::Image ){
        addRawImage( d.second );
    } else if ( d.first == ResourceGroup::Profile ){
        addProfile( d.second );
    } else if ( d.first == ResourceGroup::Color ){
        addMaterialColor( d.second );
    } else if ( d.first == ResourceGroup::Font ){
        addFont( d.second );
    } else if ( d.first == ResourceGroup::UI ){
        addUI( d.second );
    } else if ( d.first == ResourceGroup::Light ){
        addLight( d.second );
    }else if ( d.first == ResourceGroup::CameraRig ){
//            addC( d.second );
    }
}