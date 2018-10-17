//
//  follower.hpp
//  6thViewImporter
//
//  Created by Dado on 15/10/2015.
//
//

#pragma once

#include <vector>
#include <map>
#include "core/util.h"
#include "core/math/vector4f.h"
#include "core/math/plane3f.h"
#include "profile.hpp"
#include "geom_data.hpp"

struct FollowerPoly {
    FollowerPoly( const std::vector<Vector3f>& rp1, const std::vector<Vector3f>& rp2,
                  const std::array<size_t,4>& indices );
	std::array<Vector3f, 4> vs;
	std::array<Vector2f, 4> vtcs;
    std::array<Vector3f, 4> vncs;
	Vector3f vn;
};

class Follower {
public:
	Follower( subdivisionAccuray subDivs = accuracyNone,
			  const FollowerFlags& ff = FollowerFlags::Defaults,
			  const std::string& _name = "FollowerUnnamed",
			  const MappingDirection _md = MappingDirection::X_POS,
			  bool _bUseFlatMapping = false );

	std::shared_ptr<GeomData> operator()( std::shared_ptr<PBRMaterial> material, const std::vector<Vector3f>& verts,
								 		  std::shared_ptr<Profile> profile,
										  const FollowerGap& gaps = FollowerGap::Empty );

	std::shared_ptr<HierGeom> extrude( const std::vector<Vector3f>& verts, std::shared_ptr<Profile> profile, const FollowerGap& gaps = FollowerGap::Empty );
	std::shared_ptr<HierGeom> extrude( const std::vector<Vector2f>& verts, std::shared_ptr<Profile> profile, float _z, const FollowerGap& gaps = FollowerGap::Empty );
	std::shared_ptr<HierGeom> extrude( const std::initializer_list<Vector3f>& verts, std::shared_ptr<Profile> profile, const FollowerGap& gaps = FollowerGap::Empty );

	std::shared_ptr<HierGeom> pullPillow( const std::vector<Vector3f>& verts, float pullHeight, PillowEdges pe );

	std::vector<Vector2f> vboundingContours2f() const;

	FollowerFlags CurrentFlags() const { return mCurrentFlags; }
	void CurrentFlags( FollowerFlags val ) { mCurrentFlags = val; }
	void addFlag( FollowerFlags val ) { mCurrentFlags |= val; }
	void excludeAxisFromExtrusion( const Vector3f& _axis );

	void type( GeomHierType _gt );
	void capsType( GeomHierType _gt );

	std::string Name() const;
	void Name( const std::string& val );

	MappingDirection getMappingDirection() const { return mMappingDirection; }
	void setMappingDirection( MappingDirection val ) { mMappingDirection = val; }

	bool UsePlanarMapping() const { return mbUsePlanarMapping; }
	void UsePlanarMapping( bool val ) { mbUsePlanarMapping = val; }

	const std::vector<Vector3f>& VCoords() { return vcoords; }
	std::vector<Vector3f> VCoords() const { return vcoords; }

	const std::vector<Vector3f>& VPlanesb() { return vplanesb; }
	std::vector<Vector3f> VPlanesb() const { return vplanesb; }
	const std::vector<Vector3f>& VPlanest() { return vplanest; }
	std::vector<Vector3f> VPlanest() const { return vplanest; }

private:
	void compositePolys( std::shared_ptr<GeomData> _geom, std::vector<FollowerPoly>& polys,
						 const CompositeWrapping cpw = CompositeWrapping::Wrap );
	void createLineFromVerts( const std::vector<Vector3f>& _verts, const FollowerGap& gaps );
	void triangulate( std::shared_ptr<GeomData> _geom );

	bool skipGapAt( int t );

	void reserveVBounding( uint64_t size );
	void updateInternalTriangulationVars( std::shared_ptr<GeomData> lGeom, const Vector3f vs[], uint64_t m, uint64_t t );

private:
	// 2d Followers data
	std::shared_ptr<Profile> mProfile;
	Profile					 mBBoxProfile;
	FollowerFlags			 mCurrentFlags;
	Vector3f				 mDefaultUpAxis = Vector3f::UP_AXIS;

	WindingOrderT mStartWindingOrder; // This is used to store the initial winding order of the triangulation of the extrusion, just an util var

	std::vector<Vector3f> vcoords;
	std::vector<Vector3f> vplanest;
	std::vector<Vector3f> vplanesb;
	std::vector<Vector3f> vnormals3d;
	std::vector<Vector4f> vtcoords;

	FollowerGap			  mGaps;
	std::vector<Vector3f> mAxisExtrudeExclusions;
	// Exported data
	std::vector<Vector3f> vboundingContours;

	// BoundingBox triangulation data
	std::vector<std::vector<Vector3f>> mBBoxExtrusion;

	subdivisionAccuray mSubDivAcc;
	MappingDirection mMappingDirection;
	GeomHierType mGeomType;
	GeomHierType mCapsGeomType;
	std::string mGeomName;
	bool mbCapStage;
	bool mbUsePlanarMapping;
public:
	static void createQuadPath( std::vector<Vector2f>& fverts, float width, float height, PivotPointPosition alignment = PivotPointPosition::PPP_CENTER );
};
