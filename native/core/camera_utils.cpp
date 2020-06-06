//
// Created by dado on 06/06/2020.
//

#include "camera_utils.hpp"
#include <core/file_manager.h>
#include <core/string_util.h>

std::string cubeMapFace( const std::string& filename, CubemapFaces cf ) {

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

std::string cubeMapTName( const std::string& filename ) {

    auto c = string_trim_upto( filename, { "_posy",
                                           "_negy",
                                           "_negx",
                                           "_posx",
                                           "_posz",
                                           "_negz" } );
    return c + getFileNameExt( filename );
}

std::string cubemapFaceToString( const CubemapFaces cmf ) {
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

