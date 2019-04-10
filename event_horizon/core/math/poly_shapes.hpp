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

PolyStruct createGeomForSphere( const Vector3f& center, const float diameter, const int subdivs );
PolyStruct createGeomForCylinder( const Vector3f& center, const Vector2f& size, const int subdivs = 1 );
PolyStruct createGeomForCube( const Vector3f& center, const Vector3f& size );
PolyStruct createGeomForPanel( const Vector3f& center, const Vector3f& size );
PolyStruct createGeomForPillow( const Vector3f& center, const Vector3f& size, const int subdivs = 5, float radius = 0.15f );
PolyStruct createGeomForRoundedCube( const Vector3f& center, const Vector3f& size, const int subdivs = 5, float radius = 0.05f );

ShapeType shapeTypeFromString( const std::string& value );
std::string shapeTypeToString( ShapeType value );