//
// Created by dado on 15/10/2020.
//

#pragma once

#include <memory>

class CollisionMesh;
struct OSMData;

namespace OSMService {
    std::shared_ptr<CollisionMesh> createCollisionMesh( const OSMData *house );
    void loadOSM( float lon, float lat, int zoomLevel );
}

class OsmOrchestrator {

};