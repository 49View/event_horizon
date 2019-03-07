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

// Rig utils
using CubeMapRigContainer = std::array<std::shared_ptr<CameraRig>, 6>;
CubeMapRigContainer addCubeMapRig( const std::string& _name, const Vector3f& _pos, const Rect2f& _viewPort );
std::string cubeRigName( int t, const std::string& _probeName );