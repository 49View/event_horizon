//
// Created by Dado on 2019-01-13.
//

#include "svgtopoly.hpp"

#include <core/math/rect2f.h>
#include <poly/nanosvg.h>

namespace SVGC {

    std::vector<std::vector<Vector2f>> SVGToPoly( const std::string& _sourceString ) {
        NSVGimage *image = nsvgParse( const_cast<char *>(_sourceString.c_str()), "pc", 96 );
        Rect2f lbbox = Rect2f::INVALID;
        std::vector<std::vector<Vector2f>> rawPoints;
        int subDivs = 4;
        for ( auto shape = image->shapes; shape != NULL; shape = shape->next ) {
//        LOGR("Shape %s", shape->id );
            for ( auto path = shape->paths; path != NULL; path = path->next ) {
//            LOGR("    Path, points %d", path->npts );
                std::vector<V2f> pathPoints;
                for ( auto i = 0; i < path->npts - 1; i += 3 ) {
                    float *p = &path->pts[i * 2];
                    for ( int s = 0; s < subDivs + 1; s++ ) {
                        float t = s / static_cast<float>(subDivs + 1);
                        Vector2f pi = interpolateBezier( Vector2f{ p[0], p[1] }, Vector2f{ p[2], p[3] },
                                                         Vector2f{ p[4], p[5] }, Vector2f{ p[6], p[7] }, t );
                        pi *= 0.10f;
                        pathPoints.push_back( pi );
                        lbbox.expand( pi );
                    }
                }
                rawPoints.emplace_back( pathPoints );
            }
        }
        nsvgDelete( image );

        return rawPoints;
    }

}