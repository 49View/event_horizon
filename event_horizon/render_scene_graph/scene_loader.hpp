//
// Created by Dado on 2019-07-12.
//

#pragma once

#include <vector>
#include <string>
#include <core/http/webclient.h>

class SceneGraph;

class SceneLoader {
public:
    explicit SceneLoader( SceneGraph& sg );
    void loadSceneEntities();
protected:
    virtual void loadResCount( HttpResouceCBSign _key );
    virtual void loadGeomResCount( HttpResouceCBSign _key );
    virtual void activateGeomLoad();
    virtual void activatePostLoad() = 0;

protected:
    SceneGraph& sgl;
    std::vector<std::string> loadedResCounter;

    std::vector<std::string> geomResourceLoad;
    std::vector<std::string> materialColorResourceLoad;
    std::vector<std::string> materialResourceLoad;
    std::vector<std::string> profileResourceLoad;
    std::vector<std::string> rawImageResourceLoad;
    std::vector<std::string> fontResourceLoad;
};



