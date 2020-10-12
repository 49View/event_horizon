//
// Created by dado on 02/09/2020.
//

#pragma once

#include "core/serialization.hpp"

namespace OSMElementName {
    static inline std::string building() {
        return "building";
    }
    static inline std::string park() {
        return "park";
    }
    static inline std::string water() {
        return "water";
    }
    static inline std::string tree() {
        return "tree";
    }
    static inline std::string barrier() {
        return "barrier";
    }
}

template <typename T>
[[maybe_unused]] T latToY( T lat ) {
    return radToDeg(log(tan((lat / 90.0 + 1.0) * M_PI_4 )));
}

template <typename T>
[[maybe_unused]] float calcGeoDistance( T lat1, T lon1, T lat2, T lon2 ) {
    auto R = 6371e3; // metres
    auto phi1 = degToRad(lat1); // φ, λ in radians
    auto phi2 = degToRad(lat2);
    auto deltaPhi = degToRad(lat2-lat1);
    auto deltaLambda = degToRad(lon2-lon1);

    auto a = sin(deltaPhi/2.0) * sin(deltaPhi/2.0) +
             cos(phi1) * cos(phi2) *
             sin(deltaLambda/2.0) * sin(deltaLambda/2.0);
    auto c = 2.0 * atan2(sqrt(a), sqrt(1.0-a));

    auto d = (R * c); // in metres

    return d;
}

JSONDATA(OSMCenter, x, y, lat, lon, deltaPosInTile)
    double x = 0.0;
    double y = 0.0;
    double lat = 0.0;
    double lon = 0.0;
    std::vector<double> deltaPosInTile{};
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

JSONDATA(OSMData, elements)
    std::vector<OSMElement> elements;
};
