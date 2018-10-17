//
//
//  Created by Dado on 30/07/2017.
//
//

#pragma once

#include "core/serialization.hpp"
//#include "cpp_common/core/math/rect2f.h"

//struct Dem {
//	std::unique_ptr<Vector3f[]> voxels;
//	
//	Dem( const MegaReader& reader ) {
//		deserialize( reader );
//	}
//
//	void deserialize( const MegaReader& reader ) {
//	}
//};

//JSONDATA ( OSMGrid, grid, cols, rows )
//	std::vector<float> grid;
//	uint64_t cols = 0;
//	uint64_t rows = 0;
//};

//JSONDATA ( OSMNode, id, lat, lon, alt, visible )
//	uint64_t id = 0;
//	float lat = 0.0f;
//	float lon = 0.0f;
//	float alt = 0.0f;
//	bool  visible = false;
//	std::map<std::string, std::string> tags;
//};

//JSONDATA (OSMWay, id, visible, nodesId )
//	uint64_t id = 0;
//	bool visible = false;
//	std::vector< uint64_t > nodesId;
//	std::map<std::string, std::string> tags;
//};

//JSONDATA_R( OsmArea, name, minLat, minLon, maxLat, maxLon, grid, nodes, ways )
//	std::string name = "";
//	float minLat = 0.0f;
//	float minLon = 0.0f;
//	float maxLat = 0.0f;
//	float maxLon = 0.0f;
//	OSMGrid grid;
//	std::vector<OSMNode> nodes;
//	std::vector<OSMWay> ways;
//};

JSONDATA( OSMPoint, latitude, longitude, elevation )

    float latitude = 0.0f;
    float longitude = 0.0f;
    float elevation = 0.0f;
};

JSONDATA( OSMHighway, points, type, lanes, width, color )

    std::vector<OSMPoint> points;
    std::string type;
    float lanes;
    float width;
    Color4f color;
};

JSONDATA( OSMGrid, rows, columns, grid )

    uint32_t rows = 0;
    uint32_t columns = 0;
    std::vector<float> grid;
};

JSONDATA( OSMBuildingPolygon, points )

    std::vector<OSMPoint> points;
};

JSONDATA( OSMBuildingPart, outer, inners, baseHeight, topHeight )

    OSMBuildingPolygon outer;
    std::vector<OSMBuildingPolygon> inners;
    float baseHeight;
    float topHeight;
};

JSONDATA( OSMBuilding, parts )

    std::vector<OSMBuildingPart> parts;
};

JSONDATA_R( OsmArea, name, minLatitude, minLongitude, maxLatitude, maxLongitude, terrainGrid, highways, buildings )

    std::string name = "";
    float minLatitude = 0.0f;
    float minLongitude = 0.0f;
    float maxLatitude = 0.0f;
    float maxLongitude = 0.0f;
    OSMGrid terrainGrid;
    std::vector<OSMHighway> highways;
    std::vector<OSMBuilding> buildings;
};