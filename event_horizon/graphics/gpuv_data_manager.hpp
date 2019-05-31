//
// Created by Dado on 2019-04-08.
//

#pragma once

#include <core/hash_shared_map.hpp>
#include <core/htypes_shared.hpp>

class  GPUVData;
struct cpuVBIB;

class GPUVDataManager : public HashSharedMap<GPUVData> {
public:
    std::shared_ptr<GPUVData> addGPUVData( cpuVBIB&& _val, const StringUniqueCollection& _names );
};



