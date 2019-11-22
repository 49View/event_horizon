//
//  graphic_functions.cpp
//  sixthview
//
//  Created by Dado on 10/05/2016.
//  Copyright © 2016 JFDP Labs. All rights reserved.
//

#include "graphic_functions.hpp"

PixelFormat channelsToFormat( int _channels, int bpp ) {
	switch ( _channels ) {
		case 1:
		    switch ( bpp ) {
		        case 8:
                    return PIXEL_FORMAT_LUMINANCE;
                case 16:
                    return PIXEL_FORMAT_HDR_R16;
                case 32:
                    return PIXEL_FORMAT_HDR_RG32;
		    }
			return PIXEL_FORMAT_LUMINANCE;
			break;
		case 2:
            switch ( bpp ) {
                case 8:
                    return PIXEL_FORMAT_LUMINANCE_ALPHA;
                case 16:
                    return PIXEL_FORMAT_HDR_RG_16;
                case 32:
                    return PIXEL_FORMAT_HDR_RG_32;
            }
			return PIXEL_FORMAT_LUMINANCE_ALPHA;
			break;
		case 3:
            switch ( bpp ) {
                case 8:
                    return PIXEL_FORMAT_RGB;
                case 16:
                    return PIXEL_FORMAT_HDR_RGB_16;
                case 32:
                    return PIXEL_FORMAT_HDR_RGB_32;
            }
			return PIXEL_FORMAT_RGB;
			break;
		case 4:
            switch ( bpp ) {
                case 8:
                    return PIXEL_FORMAT_RGBA;
                case 16:
                    return PIXEL_FORMAT_HDR_RGBA_16;
                case 32:
                    return PIXEL_FORMAT_HDR_RGBA_32;
            }
			return PIXEL_FORMAT_RGBA;
			break;
		default:
			return PIXEL_FORMAT_INVALID;
	}
}
