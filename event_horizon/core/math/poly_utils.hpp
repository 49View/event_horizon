#pragma once

#include <memory>
#include <array>
#include "vector4f.h"
#include "rect2f.h"
#include "../htypes_shared.hpp"

class DeserializeBin;

enum class GeomMapping {
    Spherical,
    Cube,
    Planar,
    Cylindrical
};

enum ReservedPolyTags {
    GT_Skybox = 1000,
    GT_Mesh,
};

struct PolyStruct {
public:
    std::unique_ptr<Vector3f[]> verts;
    std::unique_ptr<Vector2f[]> uvs;
    std::unique_ptr<Vector2f[]> uv2s;
    std::unique_ptr<Vector3f[]> normals;
    std::unique_ptr<Vector3f[]> tangents;
    std::unique_ptr<Vector3f[]> binormals;
    std::unique_ptr<Vector4f[]> colors;
    std::unique_ptr<int32_t[]> indices;
    int numVerts = 0;
    int32_t numIndices = 0;
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

struct GeomDeserializeDependencies {
    std::vector<std::string> textureDeps;
    std::vector<std::string> materialDeps;
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
void tbCalc( const Vector3f& v1, const Vector3f& v2, const Vector3f& v3,
             const Vector2f& uv1, const Vector2f& uv2, const Vector2f& uv3,
             Vector3f& tangent1, Vector3f& tangent2, Vector3f& tangent3,
             Vector3f& bitangent1, Vector3f& bitangent2, Vector3f& bitangent3 );

std::vector<Vector3f> createQuadPoints( const Vector2f& size = Vector2f::ONE );

GeomDeserializeDependencies gatherGeomDependencies( std::shared_ptr<DeserializeBin> reader );

const std::vector<std::string>& pbrNames();

template<typename T>
std::vector<T> lineRL( const T& direction, float length ) {
    return { direction*(length*0.5f), -direction*(length*0.5f) };
}

template<typename T>
std::vector<T> lineLR( const T& direction, float length ) {
    return { -direction*(length*0.5f), direction*(length*0.5f) };
}