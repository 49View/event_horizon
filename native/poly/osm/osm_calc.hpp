//
// Created by dado on 12/10/2020.
//

#pragma once

#include <core/math/vector3f.h>

struct OSMElement;

template<typename T>
[[maybe_unused]] T latToY( T lat ) {
    return radToDeg(log(tan(( lat / 90.0 + 1.0 ) * M_PI_4)));
}

template<typename T>
[[maybe_unused]] float calcGeoDistance( T lat1, T lon1, T lat2, T lon2 ) {
    auto R = 6371e3; // metres
    auto phi1 = degToRad(lat1); // φ, λ in radians
    auto phi2 = degToRad(lat2);
    auto deltaPhi = degToRad(lat2 - lat1);
    auto deltaLambda = degToRad(lon2 - lon1);

    auto a = sin(deltaPhi / 2.0) * sin(deltaPhi / 2.0) +
             cos(phi1) * cos(phi2) *
             sin(deltaLambda / 2.0) * sin(deltaLambda / 2.0);
    auto c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

    auto d = ( R * c ); // in metres

    return d;
}

[[maybe_unused]] V3f osmTileProject( const V3f& vertex, const V3f& elemCenterProj3d, float globalOSMScale );
V2f coordToProjection( const V2f& latLon );
[[maybe_unused]] V3f osmTileDeltaPos( const OSMElement& element );
