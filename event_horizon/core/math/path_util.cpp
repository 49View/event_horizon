//
//  path_util.cpp
//  SixthView
//
//  Created by Dado on 27/02/2015.
//  Copyright (c) 2015 JFDP Labs. All rights reserved.
//

#include "path_util.h"

void appendMirror( std::vector<Vector2f>& points, const Vector2f& mirrorAxis ) {

    for ( auto ri = points.rbegin(); ri != points.rend(); ++ri ) {
        points.emplace_back( *ri * mirrorAxis );
    }
}

void flipAxis( std::vector<Vector2f>& points ) {

    for ( auto& p : points ) {
        p.swizzle(0,1);
    }
}
