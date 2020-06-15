//
// Created by Dado on 2020-06-14.
//

#pragma once

#include <core/serialization.hpp>

JSONDATA(EntityMetaData, group, source, name, project, isPublic, isRestricted, contentType, hash, userId, thumb, tags)
    std::string group;
    std::string source;
    std::string name;
    std::string project;
    bool isPublic;
    bool isRestricted;
    std::string contentType;
    std::string hash;
    std::string userId;
    std::string thumb;
    std::vector<std::string> tags;
};

using ResourceMetadataList = std::vector<EntityMetaData>;
using CRefResourceMetadataList = const std::vector<EntityMetaData>&;
using ResourceMetadataListCallback = std::function<void(CRefResourceMetadataList)>;

namespace ResourceMetaData {
    void getListOf( const std::string& entityGroup, const std::string& tags, const ResourceMetadataListCallback& ccf );
}

