//
//  shapes.hpp
//  6thView
//
//  Created by Dado on 99/99/1970.
//
//

#pragma once
#include "poly_utils.hpp"

enum class ShapeType {
	None,
	Sphere,
	Cylinder,
	Pyramid,
	Cube,
	Pillow,
	Arrow
};

PolyStruct createGeomForSphere( const Vector3f& center, const float diameter, const int subdivs );
PolyStruct createGeomForCube( const Vector3f& center, const Vector3f& size );
PolyStruct createGeomForPillow( const Vector3f& center, const Vector3f& size, const int subdivs = 5, float radius = 0.15f );