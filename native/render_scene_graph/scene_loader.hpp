//
// Created by Dado on 2019-07-12.
//

#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <core/app_data.hpp>

class SceneGraph;
class RenderOrchestrator;

class ScenePreLoader {
public:
    explicit ScenePreLoader( SceneGraph& sg, RenderOrchestrator& _rsg );
    void loadSceneEntities();
    const SerializableContainer& getPreloadCustomSerializableContainer( const std::string& _key, size_t _index );
protected:
    virtual void loadResCount( HttpResouceCBSign _key );
    virtual void loadGeomResCount( HttpResouceCBSign _key );
    virtual void loadCustomResCount( HttpResouceCBSign _key );
    virtual void activateGeomLoad();
    void activatePostLoadInternal();
    void activateFinalLoadInternal();
    virtual void activatePostLoad() = 0;

protected:
    SceneGraph& sgl;
    RenderOrchestrator& rsgl;
    std::vector<std::string> loadedResCounter;
    std::unordered_map<std::string, std::vector<SerializableContainer> > customResources;
    AppData appData;
};



