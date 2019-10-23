//
// Created by Dado on 2019-07-12.
//

#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <core/http/webclient.h>
#include <core/app_data.hpp>

class SceneGraph;

class SceneDependencyResolver {
public:
    explicit SceneDependencyResolver( SceneGraph& sg );

    void resolve();
    void addResolveCallback( std::function<void()> _activatePostLoad );
    void addDeps( const DependencyList& _deps );
    void addDep( const StringsPair& d );
    void addGeom         ( CResourceRef _value ) { add( S::GEOMS, _value); }
    void addMaterialColor( CResourceRef _value ) { add( S::COLORS, _value); }
    void addMaterial     ( CResourceRef _value ) { add( S::MATERIALS, _value); }
    void addProfile      ( CResourceRef _value ) { add( S::PROFILES, _value); }
    void addRawImage     ( CResourceRef _value ) { add( S::IMAGES, _value); }
    void addFont         ( CResourceRef _value ) { add( S::FONTS, _value); }
    void addUI           ( CResourceRef _value ) { add( S::UIS, _value); }

protected:

    void add( const std::string& _key, const std::string& _value ) {
        for ( auto& ent : entities ) {
            if ( ent.key == _key ) {
                ent.value.emplace_back(_value);
                return;
            }
        }
        // Not found and active key yet, create one
        entities.emplace_back( AppEntities{_key, {_value}} );
    }

    [[nodiscard]] size_t firstTierResourceCount() const {
        size_t ret = 0;
        for ( const auto& ent : entities ) {
            if ( ent.key == S::COLORS ) {
                ret += ent.value.size();
            }
            if ( ent.key == S::MATERIALS ) {
                ret += ent.value.size();
            }
            if ( ent.key == S::PROFILES ) {
                ret += ent.value.size();
            }
            if ( ent.key == S::IMAGES ) {
                ret += ent.value.size();
            }
            if ( ent.key == S::FONTS ) {
                ret += ent.value.size();
            }
        }
        return ret;
    };

    [[nodiscard]] size_t secondTierResourceCount() const {
        size_t ret = 0;
        for ( const auto& ent : entities ) {
            if ( ent.key == S::GEOMS ) {
                ret += ent.value.size();
            }
            if ( ent.key == S::UIS ) {
                ret += ent.value.size();
            }
        }
        return ret;
    }

    [[nodiscard]] size_t customTierResourceCount() const {
        size_t ret = 0;
        for ( const auto& ent : entities ) {
            if ( ent.key != S::COLORS && ent.key != S::MATERIALS && ent.key != S::PROFILES && ent.key != S::IMAGES &&
                 ent.key != S::FONTS && ent.key != S::GEOMS && ent.key != S::UIS) {
                ret += ent.value.size();
            }
        }
        return ret;
    };

    [[nodiscard]] size_t totalResourceCount() const {
        return firstTierResourceCount() + secondTierResourceCount() + customTierResourceCount();
    }

    [[nodiscard]] std::vector<std::string> Geoms() {
        for ( const auto& ent : entities ) {
            if ( ent.key == S::GEOMS ) { return ent.value; }
        }
        return {};
    }
    [[nodiscard]] std::vector<std::string> UIs() {
        for ( const auto& ent : entities ) {
            if ( ent.key == S::UIS ) { return ent.value; }
        }
        return {};
    }
    [[nodiscard]] std::vector<std::string> Materials() {
        for ( const auto& ent : entities ) {
            if ( ent.key == S::MATERIALS ) { return ent.value; }
        }
        return {};
    }
    [[nodiscard]] std::vector<std::string> Profiles() {
        for ( const auto& ent : entities ) {
            if ( ent.key == S::PROFILES ) { return ent.value; }
        }
        return {};
    }
    [[nodiscard]] std::vector<std::string> RawImages(){
        for ( const auto& ent : entities ) {
            if ( ent.key == S::IMAGES ) { return ent.value; }
        }
        return {};
    }
    [[nodiscard]] std::vector<std::string> Fonts(){
        for ( const auto& ent : entities ) {
            if ( ent.key == S::FONTS ) { return ent.value; }
        }
        return {};
    }

    void loadResCount( HttpResouceCBSign _key );
    void loadGeomResCount( HttpResouceCBSign _key );
    void activateGeomLoad();
    void activatePostLoadInternal();
    void activateFinalLoadInternal();
    std::function<void()> activatePostLoad = nullptr;

protected:
    SceneGraph& sgl;
    std::vector<std::string> loadedResCounter;

    AppMaterialsRemapping    matRemapping;
    std::vector<AppEntities> entities;
};



