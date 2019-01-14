//
// Created by Dado on 2019-01-13.
//

#pragma once

#include <vector>
#include <string>
#include <core/math/vector2f.h>

namespace SVGC {
    std::vector<std::vector<Vector2f>> SVGToPoly( const std::string& _sourceString );
}


