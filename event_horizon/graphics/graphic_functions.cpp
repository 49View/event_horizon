//
//  graphic_functions.cpp
//  sixthview
//
//  Created by Dado on 10/05/2016.
//  Copyright © 2016 JFDP Labs. All rights reserved.
//

#include "graphic_functions.hpp"

PixelFormat channelsToFormat( int _channels ) {
	switch ( _channels ) {
		case 1:
			return PIXEL_FORMAT_LUMINANCE;
			break;
		case 2:
			return PIXEL_FORMAT_LUMINANCE_ALPHA;
			break;
		case 3:
			return PIXEL_FORMAT_RGB;
			break;
		case 4:
			return PIXEL_FORMAT_RGBA;
			break;
		default:
			return PIXEL_FORMAT_INVALID;
	}
}
