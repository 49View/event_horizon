//
//  geom_data.hpp
//  6thViewImporter
//
//  Created by Dado on 15/10/2015.
//
//

#pragma once

#include <cstring>
#include <vector>
#include <array>
#include <map>
#include <list>
#include <iostream>
#include <string>

#include <core/htypes_shared.hpp>
#include <core/math/poly_utils.hpp>
#include <core/math/poly_shapes.hpp>
#include <core/math/matrix4f.h>
#include <core/math/rect2f.h>
#include <core/math/plane3f.h>
#include <core/math/aabb.h>
#include <core/math/quad_vertices.h>
#include <core/soa_utils.h>
#include <core/boxable.hpp>
#include <core/name_policy.hpp>
#include <core/serialization.hpp>
#include <core/v_data.hpp>

#include <poly/polypartition.h>
#include <poly/poly_helper.h>

class Profile;
class GeomData;
class RenderChunk;
class GeomBuilder;

enum PullFlags : uint32_t {
    Sides = 1 << 0,
    Tops = 1 << 1,
    All = 0xffffffff,
};

class GeomData : public Boxable<JMATH::AABB>, public NamePolicy<> {
public:
    GeomData();
    virtual ~GeomData();
    GeomData( ShapeType _st,
              const Vector3f& _pos, const Vector3f& _axis, const Vector3f& _scale,
              const GeomMappingData& _mapping );

    GeomData( const std::vector<PolyOutLine>& verts,
              const GeomMappingData& _mapping, PullFlags pullFlags = PullFlags::All );

    GeomData( const std::vector<PolyLine>& _polyLine,
              const GeomMappingData& _mapping );

    GeomData( const QuadVector3fNormalfList& quads, const GeomMappingData& _mapping );
public:
    void addShape( ShapeType st, const Vector3f& center, const Vector3f& size, int subDivs = 0 );

    MappingDirection getMappingDirection() const { return mapping.direction; }
    void setMappingDirection( MappingDirection val ) { mapping.direction = val; }

    inline VData&  vData() { return mVdata; }

    void addFlatPoly( size_t vsize, const Vector3f *verts, const Vector3f& normal, bool reverseIfTriangulated = false );
    void addFlatPoly( size_t vsize, const std::vector<Vector2f>& verts, float z, const Vector3f& normal,
                      bool reverseIfTriangulated = false );
    void
    addFlatPoly( const std::array<Vector3f, 4>& verts, const Vector3f& normal, bool reverseIfTriangulated = false );
    void addFlatPolyTriangulated( size_t vsize, const Vector3f *verts, const Vector3f& normal, bool reverse = false );
    void addFlatPolyTriangulated( size_t vsize, const std::vector<Vector2f>& verts, float z, const Vector3f& normal,
                                  bool reverse = false );
    void addFlatPolyWithMapping( size_t vsize, const Vector3f *vs, const Vector2f *vts,
                                 const Vector3f& vn = Vector3f::HUGE_VALUE_NEG );
    void addFlatPolyWithMapping( const QuadStripUV& qs );
    std::vector<Vector3f> pullWindingOrderCheck( const std::vector<Vector2f>& verts, float zOffset );
    void pull( const std::vector<Vector3f>& verts, float height, PullFlags pullFlags = PullFlags::All );
    void pull( const std::vector<Vector2f>& verts, float height, float zOffset = 0.0f,
               PullFlags pullFlags = PullFlags::All );

    void checkBaricentricCoordsOn( const Vector3f& i, int32_t pIndexStart, int32_t pIndexEnd, int32_t& pIndex, float& u,
                                   float& v );

    void bake( const Matrix4f& m, std::shared_ptr<GeomData>& ret );
    void transform( const Matrix4f& m );
    void scale( float factor );
    void translate( const Vector3f& pos );
    void rotate( float angle, const Vector3f& axis, const Vector3f& pivot = Vector3f::ZERO );
    void flipNormals();

    // Utils
    void doNotScaleMapping();
    Vector3f normalFromPoints( const Vector3f *vs );
    static std::vector<Vector3f> utilGenerateFlatBoxFromRect( const JMATH::Rect2f& bbox, float z );
    static std::vector<Vector2f> utilGenerateFlatRect( const Vector2f& size, const WindingOrderT wo = WindingOrder::CCW,
                                                       PivotPointPosition ppp = PivotPointPosition::PPP_CENTER,
                                                       const Vector2f& pivot = Vector2f::ZERO );
    static std::vector<Vector3f> utilGenerateFlatBoxFromSize( float width, float height, float z );

    void Bevel( const Vector3f& bevelAmount );
    Vector3f Bevel() const;

    // Mapping
    Vector2f MappingOffset() const { return mapping.offset; }
    void MappingOffset( const Vector2f& val ) { mapping.offset = val; }
    MappingMirrorE MappingMirror() const { return mapping.mirroring; }
    void MappingMirror( MappingMirrorE val ) { mapping.mirroring = val; }
    bool UnitMapping() const { return mapping.bUnitMapping; }
    void UnitMapping( bool val ) { mapping.bUnitMapping = val; }

    // SH
//    void calcSHBounce( const Node *dad, GeomData *dest );
    subdivisionAccuray SubdivAccuracy() const { return mSubdivAccuracy; }
    void SubdivAccuracy( subdivisionAccuray val ) { mSubdivAccuracy = val; }

    // Debug
    const VData& getVData() const {
        return mVdata;
    }

    virtual void debugPrint();
    const std::vector<Vector2f>& WrapMappingCoords() const { return wrapMappingCoords; }
    void WrapMappingCoords( const std::vector<Vector2f>& val ) { wrapMappingCoords = val; }
    void setWindingOrderFlagOnly( WindingOrderT _wo );
    WindingOrderT getWindingOrder() const;

    void resetWrapMapping( const std::vector<float>& yWrapArray );
    void updateWrapMapping( Vector3f vs[4], Vector2f vtcs[4], uint64_t m, uint64_t size );
    void planarMapping( const Vector3f& normal, const Vector3f vs[], Vector2f vtcs[], int numVerts );

    template<typename TV> \
	void visit() const { traverseWithHelper<TV>( "BBbox", mVdata.BBox3d() ); }

    // All internal add polygons are now not accessible to the outside to handle topology better
    void pushQuad( const std::array<Vector3f, 4>& vs, const std::array<Vector2f, 4>& vts, const Vector3f& vn );
    void pushQuad( const std::array<Vector3f, 4>& vs, const std::array<Vector2f, 4>& vts,
                   const std::array<Vector3f, 4>& vns );

    void pushQuad( const QuadVertices3& vs, const QuadVertices2& vts );
    void pushQuad( const QuadVertices3& vs, const QuadVertices2& vts, const QuadVertices3& vns );

    void pushQuadSubDiv( const std::array<Vector3f, 4>& vss, const std::array<Vector2f, 4>& vtcs,
                         const Vector3f& vn = Vector3f::HUGE_VALUE_NEG );
    void pushQuadSubDiv( const std::array<Vector3f, 4>& vss, const std::array<Vector2f, 4>& vtcs,
                         const std::array<Vector3f, 4>& vns );

    void pushTriangleSubDiv( const Vector3f& v1, const Vector3f& v2, const Vector3f& v3,
                             const Vector2f& uv1, const Vector2f& uv2, const Vector2f& uv3,
                             const Vector3f& vn1, const Vector3f& vn2, const Vector3f& vn3 );

    void pushTriangle( const Vector3f& v1, const Vector3f& v2, const Vector3f& v3,
                       const Vector2f& uv1, const Vector2f& uv2, const Vector2f& uv3,
                       const Vector3f& vn1, const Vector3f& vn2, const Vector3f& vn3,
                       const Vector3f& vt1 = Vector3f::ZERO, const Vector3f& vt2 = Vector3f::ZERO,
                       const Vector3f& vt3 = Vector3f::ZERO,
                       const Vector3f& vb1 = Vector3f::ZERO, const Vector3f& vb2 = Vector3f::ZERO,
                       const Vector3f& vb3 = Vector3f::ZERO );

protected:

    void
    pushTriangle( const std::vector<Vector3f>& vs, const std::vector<Vector2f>& vuv, const std::vector<Vector3f>& vn );

    void addFlatPoly( const std::vector<Vector3f>& points, WindingOrderT wo );

    // Mapping
    void setMappingData( const GeomMappingData& _mapping );
    void resetMapping( uint64_t arraySize );
    void propagateWrapMapping( const GeomData *source );

    void pushTriangleSubDivRec( const Vector3f& v1, const Vector3f& v2, const Vector3f& v3,
                                const Vector2f& uv1, const Vector2f& uv2, const Vector2f& uv3,
                                const Vector3f& vn1, const Vector3f& vn2, const Vector3f& vn3,
                                int triSubDiv );

    template<typename T>
    void updatePullMapping( const std::array<T, 4>& vs, std::array<Vector2f, 4>& vtcs );

    void calcMirrorUVs( Vector2f *uvs );

protected:
    VData mVdata;

    subdivisionAccuray mSubdivAccuracy = accuracyNone;
    WindingOrderT mWindingOrder = WindingOrder::CCW;

    bool mHasBevel = false;
    Vector3f mBevelAmount = Vector3f::ZERO;

    // Mapping
    GeomMappingData mapping;

    // Mappping computed
    float fuvScale = 1.0f;
    Vector2f uvScale = Vector2f::ONE;
    Vector2f uvScaleInv = Vector2f::ONE;
    std::vector<Vector2f> wrapMappingCoords;
    Vector2f pullMappingCoords = Vector2f::ZERO;
};
