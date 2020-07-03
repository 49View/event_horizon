//
// Created by Dado on 2019-04-22.
//

#pragma once

#include <core/math/vector3f.h>

struct PickRayData {
    V3f rayNear;
    V3f rayFar;
};

template <typename T>
struct RayPair {
    RayPair() = default;
    RayPair( const T& origin, const T& dir ) : origin(origin), dir(dir) {}

    T origin{0.0f};
    T dir{0.0f};
};

using RayPair3 = RayPair<Vector3f>;
using RayPair2 = RayPair<Vector2f>;