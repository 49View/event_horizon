//
// Created by dado on 13/10/2020.
//

#pragma once

#include <memory>

class VData;
struct OSMData;

void addOSMTile( const std::shared_ptr<VData>& _ret, const OSMData* osm, float globalOSMScale );
void addOSMSolid( const std::shared_ptr<VData>& _ret, const OSMData* osm, float globalOSMScale );
