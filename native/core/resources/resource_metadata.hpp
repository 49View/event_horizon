//
// Created by Dado on 2020-06-14.
//

#pragma once

#include <core/serialization.hpp>

JSONDATA(EntityMetaData, group, source, name, brand, project, bbox3d, bboxSize, isPublic, isRestricted, contentType, hash, userId, thumb, color, tags)
    std::string group;
    std::string source;
    std::string name;
    std::string brand;
    std::string project;
    AABB bbox3d{AABB::MINVALID()};
    V3f bboxSize{V3fc::ZERO};
    bool isPublic = true;
    bool isRestricted = true;
    std::string contentType;
    std::string hash;
    std::string userId;
    std::string thumb;
    C4f color = C4fc::WHITE;
    std::vector<std::string> tags;
};

using ResourceMetadataList = std::vector<EntityMetaData>;
using CRefResourceMetadataList = const std::vector<EntityMetaData>&;
using ResourceMetadataListCallback = std::function<void(CRefResourceMetadataList)>;

namespace ResourceMetaData {
    void getListOf( const std::string& entityGroup, const std::string& tags, const ResourceMetadataListCallback& ccf );
}

