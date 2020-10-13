//
// Created by dado on 13/10/2020.
//

#pragma once

#include <memory>
#include <algorithm>

class VData;
struct Topology;
struct PolyStruct;
struct OSMTileBoundary;
struct OSMMesh;
class Vector3f;

void addOSMMesh( const std::shared_ptr<VData>& _ret, std::function<void(Topology&)> func );
void addOSMPolyStruct( const std::shared_ptr<VData>& _ret, std::function<std::vector<PolyStruct>()> func );

void addOSMTileBoundaries( const std::shared_ptr<VData>& _ret, const OSMTileBoundary& tileBoundary, float globalOSMScale );
void addOSMTileTriangles( const std::shared_ptr<VData>& _ret, const OSMMesh& osmMesh, const Vector3f& elemCenterProj3d, float globalOSMScale );
