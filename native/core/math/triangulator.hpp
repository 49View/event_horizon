//
//  triangulator.hpp
//  sixthmaker
//
//  Created by Dado on 11/03/2017.
//
//

#pragma once

#include <map>
#include <memory>

#include "core/math/vector3f.h"
#include "poly2tri/poly2tri.h"

class Triangulator {
public:
	explicit Triangulator( const vector2fList& _verts, float _accuracy = 0.001f );
	Triangulator( const vector2fList& _verts, const std::vector<Vector2fList>& _holes, float _accuracy = 0.001f );
	Triangulator( const vector3fList& _verts, const Vector3f& normal, float _accuracy = 0.001f );
	Triangulator( const Vector3f* verts, size_t vsize, const Vector3f& normal, float _accuracy = 0.001f );
    virtual ~Triangulator();
    void gather3dTriangularizationWithNeighbours( std::vector<std::vector<Vector2f>>& tri_neighbours );

	std::vector<vector2fList>& get2dTrianglesList();
	std::vector<Triangle2d>& get2dTrianglesTuple();

	std::vector<vector3fList>& get3dTrianglesList();
	std::vector<Triangle3d>& get3dTrianglesTuple();

private:
	void init2d( const Vector2f* _verts, int vsize );
	void init3d( const Vector2f* _verts, int vsize );

	void gather2dTriangularizationResult();
	void gather3dTriangularizationResult();
private:
	std::vector<Vector2fList> m2dTriangulationHoles;

	std::vector<Vector2fList> m2dTriangulation;
	std::vector<vector3fList> m3dTriangulation;

	std::vector<Triangle2d> m2dTriangulationTuples;
	std::vector<Triangle3d> m3dTriangulationTuples;

	std::map<int64_t, Vector3f> mVHash;
	IndexPair dominantPair;

	std::unique_ptr<p2t::CDT> mCDT;

public:
	static std::vector<Triangle2d> execute2d( const vector2fList& _verts, const std::vector<Vector2fList>& _holes, float _accuracy = 0.001f );
    static std::vector<Triangle3d> execute3d( const vector2fList& _verts, const std::vector<Vector2fList>& _holes, float zOff, float _accuracy = 0.001f );

    static std::vector<vector2fList> execute2dList( const vector2fList& _verts, const std::vector<Vector2fList>& _holes, float _accuracy = 0.001f );
    static std::vector<vector2fList> execute2dList( const vector2fList& _verts, float _accuracy = 0.001f );

    static std::vector<Triangle3d> setZTriangles( std::vector<Triangle3d>& _source, float zOff );
};

