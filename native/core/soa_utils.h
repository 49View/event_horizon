//
//  rendertypes.h
//  SixthView
//
//  Created by Dado on 21/11/2014.
//  Copyright (c) 2014 JFDP Labs. All rights reserved.
//

#pragma once

#include <vector>
#include <map>

#include <core/math/vector4f.h>
#include <core/math/rect2f.h>
#include <core/math/quad_vertices.h>

typedef std::pair<std::vector<Vector2f>, Vector3f> vertexListVec2RGB;

struct PolyStruct {
public:
    std::unique_ptr<Vector3f[]> verts;
    std::unique_ptr<Vector2f[]> uvs;
    std::unique_ptr<Vector2f[]> uv2s;
    std::unique_ptr<Vector3f[]> normals;
    std::unique_ptr<Vector4f[]> tangents;
    std::unique_ptr<Vector3f[]> binormals;
    std::unique_ptr<Vector4f[]> colors;
    std::unique_ptr<uint32_t[]> indices;
    int numVerts = 0;
    int32_t numIndices = 0;
    AABB bbox3d = AABB::INVALID;
};

enum Primitive {
    PRIMITIVE_TRIANGLES = 0,
    PRIMITIVE_TRIANGLE_STRIP,
    PRIMITIVE_TRIANGLE_FAN,
    PRIMITIVE_PATCHES
};

enum class VFVertexAllocation {
    Ignore = 0,
    PreAllocate,
    Full
};

struct Z_ORDER {
    constexpr static float ZERO() { return 0.0f; }

    constexpr static float ONE() { return 0.01f; }

    constexpr static float TWO() { return 0.02f; }

    constexpr static float THREE() { return 0.03f; }

    constexpr static float FOUR() { return 0.04f; }
};

// ### THE ONLY SUPPORTED TYPES AT ARE: Byte and Float
enum class VFAttribTypes {
    Byte,
    UnsignedByte,
    Short,
    UnsignedShort,
    Int,
    UnsignedInt,
    Float
};

template<typename P>
struct VFA1 {
    P pos;

    VFA1() {}

    VFA1( const P& _p ) {
        pos = _p;
    }

    VFA1( float x, float y ) {
        pos[0] = x;
        pos[1] = y;
    }

    static int numElements() { return 1; }

    static int stride() { return sizeof( P ); }

    static VFAttribTypes type( [[maybe_unused]] uint64_t index ) { return VFAttribTypes::Float; }

    static int size( uint64_t /*index*/ ) { return sizeof( P ) / sizeof( float ); }

    static uintptr_t offset( uint64_t /*index*/ ) { return 0; }
};

template<typename P, typename A1>
struct VFA2 {
    P pos;
    A1 a1;

    VFA2() {}

    VFA2( const P& _p, const A1& _t ) {
        pos = _p;
        a1 = _t;
    }

    static int numElements() { return 2; }

    static int stride() { return sizeof( P ) + sizeof( A1 ); }

    static VFAttribTypes type( uint64_t index ) {
        if ( index == 0 ) return VFAttribTypes::Float;
        return sizeof( A1 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
    }

    static int size( uint64_t index ) {
        return index == 0 ? sizeof( P ) / sizeof( float ) : sizeof( A1 ) / sizeof( float );
    }

    static uintptr_t offset( uint64_t index ) { return index == 0 ? 0 : sizeof( P ); }
};

template<typename P, typename A1, typename A2>
struct VFA3 {
    P pos;
    A1 a1;
    A2 a2;

    VFA3() {}

    VFA3( const P& _p, const A1& _t, const A2& _t2 ) {
        pos = _p;
        a1 = _t;
        a2 = _t2;
    }

    static int numElements() { return 3; }

    static int stride() { return sizeof( P ) + sizeof( A1 ) + sizeof( A2 ); }

    static VFAttribTypes type( uint64_t index ) {
        switch ( index ) {
            case 0:
                return VFAttribTypes::Float;
                break;
            case 1:
                return sizeof( A1 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 2:
                return sizeof( A2 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
        }
        // default
        return VFAttribTypes::Float;
    }

    static int size( uint64_t index ) {
        switch ( index ) {
            case 0:
                return sizeof( P ) / sizeof( float );
            case 1:
                return sizeof( A1 ) == 1 ? 1 : sizeof( A1 ) / sizeof( float );
                break;
            case 2:
                return sizeof( A2 ) == 1 ? 1 : sizeof( A2 ) / sizeof( float );
                break;
        }
        // default
        return sizeof( P ) / sizeof( float );
    }

    static uintptr_t offset( uint64_t index ) {
        switch ( index ) {
            case 0:
                return 0;
                break;
            case 1:
                return sizeof( P );
                break;
            case 2:
                return sizeof( P ) + sizeof( A1 );
                break;
        }
        // default
        return 0;
    }
};

template<typename P, typename A1, typename A2, typename A3>
struct VFA4 {
    P pos;
    A1 a1;
    A2 a2;
    A3 a3;

    VFA4() {}

    VFA4( const P& _p, const A1& _t, const A2& _t2, const A3& _t3 ) {
        pos = _p;
        a1 = _t;
        a2 = _t2;
        a3 = _t3;
    }

    static int numElements() { return 4; }

    static int stride() { return sizeof( P ) + sizeof( A1 ) + sizeof( A2 ) + sizeof( A3 ); }

    static VFAttribTypes type( uint64_t index ) {
        switch ( index ) {
            case 0:
                return VFAttribTypes::Float;
                break;
            case 1:
                return sizeof( A1 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 2:
                return sizeof( A2 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 3:
                return sizeof( A3 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
        }
        // default
        return VFAttribTypes::Float;
    }

    static int size( uint64_t index ) {
        switch ( index ) {
            case 0:
                return sizeof( P ) / sizeof( float );
            case 1:
                return sizeof( A1 ) == 1 ? 1 : sizeof( A1 ) / sizeof( float );
                break;
            case 2:
                return sizeof( A2 ) == 1 ? 1 : sizeof( A2 ) / sizeof( float );
                break;
            case 3:
                return sizeof( A3 ) == 1 ? 1 : sizeof( A3 ) / sizeof( float );
                break;
        }
        // default
        return sizeof( P ) / sizeof( float );
    }

    static uintptr_t offset( uint64_t index ) {
        switch ( index ) {
            case 0:
                return 0;
                break;
            case 1:
                return sizeof( P );
                break;
            case 2:
                return sizeof( P ) + sizeof( A1 );
                break;
            case 3:
                return sizeof( P ) + sizeof( A1 ) + sizeof( A2 );
                break;
        }
        // default
        return 0;
    }
};

template<typename P, typename A1, typename A2, typename A3, typename A4>
struct VFA5 {
    P pos;
    A1 a1;
    A2 a2;
    A3 a3;
    A4 a4;

    VFA5() {}

    VFA5( const P& _p, const A1& _t, const A2& _t2, const A3& _t3, const A4& _t4 ) {
        pos = _p;
        a1 = _t;
        a2 = _t2;
        a3 = _t3;
        a4 = _t4;
    }

    static int numElements() { return 5; }

    static int stride() { return sizeof( P ) + sizeof( A1 ) + sizeof( A2 ) + sizeof( A3 ) + sizeof( A4 ); }

    static VFAttribTypes type( uint64_t index ) {
        switch ( index ) {
            case 0:
                return VFAttribTypes::Float;
                break;
            case 1:
                return sizeof( A1 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 2:
                return sizeof( A2 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 3:
                return sizeof( A3 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 4:
                return sizeof( A4 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
        }
        // default
        return VFAttribTypes::Float;
    }

    static int size( uint64_t index ) {
        switch ( index ) {
            case 0:
                return sizeof( P ) / sizeof( float );
            case 1:
                return sizeof( A1 ) == 1 ? 1 : sizeof( A1 ) / sizeof( float );
                break;
            case 2:
                return sizeof( A2 ) == 1 ? 1 : sizeof( A2 ) / sizeof( float );
                break;
            case 3:
                return sizeof( A3 ) == 1 ? 1 : sizeof( A3 ) / sizeof( float );
                break;
            case 4:
                return sizeof( A4 ) == 1 ? 1 : sizeof( A4 ) / sizeof( float );
                break;
        }
        // default
        return sizeof( P ) / sizeof( float );
    }

    static uintptr_t offset( uint64_t index ) {
        switch ( index ) {
            case 0:
                return 0;
                break;
            case 1:
                return sizeof( P );
                break;
            case 2:
                return sizeof( P ) + sizeof( A1 );
                break;
            case 3:
                return sizeof( P ) + sizeof( A1 ) + sizeof( A2 );
                break;
            case 4:
                return sizeof( P ) + sizeof( A1 ) + sizeof( A2 ) + sizeof( A3 );
                break;
        }
        // default
        return 0;
    }
};

template<typename P, typename A1, typename A2, typename A3, typename A4, typename A5>
struct VFA6 {
    P pos;
    A1 a1;
    A2 a2;
    A3 a3;
    A4 a4;
    A5 a5;

    VFA6() {}

    VFA6( const P& _p, const A1& _t, const A2& _t2, const A3& _t3, const A4& _t4, const A5& _t5 ) {
        pos = _p;
        a1 = _t;
        a2 = _t2;
        a3 = _t3;
        a4 = _t4;
        a5 = _t5;
    }

    static int numElements() { return 6; }

    static int stride() {
        return sizeof( P ) + sizeof( A1 ) + sizeof( A2 ) + sizeof( A3 ) + sizeof( A4 ) + sizeof( A5 );
    }

    static VFAttribTypes type( uint64_t index ) {
        switch ( index ) {
            case 0:
                return VFAttribTypes::Float;
                break;
            case 1:
                return sizeof( A1 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 2:
                return sizeof( A2 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 3:
                return sizeof( A3 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 4:
                return sizeof( A4 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 5:
                return sizeof( A5 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
        }
        // default
        return VFAttribTypes::Float;
    }

    static int size( uint64_t index ) {
        switch ( index ) {
            case 0:
                return sizeof( P ) / sizeof( float );
            case 1:
                return sizeof( A1 ) == 1 ? 1 : sizeof( A1 ) / sizeof( float );
                break;
            case 2:
                return sizeof( A2 ) == 1 ? 1 : sizeof( A2 ) / sizeof( float );
                break;
            case 3:
                return sizeof( A3 ) == 1 ? 1 : sizeof( A3 ) / sizeof( float );
                break;
            case 4:
                return sizeof( A4 ) == 1 ? 1 : sizeof( A4 ) / sizeof( float );
                break;
            case 5:
                return sizeof( A5 ) == 1 ? 1 : sizeof( A5 ) / sizeof( float );
                break;
        }
        // default
        return sizeof( P ) / sizeof( float );
    }

    static uintptr_t offset( uint64_t index ) {
        switch ( index ) {
            case 0:
                return 0;
                break;
            case 1:
                return sizeof( P );
                break;
            case 2:
                return sizeof( P ) + sizeof( A1 );
                break;
            case 3:
                return sizeof( P ) + sizeof( A1 ) + sizeof( A2 );
                break;
            case 4:
                return sizeof( P ) + sizeof( A1 ) + sizeof( A2 ) + sizeof( A3 );
                break;
            case 5:
                return sizeof( P ) + sizeof( A1 ) + sizeof( A2 ) + sizeof( A3 ) + sizeof( A4 );
                break;
        }
        // default
        return 0;
    }
};

template<typename P, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct VFA7 {
    P pos;
    A1 a1;
    A2 a2;
    A3 a3;
    A4 a4;
    A5 a5;
    A6 a6;

    VFA7() {}

    VFA7( const P& _p, const A1& _t, const A2& _t2, const A3& _t3, const A4& _t4, const A5& _t5, const A6& _t6 ) {
        pos = _p;
        a1 = _t;
        a2 = _t2;
        a3 = _t3;
        a4 = _t4;
        a5 = _t5;
        a6 = _t6;
    }

    static int numElements() { return 7; }

    static int stride() {
        return sizeof( P ) + sizeof( A1 ) + sizeof( A2 ) + sizeof( A3 ) + sizeof( A4 ) + sizeof( A5 ) + sizeof( A6 );
    }

    static VFAttribTypes type( uint64_t index ) {
        switch ( index ) {
            case 0:
                return VFAttribTypes::Float;
                break;
            case 1:
                return sizeof( A1 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 2:
                return sizeof( A2 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 3:
                return sizeof( A3 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 4:
                return sizeof( A4 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 5:
                return sizeof( A5 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 6:
                return sizeof( A6 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
        }
        // default
        return VFAttribTypes::Float;
    }

    static int size( uint64_t index ) {
        switch ( index ) {
            case 0:
                return sizeof( P ) / sizeof( float );
            case 1:
                return sizeof( A1 ) == 1 ? 1 : sizeof( A1 ) / sizeof( float );
                break;
            case 2:
                return sizeof( A2 ) == 1 ? 1 : sizeof( A2 ) / sizeof( float );
                break;
            case 3:
                return sizeof( A3 ) == 1 ? 1 : sizeof( A3 ) / sizeof( float );
                break;
            case 4:
                return sizeof( A4 ) == 1 ? 1 : sizeof( A4 ) / sizeof( float );
                break;
            case 5:
                return sizeof( A5 ) == 1 ? 1 : sizeof( A5 ) / sizeof( float );
                break;
            case 6:
                return sizeof( A6 ) == 1 ? 1 : sizeof( A6 ) / sizeof( float );
                break;
        }
        // default
        return sizeof( P ) / sizeof( float );
    }

    static uintptr_t offset( uint64_t index ) {
        switch ( index ) {
            case 0:
                return 0;
                break;
            case 1:
                return sizeof( P );
                break;
            case 2:
                return sizeof( P ) + sizeof( A1 );
                break;
            case 3:
                return sizeof( P ) + sizeof( A1 ) + sizeof( A2 );
                break;
            case 4:
                return sizeof( P ) + sizeof( A1 ) + sizeof( A2 ) + sizeof( A3 );
                break;
            case 5:
                return sizeof( P ) + sizeof( A1 ) + sizeof( A2 ) + sizeof( A3 ) + sizeof( A4 );
                break;
            case 6:
                return sizeof( P ) + sizeof( A1 ) + sizeof( A2 ) + sizeof( A3 ) + sizeof( A4 ) + sizeof( A5 );
                break;
        }
        // default
        return 0;
    }
};

template<typename P, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7,
        typename A8>
struct VFA8 {
    P pos;
    A1 a1;
    A2 a2;
    A3 a3;
    A4 a4;
    A5 a5;
    A6 a6;
    A7 a7;
    A8 a8;

    VFA8() = default;

    VFA8( const P& _p, const A1& _t, const A2& _t2, const A3& _t3, const A4& _t4, const A5& _t5, const A6& _t6,
          const A7& _t7, const A8& _t8) {
        pos = _p;
        a1 = _t;
        a2 = _t2;
        a3 = _t3;
        a4 = _t4;
        a5 = _t5;
        a6 = _t6;
        a7 = _t7;
        a8 = _t8;
    }

    static int numElements() { return 9; }

    static int stride() {
        return sizeof( P ) + sizeof( A1 ) + sizeof( A2 ) + sizeof( A3 ) + sizeof( A4 ) + sizeof( A5 ) + sizeof( A6 ) +
               sizeof( A7 ) + sizeof( A8 );
    }

    static VFAttribTypes type( uint64_t index ) {
        switch ( index ) {
            case 0:
                return VFAttribTypes::Float;
                break;
            case 1:
                return sizeof( A1 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 2:
                return sizeof( A2 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 3:
                return sizeof( A3 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 4:
                return sizeof( A4 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 5:
                return sizeof( A5 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 6:
                return sizeof( A6 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 7:
                return sizeof( A7 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
            case 8:
                return sizeof( A8 ) == 1 ? VFAttribTypes::Byte : VFAttribTypes::Float;
                break;
        }
        // default
        return VFAttribTypes::Float;
    }

    static int size( uint64_t index ) {
        switch ( index ) {
            case 0:
                return sizeof( P ) / sizeof( float );
            case 1:
                return sizeof( A1 ) == 1 ? 1 : sizeof( A1 ) / sizeof( float );
                break;
            case 2:
                return sizeof( A2 ) == 1 ? 1 : sizeof( A2 ) / sizeof( float );
                break;
            case 3:
                return sizeof( A3 ) == 1 ? 1 : sizeof( A3 ) / sizeof( float );
                break;
            case 4:
                return sizeof( A4 ) == 1 ? 1 : sizeof( A4 ) / sizeof( float );
                break;
            case 5:
                return sizeof( A5 ) == 1 ? 1 : sizeof( A5 ) / sizeof( float );
                break;
            case 6:
                return sizeof( A6 ) == 1 ? 1 : sizeof( A6 ) / sizeof( float );
                break;
            case 7:
                return sizeof( A7 ) == 1 ? 1 : sizeof( A7 ) / sizeof( float );
                break;
            case 8:
                return sizeof( A8 ) == 1 ? 1 : sizeof( A8 ) / sizeof( float );
                break;
        }
        // default
        return sizeof( P ) / sizeof( float );
    }

    static uintptr_t offset( uint64_t index ) {
        switch ( index ) {
            case 0:
                return 0;
                break;
            case 1:
                return sizeof( P );
                break;
            case 2:
                return sizeof( P ) + sizeof( A1 );
                break;
            case 3:
                return sizeof( P ) + sizeof( A1 ) + sizeof( A2 );
                break;
            case 4:
                return sizeof( P ) + sizeof( A1 ) + sizeof( A2 ) + sizeof( A3 );
                break;
            case 5:
                return sizeof( P ) + sizeof( A1 ) + sizeof( A2 ) + sizeof( A3 ) + sizeof( A4 );
                break;
            case 6:
                return sizeof( P ) + sizeof( A1 ) + sizeof( A2 ) + sizeof( A3 ) + sizeof( A4 ) + sizeof( A5 );
                break;
            case 7:
                return sizeof( P ) + sizeof( A1 ) + sizeof( A2 ) + sizeof( A3 ) + sizeof( A4 ) + sizeof( A5 ) +
                       sizeof( A6 );
                break;
            case 8:
                return sizeof( P ) + sizeof( A1 ) + sizeof( A2 ) + sizeof( A3 ) + sizeof( A4 ) + sizeof( A5 ) +
                        + sizeof( A6 ) + sizeof( A7 );
                break;
        }
        // default
        return 0;
    }
};

template<class V>
class VertexStripIBVB {
public:
    VertexStripIBVB() = default;

    VertexStripIBVB( const VertexStripIBVB& cc ) {
        init( cc.numVerts, cc.primiteType, cc.numIndices, cc.verts, cc.indices );
    }

    VertexStripIBVB( Primitive _primitiveType ) {
        primiteType = _primitiveType;
    }

    VertexStripIBVB( int32_t _numVerts, Primitive _primitiveType, VFVertexAllocation vAlloc ) {
        std::unique_ptr<uint32_t[]> _indices;
        std::unique_ptr<V[]> _verts;
        init( _numVerts, _primitiveType, 0, _verts, std::move(_indices), vAlloc );
    }

    VertexStripIBVB( int32_t _numVerts, int32_t _numIndices, Primitive _primitiveType, VFVertexAllocation vAlloc ) {
        std::unique_ptr<uint32_t[]> _indices;
        std::unique_ptr<V[]> _verts;
        init( _numVerts, _primitiveType, _numIndices, _verts, std::move(_indices), vAlloc );
    }

    VertexStripIBVB( int32_t _numVerts, Primitive _primitiveType, VFVertexAllocation vAlloc, int32_t _numIndices,
                     std::unique_ptr<uint32_t[]>&& _indices ) {
        std::unique_ptr<V[]> _verts;
        init( _numVerts, _primitiveType, _numIndices, _verts, std::move(_indices), vAlloc );
    }

    VertexStripIBVB( int32_t _numVerts, Primitive _primitiveType, std::unique_ptr<V[]> _verts ) {
        std::unique_ptr<uint32_t[]> _indices;
        init( _numVerts, _primitiveType, 0, _verts, std::move(_indices) );
    }

    VertexStripIBVB( int32_t _numVerts, Primitive _primitiveType ) {
        std::unique_ptr<uint32_t[]> _indices;
        std::unique_ptr<V[]> _verts;
        init( _numVerts, _primitiveType, 0, _verts, std::move(_indices) );
    }

    VertexStripIBVB( int32_t _numVerts, Primitive _pt, int32_t _numIndices, std::unique_ptr<V[]>& _verts,
                     std::unique_ptr<uint32_t[]>&& _indices, VFVertexAllocation vAlloc = VFVertexAllocation::Ignore ) {
        init( _numVerts, _pt, _numIndices, _verts, std::move(_indices), vAlloc );
    }

    VertexStripIBVB( const JMATH::Rect2f& rect ) {
        std::unique_ptr<uint32_t[]> _indices = std::unique_ptr<uint32_t[]>( new uint32_t[4]{ 0, 1, 2, 3 } );
        std::unique_ptr<V[]> _verts = std::unique_ptr<V[]>( new V[4] );

        _verts[0].pos = Vector2f( rect.bottomRight());
        _verts[1].pos = Vector2f( rect.topRight());
        _verts[2].pos = Vector2f( rect.bottomLeft());
        _verts[3].pos = Vector2f( rect.topLeft());

        init( 4, PRIMITIVE_TRIANGLE_STRIP, 4, _verts, std::move(_indices) );
    }

    VertexStripIBVB( const JMATH::Rect2f& rect, float z ) {
        std::unique_ptr<uint32_t[]> _indices = std::unique_ptr<uint32_t[]>( new uint32_t[4]{ 0, 1, 2, 3 } );
        std::unique_ptr<V[]> _verts = std::unique_ptr<V[]>( new V[4] );

        _verts[0].pos = XZY::C( rect.bottomRight() );
        _verts[1].pos = XZY::C( rect.topRight());
        _verts[2].pos = XZY::C( rect.bottomLeft() );
        _verts[3].pos = XZY::C( rect.topLeft() );

        init( 4, PRIMITIVE_TRIANGLE_STRIP, 4, _verts, std::move(_indices) );
    }

    VertexStripIBVB( const JMATH::Rect2f& rect, const QuadVertices2& tcoords, [[maybe_unused]] float z ) {
        std::unique_ptr<uint32_t[]> _indices = std::unique_ptr<uint32_t[]>( new uint32_t[4]{ 0, 1, 2, 3 } );
        std::unique_ptr<V[]> _verts = std::unique_ptr<V[]>( new V[4] );

        _verts[0].pos = XZY::C( rect.bottomRight() );
        _verts[1].pos = XZY::C( rect.topRight() );
        _verts[2].pos = XZY::C( rect.bottomLeft() );
        _verts[3].pos = XZY::C( rect.topLeft() );
        _verts[0].a1 = tcoords[0];// Vector2f(1.0f, 0.0f);
        _verts[1].a1 = tcoords[1];//Vector2f(1.0f, 1.0f);
        _verts[2].a1 = tcoords[2];//Vector2f(0.0f, 0.0f);
        _verts[3].a1 = tcoords[3];//Vector2f(0.0f, 1.0f);

        init( 4, PRIMITIVE_TRIANGLE_STRIP, 4, _verts, std::move(_indices) );
    }

    VertexStripIBVB( const JMATH::Rect2f& rect, const QuadVertices2& tcoords ) {
        std::unique_ptr<uint32_t[]> _indices = std::unique_ptr<uint32_t[]>( new uint32_t[4]{ 0, 1, 2, 3 } );
        std::unique_ptr<V[]> _verts = std::unique_ptr<V[]>( new V[4] );

        _verts[0].pos = Vector2f( rect.bottomRight());
        _verts[0].a1 = tcoords[0];// Vector2f(1.0f, 0.0f);
        _verts[1].pos = Vector2f( rect.topRight());
        _verts[1].a1 = tcoords[1];//Vector2f(1.0f, 1.0f);
        _verts[2].pos = Vector2f( rect.bottomLeft());
        _verts[2].a1 = tcoords[2];//Vector2f(0.0f, 0.0f);
        _verts[3].pos = Vector2f( rect.topLeft());
        _verts[3].a1 = tcoords[3];//Vector2f(0.0f, 1.0f);

        init( 4, PRIMITIVE_TRIANGLE_STRIP, 4, _verts, std::move(_indices) );
    }

    VertexStripIBVB( const JMATH::Rect2f& rect, const QuadVertices2& tcoords, const QuadVertices3& tcoords2 ) {
        std::unique_ptr<uint32_t[]> _indices = std::unique_ptr<uint32_t[]>( new uint32_t[4]{ 0, 1, 2, 3 } );
        std::unique_ptr<V[]> _verts = std::unique_ptr<V[]>( new V[4] );

        _verts[0].pos = Vector2f( rect.bottomRight());
        _verts[0].a1 = tcoords[0];// Vector2f(1.0f, 0.0f);
        _verts[0].a2 = tcoords2[0];// Vector2f(1.0f, 0.0f);
        _verts[1].pos = Vector2f( rect.topRight());
        _verts[1].a1 = tcoords[1];//Vector2f(1.0f, 1.0f);
        _verts[1].a2 = tcoords2[1];//Vector2f(1.0f, 1.0f);
        _verts[2].pos = Vector2f( rect.bottomLeft());
        _verts[2].a1 = tcoords[2];//Vector2f(0.0f, 0.0f);
        _verts[2].a2 = tcoords2[2];//Vector2f(0.0f, 0.0f);
        _verts[3].pos = Vector2f( rect.topLeft());
        _verts[3].a1 = tcoords[3];//Vector2f(0.0f, 1.0f);
        _verts[3].a2 = tcoords2[3];//Vector2f(0.0f, 1.0f);

        init( 4, PRIMITIVE_TRIANGLE_STRIP, 4, _verts, std::move(_indices) );
    }

    Vector3f centre() const {
        AABB ret{AABB::INVALID};
        for ( int32_t t = 0; t < numVerts; t++ ) ret.expand( verts[t].pos );
        return ret.calcCentre();
    }

    AABB BBox3d() const {
        AABB ret{AABB::INVALID};
        for ( int32_t t = 0; t < numVerts; t++ ) ret.expand( verts[t].pos );
        return ret;
    }

    void translate( const Vector3f& pos ) {
        for ( int32_t t = 0; t < numVerts; t++ ) verts[t].pos += pos;
    }

    void rotate( const Matrix2f& _rmat ) {
        for ( int32_t t = 0; t < numVerts; t++ ) {
            verts[t].pos = XZY::C({_rmat.transform(verts[t].pos.xz()), verts[t].pos.y()});
        }
    }

    void transform( const Matrix4f& _mat ) {
        for ( auto t = 0u; t < numVerts; t++ ) verts[t].pos = _mat.transform(verts[t].pos );
    }

    void generateStripsFromVerts( const std::vector<Vector3f>& vList, Primitive _prim ) {
        if ( vList.size() < 3 ) return;

        numVerts = static_cast<uint32_t >(vList.size());
        verts = std::make_unique<V[]>( numVerts );
        primiteType = _prim;

        size_t vc = 0;
        for ( auto& v : vList ) {
            verts[vc++].pos = v;
        }
    }

    void generateTriangleListFromVerts( const std::vector<Vector3f>& vList ) {
        if ( vList.size() < 3 ) return;

        int32_t startIndex = numVerts;

        for ( auto& v : vList ) {
            addVertex( v );
        }

        bool wind = true;
        for ( size_t triIndex = 2; triIndex < vList.size(); triIndex++ ) {
            incTriList( startIndex + triIndex-2, wind );
        }

    }

    void incTriList( int _localTriIndex, bool& wind ) {
        if ( wind ) {
            indices[numIndices] = _localTriIndex;
            indices[numIndices + 1] = _localTriIndex + 1;
            indices[numIndices + 2] = _localTriIndex + 2;
        } else {
            indices[numIndices] = _localTriIndex;
            indices[numIndices + 1] = _localTriIndex + 2;
            indices[numIndices + 2] = _localTriIndex + 1;
        }
        numIndices+=3;
        wind = !wind;
    }

    int32_t addVertex( const V& p ) {
        ASSERT( verts != nullptr );
        if ( verts == nullptr ) return 0;
        verts[numVerts] = p;
        ++numVerts;
        return numVerts;
    }

    template<typename P>
    int32_t addVertex( const P& p ) {
        ASSERT( verts != nullptr );
        if ( verts == nullptr ) return 0;
        verts[numVerts].pos = p;
        ++numVerts;
        return numVerts;
    }

    template<typename P, typename A1>
    int32_t addVertex( const P& p, const A1& a1 ) {
        ASSERT( verts != nullptr );
        if ( verts == nullptr ) return 0;
        verts[numVerts].pos = p;
        verts[numVerts].a1 = a1;
        ++numVerts;
        return numVerts;
    }

    template<typename P, typename A1, typename A2>
    int32_t addVertex( const P& p, const A1& a1, const A2& a2 ) {
        ASSERT( verts != nullptr );
        if ( verts == nullptr ) return 0;
        verts[numVerts].pos = p;
        verts[numVerts].a1 = a1;
        verts[numVerts].a2 = a2;
        ++numVerts;
        return numVerts;
    }

    template<typename P, typename A1, typename A2, typename A3>
    int32_t addVertex( const P& p, const A1& a1, const A2& a2, const A3& a3 ) {
        ASSERT( verts != nullptr );
        if ( verts == nullptr ) return 0;
        verts[numVerts].pos = p;
        verts[numVerts].a1 = a1;
        verts[numVerts].a2 = a2;
        verts[numVerts].a3 = a3;
        ++numVerts;
        return numVerts;
    }

    template<typename P, typename A1, typename A2, typename A3, typename A4>
    int32_t addVertex( const P& p, const A1& a1, const A2& a2, const A3& a3, const A4& a4 ) {
        ASSERT( verts != nullptr );
        if ( verts == nullptr ) return 0;
        verts[numVerts].pos = p;
        verts[numVerts].a1 = a1;
        verts[numVerts].a2 = a2;
        verts[numVerts].a3 = a3;
        verts[numVerts].a4 = a4;
        ++numVerts;
        return numVerts;
    }

    template<typename P, typename A1, typename A2, typename A3, typename A4, typename A5>
    int32_t addVertex( const P& p, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5 ) {
        ASSERT( verts != nullptr );
        if ( verts == nullptr ) return 0;
        verts[numVerts].pos = p;
        verts[numVerts].a1 = a1;
        verts[numVerts].a2 = a2;
        verts[numVerts].a3 = a3;
        verts[numVerts].a4 = a4;
        verts[numVerts].a5 = a5;
        ++numVerts;
        return numVerts;
    }

    template<typename P, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
    int32_t
    addVertex( const P& p, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6 ) {
        ASSERT( verts != nullptr );
        if ( verts == nullptr ) return 0;
        verts[numVerts].pos = p;
        verts[numVerts].a1 = a1;
        verts[numVerts].a2 = a2;
        verts[numVerts].a3 = a3;
        verts[numVerts].a4 = a4;
        verts[numVerts].a5 = a5;
        verts[numVerts].a6 = a6;
        ++numVerts;
        return numVerts;
    }

    template<typename P, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
    int32_t addVertex( const P& p, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6,
                       const A7& a7 ) {
        ASSERT( verts != nullptr );
        if ( verts == nullptr ) return 0;
        verts[numVerts].pos = p;
        verts[numVerts].a1 = a1;
        verts[numVerts].a2 = a2;
        verts[numVerts].a3 = a3;
        verts[numVerts].a4 = a4;
        verts[numVerts].a5 = a5;
        verts[numVerts].a6 = a6;
        verts[numVerts].a7 = a7;
        ++numVerts;
        return numVerts;
    }

    template<typename P, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7,
            typename A8>
    int32_t addVertex( const P& p, const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6,
                       const A7& a7, const A8& a8 ) {
        ASSERT( verts != nullptr );
        if ( verts == nullptr ) return 0;
        verts[numVerts].pos = p;
        verts[numVerts].a1 = a1;
        verts[numVerts].a2 = a2;
        verts[numVerts].a3 = a3;
        verts[numVerts].a4 = a4;
        verts[numVerts].a5 = a5;
        verts[numVerts].a6 = a6;
        verts[numVerts].a7 = a7;
        verts[numVerts].a8 = a8;
        ++numVerts;
        return numVerts;
    }

    template<typename P>
    int32_t addStripVertex( const P& p ) {
        indices[numIndices] = numIndices;
        verts[numIndices].pos = p;
        ++numIndices;
        ++numVerts;
        return numIndices;
    }

    template<typename P, typename A>
    int32_t addStripVertex( const P& p, const A& a ) {
        indices[numIndices] = numIndices;
        verts[numIndices].pos = p;
        verts[numIndices].a1 = a;
        ++numIndices;
        ++numVerts;
        return numIndices;
    }

    template<typename P, typename A>
    void setVertex( int32_t index, const P& p, const A& a ) {
        ASSERT( numVerts > index );
        verts[index].pos = p;
        verts[index].a1 = a;
    }

    template<typename A>
    void setVertexUV1( uint32_t index, const A& a ) {
        ASSERT( numVerts > index );
        verts[index].a1 = a;
    }

    void init( int32_t _numVerts, Primitive _pt, int32_t _numIndices, std::unique_ptr<V[]>& _verts,
               std::unique_ptr<uint32_t[]>&& _indices, VFVertexAllocation vAlloc = VFVertexAllocation::Ignore ) {
        ASSERT( _numVerts > 0 );

        numIndices = _indices ? _numIndices : 0;
        numVerts = vAlloc == VFVertexAllocation::PreAllocate ? 0 : _numVerts;
        primiteType = _pt;

        if ( _verts ) {
            verts = std::move( _verts );
        } else {
            verts = std::make_unique<V[]>( _numVerts );
        }
        if ( _indices ) {
            indices = std::move( _indices );
        } else {
            indices = std::make_unique<uint32_t[]>( _numIndices );
        }
    }

    bool hasGotIB() const {
        return numIndices > 0;
    }

    static std::unique_ptr<V[]> vtoVF( std::unique_ptr<Vector3f[]>& _verts, int _numVerts ) {
        std::unique_ptr<V[]> retArray = std::unique_ptr<V[]>( new V[_numVerts] );
        for ( int t = 0; t < _numVerts; t++ ) {
            retArray[t].pos = _verts[t];
        }
        return retArray;
    }

    std::unique_ptr<uint32_t[]> indices;
    std::unique_ptr<V[]> verts;

    uint32_t numIndices = 0;
    uint32_t numVerts = 0;

    Primitive primiteType = PRIMITIVE_TRIANGLE_STRIP;
};

typedef VFA1<Vector2f> VFPos2d;
typedef VFA1<Vector3f> VFPos3d;
typedef VFA1<Vector4f> VFPos4d;

typedef VFA2<Vector2f, Vector2f> VFPosTex2d;
typedef VFA2<Vector3f, Vector2f> VFPosTex3d;
typedef VFA2<Vector4f, Vector2f> VFPosTex4d;

typedef VFA3<Vector2f, Vector2f, Vector3f> VFPosTexNor2d;
typedef VFA3<Vector3f, Vector2f, Vector3f> VFPosTexNor3d;

typedef VFA2<Vector2f, Vector4f> VFPosCol2d;
typedef VFA2<Vector3f, Vector4f> VFPosCol3d;
typedef VFA2<Vector4f, Vector4f> VFPosCol4d;

typedef VFA4<Vector2f, Vector2f, Vector3f, Vector3f> VFPosTexNorTan2d;
typedef VFA4<Vector3f, Vector2f, Vector3f, Vector3f> VFPosTexNorTan3d;
typedef VFA4<Vector4f, Vector2f, Vector3f, Vector3f> VFPosTexNorTan4d;

typedef VFA5<Vector3f, Vector2f, Vector3f, Vector3f, Vector3f> VFPosTexNorTanBin3d;
typedef VFA6<Vector3f, Vector2f, Vector3f, Vector3f, Vector3f, Vector4f> VFPosTexNorTanBinCol3d;
typedef VFA6<Vector3f, Vector2f, Vector3f, Vector3f, Vector3f, Matrix3f> VFPosTexNorTanBinSH3d;

typedef VFA7<Vector3f, Vector2f, Vector2f, Vector3f, Vector4f, Vector3f, Vector4f> PosTexNorTanBinUV2Col3d;

typedef VFA8<Vector3f, Vector2f, Vector3f, Vector3f, Vector3f, Matrix3f, Matrix3f, Matrix3f, Vector4f>
        PosTexNorTanBinSHCol3d;

typedef VFA2<Vector3f, V2f> VFFont;

typedef VertexStripIBVB<VFPos2d> Pos2dStrip;
typedef VertexStripIBVB<VFPos3d> Pos3dStrip;
typedef VertexStripIBVB<VFPos4d> Pos4dStrip;

typedef VertexStripIBVB<VFPosTex2d> PosTex2dStrip;
typedef VertexStripIBVB<VFPosTex3d> PosTex3dStrip;
typedef VertexStripIBVB<VFPosTex4d> PosTex4dStrip;

typedef VertexStripIBVB<VFPosTexNor2d> PosTexNor2dStrip;
typedef VertexStripIBVB<VFPosTexNor3d> PosTexNor3dStrip;

typedef VertexStripIBVB<VFPosCol2d> PosCol2dStrip;
typedef VertexStripIBVB<VFPosCol3d> PosCol3dStrip;
typedef VertexStripIBVB<VFPosCol4d> PosCol4dStrip;

typedef VertexStripIBVB<VFPosTexNorTan2d> PosTexNorTan2dStrip;
typedef VertexStripIBVB<VFPosTexNorTan3d> PosTexNorTan3dStrip;
typedef VertexStripIBVB<VFPosTexNorTanBin3d> PosTexNorTanBin3dStrip;
typedef VertexStripIBVB<VFPosTexNorTan4d> PosTexNorTan4dStrip;

typedef VertexStripIBVB<VFPosTexNorTanBinCol3d> PosTexNorTanBinCol3dStrip;
typedef VertexStripIBVB<VFPosTexNorTanBinSH3d> PosTexNorTanBinSH3dStrip;
typedef VertexStripIBVB<PosTexNorTanBinSHCol3d> PosTexNorTanBinSHCol3dStrip;
typedef VertexStripIBVB<PosTexNorTanBinUV2Col3d> PosTexNorTanBinUV2Col3dStrip;

using PUUNTBC = PosTexNorTanBinUV2Col3d;

typedef VertexStripIBVB<VFFont> FontStrip;
