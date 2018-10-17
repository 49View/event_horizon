//
// Created by Dado on 07/09/2018.
//

#pragma once

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
    unsigned int vertexCount, indexCount;
} ;
