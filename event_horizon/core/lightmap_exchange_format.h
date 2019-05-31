//
// Created by Dado on 07/09/2018.
//

#pragma once

#include <vector>
#include <string>
#include <core/soa_utils.h>

struct vertex_t {
    float p[3];
    float t[2];

    PUUNTBC orig;
} ;

struct VertexOffsetScene {
    VertexOffsetScene( const std::string& uuid, size_t offset, size_t size ) : uuid( uuid ), offset( offset ),
                                                                               size( size ) {}

    std::string uuid;
    size_t offset;
    size_t size;
};

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

    std::vector<VertexOffsetScene> unchart;
};
