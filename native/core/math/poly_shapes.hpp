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
	Arrow
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

    uint32_t indexCount() const { return static_cast<uint32_t >( triangles.size()); }
    uint32_t triangleCount() const { return static_cast<uint32_t >( triangles.size() / 3 ); }
    uint32_t vertexCount() const { return static_cast<uint32_t >( vertices.size()); }

    Vector3f normalFromSmartSmoothing( uint32_t index, int vIndex );

    void addVertex( const V3f& _vertex );
    void addTriangle( uint32_t a, uint32_t b, uint32_t c );
    void addQuad( uint32_t a, uint32_t b, uint32_t c, uint32_t d );
    void addQuadAlt( uint32_t a, uint32_t b, uint32_t c, uint32_t d );
    void clear();
    double distance( const Vector3f& p, uint32_t tidx ) const;
    double distance( const Vector3f& p ) const;
};

PolyStruct createGeomForSphere( const Vector3f& center, const float diameter, const int subdivs );
PolyStruct createGeomForCylinder( const Vector3f& center, const Vector2f& size, const int subdivs = 1 );
PolyStruct createGeomForCube( const Vector3f& center, const Vector3f& size );
PolyStruct createGeomForPanel( const Vector3f& center, const Vector3f& size );
PolyStruct createGeomForPillow( const Vector3f& center, const Vector3f& size, const int subdivs = 5, float radius = 0.15f );
PolyStruct createGeomForRoundedCube( const Vector3f& center, const Vector3f& size, const int subdivs = 5, float radius = 0.05f );

PolyStruct createGeom( Topology& mesh, const Vector3f& size, GeomMapping mt,
                       int subdivs = 0, ReverseFlag rf = ReverseFlag::False );

ShapeType shapeTypeFromString( const std::string& value );
std::string shapeTypeToString( ShapeType value );