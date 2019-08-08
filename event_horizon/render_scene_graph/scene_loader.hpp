//
// Created by Dado on 2019-07-12.
//

#pragma once

#include <vector>
#include <string>
#include <core/http/webclient.h>
#include <core/app_data.hpp>

class SceneGraph;
class RenderOrchestrator;

class ScenePreLoader {
public:
    explicit ScenePreLoader( SceneGraph& sg, RenderOrchestrator& _rsg );
    void loadSceneEntities();
protected:
    virtual void loadResCount( HttpResouceCBSign _key );
    virtual void loadGeomResCount( HttpResouceCBSign _key );
    virtual void activateGeomLoad();
    void activatePostLoadInternal();
    virtual void activatePostLoad() = 0;

protected:
    SceneGraph& sgl;
    RenderOrchestrator& rsgl;
    std::vector<std::string> loadedResCounter;
    AppData appData;
};



