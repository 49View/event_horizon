//
// Created by dado on 02/09/2020.
//

#pragma once

#include "core/serialization.hpp"


JSONDATA(OSMCenter, x, y, lat, lon, deltaPosInTile)
    double x = 0.0;
    double y = 0.0;
    double lat = 0.0;
    double lon = 0.0;
    std::vector<double> deltaPosInTile{};
};

JSONDATA(OSMSize, x, y)
    double x = 0.0;
    double y = 0.0;
};

JSONDATA(OSMPos, x, y)
    double x = 0.0;
    double y = 0.0;
};

namespace OSMMeshVertexType {
    static inline constexpr int triangles = 0;
    static inline constexpr int points = 1;
}

JSONDATA(OSMMesh, colour, part, vertexType, vertices)
    std::string colour;
    std::string part;
    int vertexType = OSMMeshVertexType::triangles;
    std::vector<V3f> vertices;
};

JSONDATA(OSMTags, name)
    std::string name;
};

JSONDATA(OSMElement, id, type, center, meshes, tags)
    std::string id;
    std::string type;
    OSMCenter center;
    std::vector<OSMMesh> meshes;
    OSMTags tags;
};

JSONDATA(OSMTileBoundary, zoom, tileX, tileY, bbox, rect, center, size, tilePos)
    uint64_t zoom = 0;
    uint64_t tileX = 0;
    uint64_t tileY = 0;

    Rect2f bbox{Rect2f::INVALID};
    Rect2f rect{Rect2f::INVALID};
    OSMCenter center{};
    OSMSize size{};
    OSMSize tilePos{};
};


JSONDATA(OSMData, tileBoundary, elements)
    OSMTileBoundary tileBoundary;
    std::vector<OSMElement> elements;
};
