//
//  shapes.hpp
//  6thView
//
//  Created by Dado on 99/99/1970.
//
//

#pragma once

#include "poly_utils.hpp"

struct PolyStruct;

enum class ShapeType {
	None,
	Sphere,
	Cylinder,
	Pyramid,
	Cube,
	RoundedCube,
	Panel,
	Pillow,
	Arrow,
	AABB
};

struct Edge {
    uint32_t v0;
    uint32_t v1;

    Edge( uint32_t v0, uint32_t v1 )
            : v0( v0 < v1 ? v0 : v1 ), v1( v0 < v1 ? v1 : v0 ) {
    }

    bool operator<( const Edge& rhs ) const {
        return v0 < rhs.v0 || ( v0 == rhs.v0 && v1 < rhs.v1 );
    }
};

struct Topology {
    std::vector<Vector3f> vertices;
    std::vector<uint32_t> triangles;
    std::unordered_map<size_t, std::vector<Vector3f>> smoothing;
    std::vector<Vector3f> vertexNormals;
    std::vector<Vector4f> vertexUVs;
    std::vector<Vector4f> colors;

    uint32_t indexCount() const { return static_cast<uint32_t >( triangles.size()); }
    uint32_t triangleCount() const { return static_cast<uint32_t >( triangles.size() / 3 ); }
    uint32_t vertexCount() const { return static_cast<uint32_t >( vertices.size()); }

    Vector3f normalFromSmartSmoothing( uint32_t index, int vIndex );

    [[maybe_unused]] void addVertex( const V3f& _vertex );
    [[maybe_unused]] void addVertex( const V3f& _vertex, const V2f& _uv );
    [[maybe_unused]] void addVertex( const V3f& _vertex, const V4f& _uv );
    [[maybe_unused]] void addVertex( const V3f& _vertex, const V4f& _uv, const C4f& _color );
    [[maybe_unused]] void addVertexOfTriangle( const V3f& _vertex, const V4f& _uv, const C4f& _color );
    [[maybe_unused]] void addQuad( const std::vector<V3f>& _vertex, const std::vector<V4f>& _uv, const std::vector<C4f>& _color );
    void addTriangle( uint32_t a, uint32_t b, uint32_t c );
    void addQuad( uint32_t a, uint32_t b, uint32_t c, uint32_t d );
    void addQuadAlt( uint32_t a, uint32_t b, uint32_t c, uint32_t d );
    void clear();
    double distance( const Vector3f& p, uint32_t tidX ) const;
    double distance( const Vector3f& p ) const;
};

PolyStruct createGeomForSphere( const Vector3f& center, float diameter, int subdivs = 3, const C4f& color = C4fc::WHITE );
PolyStruct createGeomForCylinder( const Vector3f& center, const Vector2f& size, int subdivs = 1, const C4f& color = C4fc::WHITE );
PolyStruct createGeomForCone( const Vector3f& center, const Vector3f& size, int subdivs = 1, const C4f& color = C4fc::WHITE );
PolyStruct createGeomForCube( const Vector3f& center, const Vector3f& size, const C4f& color = C4fc::WHITE );
PolyStruct createGeomForAABB( const AABB& aabb, const C4f& color = C4fc::WHITE );
PolyStruct createGeomForPanel( const Vector3f& center, const Vector3f& size, const C4f& color = C4fc::WHITE );
PolyStruct createGeomForPillow( const Vector3f& center, const Vector3f& size, int subdivs = 5, float radius = 0.15f, const C4f& color = C4fc::WHITE );
PolyStruct createGeomForRoundedCube( const Vector3f& center, const Vector3f& size, int subdivs = 5, float radius = 0.05f, const C4f& color = C4fc::WHITE );

PolyStruct createGeom( Topology& mesh, const Vector3f& center = V3f::ZERO(), const Vector3f& size = V3fc::ONE, GeomMapping mt = GeomMapping{ GeomMappingT::PreBaked },
                       int subdivs = 0, ReverseFlag rf = ReverseFlag::False );

ShapeType shapeTypeFromString( const std::string& value );
std::string shapeTypeToString( ShapeType value );