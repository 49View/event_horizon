//
// Created by Dado on 2019-07-12.
//

#pragma once

#include <vector>
#include <string>
#include <core/http/webclient.h>
#include <core/app_data.hpp>

class SceneGraph;

class ScenePreLoader {
public:
    explicit ScenePreLoader( SceneGraph& sg );
    void loadSceneEntities();
protected:
    virtual void loadResCount( HttpResouceCBSign _key );
    virtual void loadGeomResCount( HttpResouceCBSign _key );
    virtual void activateGeomLoad();
    virtual void activatePostLoad() = 0;

protected:
    SceneGraph& sgl;
    std::vector<std::string> loadedResCounter;
    AppData appData;
};



