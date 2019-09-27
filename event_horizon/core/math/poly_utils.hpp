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
    Cylindrical
};

struct GeomMapping {
    GeomMapping( GeomMappingT _type ) : type(_type) {}
    GeomMapping( GeomMappingT _type, const V3f& _scaling ) : type(_type), scaling(_scaling) {}

    GeomMappingT type;
    V3f scaling = V3f::ONE;
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
                                     subdivisionAccuray _accuracy,
                                     FlipSubdiv subDivDir = FlipSubdiv::False );

std::vector<polyQuadSub> quadSubDiv(const std::array<Vector3f, 4>& vss,
                                      const std::array<Vector2f, 4>& vtcs,
                                      const Vector3f& nomal,
                                      subdivisionAccuray _accuracy );

std::vector<polyQuadSub> quadSubDivInv(const std::array<Vector3f, 4>& vss,
                                    const std::array<Vector2f, 4>& vtcs,
                                    const Vector3f& nomal,
                                    subdivisionAccuray _accuracy );

Vector3f utilMirrorFlip( const Vector3f& v, WindingOrderT wow, WindingOrderT woh, const Rect2f& bbox );
WindingOrderT detectWindingOrder( const Vector2f& pa, const Vector2f& pb, const Vector2f& pc );
WindingOrderT detectWindingOrder( const Vector3f& v1, const Vector3f& v2, const Vector3f& v3 );

template <typename VT>
WindingOrderT detectWindingOrder( const std::vector<VT>& _input ) {
    size_t i1, i2;
    float area = 0;
    for ( i1 = 0; i1 < _input.size(); i1++ ) {
        i2 = i1 + 1;
        if ( i2 == _input.size() ) i2 = 0;
        if constexpr ( std::is_same_v<VT, V2f> ) {
            area += _input[i1].x() * _input[i2].y() - _input[i1].y() * _input[i2].x();
        } else if constexpr ( std::is_same_v<VT, V3f> ) {
            area += _input[i1].x() * _input[i2].z() - _input[i1].y() * _input[i2].z();
        }
    }
    if ( area > 0 ) return WindingOrder::CW;
    if ( area < 0 ) return WindingOrder::CCW;
    LOGR("[ERROR] cannot get winding order of these points cos area is 0");
    return WindingOrder::CCW;
}

template <typename VT, typename ...Args>
std::vector<VT> forceWindingOrder( const std::vector<VT>& _input,
                                         WindingOrderT _forcedWO, Args& ... args ) {

    std::vector<VT> ret = _input;

    if ( detectWindingOrder( _input ) == _forcedWO ) return ret;
    std::reverse( std::begin(ret), std::end(ret) );
    (std::reverse( std::begin(args), std::end(args)), ...);
    return ret;
}

std::vector<Vector3f> createQuadPoints( const Vector2f& size = Vector2f::ONE );

template<typename T>
std::vector<T> lineRL( const T& direction, float length ) {
    return { direction*(length*0.5f), -direction*(length*0.5f) };
}

template<typename T>
std::vector<T> lineLR( const T& direction, float length ) {
    return { -direction*(length*0.5f), direction*(length*0.5f) };
}

float areaOf( const V2fVector& vtri );
