#pragma once

#include <vector>
#include <memory>
#include "core/math/vector4f.h"
#include "mkl/flat_matrix.h"
#include "core/chunks.h"
#include "ray_tracing/shapes.h"

struct Triangle;
struct ChunkInfo;

class FlatMatrix;
class HierGeom;
class GeometryOffset;
class SHRelightCollection;

class MappingGeomExtensions {
public:
    MappingGeomExtensions();
    void initialize( std::vector<std::shared_ptr<HierGeom>>& sceneGeoms );
    void ensureInitialized( std::vector<std::shared_ptr<HierGeom>>& sceneGeoms );
    void update( const FlatMatrix& vertPerFuncMatrix, const ChunkInfo& chunk, const int& numberOfSHFunctions ) const;
    void update( float *sh_buffer, int numberOfSHFunctions ) const;
    FlatMatrix getNormalsMatrixForChunck( const ChunkInfo& chunk ) const;
    void finalize();
    void fillSHValues( float *sh_buffer, const int& numberOfSHFunctions ) const;
    void createAlbedos( float k );

public:
    std::vector<Vector4f> vertices;
    std::vector<Vector4f> verticesPlus;
    std::vector<Triangle> triangles;
    std::vector<Vector3f> albedos;
    std::vector<Vector3f> normals;

private:
    size_t getGeometryIndexAt( size_t t ) const;

private:
    std::vector<std::shared_ptr<GeometryOffset>> geometryOffsets;
    std::shared_ptr<SHRelightCollection> shRelightCollection;
    bool isInitialized = false;
};