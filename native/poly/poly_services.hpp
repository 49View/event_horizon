//
// Created by Dado on 2019-04-10.
//

#pragma once

#include <memory>
#include <vector>
#include <core/htypes_shared.hpp>
#include <core/math/vector3f.h>

class QuadStripUV;
struct GeomMappingData;
class QuadVertices3;
class QuadVertices2;
class VData;
using VDataSP = std::shared_ptr<VData>;
namespace ClipperLib {
    struct IntPoint;
    typedef std::vector< IntPoint > Path;
    typedef std::vector< Path > Paths;
}

enum PullFlags : uint32_t {
    Sides = 1 << 0,
    Tops = 1 << 1,
    All = 0xffffffff,
};

struct PolyLineBase3d {
    explicit PolyLineBase3d( std::vector<Vector3f> verts ) : verts( std::move( verts )) {}
    explicit PolyLineBase3d( const Triangle3d& _verts ) {
        const auto& [v1,v2,v3] = _verts;
        verts.emplace_back(v1);
        verts.emplace_back(v2);
        verts.emplace_back(v3);
    }

    std::vector<Vector3f> verts;
};

struct PolyLineBase2d  {
    explicit PolyLineBase2d( std::vector<Vector2f> verts ) : verts( std::move( verts )) {}

    std::vector<Vector2f> verts;
};

struct PolyLineCommond {
    PolyLineCommond( const Vector3f& normal, ReverseFlag reverseFlag ) : normal( normal ), reverseFlag( reverseFlag ) {}

    Vector3f normal = Vector3f::ZERO;
    ReverseFlag reverseFlag = ReverseFlag::False;
};

struct PolyLine : public PolyLineBase3d, public PolyLineCommond {
    PolyLine( const std::vector<Vector3f>& _verts, const Vector3f& _normal,
              const ReverseFlag _reverseFlag = ReverseFlag::False ) :
            PolyLineBase3d(_verts), PolyLineCommond(_normal, _reverseFlag) {}
    PolyLine( const Triangle3d& _verts, const Vector3f& _normal,
              const ReverseFlag _reverseFlag = ReverseFlag::False ) :
            PolyLineBase3d(_verts), PolyLineCommond(_normal, _reverseFlag) {}
};

struct PolyOutLine : public PolyLineBase3d {
    PolyOutLine( const std::vector<Vector3f>& _verts, const Vector3f& _normal, const float _zPull,
                 const ReverseFlag _reverseFlag = ReverseFlag::False ) :
            PolyLineBase3d(_verts), normal(_normal), zPull(_zPull), reverseFlag(_reverseFlag) {}

    Vector3f normal;
    float zPull;
    ReverseFlag reverseFlag = ReverseFlag::False;
};

struct PolyLine2d : public PolyLineBase2d, public PolyLineCommond {
    PolyLine2d( const std::vector<Vector2f>& _verts, const Vector3f& _normal,
                const ReverseFlag _reverseFlag = ReverseFlag::False ) :
            PolyLineBase2d(_verts), PolyLineCommond(_normal, _reverseFlag) {}
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

    enum ClipMode {
        Union,
        Intersection
    };

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

    void clipperToPolylines( std::vector<PolyLine2d>& ret, const ClipperLib::Paths& solution, const Vector3f& _normal,
                             ReverseFlag rf = ReverseFlag::False );
    std::vector<PolyLine2d> clipperToPolylines( const ClipperLib::Paths& source, const ClipperLib::Path& clipAgainst,
                                                const Vector3f& _normal, ReverseFlag rf = ReverseFlag::False );
    ClipperLib::Path v2ListToClipperPath( const std::vector<Vector2f>& _values );
    V2fVectorOfVector clipperPathsToV2list( const ClipperLib::Paths& paths );

    V2fVector clipAgainst( const V2fVector& path1, const V2fVector& path2, ClipMode clipMode = ClipMode::Intersection );
}
