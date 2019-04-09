//
// Created by Dado on 2019-03-31.
//

#pragma once

#include <vector>
#include <cstdint>
#include <core/boxable.hpp>
#include <core/math/vector4f.h>
#include <core/soa_utils.h>
#include <core/serializebin.hpp>

template<typename TVisitor, typename ...T> inline void serializeBinVariadic( TVisitor& visitor, T &...v ) {
    ((void)visitor.write(std::forward<T>(v)), ...);
}

template<typename TVisitor, typename ...T> inline void deserializeBinVariadic( TVisitor& visitor, T &...v ) {
    ((void)visitor.read(static_cast<T&>(v)), ...);
}

#define JSONSERIALBIN(CLASSNAME,...) \
    RESOURCE_CTORS(CLASSNAME); \
	explicit CLASSNAME( DeserializeBin& reader ) { deserialize( reader ); } \
	inline void serialize( SerializeBin& visitor ) const { serializeBinVariadic(visitor, __VA_ARGS__ ); } \
	inline SerializableContainer serialize() const { SerializeBin mw; serialize(mw); return mw.serialize();} \
	inline void deserialize( DeserializeBin& visitor ) { deserializeBinVariadic(visitor, __VA_ARGS__ ); } \
	void bufferDecode( const unsigned char *_buffer, size_t _length ) { \
        DeserializeBin reader( _buffer, _length ); \
        deserialize( reader ); \
    }

class VData : public Boxable<JMATH::AABB> {
public:
    VData() = default;
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

    const std::vector<int32_t>&  getVIndices() const;
    const std::vector<Vector3f>& getVcoords3d() const;
    const std::vector<Vector3f>& getVnormals3d() const;
    const std::vector<Vector3f>& getVtangents3d() const;
    const std::vector<Vector3f>& getVbinormals3d() const;
    const std::vector<Vector2f>& getVUVs() const;
    const std::vector<Vector2f>& getVUV2s() const;
    const std::vector<Vector4f>& getVColor() const;
    const Vector3f& getMin() const;
    const Vector3f& getMax() const;
    Primitive getPrimitive() const;

    void setVIndices( size_t _index, const int32_t& _value );
    void setVcoords3d( size_t _index, const Vector3f& _value );
    void setVnormals3d( size_t _index, const Vector3f& _value );
    void setVtangents3d( size_t _index, const Vector3f& _value );
    void setVbinormals3d( size_t _index, const Vector3f& _value );
    void setVUVs( size_t _index, const Vector2f& _value );
    void setVUV2s( size_t _index, const Vector2f& _value );
    void setVColor( size_t _index, const Vector4f& _value );
    void setMin( const Vector3f& min );
    void setMax( const Vector3f& max );
    void setPrimitive( Primitive _primitive );

    inline const int32_t *Indices() const { return vIndices.data(); }
    inline int32_t vindexAt( int32_t i ) const { return vIndices[i]; }
    inline Vector3f vertexAt( int32_t i ) const { return vcoords3d[i]; }
    inline Vector2f uvAt( int32_t i ) const { return vUVs[i]; }
    inline Vector2f uv2At( int32_t i ) const { return vUV2s[i]; }
    inline Vector3f normalAt( int32_t i ) const { return vnormals3d[i]; }
    inline Vector3f tangentAt( int32_t i ) const { return vtangents3d[i]; }
    inline Vector3f binormalAt( int32_t i ) const { return vbinormals3d[i]; }
    inline Vector4f colorAt( int32_t i ) const { return vColor[i]; }

    friend class GeomData;

    JSONSERIALBIN( VData, vIndices, vcoords3d, vnormals3d, vtangents3d, vbinormals3d, vUVs, vUV2s, vColor, primitive, bbox3d)
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
