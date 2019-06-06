//
// Created by Dado on 2019-03-31.
//

#pragma once

#include <vector>
#include <memory>
#include <cstdint>
#include <core/boxable.hpp>
#include <core/math/vector4f.h>
#include <core/soa_utils.h>
#include <core/resources/resource_serialization.hpp>

class VData : public Boxable<JMATH::AABB> {
public:
    VData() = default;
    size_t numIndices() const;
    size_t numVerts() const;
    void add( uint32_t _i, const Vector3f& _v,
              const Vector3f& _n,
              const Vector2f& _uv,
              const Vector2f& _uv2,
              const Vector4f& _t,
              const Vector3f& _b,
              const Vector4f& _c );
    void fill( const PolyStruct& ps );
    void fillIndices( const std::vector<uint32_t>& _indices );
    void fillCoors3d( const std::vector<Vector3f>& _verts );
    void fillUV( const std::vector<Vector2f>& _uvs, uint32_t _index = 0 );
    void fillSetUV( size_t _size, const Vector2f& _uvs, uint32_t _index );
    void fillNormals( const std::vector<Vector3f>& _normals, bool _bInvert = false );
    void fillTangets( const std::vector<Vector3f>& _tangents, bool _bInvert = false );
    void fillTangets( const std::vector<Vector4f>& _tangents, bool _bInvert = false );
    void fillBinormal( const std::vector<Vector3f>& _binormals, bool _bInvert = false );
    void fillColors( const std::vector<Vector4f>& _colors );

    void flattenStride( void* ret, size_t _index, std::shared_ptr<Matrix4f> _mat = nullptr );

    void mapIndices( void* ret, uint32_t _startIndex, uint32_t _voffsetIndex,
                     const std::string& _oRef, std::unordered_map<uint32_t, HashIndexPairU32>& _oMap );

    void forcePlanarMapping();
    void changeWindingOrder();
    void sanitizeUVMap();
    void calcBinormal();
    void calcBinormalFromNormAndTang();
    void changeHandness();
    void flipNormals();
    void mirrorFlip( WindingOrderT wow, WindingOrderT woh, const Rect2f& bbox );
    void checkBaricentricCoordsOn( const Vector3f& i, uint32_t pIndexStart, uint32_t pIndexEnd,
                                   uint32_t& pIndex, float& u, float& v );
    void addTriangleVertex( const Vector3f& _vc, const Vector2f& _uv, const Vector2f& _uv2, const Vector3f& _vn,
                            const Vector4f& _vt, const Vector3f& _vb, const Vector3f& _v8 );
    void swapIndicesWinding( Primitive _pr );

    const std::vector<uint32_t>&  getVIndices() const;
//    const std::vector<Vector3f>& getVcoords3d() const;
//    const std::vector<Vector3f>& getVnormals3d() const;
//    const std::vector<Vector3f>& getVtangents3d() const;
//    const std::vector<Vector3f>& getVbinormals3d() const;
//    const std::vector<Vector2f>& getVUVs() const;
//    const std::vector<Vector2f>& getVUV2s() const;
//    const std::vector<Vector4f>& getVColor() const;
    const Vector3f& getMin() const;
    const Vector3f& getMax() const;
    Primitive getPrimitive() const;

    void setVIndices( size_t _index, const uint32_t& _value );
//    void setVcoords3d( size_t _index, const Vector3f& _value );
//    void setVnormals3d( size_t _index, const Vector3f& _value );
//    void setVtangents3d( size_t _index, const Vector3f& _value );
//    void setVbinormals3d( size_t _index, const Vector3f& _value );
//    void setVUVs( size_t _index, const Vector2f& _value );
    void setVUV2s( size_t _index, const Vector2f& _value );
//    void setVColor( size_t _index, const Vector4f& _value );
    void setMin( const Vector3f& min );
    void setMax( const Vector3f& max );
    void setPrimitive( Primitive _primitive );

    inline const uint32_t *Indices() const { return vIndices.data(); }
    inline uint32_t vindexAt( uint32_t i ) const { return vIndices[i]; }
    inline PUUNTBC soaAt( uint32_t i ) const { return vSoaData[i]; }
    inline Vector3f vertexAt( uint32_t i ) const { return vSoaData[i].pos; }
    inline Vector2f uvAt( uint32_t i ) const { return vSoaData[i].a1; }
    inline Vector2f uv2At( uint32_t i ) const { return vSoaData[i].a2; }
    inline Vector3f normalAt( uint32_t i ) const { return vSoaData[i].a3; }
    inline Vector4f tangentAt( uint32_t i ) const { return vSoaData[i].a4; }
    inline Vector3f binormalAt( uint32_t i ) const { return vSoaData[i].a5; }
    inline Vector4f colorAt( uint32_t i ) const { return vSoaData[i].a6; }

    JSONRESOURCECLASSSERIALBIN( VData, vIndices, vSoaData, primitive, bbox3d)
private:
    std::vector<uint32_t>  vIndices;
    std::vector<PUUNTBC>  vSoaData;
    Primitive primitive = PRIMITIVE_TRIANGLES;
};
