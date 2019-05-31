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

static bool s_verbose = false;

class Stopwatch
{
public:
    Stopwatch() { reset(); }
    void reset() { m_start = clock(); }
    double elapsed() const { return (clock() - m_start) * 1000.0 / CLOCKS_PER_SEC; }
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

static void PrintProgress(const char *name, const char *indent1, const char *indent2, int progress, Stopwatch *stopwatch)
{
    if (s_verbose)
        return;
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

static void ProgressCallback(xatlas::ProgressCategory::Enum category, int progress, void *userData)
{
    Stopwatch *stopwatch = (Stopwatch *)userData;
    PrintProgress(xatlas::StringForEnum(category), "   ", "      ", progress, stopwatch);
}

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
                for (uint32_t f = 0; f < chart->indexCount; f += 3) {
                    fprintf(file, "f ");
                    for (uint32_t j = 0; j < 3; j++) {
                        const uint32_t index = firstVertex + chart->indexArray[f + j] + 1; // 1-indexed
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
//            const float *pos = &shapes[i].mesh.positions[v * 3];
            size_t voff = (v * sizeof(vertex_t));
            float uvs[2];
            uvs[0] = vertex.uv[0] / atlas->width;
            uvs[1] = vertex.uv[1] / atlas->height;
            memcpy( (char*)scene->vertices + voff + 0, pos, sizeof(float) * 3 );
            memcpy( (char*)scene->vertices + voff + sizeof(float) * 3, uvs, sizeof(float) * 2 );
            scene->xrefs[v] = vertex.xref;
//            fprintf(file, "v %g %g %g\n", pos[0], pos[1], pos[2]);
//            if (!shapes[i].mesh.normals.empty()) {
//                const float *normal = &shapes[i].mesh.normals[vertex.xref * 3];
//                fprintf(file, "vn %g %g %g\n", normal[0], normal[1], normal[2]);
//            }
//            fprintf(file, "vt %g %g\n", vertex.uv[0] / atlas->width, vertex.uv[1] / atlas->height);
        }
//        fprintf(file, "o mesh%03u\n", i);
//        fprintf(file, "s off\n");
        for (uint32_t f = 0; f < mesh.indexCount; f += 3) {
//            fprintf(file, "f ");
            for (uint32_t j = 0; j < 3; j++) {
                const uint32_t index = firstVertex + mesh.indexArray[f + j]; // add +1 for obj file indexed
                scene->indices[f+j] = index;
//                fprintf(file, "%d/%d/%d%c", index, index, index, j == 2 ? '\n' : ' ');
            }
        }
//        fprintf(file, "g charts\n");
//        for (uint32_t c = 0; c < mesh.chartCount; c++) {
//            const xatlas::Chart *chart = &mesh.chartArray[c];
//            fprintf(file, "o chart%04u\n", c);
//            fprintf(file, "s off\n");
//            for (uint32_t f = 0; f < chart->indexCount; f += 3) {
//                fprintf(file, "f ");
//                for (uint32_t j = 0; j < 3; j++) {
//                    const uint32_t index = firstVertex + chart->indexArray[f + j] + 1; // 1-indexed
//                    fprintf(file, "%d/%d/%d%c", index, index, index, j == 2 ? '\n' : ' ');
//                }
//            }
//        }
        firstVertex += mesh.vertexCount;
    }

}

int xatlasParametrize( std::vector<tinyobj::shape_t>& shapes, scene_t* scene ) {

    // Create atlas.
    xatlas::SetPrint(Print, false);
    xatlas::Atlas *atlas = xatlas::Create();
    // Add meshes to atlas.
    Stopwatch globalStopwatch, stopwatch;
    int progress = 0;
    PrintProgress("Adding meshes", "", "   ", 0, &stopwatch);
    uint32_t totalVertices = 0, totalFaces = 0;
    for (int i = 0; i < (int)shapes.size(); i++) {
        const tinyobj::mesh_t &objMesh = shapes[i].mesh;
        xatlas::MeshDecl meshDecl;
        meshDecl.vertexCount = (int)objMesh.positions.size() / 3;
        meshDecl.vertexPositionData = objMesh.positions.data();
        meshDecl.vertexPositionStride = sizeof(float) * 3;
        if (!objMesh.normals.empty()) {
            meshDecl.vertexNormalData = objMesh.normals.data();
            meshDecl.vertexNormalStride = sizeof(float) * 3;
        }
        if (!objMesh.texcoords.empty()) {
            meshDecl.vertexUvData = objMesh.texcoords.data();
            meshDecl.vertexUvStride = sizeof(float) * 2;
        }
        meshDecl.indexCount = (int)objMesh.indices.size();
        meshDecl.indexData = objMesh.indices.data();
        meshDecl.indexFormat = xatlas::IndexFormat::UInt32;
        xatlas::AddMeshError::Enum error = xatlas::AddMesh(atlas, meshDecl);
        if (error != xatlas::AddMeshError::Success) {
            printf("\rError adding mesh %d '%s': %s\n", i, shapes[i].name.c_str(), xatlas::StringForEnum(error));
            return EXIT_FAILURE;
        }
        totalVertices += meshDecl.vertexCount;
        totalFaces += meshDecl.indexCount / 3;
        const int newProgress = int((i + 1) / (float)shapes.size() * 100.0f);
        if (newProgress != progress)
        {
            progress = newProgress;
            PrintProgress("Adding meshes", "", "   ", progress, &stopwatch);
        }
    }
    if (progress != 100)
        PrintProgress("Adding meshes", "", "   ", 100, &stopwatch);
    printf("   %u total vertices\n", totalVertices);
    printf("   %u total triangles\n", totalFaces);
    // Generate atlas.
    printf("Generating atlas\n");
    xatlas::PackOptions packerOptions;
    packerOptions.conservative = true;
    packerOptions.padding = 1;
    xatlas::Generate(atlas, xatlas::ChartOptions(), NULL, packerOptions, ProgressCallback, &stopwatch);
    printf("   %d charts\n", atlas->chartCount);
    printf("   %d atlases\n", atlas->atlasCount);
    for (uint32_t i = 0; i < atlas->atlasCount; i++)
        printf("      %d: %0.2f%% utilization\n", i, atlas->utilization[i] * 100.0f);
    printf("   %ux%u resolution\n", atlas->width, atlas->height);
    totalVertices = 0;
    totalFaces = 0;
    for (uint32_t i = 0; i < atlas->meshCount; i++) {
        const xatlas::Mesh &mesh = atlas->meshes[i];
        totalVertices += mesh.vertexCount;
        totalFaces += mesh.indexCount / 3;
    }
    printf("   %u total vertices\n", totalVertices);
    printf("   %u total triangles\n", totalFaces);
    printf("%.2f seconds (%g ms) elapsed total\n", globalStopwatch.elapsed() / 1000.0, globalStopwatch.elapsed());

    saveToSceneT( atlas, shapes, scene );

//    saveToObj( atlas, shapes );
    xatlasDump(atlas);

    // Cleanup.
    xatlas::Destroy(atlas);
    return 0;
}
