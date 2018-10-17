#pragma once

#include <functional>
#include <future>
#include <vector>
#include <string>
#include "sh/calculator_context.h"
#include "ray_tracing/ray_tracer.h"

class CancellationToken;

class SHCalculator {
public:
    SHCalculator();
    ~SHCalculator();

    void calculate( std::vector<std::shared_ptr<HierGeom>>& sceneGeoms, const CalculatorOption& options,
                    std::shared_ptr<CancellationToken> token,
                    std::function<void( std::string, size_t, size_t )> onProgress,
                    std::function<void( void )> onCompletion );

private:

    void calculateBouncing( const std::vector<VertexHits>& shCalculationResults,
                            const std::shared_ptr<CalculatorContext> context ) const;

    const std::vector<VertexHits> collectResults( const std::vector<std::shared_ptr<std::future<VertexHits>>>& tasks,
                                                  const std::shared_ptr<CalculatorContext> context ) const;

    void
    countOccluded( const std::shared_ptr<CalculatorContext> context, const std::vector<VertexHits>& results ) const;

    size_t calculateBounceAsync( int bounceIdx, const VertexHits& shCalculationResult, float **sh_buffer,
                                 const std::shared_ptr<CalculatorContext> context ) const;

    VertexHits calculateFirstPassAsync( const ChunkInfo& chunk, const std::shared_ptr<CalculatorContext> ctx ) const;

private:

    int totalsamples;
    int totalFunctions;
    std::vector<Vector4f> sampleDirections;
    std::shared_ptr<FlatMatrix> shvaluesMatrix;
    std::shared_ptr<FlatMatrix> directionMatrix;
};
