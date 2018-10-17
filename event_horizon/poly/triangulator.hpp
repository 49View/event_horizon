//
//  triangulator.hpp
//  sixthmaker
//
//  Created by Dado on 11/03/2017.
//
//

#ifndef triangulator_hpp
#define triangulator_hpp

#include <map>
#include <memory>

#include "core/math/vector3f.h"
#include "poly2tri/poly2tri.h"

class Triangulator {
public:
	Triangulator( const vector2fList& _verts, float _accuracy = 0.001f );
	Triangulator( const vector3fList& _verts, const Vector3f& normal, float _accuracy = 0.001f );
	Triangulator( const Vector3f* verts, const size_t vsize, const Vector3f& normal, float _accuracy = 0.001f );

	void gather3dTriangularizationWithNeighbours( std::vector<std::vector<Vector2f>>& tri_neighbours );

	std::vector<vector2fList>& get2dTrianglesList();
	std::vector<Triangle2d>& get2dTrianglesTuple();

	std::vector<vector3fList>& get3dTrianglesList();
	std::vector<Triangle3d>& get3dTrianglesTuple();

private:
	void init2d( const Vector2f* _verts, const int vsize );
	void init3d( const Vector2f* _verts, const int vsize );

	void gather2dTriangularizationResult();
	void gather3dTriangularizationResult();
private:
	std::vector<vector2fList> m2dTriangulation;
	std::vector<vector3fList> m3dTriangulation;

	std::vector<Triangle2d> m2dTriangulationTuples;
	std::vector<Triangle3d> m3dTriangulationTuples;

	vector2fList m2dCollinearCheck;
	std::map<int64_t, Vector3f> mVHash;
	IndexPair dominantPair;

	std::unique_ptr<p2t::CDT> mCDT;
};

#endif /* triangulator_hpp */