//
//  path_util.h
//  SixthView
//
//  Created by Dado on 27/02/2015.
//  Copyright (c) 2015 JFDP Labs. All rights reserved.
//

#pragma once

#include <vector>
#include "vector4f.h"
#include "math_util.h"



typedef std::pair<int32_t, float> traversePair;

template <typename T>
traversePair traversePathWithTime( const std::vector<T> cameraPath, std::vector< float > lenghts,
								   float currCameraPathTime, int32_t firstIndec, int32_t lastIndex ) {
	int64_t hermiteIndex = 1;
	float hermiteT = 0.0f;

	for ( uint64_t t = firstIndec; t < cameraPath.size() - lastIndex; t++ ) {
		if ( currCameraPathTime > lenghts[t] && currCameraPathTime < lenghts[t + 1] ) {
			hermiteIndex = t;
			hermiteT = ( currCameraPathTime - lenghts[t] ) / ( lenghts[t + 1] - lenghts[t] );
			break;
		}
	}

	return std::make_pair( static_cast<int32_t>( hermiteIndex ), hermiteT );
}

template<typename T>
T traversePathHermite( const std::vector<T> cameraPath, float currCameraPathTime ) {
	
	traversePair tp = traversePathWithTime( cameraPath, prepaprePath(cameraPath), currCameraPathTime, 1, 2 );

	return interpolateHermite( cameraPath[tp.first - 1],
							   cameraPath[tp.first],
							   cameraPath[tp.first + 1],
							   cameraPath[tp.first + 2],
							   tp.second, 0.0f, 0.0f );
}

template<typename T>
T traversePathHermite( const std::vector<T> cameraPath, std::vector<float> lentghs, float currCameraPathTime ) {
	traversePair tp = traversePathWithTime( cameraPath, lentghs, currCameraPathTime, 1, 2 );

	return interpolateHermite( cameraPath[tp.first - 1],
							   cameraPath[tp.first],
							   cameraPath[tp.first + 1],
							   cameraPath[tp.first + 2],
							   tp.second, 0.0f, 0.0f );
}

template<typename T>
std::vector< float > prepaprePath(const std::vector<T>& path) {
	std::vector<float> vaLengths;
	float pLength = 0.0f;
	float cLength = 0.0f;
	for ( size_t m = 0; m < path.size()-1; m++ ) {
		pLength += distance( path[m], path[m+1] );
	}
	for ( size_t m = 0; m < path.size()-1; m++ ) {
		cLength += distance( path[m], path[m+1] );
		vaLengths.push_back( cLength / pLength );
	}
	vaLengths.push_back( 1.0f );
	return vaLengths;
}

template<typename T>
std::vector<T> hermiteInterpolationOnPath( const std::vector<T>& path, int subdivs ) {

	std::vector<T> ret;
	auto vaLengths = prepaprePath( path );
	float cTime = 0.0f;
	float cTimeDelta = 1.0f / (path.size()*subdivs);
	for ( size_t q = 0; q < path.size()*subdivs; q++ ) {
		ret.push_back( T{traversePathHermite( path, vaLengths, cTime)} );
		cTime += cTimeDelta;
	}
	
	return ret;
}

template<typename T>
bool isPathClosed( const std::vector<T>& path, float epsilon = 0.001f ) {
    if ( path.size() < 2 ) return false;

    return isVerySimilar( path.front(), path.back(), epsilon );
}

void appendMirror( std::vector<Vector2f>& points, const Vector2f& mirrorAxis );
void flipAxis( std::vector<Vector2f>& points );