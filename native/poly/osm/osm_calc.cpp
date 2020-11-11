//
// Created by dado on 12/10/2020.
//

#include "osm_calc.hpp"
#include <core/descriptors/osm_bsdata.hpp>

double latOff = 6854659.994492018;
double lonOff = 3661211.8423855873;
double EARTH_RADIUS = 6378137.0;

double lat2y_m(double lat) { return log(tan( degToRad(lat) / 2 + M_PI/4 )) * EARTH_RADIUS; }
double lon2x_m(double lon) { return          degToRad(lon)                 * EARTH_RADIUS; }

//[[maybe_unused]] V3f osmTileProject( const V3f& vertex, const V3f& elemCenterProj3d, float globalOSMScale ) {
//    V3f pp{ XZY::C(vertex) + elemCenterProj3d };
//    pp *= globalOSMScale;
//    return pp;
//}

[[maybe_unused]] V3f osmTileProject( const std::vector<double>& vertex, double lcx, double lcy ) {
    return XZY::C(lcx- vertex[0], (lcy-vertex[1])*-1.0f, vertex[2]) * 0.1f;
}

V2f coordToProjection( const V2f& latLon ) {
    return V2f{ latLon.x(), radToDeg(log(tan(( latLon.y() / 90.0f + 1.0f ) * M_PI_4))) };
}

[[maybe_unused]] V3f osmTileDeltaPos( const OSMElement& element ) {
//    auto s = 1.0;
    return XZY::C(V2f{ element.center.tilePos[0]-latOff, element.center.tilePos[1]-lonOff }, 0.0f);
}
