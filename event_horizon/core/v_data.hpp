//
// Created by Dado on 2019-03-31.
//

#pragma once

#include <vector>
#include <cstdint>
#include <core/boxable.hpp>
#include <core/math/vector4f.h>
#include <core/soa_utils.h>

class VData : public Boxable<JMATH::AABB> {
public:
    size_t numIndices() const;
    size_t numVerts() const;
    void add( int32_t _i, const Vector3f& _v,
              const Vector3f& _n,
              const Vector2f& _uv,
              const Vector2f& _uv2,
              const Vector3f& _t,
              const Vector3f& _b,
              const Vector4f& _c );
    void fill( const PolyStruct& ps );
    void fillIndices( const std::vector<int32_t>& _indices );
    void fillCoors3d( const std::vector<Vector3f>& _verts );
    void fillUV( const std::vector<Vector2f>& _uvs, uint32_t _index = 0 );
    void fillNormals( const std::vector<Vector3f>& _normals, bool _bInvert = false );
    void fillTangets( const std::vector<Vector3f>& _tangents, bool _bInvert = false );
    void fillBinormal( const std::vector<Vector3f>& _binormals, bool _bInvert = false );
    void fillColors( const std::vector<Vector4f>& _colors );
    void allocateSpaceForVertices( int _numVerts );
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

    const std::vector<int32_t>& getVIndices() const;
    const std::vector<Vector3f>& getVcoords3d() const;
    const std::vector<Vector3f>& getVnormals3d() const;
    const std::vector<Vector3f>& getVtangents3d() const;
    const std::vector<Vector3f>& getVbinormals3d() const;
    const std::vector<Vector2f>& getVUVs() const;
    const std::vector<Vector2f>& getVUV2s() const;
    const std::vector<Vector4f>& getVColor() const;

    void setVIndices( const size_t _index, const int32_t& _value );
    void setVcoords3d( const size_t _index, const Vector3f& _value );
    void setVnormals3d( const size_t _index, const Vector3f& _value );
    void setVtangents3d( const size_t _index, const Vector3f& _value );
    void setVbinormals3d( const size_t _index, const Vector3f& _value );
    void setVUVs( const size_t _index, const Vector2f& _value );
    void setVUV2s( const size_t _index, const Vector2f& _value );
    void setVColor( const size_t _index, const Vector4f& _value );
    const Vector3f& getMin() const;
    void setMin( const Vector3f& min );
    const Vector3f& getMax() const;
    void setMax( const Vector3f& max );
    Primitive getPrimitive() const;
    void setPrimitive( Primitive _primitive );

    friend class GeomData;

private:
    std::vector<int32_t>  vIndices;
    std::vector<Vector3f> vcoords3d;
    std::vector<Vector3f> vnormals3d;
    std::vector<Vector3f> vtangents3d;
    std::vector<Vector3f> vbinormals3d;
    std::vector<Vector2f> vUVs;
    std::vector<Vector2f> vUV2s;
    std::vector<Vector4f> vColor;
    Primitive primitive = PRIMITIVE_TRIANGLES;
};



