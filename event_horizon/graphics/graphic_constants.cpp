//
// Created by Dado on 12/12/2017.
//
#include "graphic_constants.h"

#include "core/util.h"

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