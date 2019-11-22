//
// Created by Dado on 2019-05-17.
//

#pragma once

#include <core/lightmap_exchange_format.h>

class Renderer;

namespace LightmapManager {
    int initScene( scene_t *scene, Renderer& rr );
    int bake( scene_t *scene, Renderer& rr );
    void apply( scene_t &scene, Renderer& rr );
}
