//
// Created by Dado on 2019-01-13.
//

#include "svgtopoly.hpp"

#include <core/math/rect2f.h>
#include <poly/nanosvg.h>

namespace SVGC {

    Color4f pathColor( NSVGshape* shape ) {
        if ( shape->stroke.type != 0 ) {
            return Vector4f::ITORGBA( shape->stroke.color );
        }
        if ( shape->fill.type != 0 ) {
            return Vector4f::ITORGBA( shape->fill.color );
        }
        return Vector4f::DARK_CYAN;
    }

    std::vector<SVGPath> SVGToPoly( const std::string& _sourceString ) {
        NSVGimage *image = nsvgParse( const_cast<char *>(_sourceString.c_str()), "pc", 96 );
        Rect2f lbbox = Rect2f::INVALID;
        std::vector<SVGPath> rawPoints;
        int subDivs = 4;
        for ( auto shape = image->shapes; shape != NULL; shape = shape->next ) {
//        LOGR("Shape %s", shape->id );

            auto pcol = pathColor( shape );
            for ( auto path = shape->paths; path != NULL; path = path->next ) {
//            LOGR("    Path, points %d", path->npts );
                SVGPath pathPoints;
                pathPoints.strokeColor = pcol;
                for ( auto i = 0; i < path->npts - 1; i += 3 ) {
                    float *p = &path->pts[i * 2];
                    for ( int s = 0; s < subDivs + 1; s++ ) {
                        float t = s / static_cast<float>(subDivs + 1);
                        Vector2f pi = interpolateBezier( Vector2f{ p[0], p[1] }, Vector2f{ p[2], p[3] },
                                                         Vector2f{ p[4], p[5] }, Vector2f{ p[6], p[7] }, t );
                        pi *= 0.10f;
                        pathPoints.path.emplace_back( pi );
                        lbbox.expand( pi );
                    }
                }
                rawPoints.emplace_back( pathPoints );
            }
        }
        nsvgDelete( image );

        // Center on BBox
        for ( auto& pv : rawPoints ) {
            for ( auto& p : pv.path ) {
                p -= lbbox.centre();
            }
        }
        return rawPoints;
    }

}