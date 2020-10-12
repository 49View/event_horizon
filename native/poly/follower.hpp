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
#include "core/v_data.hpp"
#include "core/math/vector4f.h"
#include "core/math/plane3f.h"
#include "core/resources/profile.hpp"

namespace FollowerService {

    PolyStruct extrudePolyStruct( const std::vector<Vector3f>& _verts,
                                  const Profile& profile,
                                  const C4f& color,
                                  const Vector3f& _suggestedAxis = V3fc::ZERO,
                                  const FollowerFlags& ff = FollowerFlags::Defaults );

    void extrude( std::shared_ptr<VData> geom,
                  const std::vector<Vector3f>& verts,
                  const Profile& profile,
                  const Vector3f& suggestedAxis = V3fc::ZERO,
                  const FollowerFlags& ff = FollowerFlags::Defaults );

    std::vector<Vector3f> createLinePath( const V2f& a, const V2f& b, float width, float z );
}

//class Follower {
//public:
//    explicit Follower( subdivisionAccuray subDivs = accuracyNone,
//                       const FollowerFlags& ff = FollowerFlags::Defaults,
//                       const std::string& _name = "FollowerUnnamed",
//                       MappingDirection _md = MappingDirection::X_POS,
//                       bool _bUseFlatMapping = false );
//
//    [[nodiscard]] std::vector<Vector2f> vboundingContours2f() const;
//
//    [[nodiscard]] FollowerFlags CurrentFlags() const { return mCurrentFlags; }
//    void CurrentFlags( FollowerFlags val ) { mCurrentFlags = val; }
//    void addFlag( FollowerFlags val ) { mCurrentFlags |= val; }
////	void excludeAxisFromExtrusion( const Vector3f& _axis );
//
//    void type( NodeType _gt );
//    void capsType( NodeType _gt );
//
//    [[nodiscard]] std::string Name() const;
//    void Name( const std::string& val );
//
//    [[nodiscard]] MappingDirection getMappingDirection() const { return mMappingDirection; }
//    void setMappingDirection( MappingDirection val ) { mMappingDirection = val; }
//
//    [[nodiscard]] bool UsePlanarMapping() const { return mbUsePlanarMapping; }
//    void UsePlanarMapping( bool val ) { mbUsePlanarMapping = val; }
//
//private:
//
//    void reserveVBounding( uint64_t size );
//
//private:
//    // 2d Followers data
//    std::shared_ptr<Profile> mProfile;
//    FollowerFlags mCurrentFlags;
//
//    // Exported data
//    std::vector<Vector3f> vboundingContours;
//
//    // BoundingBox triangulation data
//    std::vector<std::vector<Vector3f>> mBBoxExtrusion;
//
//    subdivisionAccuray mSubDivAcc;
//    MappingDirection mMappingDirection;
//    NodeType mGeomType;
//    NodeType mCapsGeomType;
//    std::string mGeomName;
//    bool mbUsePlanarMapping;
//public:
//    static void createQuadPath( std::vector<Vector2f>& fverts, float width, float height,
//                                PivotPointPosition alignment = PivotPointPosition::Center );
//};
