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

struct vertex_t {
    float p[3];
    float t[2];
} ;

struct scene_t
{
    unsigned int program;
    int u_lightmap;
    int u_projection;
    int u_view;

    unsigned int lightmap;
    int w, h;

    unsigned int vao, vbo, ibo;
    vertex_t *vertices;
    unsigned short *indices;
    uint32_t *xrefs;
    unsigned int vertexCount, indexCount;

    std::unordered_map<uint32_t, HashIndexPairU32> unchart;
    std::unordered_map<std::string, std::shared_ptr<VData>> ggLImap;
};
