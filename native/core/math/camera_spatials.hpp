//
// Created by dado on 29/06/2020.
//

#pragma once

#include <core/math/vector3f.h>
#include <core/math/quaternion.h>
#include <core/math/anim_type.hpp>

struct CameraSpatials {
    V3fa mPos;
    Quaterniona qangle; // angles of x,y,z axis to be fed into quaternion math
    floata mFov;
};
