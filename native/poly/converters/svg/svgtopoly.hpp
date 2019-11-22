//
// Created by Dado on 2019-01-13.
//

#pragma once

#include <vector>
#include <string>
#include <core/math/vector4f.h>

struct SVGPath {
    Color4f strokeColor;
    std::vector<Vector2f> path;
};

namespace SVGC {
    std::vector<SVGPath> SVGToPoly( const std::string& _sourceString );
}


