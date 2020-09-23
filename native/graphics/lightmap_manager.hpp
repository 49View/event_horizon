//
// Created by Dado on 2019-05-17.
//

#pragma once

#include <core/lightmap_exchange_format.h>

class Renderer;
class SceneGraph;

namespace LightmapManager {
    void bakeLightmaps( SceneGraph& sg, Renderer& rr );
}
