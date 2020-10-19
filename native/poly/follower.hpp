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

    void extrude( const std::shared_ptr<VData>& geom,
                  const std::vector<Vector3f>& verts,
                  const Profile& profile,
                  const Vector3f& suggestedAxis = V3fc::ZERO,
                  const FollowerFlags& ff = FollowerFlags::Defaults );

    std::vector<Vector3f> createLinePath( const V2f& a, const V2f& b, float width, float z );
}
