//
// Created by Dado on 2019-04-08.
//

#include "gpuv_data_manager.hpp"

#ifdef _OPENGL
#include <graphics/opengl/gpuv_data.h>
#endif

std::shared_ptr<GPUVData> GPUVDataManager::addGPUVData( cpuVBIB&& _val, const StringUniqueCollection& _names ) {
    auto gpuv = std::make_shared<GPUVData>( std::move(_val) );
    add( _names, gpuv );
    return gpuv;
}
