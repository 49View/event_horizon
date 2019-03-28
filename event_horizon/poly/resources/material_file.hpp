//
// Created by Dado on 2019-03-27.
//

#pragma once

#include <core/heterogeneous_map.hpp>
#include <poly/resources/resource_utils.hpp>

class MaterialFile {
public:

    SerializableContainer serialize() const;
private:
    std::string type;
    std::vector<ResourceRef> images;
    HeterogeneousMap values;
};



