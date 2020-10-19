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

JSONDATA(OSMMesh, colour, part, vertexType, vertices, minHeight, maxHeight)
    std::string colour;
    std::string part;
    int vertexType = OSMMeshVertexType::triangles;
    std::vector<V3f> vertices;
    float minHeight = 0.0f;
    float maxHeight = 0.0f;
};

JSONDATA(OSMElement, id, type, center, tags, meshes )
    std::string id;
    std::string type;
    OSMCenter center;
    KVStringMap tags;
    std::vector<OSMMesh> meshes;
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
