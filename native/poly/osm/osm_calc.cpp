//
// Created by dado on 12/10/2020.
//

#include "osm_calc.hpp"
#include <core/descriptors/osm_bsdata.hpp>

[[maybe_unused]] V3f osmTileProject( const V3f& vertex, const V3f& elemCenterProj3d, float globalOSMScale ) {
    V3f pp{ XZY::C(vertex) + elemCenterProj3d };
    pp *= globalOSMScale;
    return pp;
}

V2f coordToProjection( const V2f& latLon ) {
    return V2f{ latLon.x(), radToDeg(log(tan(( latLon.y() / 90.0f + 1.0f ) * M_PI_4))) };
}

[[maybe_unused]] V3f osmTileDeltaPos( const OSMElement& element ) {
    return XZY::C(V2f{ element.center.deltaPosInTile[0], element.center.deltaPosInTile[1] }, 0.0f);
}
