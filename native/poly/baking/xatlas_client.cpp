//
// Created by Dado on 2019-05-17.
//

#include "xatlas_client.hpp"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <poly/baking/xatlas.h>
#include <poly/baking/xatlas_dump.hpp>
#include <core/lightmap_exchange_format.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4201)
#endif
#include <poly/converters/obj/tiny_obj_loader.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#ifdef _MSC_VER
#define FOPEN(_file, _filename, _mode) { if (fopen_s(&_file, _filename, _mode) != 0) _file = NULL; }
#define STRICMP _stricmp
#else
#define FOPEN(_file, _filename, _mode) _file = fopen(_filename, _mode)
#include <strings.h>

#define STRICMP strcasecmp
#endif

#include <core/v_data.hpp>
#include <core/recursive_transformation.hpp>
#include <core/geom.hpp>
#include <poly/scene_graph.h>

static bool s_verbose = false;

class Stopwatch
{
public:
    Stopwatch() { reset(); }
    void reset() { m_start = clock(); }
    double elapsed() const { return (clock() - m_start) * 1000.0 / CLOCKS_PER_SEC; }
private:
    clock_t m_start;
};

static int Print(const char *format, ...)
{
    va_list arg;
    va_start(arg, format);
    printf("\r"); // Clear progress text (PrintProgress).
    const int result = vprintf(format, arg);
    va_end(arg);
    return result;
}

// May be called from any thread.
static void PrintProgress(const char *name, const char *indent1, const char *indent2, int progress, Stopwatch *stopwatch)
{
    if (s_verbose)
        return;
    static std::mutex progressMutex;
    std::unique_lock<std::mutex> lock(progressMutex);
    if (progress == 0)
        stopwatch->reset();
    printf("\r%s%s [", indent1, name);
    for (int i = 0; i < 10; i++)
        printf(progress / ((i + 1) * 10) ? "*" : " ");
    printf("] %d%%", progress);
    fflush(stdout);
    if (progress == 100)
        printf("\n%s%.2f seconds (%g ms) elapsed\n", indent2, stopwatch->elapsed() / 1000.0, stopwatch->elapsed());
}

static bool ProgressCallback(xatlas::ProgressCategory::Enum category, int progress, void *userData)
{
    Stopwatch *stopwatch = (Stopwatch *)userData;
    PrintProgress(xatlas::StringForEnum(category), "   ", "      ", progress, stopwatch);
    return true;
}

//static void ProgressCallback(xatlas::ProgressCategory::Enum category, int progress, void *userData)
//{
//    Stopwatch *stopwatch = (Stopwatch *)userData;
//    PrintProgress(xatlas::StringForEnum(category), "   ", "      ", progress, stopwatch);
//}

void saveToObj( xatlas::Atlas *atlas, std::vector<tinyobj::shape_t>& shapes ) {
    // Write meshes.
    char filename[256];
    snprintf(filename, sizeof(filename), "output.obj");
    printf("Writing '%s'...\n", filename);
    FILE *file;
    FOPEN(file, filename, "w");
    if (file) {
        uint32_t firstVertex = 0;
        for (uint32_t i = 0; i < atlas->meshCount; i++) {
            const xatlas::Mesh &mesh = atlas->meshes[i];
            for (uint32_t v = 0; v < mesh.vertexCount; v++) {
                const xatlas::Vertex &vertex = mesh.vertexArray[v];
                const float *pos = &shapes[i].mesh.positions[vertex.xref * 3];
                fprintf(file, "v %g %g %g\n", pos[0], pos[1], pos[2]);
                if (!shapes[i].mesh.normals.empty()) {
                    const float *normal = &shapes[i].mesh.normals[vertex.xref * 3];
                    fprintf(file, "vn %g %g %g\n", normal[0], normal[1], normal[2]);
                }
                fprintf(file, "vt %g %g\n", vertex.uv[0] / atlas->width, vertex.uv[1] / atlas->height);
            }
            fprintf(file, "o mesh%03u\n", i);
            fprintf(file, "s off\n");
            for (uint32_t f = 0; f < mesh.indexCount; f += 3) {
                fprintf(file, "f ");
                for (uint32_t j = 0; j < 3; j++) {
                    const uint32_t index = firstVertex + mesh.indexArray[f + j] + 1; // 1-indexed
                    fprintf(file, "%d/%d/%d%c", index, index, index, j == 2 ? '\n' : ' ');
                }
            }
            fprintf(file, "g charts\n");
            for (uint32_t c = 0; c < mesh.chartCount; c++) {
                const xatlas::Chart *chart = &mesh.chartArray[c];
                fprintf(file, "o chart%04u\n", c);
                fprintf(file, "s off\n");
                for (uint32_t f = 0; f < chart->faceCount; f += 3) {
                    fprintf(file, "f ");
                    for (uint32_t j = 0; j < 3; j++) {
                        const uint32_t index = firstVertex + chart->faceArray[f + j] + 1; // 1-indexed
                        fprintf(file, "%d/%d/%d%c", index, index, index, j == 2 ? '\n' : ' ');
                    }
                }
            }
            firstVertex += mesh.vertexCount;
        }
        fclose(file);
    }
}

void saveToSceneT( xatlas::Atlas *atlas, std::vector<tinyobj::shape_t>& shapes, scene_t* scene ) {

    // allocate memory
    scene->vertexCount = atlas->meshes[0].vertexCount;
    scene->vertices = (vertex_t *)calloc(scene->vertexCount, sizeof(vertex_t));
    scene->indexCount = atlas->meshes[0].indexCount;
    scene->indices = (unsigned short *)calloc(scene->indexCount, sizeof(unsigned short));
    scene->xrefs   = (uint32_t *)calloc(scene->indexCount, sizeof(uint32_t));

    uint32_t firstVertex = 0;
    for (uint32_t i = 0; i < atlas->meshCount; i++) {
        const xatlas::Mesh &mesh = atlas->meshes[i];
        for (uint32_t v = 0; v < mesh.vertexCount; v++) {
            const xatlas::Vertex &vertex = mesh.vertexArray[v];
            const float *pos = &shapes[i].mesh.positions[vertex.xref * 3];
            size_t voff = (v * sizeof(vertex_t));
            float uvs[2];
            uvs[0] = vertex.uv[0] / atlas->width;
            uvs[1] = vertex.uv[1] / atlas->height;
            memcpy( (char*)scene->vertices + voff + 0, pos, sizeof(float) * 3 );
            memcpy( (char*)scene->vertices + voff + sizeof(float) * 3, uvs, sizeof(float) * 2 );
            scene->xrefs[v] = vertex.xref;
        }
        for (uint32_t f = 0; f < mesh.indexCount; f += 3) {
            for (uint32_t j = 0; j < 3; j++) {
                const uint32_t index = firstVertex + mesh.indexArray[f + j]; // add +1 for obj file indexed
                scene->indices[f+j] = index;
            }
        }
        firstVertex += mesh.vertexCount;
    }
}

void saveToSceneT( SceneGraph& sg, xatlas::Atlas *atlas, const float* positions, scene_t* scene ) {

    // allocate memory
    scene->vertexCount = atlas->meshes[0].vertexCount;
    scene->vertices = (vertex_t *)calloc(scene->vertexCount, sizeof(vertex_t));
    scene->indexCount = atlas->meshes[0].indexCount;
    scene->indices = (unsigned short *)calloc(scene->indexCount, sizeof(unsigned short));

    uint32_t firstVertex = 0;
    for (uint32_t i = 0; i < atlas->meshCount; i++) {
        const xatlas::Mesh &mesh = atlas->meshes[i];
        for (uint32_t v = 0; v < mesh.vertexCount; v++) {
            const xatlas::Vertex &vertex = mesh.vertexArray[v];
            const float *pos        = &positions[vertex.xref * 3];

            size_t voff = (v * sizeof(vertex_t));
            float uvs[2];
            uvs[0] = vertex.uv[0] / atlas->width;
            uvs[1] = vertex.uv[1] / atlas->height;
            int strideOff = 0;
            memcpy( (char*)scene->vertices + voff + strideOff, pos, sizeof(float) * 3 );
            strideOff +=sizeof(float) * 3;
            memcpy( (char*)scene->vertices + voff + strideOff, uvs, sizeof(float) * 2 );

//            auto lNode = sg.getNode( scene->unchart[vertex.xref].hash );
//            auto gref = lNode->DataRef();
//            auto vdata = sg.get<VData>(gref.vData);
//            vdata->setVUV2s( scene->unchart[vertex.xref].index, V2f{uvs[0], uvs[1]} );
        }
        for (uint32_t f = 0; f < mesh.indexCount; f++) {
            const uint32_t index = firstVertex + mesh.indexArray[f]; // add +1 for obj file indexed
            scene->indices[f] = index;
        }
        firstVertex += mesh.indexCount;
    }
}

void mapXAtlasScene( SceneGraph& sg, GeomSP gg, scene_t* scene, size_t& totalVerts, size_t& totalIndices ) {
    for ( const auto& dd : gg->DataV() ) {
        auto vData = sg.VL().get(dd.vData);
        totalVerts += vData->numVerts();
        totalIndices += vData->numIndices();
//        scene->ggLImap[gg->UUiD()] = sg.VL().get(gg->Data(0).vData);
        scene->ggLImap[gg->UUiD() + dd.vData] = vData;
    }

    for ( const auto& c : gg->Children() ) {
        mapXAtlasScene( sg, c, scene, totalVerts, totalIndices);
    }
}

void flattenXAtlasScene( SceneGraph& sg, GeomSP gg, scene_t* scene,
                         size_t& vcurrUnchartOffset, size_t& icurrUnchartOffset,
                         xatlas::MeshDecl& meshDecl,
                         char* posData, char* uvData, char* normalData, char* indicesData ) {

    for ( const auto& dd : gg->DataV() ) {
        auto vData = sg.VL().get(dd.vData);
        auto mat = gg->getLocalHierTransform();
        auto ghash = gg->UUiD() + dd.vData;

        vData->flattenStride(posData + vcurrUnchartOffset * meshDecl.vertexPositionStride, 0, mat.get());
        vData->flattenStride(uvData + vcurrUnchartOffset * meshDecl.vertexUvStride, 1, mat.get());
        vData->flattenStride(normalData + vcurrUnchartOffset * meshDecl.vertexNormalStride, 3, mat.get());
        vData->mapIndices(indicesData, icurrUnchartOffset, vcurrUnchartOffset, ghash, scene->unchart);

        vcurrUnchartOffset += vData->numVerts();
        icurrUnchartOffset += vData->numIndices();
    }

    for ( const auto& c : gg->Children() ) {
        flattenXAtlasScene( sg, c, scene, vcurrUnchartOffset, icurrUnchartOffset, meshDecl, posData, uvData, normalData, indicesData);
    }

}


int xatlasParametrize( SceneGraph& sg, const NodeGraphContainer& nodes, scene_t* scene ) {

    // Create atlas.
    xatlas::SetPrint(Print, false);
    xatlas::Atlas *atlas = xatlas::Create();
    Stopwatch globalStopwatch, stopwatch;
    xatlas::SetProgressCallback(atlas, ProgressCallback, &stopwatch);

    // Create single flattened mesh
    xatlas::MeshDecl meshDecl;// = saoToXMesh(source);
    size_t totalVerts = 0;
    size_t totalIndices = 0;

    for ( const auto& [k, gg] : nodes ) {
        mapXAtlasScene( sg, gg, scene, totalVerts, totalIndices);
    }

    meshDecl.vertexCount = totalVerts;//source->numVerts();// (int)objMesh.positions.size() / 3;
    meshDecl.vertexPositionStride = sizeof(float) * 3;
    auto totalPosSize = totalVerts*meshDecl.vertexPositionStride;
    auto posData = new char[totalPosSize];
    meshDecl.vertexPositionData = posData;
    meshDecl.vertexNormalStride = sizeof(float) * 3;
    auto normalData = new char[totalVerts*meshDecl.vertexNormalStride];
    meshDecl.vertexNormalData = normalData;
    meshDecl.vertexUvStride = sizeof(float) * 2;
    auto uvData = new char[totalVerts*meshDecl.vertexUvStride];
    meshDecl.vertexUvData = uvData;

    meshDecl.indexCount = totalIndices;//(int)objMesh.indices.size();
    auto indicesData = new char[totalIndices*sizeof(uint32_t)]; //objMesh.indices.data();
    meshDecl.indexData = indicesData;
    meshDecl.indexFormat = xatlas::IndexFormat::UInt32;

    size_t vcurrUnchartOffset = 0;
    size_t icurrUnchartOffset = 0;
    for ( const auto& [k, gg] : nodes ) {
        flattenXAtlasScene( sg, gg, scene, vcurrUnchartOffset, icurrUnchartOffset, meshDecl, posData, uvData, normalData, indicesData);
    }

    xatlas::AddMeshError::Enum error = xatlas::AddMesh(atlas, meshDecl);
    if (error != xatlas::AddMeshError::Success) {
        LOGR("\rError adding saoToXMesh: %s\n", xatlas::StringForEnum(error));
    }

    // Generate atlas.
    printf("Generating atlas\n");
    xatlas::PackOptions packerOptions;
    packerOptions.resolution = 128;
    xatlas::Generate(atlas, xatlas::ChartOptions(), packerOptions);
    printf("   %d charts\n", atlas->chartCount);
    printf("   %d atlases\n", atlas->atlasCount);
    for (uint32_t i = 0; i < atlas->atlasCount; i++)
        printf("      %d: %0.2f%% utilization\n", i, atlas->utilization[i] * 100.0f);
    printf("   %ux%u resolution\n", atlas->width, atlas->height);
    uint32_t totalVertices = 0;
    uint32_t totalFaces = 0;
    for (uint32_t i = 0; i < atlas->meshCount; i++) {
        const xatlas::Mesh &mesh = atlas->meshes[i];
        totalVertices += mesh.vertexCount;
        totalFaces += mesh.indexCount / 3;
    }
    printf("   %u total vertices\n", totalVertices);
    printf("   %u total triangles\n", totalFaces);
    printf("%.2f seconds (%g ms) elapsed total\n", globalStopwatch.elapsed() / 1000.0, globalStopwatch.elapsed());

    saveToSceneT( sg, atlas, (float*)posData, scene );

#ifndef ANDROID
    xatlasDump(atlas);
#endif
    // Cleanup.
//    xatlas::Destroy(atlas);
    return 0;
}
