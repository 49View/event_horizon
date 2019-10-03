//
// Created by Dado on 2019-04-10.
//

#pragma once

#include <memory>
#include <vector>
#include <core/htypes_shared.hpp>

class Vector2f;
class Vector3f;
class QuadStripUV;
struct GeomMappingData;
class QuadVertices3;
class QuadVertices2;
class VData;
using VDataSP = std::shared_ptr<VData>;

enum PullFlags : uint32_t {
    Sides = 1 << 0,
    Tops = 1 << 1,
    All = 0xffffffff,
};

std::vector<Vector2f> utilGenerateFlatRect( const Vector2f& size, WindingOrderT wo, PivotPointPosition ppp );

namespace MappingServices {
    void doNotScaleMapping( GeomMappingData& m );
    void resetMapping( GeomMappingData& m, uint64_t arraySize );
    void resetWrapMapping( GeomMappingData& m, const std::vector<float>& yWrapArray );
    void propagateWrapMapping( GeomMappingData& m, const VData* source );
    void updateWrapMapping( GeomMappingData& m, Vector3f vs[4], Vector2f vtcs[4], uint64_t mi, uint64_t size );
    template <typename T>
    void updatePullMapping( GeomMappingData& m, const std::array<T, 4>& vs, std::array<Vector2f, 4>& vtcs );
    void planarMapping(GeomMappingData& m, const Vector3f& normal, const Vector3f vs[], Vector2f vtcs[], int numVerts);
    void calcMirrorUVs( GeomMappingData& m, Vector2f* uvs );
}

namespace PolyServices {
    float areaOf( const std::vector<Vector2f>& vtri );

    void addQuad( VDataSP vdata, const std::array<Vector3f, 4>& vs, const std::array<Vector2f, 4>& vts,
                  const std::array<Vector3f, 4>& vns, GeomMappingData& m );

    void addFlatPoly( VDataSP vdata, const std::vector<Vector3f>& points, WindingOrderT wo, GeomMappingData& m );

    void addFlatPoly( VDataSP vdata, size_t vsize, const Vector3f *verts, const Vector3f& normal, GeomMappingData& m,
                      bool reverseIfTriangulated = false );
    void addFlatPoly( VDataSP vdata,size_t vsize, const std::vector<Vector2f>& verts, float z, const Vector3f& normal,
                      GeomMappingData& m, bool reverseIfTriangulated = false );
    void addFlatPoly( VDataSP vdata,const std::array<Vector3f, 4>& verts, const Vector3f& normal, GeomMappingData& m,
                      bool reverseIfTriangulated = false );
    void addFlatPolyTriangulated( VDataSP vdata,size_t vsize, const Vector3f *verts, const Vector3f& normal,
                                  GeomMappingData& m, bool reverse = false );
    void addFlatPolyTriangulated( VDataSP vdata,size_t vsize, const std::vector<Vector2f>& verts, float z,
                                  const Vector3f& normal, GeomMappingData& m, bool reverse = false );
    void addFlatPolyWithMapping( VDataSP vdata,size_t vsize, const Vector3f *vs, const Vector2f *vts,
                                 GeomMappingData& m, const Vector3f* vn = nullptr );
    void addFlatPolyWithMapping( VDataSP vdata, const QuadStripUV& qs, GeomMappingData& m );
    void pull( VDataSP vdata, const std::vector<Vector2f>& verts, const Vector3f& normal, float height, float zOffset,
               GeomMappingData& m, PullFlags pullFlags = PullFlags::All );
    void pull( VDataSP vdata, const std::vector<Vector3f>& verts, const Vector3f& normal, float height,
               GeomMappingData& m, PullFlags pullFlags = PullFlags::All );
}
