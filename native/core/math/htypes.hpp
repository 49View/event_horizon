//
// Created by dado on 03/07/2020.
//

#pragma once

class Vector3f;
class Vector2f;
template <typename T> struct RayPair;

using RayPair3 = RayPair<Vector3f>;
using RayPair2 = RayPair<Vector2f>;

struct CameraSpatialsKeyFrame;
struct CameraPath;