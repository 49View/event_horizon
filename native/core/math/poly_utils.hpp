#pragma once

#include <memory>
#include <array>
#include "vector4f.h"
#include "rect2f.h"
#include "aabb.h"
#include "../htypes_shared.hpp"

class DeserializeBin;

enum class GeomMappingT {
    Spherical,
    SphericalUV,
    Cube,
    Planar,
    PlanarNoTile,
    Cylindrical,
    PreBaked
};

struct GeomMapping {
    GeomMapping( GeomMappingT _type ) : type(_type) {}
    GeomMapping( GeomMappingT _type, const V3f& _scaling ) : type(_type), scaling(_scaling) {}

    GeomMappingT type;
    V3f scaling = V3fc::ONE;
};

enum ReservedPolyTags {
    GT_Skybox = 1000,
    GT_Mesh,
};

struct polyQuadSub {
    std::array<Vector3f, 4> vcoords;
    std::array<Vector2f, 4> uvs;
    std::array<Vector3f, 4> normals;
};

enum class FlipSubdiv {
    True,
    False,
};

enum class PolyRaise {
    None,
    HorizontalPos,
    HorizontalNeg,
    VerticalPos,
    VerticalNeg
};

std::vector<polyQuadSub> quadSubDiv( const std::array<Vector3f, 4>& vss,
                                     const std::array<Vector2f, 4>& vtcs,
                                     const std::array<Vector3f, 4>& vns,
                                     subdivisionAccuracy _accuracy,
                                     FlipSubdiv subDivDir = FlipSubdiv::False );

std::vector<polyQuadSub> quadSubDiv( const std::array<Vector3f, 4>& vss,
                                     const std::array<Vector2f, 4>& vtcs,
                                     const Vector3f& nomal,
                                     subdivisionAccuracy _accuracy );

std::vector<polyQuadSub> quadSubDivInv( const std::array<Vector3f, 4>& vss,
                                        const std::array<Vector2f, 4>& vtcs,
                                        const Vector3f& nomal,
                                        subdivisionAccuracy _accuracy );

Vector3f utilMirrorFlip( const Vector3f& v, WindingOrderT wow, WindingOrderT woh, const Rect2f& bbox );
WindingOrderT detectWindingOrder( const Vector2f& pa, const Vector2f& pb, const Vector2f& pc );
WindingOrderT detectWindingOrder( const Vector3f& v1, const Vector3f& v2, const Vector3f& v3 );
WindingOrderT detectWindingOrderOpt2d( const V2fVector& vlist );

WindingOrderT detectWindingOrder( const std::vector<V2f>& _input );
WindingOrderT detectWindingOrder( const std::vector<V3f>& _input, const V3f& axis );

template <typename ...Args>
std::vector<V2f> forceWindingOrder( const std::vector<V2f>& _input, WindingOrderT _forcedWO, Args& ... args ) {

    std::vector<V2f> ret = _input;

    if ( detectWindingOrder( _input ) == _forcedWO ) return ret;
    std::reverse( std::begin(ret), std::end(ret) );
    (std::reverse( std::begin(args), std::end(args)), ...);
    return ret;
}

template <typename ...Args>
std::vector<V3f> forceWindingOrder( const std::vector<V3f>& _input, const V3f& axis,
                                   WindingOrderT _forcedWO, Args& ... args ) {

    std::vector<V3f> ret = _input;

    if ( detectWindingOrder( _input, axis ) == _forcedWO ) return ret;
    std::reverse( std::begin(ret), std::end(ret) );
    (std::reverse( std::begin(args), std::end(args)), ...);
    return ret;
}

std::vector<Vector3f> createQuadPoints( const Vector2f& size = V2fc::ONE );

template<typename T>
std::vector<T> lineRL( const T& direction, float length ) {
    return { direction*(length*0.5f), -direction*(length*0.5f) };
}

template<typename T>
std::vector<T> lineLR( const T& direction, float length ) {
    return { -direction*(length*0.5f), direction*(length*0.5f) };
}
