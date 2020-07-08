//
// Created by dado on 29/06/2020.
//

#pragma once

#include <core/math/vector3f.h>
#include <core/math/quaternion.h>
#include <core/math/anim_type.hpp>
#include <core/serialization.hpp>

struct CameraSpatials {
    V3fa mPos;
    Quaterniona qangle; // angles of x,y,z axis to be fed into quaternion math
    floata mFov;
};

JSONDATA( CameraSpatialsKeyFrame, pos, qangle, fov, timestamp )
    CameraSpatialsKeyFrame( const CameraSpatials& _cs, float _timestamp ) : timestamp(_timestamp) {
        pos = _cs.mPos->value;
        qangle = _cs.qangle->value;
        fov = _cs.mFov->value;
    }
    V3f pos = V3f::ZERO;
    Quaternion qangle{};
    float fov = 0.0f;
    float timestamp = 0.0f;
};

JSONDATA( CameraPath, path )
    std::vector<CameraSpatialsKeyFrame> path{};
};