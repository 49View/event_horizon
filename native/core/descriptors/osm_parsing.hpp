//
// Created by dado on 02/09/2020.
//

#pragma once

#include "core/serialization.hpp"

JSONDATA(OSMCenter, x, y, lat, lon)
    double x;
    double y;
    double lat;
    double lon;
};

JSONDATA(OSMGroup, colour, triangles)
    std::string colour;
    std::vector<V3f> triangles;
};

JSONDATA(OSMTags, name)
    std::string name;
};

JSONDATA(OSMElement, id, type, center, groups, tags)
    std::string id;
    std::string type;
    OSMCenter center;
    std::vector<OSMGroup> groups;
    OSMTags tags;
};

JSONDATA(OSMData, elements)
    std::vector<OSMElement> elements;
};
