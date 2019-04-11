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
#include "core/v_data.hpp"
#include "core/math/vector4f.h"
#include "core/math/plane3f.h"
#include "core/resources/profile.hpp"

struct FollowerPoly {
    FollowerPoly( const std::vector<Vector3f>& rp1, const std::vector<Vector3f>& rp2,
                  const std::array<size_t,4>& indices, WindingOrderT _wo );
	std::array<Vector3f, 4> vs;
	std::array<Vector2f, 4> vtcs;
    std::array<Vector3f, 4> vncs;
	std::array<size_t,   4> vindices;
	Vector3f vn;
};

struct FollowerIntermediateData {
	std::vector<Vector3f> vcoords;
	Vector3f			  vplanet = Vector3f::ZERO;
	std::vector<Vector3f> vdirs;
	std::vector<Plane3f>  vplanesn;
	std::vector<Vector3f> vplanesb;
	std::vector<Vector4f> vtcoords;
};

namespace FollowerService {
	std::shared_ptr<VData> extrude( const std::vector<Vector3f>& verts,
                                    const Profile& profile,
                                    const Vector3f& suggestedAxis = Vector3f::ZERO,
                                    const FollowerFlags& ff = FollowerFlags::Defaults );
}

class Follower {
public:
	explicit Follower( subdivisionAccuray subDivs = accuracyNone,
			  const FollowerFlags& ff = FollowerFlags::Defaults,
			  const std::string& _name = "FollowerUnnamed",
              MappingDirection _md = MappingDirection::X_POS,
			  bool _bUseFlatMapping = false );

	std::vector<Vector2f> vboundingContours2f() const;

	FollowerFlags CurrentFlags() const { return mCurrentFlags; }
	void CurrentFlags( FollowerFlags val ) { mCurrentFlags = val; }
	void addFlag( FollowerFlags val ) { mCurrentFlags |= val; }
//	void excludeAxisFromExtrusion( const Vector3f& _axis );

	void type( NodeType _gt );
	void capsType( NodeType _gt );

	std::string Name() const;
	void Name( const std::string& val );

	MappingDirection getMappingDirection() const { return mMappingDirection; }
	void setMappingDirection( MappingDirection val ) { mMappingDirection = val; }

	bool UsePlanarMapping() const { return mbUsePlanarMapping; }
	void UsePlanarMapping( bool val ) { mbUsePlanarMapping = val; }

private:

	void reserveVBounding( uint64_t size );

private:
	// 2d Followers data
	std::shared_ptr<Profile> mProfile;
	FollowerFlags			 mCurrentFlags;

	// Exported data
	std::vector<Vector3f> vboundingContours;

	// BoundingBox triangulation data
	std::vector<std::vector<Vector3f>> mBBoxExtrusion;

	subdivisionAccuray mSubDivAcc;
	MappingDirection mMappingDirection;
	NodeType mGeomType;
	NodeType mCapsGeomType;
	std::string mGeomName;
	bool mbUsePlanarMapping;
public:
	static void createQuadPath( std::vector<Vector2f>& fverts, float width, float height, PivotPointPosition alignment = PivotPointPosition::PPP_CENTER );
};
