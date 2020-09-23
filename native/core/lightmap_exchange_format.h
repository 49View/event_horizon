//
// Created by Dado on 07/09/2018.
//

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <unordered_set>
#include <core/soa_utils.h>
#include <core/htypes_shared.hpp>

class VData;

struct LightmapVertexExchanger {
    float p[3];
    float t[2];
} ;

struct LightmapSceneExchanger
{
    unsigned int program = 0;
    int u_lightmap = 0;
    int u_projection = 0;
    int u_view = 0;

    unsigned int lightmap = 0;
    int w = 0;
    int h = 0;

    unsigned int vao = 0;
    unsigned int vbo = 0;
    unsigned int ibo = 0;
    LightmapVertexExchanger *vertices = nullptr;
    unsigned short *indices = nullptr;
    unsigned int vertexCount = 0;
    unsigned int indexCount = 0;

//    uint32_t *xrefs;
//    std::unordered_map<uint32_t, HashIndexPairU32> unchart;
//    std::unordered_map<std::string, std::shared_ptr<VData>> ggLImap;
};
