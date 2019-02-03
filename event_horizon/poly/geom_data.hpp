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

#include "core/descriptors/material.h"
#include "core/htypes_shared.hpp"
#include "core/math/poly_utils.hpp"
#include "core/math/poly_shapes.hpp"
#include "core/math/matrix4f.h"
#include "core/math/rect2f.h"
#include "core/math/plane3f.h"
#include "core/math/aabb.h"
#include "core/math/quad_vertices.h"
#include "core/soa_utils.h"

#include "polypartition.h"
#include "poly_helper.h"

class Profile;
class SerializeBin;
class DeserializeBin;
class GeomData;
class VertexProcessing;
class GeomBuilder;

enum PullFlags : uint32_t {
    Sides = 1 << 0,
    Tops = 1 << 1,
    All = 0xffffffff,
};

//static const int OutputFloatPrecision = 4;

//class DeepIntersectData {
//public:
//    Vector3f intersectPoint;
//    float minDist;
//    GeomData *geom;
//    GeomData *geomBaked;
//    Node *hierOfGeom;
//    int32_t indexStart;
//    int32_t indexEnd;
//
//    DeepIntersectData() {
//        minDist = -1.0f;
//    }
//
//    void set( const Vector3f& i, float md, GeomData *g, GeomData *gBaked, int32_t ps, int32_t pe, Node *hg ) {
//        intersectPoint = i;
//        minDist = md;
//        geom = g;
//        geomBaked = gBaked;
//        indexStart = ps;
//        indexEnd = pe;
//        hierOfGeom = hg;
//    }
//};

struct VData {

//    void allocateEmptySHData();
    void clear();

    size_t numIndices() const { return vIndices.size(); }
    size_t numVerts() const { return vcoords3d.size(); }
    void add( int32_t _i, const Vector3f& _v,
                          const Vector3f& _n,
                          const Vector2f& _uv,
                          const Vector2f& _uv2,
                          const Vector3f& _t,
                          const Vector3f& _b,
                          const Vector4f& _c );
    void fill( const PolyStruct& ps );
    void fillIndices( const std::vector<int>& _indices );
    void fillCoors3d( const std::vector<Vector3f>& _verts );
    void fillUV( const std::vector<Vector2f>& _uvs, uint32_t _index = 0 );
    void fillNormals( const std::vector<Vector3f>& _normals );
    void fillTangets( const std::vector<Vector3f>& _tangents );
    void fillBinormal( const std::vector<Vector3f>& _binormals );
    void fillColors( const std::vector<Vector4f>& _colors );
    void allocateSpaceForVertices( const int _numVerts );
    void changeWindingOrder();
    void sanitizeUVMap();
    void calcBinormal();
    void changeHandness();
    void mirrorFlip( WindingOrderT wow, WindingOrderT woh, const Rect2f& bbox );
    void checkBaricentricCoordsOn( const Vector3f& i, int32_t pIndexStart, int32_t pIndexEnd,
                                   int32_t& pIndex, float& u, float& v );
    void addTriangleVertex( const Vector3f& _vc, const Vector2f& _uv, const Vector2f& _uv2, const Vector3f& _vn,
                            const Vector3f& _vt, const Vector3f& _vb, const Vector3f& _v8 );
    void swapIndicesWinding( Primitive _pr );

    const std::vector<int32_t>& getVIndices() const {
        return vIndices;
    }

    const std::vector<Vector3f>& getVcoords3d() const {
        return vcoords3d;
    }

    const std::vector<Vector3f>& getVnormals3d() const {
        return vnormals3d;
    }

    const std::vector<Vector3f>& getVtangents3d() const {
        return vtangents3d;
    }

    const std::vector<Vector3f>& getVbinormals3d() const {
        return vbinormals3d;
    }

    const std::vector<Vector2f>& getVUVs() const {
        return vUVs;
    }

    const std::vector<Vector2f>& getVUV2s() const {
        return vUV2s;
    }

    const std::vector<Vector4f>& getVColor() const {
        return vColor;
    }

    void setVIndices( const size_t _index, const int32_t& _value ) {
        vIndices[_index] = _value;
    }

    void setVcoords3d( const size_t _index, const Vector3f& _value ) {
        vcoords3d[_index] = _value;
    }

    void setVnormals3d( const size_t _index, const Vector3f& _value ) {
        vnormals3d[_index] = _value;
    }

    void setVtangents3d( const size_t _index, const Vector3f& _value ) {
        vtangents3d[_index] = _value;
    }

    void setVbinormals3d( const size_t _index, const Vector3f& _value ) {
        vbinormals3d[_index] = _value;
    }

    void setVUVs( const size_t _index, const Vector2f& _value ) {
        vUVs[_index] = _value;
    }

    void setVUV2s( const size_t _index, const Vector2f& _value ) {
        vUV2s[_index] = _value;
    }

    void setVColor( const size_t _index, const Vector4f& _value ) {
        vColor[_index] = _value;
    }

    const Vector3f& getMin() const {
        return min;
    }

    void setMin( const Vector3f& min ) {
        VData::min = min;
    }

    const Vector3f& getMax() const {
        return max;
    }

    void setMax( const Vector3f& max ) {
        VData::max = max;
    }

    friend class GeomData;
    friend class HierGeom;

private:
    std::vector<int32_t>  vIndices;
    std::vector<Vector3f> vcoords3d;
    std::vector<Vector3f> vnormals3d;
    std::vector<Vector3f> vtangents3d;
    std::vector<Vector3f> vbinormals3d;
    std::vector<Vector2f> vUVs;
    std::vector<Vector2f> vUV2s;
//    std::vector<Matrix3f> vSHCoeffsR;
//    std::vector<Matrix3f> vSHCoeffsG;
//    std::vector<Matrix3f> vSHCoeffsB;
    std::vector<Vector4f> vColor;
    Vector3f min = Vector3f::ZERO;
    Vector3f max = Vector3f::ZERO;
    Primitive primitive = PRIMITIVE_TRIANGLES;
};

class GeomData {
public:
    GeomData();
    virtual ~GeomData();
    explicit GeomData( std::shared_ptr<DeserializeBin> reader );
    GeomData( const ShapeType _st,
              const Vector3f& _pos, const Vector3f& _axis, const Vector3f& _scale,
              std::shared_ptr<Material> _material,
              const GeomMappingData& _mapping );

    GeomData( const std::vector<PolyOutLine>& verts, std::shared_ptr<Material> _material,
              const GeomMappingData& _mapping, PullFlags pullFlags = PullFlags::All );

    GeomData( const std::vector<PolyLine>& _polyLine, std::shared_ptr<Material> _material,
              const GeomMappingData& _mapping );

    GeomData( const QuadVector3fNormalfList& quads, std::shared_ptr<Material> _material, const GeomMappingData& _mapping );

    static GeomDeserializeDependencies gatherDependencies( std::shared_ptr<DeserializeBin> reader );

    void serialize( std::shared_ptr<SerializeBin> writer );
    void serializeSphericalHarmonics( std::shared_ptr<SerializeBin> writer );
    void deserialize( std::shared_ptr<DeserializeBin> reader );
    void deserializeSphericalHarmonics( std::shared_ptr<DeserializeBin> reader );
    void serializeDependencies( std::shared_ptr<SerializeBin> writer );
    void deserializeDependencies( std::shared_ptr<SerializeBin> reader );

    std::string Name() const { return mName; }
    void Name( const std::string& val ) { mName = val; }

    std::shared_ptr<Material> getMaterial() { return material; }
    std::shared_ptr<Material> getMaterial() const { return material; }
    void setMaterial( std::shared_ptr<Material> _mat ) { material = _mat; }
//    Color4f getColor() const { return getMaterial()->getColor(); }
//    float getOpacity() const { return getMaterial()->getOpacity(); }
//    void setOpacity( float _opacity )  { mOpacity = _opacity; }
//    void setMaterial( const std::string& matName, float _opacity = 1.0f );
//    void setMaterial( const std::string& matName, const Color4f& col, float _opacity = 1.0f );

//    void set( const std::string& uniformName, float data );
//    void set( const std::string& uniformName, std::shared_ptr<Texture> data );
//    void setTextureSet( const std::string& baseTextureName );
//    void set( const std::string& uniformName, const Vector2f& data );
//    void set( const std::string& uniformName, const Vector3f& data );
//    void set( const std::string& uniformName, const Vector4f& data );
//    void set( const std::string& uniformName, const Matrix4f& data );
//    void set( const std::string& uniformName, const Matrix3f& data );
//
//    Vector3f getColor() const;
//    float getOpacity() const;
//    std::shared_ptr<Texture> getColorTexture() const;

    void addShape( ShapeType st, const Vector3f& center, const Vector3f& size, int subDivs = 0 );

    MappingDirection getMappingDirection() const { return mapping.direction; }
    void setMappingDirection( MappingDirection val ) { mapping.direction = val; }

    inline VData&  vData() { return mVdata; }
    inline int32_t numVerts() const { return static_cast<int32_t>( mVdata.vcoords3d.size()); }
    inline int32_t numIndices() const { return static_cast<int32_t>( mVdata.vIndices.size()); }
    inline int32_t numNormals() const { return static_cast<int32_t>( mVdata.vnormals3d.size()); }
    inline const int32_t *Indices() const { return mVdata.vIndices.data(); }
    inline int32_t vindexAt( int32_t i ) const { return mVdata.vIndices[i]; }
    inline Vector3f vertexAt( int32_t i ) const { return mVdata.vcoords3d[i]; }
    inline Vector2f uvAt( int32_t i ) const { return mVdata.vUVs[i]; }
    inline Vector2f uv2At( int32_t i ) const { return mVdata.vUV2s[i]; }
    inline Vector3f normalAt( int32_t i ) const { return mVdata.vnormals3d[i]; }
    inline Vector3f tangentAt( int32_t i ) const { return mVdata.vtangents3d[i]; }
    inline Vector3f binormalAt( int32_t i ) const { return mVdata.vbinormals3d[i]; }
//    inline const Matrix3f& shCoeffRAt( int32_t i ) const { return mVdata.vSHCoeffsR[i]; }
//    inline const Matrix3f& shCoeffGAt( int32_t i ) const { return mVdata.vSHCoeffsG[i]; }
//    inline const Matrix3f& shCoeffBAt( int32_t i ) const { return mVdata.vSHCoeffsB[i]; }
//    inline Matrix3f& shCoeffRAt( int32_t i ) { return mVdata.vSHCoeffsR[i]; }
//    inline Matrix3f& shCoeffGAt( int32_t i ) { return mVdata.vSHCoeffsG[i]; }
//    inline Matrix3f& shCoeffBAt( int32_t i ) { return mVdata.vSHCoeffsB[i]; }
    inline Vector4f colorAt( int32_t i ) const { return mVdata.vColor[i]; }

//    inline void shCoeffRAt( int32_t i, const Matrix3f& val ) {
//        ASSERT( i < numVerts());
//        mVdata.vSHCoeffsR[i] = val;
//    }
//
//    inline void shCoeffGAt( int32_t i, const Matrix3f& val ) {
//        ASSERT( i < numVerts());
//        mVdata.vSHCoeffsG[i] = val;
//    }
//
//    inline void shCoeffBAt( int32_t i, const Matrix3f& val ) {
//        ASSERT( i < numVerts());
//        mVdata.vSHCoeffsB[i] = val;
//    }
//
//    inline void shIncCoeffRAt( int32_t i, const Matrix3f& val ) {
//        ASSERT( i < numVerts());
//        mVdata.vSHCoeffsR[i] += val;
//    }
//
//    inline void shIncCoeffGAt( int32_t i, const Matrix3f& val ) {
//        ASSERT( i < numVerts());
//        mVdata.vSHCoeffsG[i] += val;
//    }
//
//    inline void shIncCoeffBAt( int32_t i, const Matrix3f& val ) {
//        ASSERT( i < numVerts());
//        mVdata.vSHCoeffsB[i] += val;
//    }
//
//    void shCoeffBaricentricRedAt( Matrix3f& dest, int index, float bu, float bv );
//    void shCoeffBaricentricGreenAt( Matrix3f& dest, int index, float bu, float bv );
//    void shCoeffBaricentricBlueAt( Matrix3f& dest, int index, float bu, float bv );

    inline std::vector<Vector3f>& Coords3d() { return mVdata.vcoords3d; };
    inline std::vector<Vector3f>& Normals3d() { return mVdata.vnormals3d; };
//    inline bool hasSHCoeffsR() const { return mVdata.vSHCoeffsR.size() > 0; };
//    inline bool hasSHCoeffsG() const { return mVdata.vSHCoeffsG.size() > 0; };
//    inline bool hasSHCoeffsB() const { return mVdata.vSHCoeffsB.size() > 0; };

    void reset();

    //	const ProgramUniformSet* MatProperties() const { return matProperties; }
    //	ProgramUniformSet* MatProperties() { return matProperties; }
    //	void MatProperties(ProgramUniformSet* val) { matProperties = val; }

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

    JMATH::AABB BBox3d() const { return mBBox3d; }
    void BBox3d( const JMATH::AABB& val ) { mBBox3d = val; }
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
    void setWindingOrderFlagOnly( const WindingOrderT _wo );
    WindingOrderT getWindingOrder() const;

    void resetWrapMapping( const std::vector<float>& yWrapArray );
    void updateWrapMapping( Vector3f vs[4], Vector2f vtcs[4], uint64_t m, uint64_t size );
    void planarMapping( const Vector3f& normal, const Vector3f vs[], Vector2f vtcs[], int numVerts );

    template<typename TV> \
	void visit() const { traverseWithHelper<TV>( "Name,BBbox", mName,mBBox3d ); }

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

    void addTriangleVertex( const Vector3f& _vc, const Vector2f& _uv, const Vector3f& _vn, const Vector3f& _vt,
                            const Vector3f& _vb );

    void
    pushTriangle( const std::vector<Vector3f>& vs, const std::vector<Vector2f>& vuv, const std::vector<Vector3f>& vn );

    void addFlatPoly( const std::vector<Vector3f>& points, WindingOrderT wo );

    // Mapping
    void setMappingData( const GeomMappingData& _mapping );
    void setTextureCoordsMultiplier();
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
    std::string mName = "DefaultGeomName";
    std::shared_ptr<Material> material;

    float mOpacity = 1.0f;
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

    // 3d Data
    JMATH::AABB mBBox3d = JMATH::AABB::INVALID;

    VData mVdata;

public:
    static uint64_t Version();

    friend class Follower;
};
