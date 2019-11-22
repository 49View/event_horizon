//
//  graphic_functions.hpp
//  sixthview
//
//  Created by Dado on 10/05/2016.
//  Copyright © 2016 JFDP Labs. All rights reserved.
//

#pragma once

#include <vector>
#include "core/math/vector3f.h"
#include "graphic_constants.h"

void setWireFrame( bool enabled );
void setAlphaBlending( bool enabled );
void setDepthWrite( bool enabled );
void setDepthTest( bool enabled, DepthFunction df = DepthFunction::LESS );
void enableDepthTest( bool enabled );
void setCullMode( CullMode cm );
void setDepthFunction( DepthFunction dp );
void enableMultiSample( bool enabled );
void grabScreen( int x, int y, int w, int h, void* buffer );

PixelFormat channelsToFormat( int _channels, int _bpp );
