#ifndef __GRAPHICS_TEXTURE_CONVERTER_H__
#define __GRAPHICS_TEXTURE_CONVERTER_H__

#include "graphic_constants.h"
#include "core/math/vector2i.h"



class TextureConverter {
public:
	// BGRA conversions
	static void convertBgraToLuminance( const void* inPixels, void* outPixels, int width, int height );
	static void convertBgraToLuminanceAlpha( const void* inPixels, void* outPixels, int width, int height );
	static void convertBgraToRgba( const void* inPixels, void* outPixels, int width, int height );
	static void convertBgraToRgb( const void* inPixels, void* outPixels, int width, int height );
	static void convertBgraToRgb565( const void* inPixels, void* outPixels, int width, int height );

	// RGB conversions
	static void convertRgbToLuminance( const void* inPixels, void* outPixels, int width, int height );

	// RGBA conversions
	static void convertRgbaToRgb( const void* inPixels, void* outPixels, int width, int height );
	static void convertRgbaToRgb565( const void* inPixels, void* outPixels, int width, int height );

	// Luminance conversions
	static void convertLuminanceToRgb( const void* inPixels, void* outPixels, int width, int height );
	static void convertLuminanceToRgba( const void* inPixels, void* outPixels, int width, int height );

	// Quadrant conversions
	static void convertQuadrantsToRgba( const void* inPixels, void* outPixels, int width, int height );
	static void convertLuminanceQuadrantsToRgba( const void* inPixels, void* outPixels, int width, int height );

	// Misc
	static void downscaleNV21Image( void* data, int width, int height, int factor );
	static Vector2i cropNV21Image( void* data, const Vector2i& size, int percentage );
	static void interlaceUVs( const void* inUs, const void* inVs, void* outUVs, int planeSize );
	static void yuv420PlanarToSemiplanar( void* inOutData, int width, int height );
	static void qualcommTiled64x32ToYuv420Semiplanar( void* inOutData, int size, int width, int height );
	static void qualcommSemiplanarToYuv420Semiplanar( void* inOutData, int size, int width, int height );

private:
	static int getTilePosition( int x, int y, int w, int h );
	static int roundDimensionUp( int dimension, int divider );
};

#endif // D_staticbuffer_H
