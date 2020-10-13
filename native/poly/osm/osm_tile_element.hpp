//
// Created by dado on 13/10/2020.
//

#pragma once

#include <memory>
#include <core/descriptors/osm_bsdata.hpp>

class VData;

void addOSMTileBoundaries( const std::shared_ptr<VData>& _ret, const OSMTileBoundary& tileBoundary, float globalOSMScale );
void addOSMTileTriangles( const std::shared_ptr<VData>& _ret, const OSMMesh& osmMesh, const V3f& elemCenterProj3d, float globalOSMScale );
