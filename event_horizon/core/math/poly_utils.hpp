#pragma once

#include <memory>
#include <array>
#include "vector4f.h"
#include "rect2f.h"
#include "aabb.h"
#include "../htypes_shared.hpp"

class DeserializeBin;

enum class GeomMapping {
    Spherical,
    SphericalUV,
    Cube,
    Planar,
    PlanarNoTile,
    Cylindrical
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
WindingOrderT detectWindingOrder( const std::vector<Vector2f>& _input );
std::vector<Vector2f> forceWindingOrder( const std::vector<Vector2f>& _input, WindingOrderT _forcedWO );

std::vector<Vector3f> createQuadPoints( const Vector2f& size = Vector2f::ONE );

template<typename T>
std::vector<T> lineRL( const T& direction, float length ) {
    return { direction*(length*0.5f), -direction*(length*0.5f) };
}

template<typename T>
std::vector<T> lineLR( const T& direction, float length ) {
    return { -direction*(length*0.5f), direction*(length*0.5f) };
}