#include "texture_converter.h"
#include "core/util.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

//====================================
// BGRA Conversions
//====================================

void TextureConverter::convertBgraToLuminance( const void* inPixels, void* outPixels, int width, int height ) {
	const uint8_t* rgbaPixels = reinterpret_cast<const uint8_t*>( inPixels );
	uint8_t* luminancePixels = reinterpret_cast<uint8_t*>( outPixels );

	int size = width * height;
	for ( int i = 0; i < size; ++i ) {
		luminancePixels[i] = rgbaPixels[i * 4 + 1];
	}
}

void TextureConverter::convertBgraToLuminanceAlpha( const void* inPixels, void* outPixels, int width, int height ) {
	const uint8_t* rgbaPixels = reinterpret_cast<const uint8_t*>( inPixels );
	uint8_t* luminanceAlphaPixels = reinterpret_cast<uint8_t*>( outPixels );

	int size = width * height;
	for ( int i = 0; i < size; ++i ) {
		// Note that this makes sure we don't trample pixel 0
		luminanceAlphaPixels[i * 2] = rgbaPixels[i * 4 + 2];
		luminanceAlphaPixels[i * 2 + 1] = rgbaPixels[i * 4 + 3];
	}
}

void TextureConverter::convertBgraToRgba( const void* inPixels, void* outPixels, int width, int height ) {
	const uint8_t* bgraPixels = reinterpret_cast<const uint8_t*>( inPixels );
	uint8_t* rgbaPixels = reinterpret_cast<uint8_t*>( outPixels );

	int size = width * height;
	for ( int i = 0; i < size * 4; i += 4 ) {
		uint8_t temp[4];

		// Do this into a temp buffer, just in case in and out are the same array
		temp[0] = bgraPixels[i + 2];
		temp[1] = bgraPixels[i + 1];
		temp[2] = bgraPixels[i];
		temp[3] = bgraPixels[i + 3];

		rgbaPixels[i] = temp[0];
		rgbaPixels[i + 1] = temp[1];
		rgbaPixels[i + 2] = temp[2];
		rgbaPixels[i + 3] = temp[3];
	}
}

void TextureConverter::convertBgraToRgb( const void* inPixels, void* outPixels, int width, int height ) {
	const uint8_t* bgraPixels = reinterpret_cast<const uint8_t*>( inPixels );
	uint8_t* rgbPixels = reinterpret_cast<uint8_t*>( outPixels );

	int size = width * height;
	for ( int i = 0; i < size; ++i ) {
		uint8_t temp[3];

		int firstBgraChannel = i * 4;
		int firstRgbChannel = i * 3;

		// Do this into a temp buffer, just in case in and out are the same array
		temp[0] = bgraPixels[firstBgraChannel + 2];
		temp[1] = bgraPixels[firstBgraChannel + 1];
		temp[2] = bgraPixels[firstBgraChannel];

		rgbPixels[firstRgbChannel] = temp[0];
		rgbPixels[firstRgbChannel + 1] = temp[1];
		rgbPixels[firstRgbChannel + 2] = temp[2];
	}
}

void TextureConverter::convertBgraToRgb565( const void* inPixels, void* outPixels, int width, int height ) {
	const uint8_t* bgraPixels = reinterpret_cast<const uint8_t*>( inPixels );
	uint16_t* rgbPixels = reinterpret_cast<uint16_t*>( outPixels );

	int size = width * height;
	for ( int i = 0; i < size; ++i ) {
		uint8_t temp[3];

		int firstBgraChannel = i * 4;

		// Do this into a temp buffer, just in case in and out are the same array
		temp[0] = bgraPixels[firstBgraChannel + 2];
		temp[1] = bgraPixels[firstBgraChannel + 1];
		temp[2] = bgraPixels[firstBgraChannel];

		uint16_t rgb565 = temp[2] >> 3;
		rgb565 |= ( static_cast<uint16_t>( temp[1] ) & 0xfc ) << 3;
		rgb565 |= ( static_cast<uint16_t>( temp[0] ) & 0xf8 ) << 8;

		rgbPixels[i] = rgb565;
	}
}

//====================================
// RGBA Conversions
//====================================

void TextureConverter::convertRgbToLuminance( const void* inPixels, void* outPixels, int width, int height ) {
	const uint8_t* rgbPixels = reinterpret_cast<const uint8_t*>( inPixels );
	uint8_t* luminancePixels = reinterpret_cast<uint8_t*>( outPixels );

	int size = width * height;
	for ( int i = 0; i < size; ++i ) {
		luminancePixels[i] = rgbPixels[i * 3];
	}
}

//====================================
// Luminance Conversions
//====================================

void TextureConverter::convertLuminanceToRgb( const void* inPixels, void* outPixels, int width, int height ) {
	uint8_t* rgbPixels = reinterpret_cast<uint8_t*>( outPixels );
	const uint8_t* luminancePixels = reinterpret_cast<const uint8_t*>( inPixels );

	int size = width * height;
	for ( int i = 0; i < size; ++i ) {
		rgbPixels[i * 3 + 0] = luminancePixels[i];
		rgbPixels[i * 3 + 1] = luminancePixels[i];
		rgbPixels[i * 3 + 2] = luminancePixels[i];
	}
}

void TextureConverter::convertLuminanceToRgba( const void* inPixels, void* outPixels, int width, int height ) {
	uint8_t* rgbPixels = reinterpret_cast<uint8_t*>( outPixels );
	const uint8_t* luminancePixels = reinterpret_cast<const uint8_t*>( inPixels );

	int size = width * height;
	for ( int i = 0; i < size; ++i ) {
		rgbPixels[i * 4 + 0] = luminancePixels[i];
		rgbPixels[i * 4 + 1] = luminancePixels[i];
		rgbPixels[i * 4 + 2] = luminancePixels[i];
		rgbPixels[i * 4 + 3] = luminancePixels[i];
	}
}

//====================================
// RGBA Conversions
//====================================

void TextureConverter::convertRgbaToRgb( const void* inPixels, void* outPixels, int width, int height ) {
	const uint8_t* rgbaPixels = reinterpret_cast<const uint8_t*>( inPixels );
	uint8_t* rgbPixels = reinterpret_cast<uint8_t*>( outPixels );

	int size = width * height;
	for ( int i = 0; i < size; ++i ) {
		uint8_t temp[3];

		int firstRgbaChannel = i * 4;
		int firstRgbChannel = i * 3;

		// Do this into a temp buffer, just in case in and out are the same array
		temp[0] = rgbaPixels[firstRgbaChannel];
		temp[1] = rgbaPixels[firstRgbaChannel + 1];
		temp[2] = rgbaPixels[firstRgbaChannel + 2];

		rgbPixels[firstRgbChannel] = temp[0];
		rgbPixels[firstRgbChannel + 1] = temp[1];
		rgbPixels[firstRgbChannel + 2] = temp[2];
	}
}

void TextureConverter::convertRgbaToRgb565( const void* inPixels, void* outPixels, int width, int height ) {
	const uint8_t* bgraPixels = reinterpret_cast<const uint8_t*>( inPixels );
	uint16_t* rgbPixels = reinterpret_cast<uint16_t*>( outPixels );

	int size = width * height;
	for ( int i = 0; i < size; ++i ) {
		uint8_t temp[3];

		int firstBgraChannel = i * 4;

		// Do this into a temp buffer, just in case in and out are the same array
		temp[0] = bgraPixels[firstBgraChannel];
		temp[1] = bgraPixels[firstBgraChannel + 1];
		temp[2] = bgraPixels[firstBgraChannel + 2];

		uint16_t rgb565 = temp[2] >> 3;
		rgb565 |= ( static_cast<uint16_t>( temp[1] ) & 0xfc ) << 3;
		rgb565 |= ( static_cast<uint16_t>( temp[0] ) & 0xf8 ) << 8;

		rgbPixels[i] = rgb565;
	}
}

//====================================
// Quadrant Conversions
//====================================

// Source texture contains four images arranged out as follows.
// Source format is RGBA. Only the green channel will be used.
// +----+----+
// | A  | B  |
// +----+----+
// | G  | R  |
// +----+----+
// Converted image will contain the four images in separate channels.
// Converted image will be half the width and height of the source.
// +----+
// |RGBA|
// +----+
void TextureConverter::convertQuadrantsToRgba( const void* inPixels, void* outPixels, int width, int height ) {
	const int srcStride = width * 4;
	const uint8_t* srcA = reinterpret_cast<const uint8_t*>( inPixels ) + 1; // +1 to get a acolor channel in both RGBA and ARGB
	const uint8_t* srcB = srcA + srcStride / 2;
	const uint8_t* srcG = srcA + srcStride * height / 2;
	const uint8_t* srcR = srcG + srcStride / 2;
	uint32_t* dst = reinterpret_cast<uint32_t*>( outPixels );

	// Dimensions must be divisible by 2
	ASSERT( ( width & 1 ) == 0 );
	ASSERT( ( height & 1 ) == 0 );

	const int dstWidth = width / 2;
	const int dstHeight = height / 2;
	for ( int y = 0; y < dstHeight; y++ ) {
		int srcOffset = srcStride * y;
		for ( int x = 0; x < dstWidth; x++ ) {
			const uint32_t r = static_cast<uint32_t>( srcR[srcOffset] );
			const uint32_t g = static_cast<uint32_t>( srcG[srcOffset] );
			const uint32_t b = static_cast<uint32_t>( srcB[srcOffset] );
			const uint32_t a = static_cast<uint32_t>( srcA[srcOffset] );
			*dst = r | ( g << 8 ) | ( b << 16 ) | ( a << 24 );
			srcOffset += 4;
			dst += 1;
		}
	}
}

void TextureConverter::convertLuminanceQuadrantsToRgba( const void* inPixels, void* outPixels, int width, int height ) {
	const int dstWidth = width / 2;
	const int dstHeight = height / 2;

	// This is half the size, but has 4 channels, so it is the same size
	uint8_t* tempPixels = new uint8_t[dstWidth * dstHeight * 4];

	const uint8_t* srcA = reinterpret_cast<const uint8_t*>( inPixels );
	const uint8_t* srcB = srcA + width / 2;
	const uint8_t* srcG = srcA + width * height / 2;
	const uint8_t* srcR = srcG + width / 2;
	uint32_t* dst = reinterpret_cast<uint32_t*>( tempPixels );

	// Dimensions must be divisible by 2
	ASSERT( ( width & 1 ) == 0 );
	ASSERT( ( height & 1 ) == 0 );

	for ( int y = 0; y < dstHeight; y++ ) {
		int srcOffset = width * y;
		for ( int x = 0; x < dstWidth; x++ ) {
			const uint32_t r = static_cast<uint32_t>( srcR[srcOffset] );
			const uint32_t g = static_cast<uint32_t>( srcG[srcOffset] );
			const uint32_t b = static_cast<uint32_t>( srcB[srcOffset] );
			const uint32_t a = static_cast<uint32_t>( srcA[srcOffset] );
			*dst = r | ( g << 8 ) | ( b << 16 ) | ( a << 24 );
			srcOffset++;
			dst += 1;
		}
	}

	memcpy( outPixels, tempPixels, dstWidth * dstHeight * 4 );

	delete[] tempPixels;
}

//====================================
// Misc
//====================================

void TextureConverter::downscaleNV21Image( void* data, int width, int height, int factor ) {
	uint8_t* byteData = reinterpret_cast<uint8_t*>( data );
	int outWidth = width / factor;
	int outHeight = height / factor;

	// Scale Y
	for ( int j = 0; j < outHeight; ++j ) {
		int inRowIndex = width * j * factor;
		int outRowIndex = outWidth * j;
		for ( int i = 0; i < outWidth; ++i ) {
			byteData[outRowIndex + i] = byteData[inRowIndex + i * factor];
		}
	}

	// Scale UVs
	int inUvWidth = width / 2;
	int inUvHeight = height / 2;
	int inUvPlaneStart = width * height;
	int outUvWidth = inUvWidth / factor;
	int outUvHeight = inUvHeight / factor;
	int outUvPlaneStart = outWidth * outHeight;

	for ( int j = 0; j < outUvHeight; ++j ) {
		int inRowIndex = inUvPlaneStart + inUvWidth * 2 * j * factor;
		int outRowIndex = outUvPlaneStart + outUvWidth * j * 2;
		for ( int i = 0; i < outUvWidth; ++i ) {
			int inIndex = inRowIndex + i * factor * 2;
			int outIndex = outRowIndex + i * 2;
			byteData[outIndex] = byteData[inIndex];
			byteData[outIndex + 1] = byteData[inIndex + 1];
		}
	}
}

Vector2i TextureConverter::cropNV21Image( void* data, const Vector2i& size, int percentage ) {
	int width = size.x();
	int height = size.y();
	uint8_t* byteData = reinterpret_cast<uint8_t*>( data );
	int targetWidth = ( ( width * percentage / 100 ) / 4 ) * 4;
	int targetHeight = ( ( height * percentage / 100 ) / 4 ) * 4;
	int x = ( width - targetWidth ) / 2;
	int y = ( height - targetHeight ) / 2;

	// Crop Y
	for ( int j = 0; j < targetHeight; ++j ) {
		for ( int i = 0; i < targetWidth; ++i ) {
			int inPixel = ( y + j ) * width + x + i;
			int outPixel = j * targetWidth + i;
			byteData[outPixel] = byteData[inPixel];
		}
	}

	// Crop UV
	int halfWidth = targetWidth / 2;
	int halfHeight = targetHeight / 2;
	int halfY = y / 2;

	int inPixel = width * height + width * halfY + x;
	int outPixel = targetWidth * targetHeight;

	for ( int j = 0; j < halfHeight; ++j ) {
		for ( int i = 0; i < halfWidth; ++i ) {
			byteData[outPixel] = byteData[inPixel];
			byteData[outPixel + 1] = byteData[inPixel + 1];
			inPixel += 2;
			outPixel += 2;
		}
		inPixel += width - targetWidth;
	}

	return Vector2i( targetWidth, targetHeight );
}

void TextureConverter::interlaceUVs( const void* inUs, const void* inVs, void* outUVs, int planeSize ) {
	int outIndex = 0;
	uint8_t* outData = reinterpret_cast<uint8_t*>( outUVs );
	const uint8_t* inUData = reinterpret_cast<const uint8_t*>( inUs );
	const uint8_t* inVData = reinterpret_cast<const uint8_t*>( inVs );

	for ( int i = 0; i < planeSize; ++i ) {
		outData[outIndex] = inUData[i];
		++outIndex;
		outData[outIndex] = inVData[i];
		++outIndex;
	}
}

void TextureConverter::yuv420PlanarToSemiplanar( void* inOutData, int width, int height ) {
	uint8_t* data = reinterpret_cast<uint8_t*>( inOutData );

	int roundedHeight = roundDimensionUp( height, 16 );
	int chromaSize = ( width * height ) / 4;

	LOGI( "Height: %d, RoundeHeight: %d", height, roundedHeight );

	uint8_t* tempUData = new uint8_t[chromaSize];
	uint8_t* tempVData = new uint8_t[chromaSize];

	uint8_t* uPlane = data + width * roundedHeight;
	uint8_t* vPlane = uPlane + ( width / 2 ) * ( roundedHeight / 2 );

	memcpy( tempUData, uPlane, chromaSize );
	memcpy( tempVData, vPlane, chromaSize );

	interlaceUVs( tempVData, tempUData, data + width * height, chromaSize );
	delete[] tempUData;
	delete[] tempVData;
}

void TextureConverter::qualcommTiled64x32ToYuv420Semiplanar( void* inOutData, int size, int width, int height ) {
	static const int TILE_WIDTH = 64;
	static const int TILE_HEIGHT = 32;
	static const int TILE_SIZE = ( TILE_WIDTH * TILE_HEIGHT );
	static const int TILE_GROUP_SIZE = ( 4 * TILE_SIZE );

	int lumaDstSize = width * height;
	uint8_t* src = new uint8_t[size];

	memcpy( src, inOutData, size );

	uint8_t* dst = reinterpret_cast<uint8_t*>( inOutData );

	int pitch = width;

	const int tileW = ( width - 1 ) / TILE_WIDTH + 1;
	const int tileWAlign = ( tileW + 1 ) & ~1;

	const int tileHLuma = ( height - 1 ) / TILE_HEIGHT + 1;
	const int tileHChroma = ( height / 2 - 1 ) / TILE_HEIGHT + 1;

	int lumaSize = tileWAlign * tileHLuma * TILE_SIZE;

	if ( ( lumaSize % TILE_GROUP_SIZE ) != 0 ) {
		lumaSize = ( ( ( lumaSize - 1 ) / TILE_GROUP_SIZE ) + 1 ) * TILE_GROUP_SIZE;
	}

	for ( int y = 0; y < tileHLuma; y++ ) {
		int rowWidth = width;
		for ( int x = 0; x < tileW; x++ ) {
			// luma source pointer for this tile
			const uint8_t *srcLuma = src + getTilePosition( x, y, tileWAlign, tileHLuma ) * TILE_SIZE;

			// chroma source pointer for this tile
			const uint8_t *srcChroma = src + lumaSize + getTilePosition( x, y / 2, tileWAlign, tileHChroma ) * TILE_SIZE;
			if ( y & 1 ) {
				srcChroma += TILE_SIZE / 2;
			}

			// account for right columns
			int tileWidth = rowWidth;
			if ( tileWidth > TILE_WIDTH ) {
				tileWidth = TILE_WIDTH;
			}

			// account for bottom rows
			int tileHeight = height;
			if ( tileHeight > TILE_HEIGHT ) {
				tileHeight = TILE_HEIGHT;
			}

			// dest luma memory index for this tile
			int lumaIndex = y * TILE_HEIGHT * pitch + x * TILE_WIDTH;

			// dest chroma memory index for this tile
			int chromaIndex = lumaDstSize + ( lumaIndex / pitch ) * pitch / 2 + ( lumaIndex % pitch );

			tileHeight /= 2; // we copy 2 luma lines at once
			while ( tileHeight-- ) {
				memcpy( &dst[lumaIndex], srcLuma, tileWidth );
				srcLuma += TILE_WIDTH;
				lumaIndex += pitch;

				memcpy( &dst[lumaIndex], srcLuma, tileWidth );
				srcLuma += TILE_WIDTH;
				lumaIndex += pitch;

				// Copy and swap the chromas, as this is NV12 and we need NV21
				for ( int i = 0; i < tileWidth; i += 2 ) {
					int thisChroma = chromaIndex + i;
					dst[thisChroma] = srcChroma[i + 1];
					dst[thisChroma + 1] = srcChroma[i];
				}

				srcChroma += TILE_WIDTH;
				chromaIndex += pitch;
			}
			rowWidth -= TILE_WIDTH;
		}
		height -= TILE_HEIGHT;
	}

	delete[] src;
}

int TextureConverter::getTilePosition( int x, int y, int w, int h ) {
	int flim = x + ( y & ~1 ) * w;

	if ( y & 1 ) {
		flim += ( x & ~3 ) + 2;
	} else if ( ( h & 1 ) == 0 || y != ( h - 1 ) ) {
		flim += ( x + 2 ) & ~3;
	}

	return flim;
}

int TextureConverter::roundDimensionUp( int dimension, int divider ) {
	return dimension % divider == 0 ? dimension : ( dimension / divider ) * divider + divider;
}

void TextureConverter::qualcommSemiplanarToYuv420Semiplanar( void* inOutData, int /*size*/, int width, int height ) {
	uint8_t* uintData = static_cast<uint8_t*>( inOutData );
	uint8_t* uvPlaneOut = uintData + width * height;
	uint8_t* uvPlaneIn = uintData + roundDimensionUp( width, 32 ) * roundDimensionUp( height, 32 );

	int uvSize = width * height / 4;
	for ( int i = 0; i < uvSize; ++i ) {
		// Use a temp variable, as there is the possibility that we are just swapping the bytes
		uint8_t tempUV = uvPlaneIn[i * 2];
		uvPlaneOut[i * 2] = uvPlaneIn[i * 2 + 1];
		uvPlaneOut[i * 2 + 1] = tempUV;
	}
}