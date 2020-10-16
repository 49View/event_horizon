//
// Created by dado on 16/10/2020.
//

#pragma once

#include <unordered_map>
#include <core/htypes_shared.hpp>

class VData;

using OSMAssetMap = std::unordered_map<ResourceRef, const VData*>;
