//
// Created by dado on 15/10/2020.
//

#pragma once

#include <memory>

class CollisionMesh;
class SceneGraph;
struct OSMData;

namespace OSMService {
    std::shared_ptr<CollisionMesh> createCollisionMesh( const OSMData *house );
}

class OsmOrchestrator {
public:
    explicit OsmOrchestrator( SceneGraph& _sg );
    void loadOSM( double lon, double lat, double zoomLevel );

    [[nodiscard]] OSMData* OSM();
    void OSM(std::shared_ptr<OSMData>);
private:
    SceneGraph& sg;
    std::shared_ptr<OSMData> osm;
};