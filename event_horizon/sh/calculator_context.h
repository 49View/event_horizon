#pragma once

#include "sh/mapping_geom_extensions.h"

class RayTracer;

class CancellationToken;

struct CalculatorOption {
    bool dumpMatrices = false;
    int verticesMaxBatchSize = 10000;
    bool createDistinctVertices = false;
    int maxBouncing = 1;
    float bouncingExitThreshold = 0.001f;
};

struct CalculatorContext {
    CalculatorContext( std::shared_ptr<MappingGeomExtensions> geomExt, const CalculatorOption& opt,
                       std::shared_ptr<RayTracer> rt, std::function<void( std::string, size_t, size_t )> progressFunc,
                       std::shared_ptr<CancellationToken> tk,  std::vector<std::shared_ptr<HierGeom>>& sceneGeoms ) :
            ge( geomExt ), options( opt ), rayTracer( rt ), onProgress( progressFunc ), token( tk ) {
        ge->ensureInitialized( sceneGeoms );
    }

    std::shared_ptr<MappingGeomExtensions> ge;
    const CalculatorOption& options;
    std::shared_ptr<RayTracer> rayTracer;
    std::function<void( std::string, size_t, size_t )> onProgress;
    std::shared_ptr<CancellationToken> token;
};