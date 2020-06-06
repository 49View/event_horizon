//
// Created by Dado on 2019-03-06.
//

#pragma once

#include <memory>
#include <array>
#include <string>

class Vector3f;
namespace JMATH { class Rect2f; }
class CameraRig;

enum class CameraProjectionType {
    Perspective,
    Orthogonal
};

enum class CameraControlType {
    Edit2d = 0,
    Orbit,
    Fly,
    Walk
};

enum CameraState {
    Active,
    InActive
};

enum CubemapFaces {
    Front = 0,
    Back,
    Left,
    Right,
    Top,
    Bottom
};

namespace ViewportToggles {
    const static int None          		= 0;
    const static int DrawWireframe 		= 1 << 0;
    const static int DrawGrid      		= 1 << 1;
}

using ViewportTogglesT = int;

std::string cubeMapFace( const std::string& filename, CubemapFaces cf );
std::string cubeMapTName( const std::string& filename );
std::string cubemapFaceToString( const CubemapFaces cmf );


// Rig utils
using CubeMapRigContainer = std::array<std::shared_ptr<CameraRig>, 6>;
CubeMapRigContainer addCubeMapRig( const std::string& _name, const Vector3f& _pos, const JMATH::Rect2f& _viewPort );
std::string cubeRigName( int t, const std::string& _probeName );