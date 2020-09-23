//
// Created by Dado on 2019-05-17.
//

#include "xatlas_client.hpp"

#include <cstdarg>
#include <cstdio>
#include <ctime>
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
    [[nodiscard]] double elapsed() const { return (clock() - m_start) * 1000.0 / CLOCKS_PER_SEC; }
private:
    clock_t m_start{};
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
    auto *stopwatch = (Stopwatch *)userData;
    PrintProgress(xatlas::StringForEnum(category), "   ", "      ", progress, stopwatch);
    return true;
}

void saveToSceneT( SceneGraph& sg, xatlas::Atlas *atlas, XAtlasExchangeMap& atlasMeshMapping ) {

    for (uint32_t i = 0; i < atlas->meshCount; i++) {
        auto vData = atlasMeshMapping[i];
        const xatlas::Mesh &mesh = atlas->meshes[i];
        for (uint32_t v = 0; v < mesh.vertexCount; v++) {
            const xatlas::Vertex &vertex = mesh.vertexArray[v];
            V2f lightmapUV{vertex.uv[0] / atlas->width, vertex.uv[1] / atlas->height};
            vData->setUV2At(vertex.xref, lightmapUV);
//            LOGR("xref %d - vt %g %g", vertex.xref, vertex.uv[0] / atlas->width, vertex.uv[1] / atlas->height);
        }
    }
}

void flattenXAtlasScene( SceneGraph& sg, GeomSP gg, xatlas::Atlas *atlas, std::size_t& atmI, XAtlasExchangeMap& atlasMeshMapping ) {

    for ( const auto& dd : gg->DataV() ) {
        auto vData = sg.VL().get(dd.vData);
        auto mat = gg->getLocalHierTransform();

        atlasMeshMapping[atmI++] = vData.get();

        xatlas::MeshDecl meshDecl;// = saoToXMesh(source);
        size_t totalVerts = vData->numVerts();
        size_t totalIndices = vData->numIndices();

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

        vData->flattenStride(posData, 0, mat.get());
        vData->flattenStride(uvData, 1, mat.get());
        vData->flattenStride(normalData, 3, mat.get());
        vData->mapIndices(indicesData, 0, 0);

        xatlas::AddMeshError::Enum error = xatlas::AddMesh(atlas, meshDecl);
        if (error != xatlas::AddMeshError::Success) {
            LOGR("\rError adding saoToXMesh: %s\n", xatlas::StringForEnum(error));
        }
    }

    for ( const auto& c : gg->Children() ) {
        flattenXAtlasScene( sg, c, atlas, atmI, atlasMeshMapping );
    }

}


int xAtlasParametrize( SceneGraph& sg, const NodeGraphContainer& nodes ) {

    // Create atlas.
    xatlas::SetPrint(Print, false);
    xatlas::Atlas *atlas = xatlas::Create();
    Stopwatch globalStopwatch, stopwatch;
    xatlas::SetProgressCallback(atlas, ProgressCallback, &stopwatch);

    XAtlasExchangeMap atlasMeshMapping{};
    size_t atmI = 0;
    for ( const auto& [k, gg] : nodes ) {
        flattenXAtlasScene( sg, gg, atlas, atmI, atlasMeshMapping );
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

    saveToSceneT( sg, atlas, atlasMeshMapping );

#ifndef ANDROID
    xatlasDump(atlas);
#endif
    // Cleanup.
//    xatlas::Destroy(atlas);
    return 0;
}
