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

enum CameraMode {
    Edit2d = 0,
    Doom,
    CameraMode_max
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

static inline std::string cubeMapFace( const std::string& filename, CubemapFaces cf ) {

    std::string fname = filename.substr( 0, filename.length() - 4 );
    switch ( cf ) {
        case Front:
            return fname + "_posy";
        case Back:
            return fname + "_negy";
        case Left:
            return fname + "_negx";
        case Right:
            return fname + "_posx";
        case Top:
            return fname + "_posz";
        case Bottom:
            return fname + "_negz";
    }
    return fname + "_posx";
}

static inline std::string cubeMapTName( const std::string& filename ) {

    auto c = string_trim_upto( filename, { "_posy",
                                           "_negy",
                                           "_negx",
                                           "_posx",
                                           "_posz",
                                           "_negz" } );
    return c + getFileNameExt( filename );
}

static inline std::string cubemapFaceToString( const CubemapFaces cmf ) {
    switch ( cmf ) {
        case CubemapFaces::Top:
            return "top";
        case CubemapFaces::Bottom:
            return "bottom";
        case CubemapFaces::Front:
            return "front";
        case CubemapFaces::Back:
            return "back";
        case CubemapFaces::Left:
            return "left";
        case CubemapFaces::Right:
            return "right";
    }
    return "";
}


// Rig utils
using CubeMapRigContainer = std::array<std::shared_ptr<CameraRig>, 6>;
CubeMapRigContainer addCubeMapRig( const std::string& _name, const Vector3f& _pos, const JMATH::Rect2f& _viewPort );
std::string cubeRigName( int t, const std::string& _probeName );