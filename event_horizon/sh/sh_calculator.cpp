#include "sh_calculator.h"

#include <iomanip>
#include <fstream>
#include <numeric>
#include <cstring>

#include "core/math/spherical_harmonics.h"
#include "core/profiler.h"
#include "core/task_utils.h"

SHCalculator::SHCalculator() {
    SSH.NumSamples( 4 /*AOS->lightingOptions.sphericalHarmonicsSamplesSquaredRooted*/ );
    SSH.generateSamples();
    SSH.generateLightsCoeffs();

    totalsamples = SSH.NumSamples();
    totalFunctions = SSH.NumFunctions();

    shvaluesMatrix = std::make_shared<FlatMatrix>( totalsamples, totalFunctions );
    directionMatrix = std::make_shared<FlatMatrix>( totalsamples, 3 );

    {
        PROFILE_BLOCK( "Creating directions and shvalues" );

        for ( int q = 0; q < SSH.NumSamples(); q++ ) {
            sampleDirections.push_back( Vector4f( SSH.Sample( q )->direction ));
        }

        float *sm = shvaluesMatrix->data;

        for ( int q = 0; q < SSH.NumSamples(); q++ ) {
            for ( int l = 0; l < SSH.NumFunctions(); ++l ) {
                sm[q * totalFunctions + l] = static_cast<float>( SSH.Sample( q )->shValues[l] );
            }
        }

        float *dm = directionMatrix->data;

        int rowStartingIndex = 0;
        for ( int q = 0; q < totalsamples; q++ ) {
            std::memcpy( &dm[rowStartingIndex], &SSH.Sample( q )->direction, sizeof( Vector3f ));
            rowStartingIndex += 3;
        }
    }
}

SHCalculator::~SHCalculator() {
}

void SHCalculator::calculate( std::vector<GeomAssetSP>& sceneGeoms, const CalculatorOption& options,
                              std::shared_ptr<CancellationToken> token,
                              std::function<void( std::string, size_t, size_t )> onProgress,
                              std::function<void( void )> onCompletion ) {
    PROFILE_BLOCK( "Compute hits with Embree and SH with matrix multiplication" );

    PROFILE_ADD( "SH samples", sampleDirections.size());

    auto rt = std::make_shared<RayTracer>();
    auto ge = std::make_shared<MappingGeomExtensions>();
    auto context = std::make_shared<CalculatorContext>( ge, options, rt, onProgress, token, sceneGeoms );

    rt->createScene( ge->triangles, ge->vertices );

    std::vector<std::shared_ptr<std::future<VertexHits>>> tasks;

    for ( const auto& chunk : Chunks::BySize( options.verticesMaxBatchSize, ge->vertices.size())) {
        auto func = std::bind( &SHCalculator::calculateFirstPassAsync, this, std::placeholders::_1,
                               std::placeholders::_2 );
        tasks.push_back(
                std::make_shared<std::future<VertexHits>>( std::async( std::launch::async, func, chunk, context )));
    }

    auto results = collectResults( tasks, context );

    if ( context->options.maxBouncing > 0 )
        calculateBouncing( results, context );

    ge->finalize();
    onCompletion();
}

const std::vector<VertexHits>
SHCalculator::collectResults( const std::vector<std::shared_ptr<std::future<VertexHits>>>& tasks,
                              const std::shared_ptr<CalculatorContext> context ) const {
    PROFILE_BLOCK( "Collecting SH calculation result" );

    auto& results = collect( "SH", tasks, context->onProgress );

    countOccluded( context, results );

    return results;
}

void SHCalculator::countOccluded( const std::shared_ptr<CalculatorContext> context,
                                  const std::vector<VertexHits>& results ) const {
    PROFILE_BLOCK( "Stats for SH calculation first pass" );

    double totalChecks = static_cast<double>( sampleDirections.size() * context->ge->vertices.size());
    double occludedFirstPass = 0;

    for ( const auto& result : results ) {
        const auto& occludedSamples = std::get<1>( result );

        occludedFirstPass += std::count( occludedSamples.begin(), occludedSamples.end(), true );
    }

    PROFILE_ADD( "Occluded samples %", std::to_string( occludedFirstPass * 100.0 / totalChecks ));
}

void SHCalculator::calculateBouncing( const std::vector<VertexHits>& shCalculationResults,
                                      const std::shared_ptr<CalculatorContext> context ) const {
    const int n_coeff = 9; // SSH.NumFunctions();
    auto n_lighting = context->ge->vertices.size();
    const float area = 4.0 * M_PI;

    //TODO make it dynamic based on  context->options.maxBouncing

    //std::vector<std::vector<float>> sh_buffer( context->options.maxBouncing + 1 );

    float *sh_buffer[6]; // list of light bounce buffers.

    for ( int i = 0; i <= context->options.maxBouncing; ++i ) {
        sh_buffer[i] = new float[n_lighting * 3 * n_coeff];
        memset( sh_buffer[i], 0, n_lighting * 3 * n_coeff * sizeof( float ));
    }

    context->ge->fillSHValues( sh_buffer[0], n_coeff ); // already calculated from direct lighting

    context->ge->createAlbedos( 1 / M_PI );

    for ( auto bounceIdx = 1; bounceIdx <= context->options.maxBouncing; bounceIdx++ ) {
        PROFILE_BLOCK( "SH bounce " + std::to_string( bounceIdx ));
        if ( context->token->isCancellationRequested()) return;

        std::vector<std::shared_ptr<std::future<size_t>>> tasks;
        for ( auto& result : shCalculationResults ) {
            auto func = std::bind( &SHCalculator::calculateBounceAsync, this, std::placeholders::_1,
                                   std::placeholders::_2, std::placeholders::_3, std::placeholders::_4 );
            tasks.push_back( std::make_shared<std::future<size_t>>(
                    std::async( std::launch::async, func, bounceIdx, result, sh_buffer, context )));
        }

        auto& results = collect( "Bounce " + std::to_string( bounceIdx ), tasks, context->onProgress );

        if ( context->token->isCancellationRequested()) break;

        size_t initValue = 0;
        size_t distinctUpdatedVertices = std::accumulate( results.rbegin(), results.rend(), initValue );

        // divide through by n_samples
        const float factor = area / sampleDirections.size();

        float *ptr = sh_buffer[bounceIdx];
        double sum = 0.0f;
        for ( int j = 0; j < n_lighting * 3 * n_coeff; ++j ) {
            *ptr *= factor;
            sum += std::abs( *ptr );
            ptr++;
        }

        // sum all bounces of self transferred light back into sh_coeff
        float *ptra = sh_buffer[0];
        float *ptrb = sh_buffer[bounceIdx];
        for ( int j = 0; j < n_lighting * 3 * n_coeff; ++j )
            *ptra++ += *ptrb++;

        context->ge->update( sh_buffer[0], totalFunctions );

        PROFILE_ADD( "Delta", sum );
        PROFILE_ADD( "Delta avg",
                     distinctUpdatedVertices > 0 ? sum / static_cast<double>( distinctUpdatedVertices * 3 * n_coeff )
                                                 : 0 );
    }

    ////dump all buffers
    //{
    //	std::ofstream bounceFile;
    //	std::string tempFolder = "c:/temp/SH/";
    //	bounceFile.open( tempFolder + "bounce.txt", std::ios::trunc );
    //	const char newLine = '\n';
    //	const char separator = ' ';
    //	//size_t howMany = 100 * 3 * n_coeff;
    //	size_t howMany = n_lighting * 3 * n_coeff;
    //	for ( size_t v = 0; v < howMany; v++ ) {
    //		for ( int i = 0; i <= n_bounces; ++i ) {
    //			bounceFile << separator << sh_buffer[i][v];
    //		}
    //		bounceFile << newLine;
    //	}
    //}

    //// sum all bounces of self transferred light back into sh_coeff
    //for ( int i = 1; i <= n_bounces; ++i ) {
    //	float *ptra = sh_buffer[0];
    //	float *ptrb = sh_buffer[i];
    //	for ( int j = 0; j < n_lighting * 3 * n_coeff; ++j )
    //		*ptra++ += *ptrb++;
    //}

    //ge->update( sh_buffer[0], totalFunctions );

    // deallocate SH buffers
    for ( int i = 0; i <= context->options.maxBouncing; ++i ) {
        delete[] sh_buffer[i];
    }
}

size_t SHCalculator::calculateBounceAsync( int bounceIdx, const VertexHits& shCalculationResult, float **sh_buffer,
                                           const std::shared_ptr<CalculatorContext> context ) const {
    const auto& chunk = std::get<0>( shCalculationResult );

    TASK_PROFILE_BLOCK( "Bounce Calculation", chunk.index );

    size_t distinctUpdatedVertices = 0;

    std::vector<float> sh( 3 * totalFunctions );

    const auto& occludedSamples = std::get<1>( shCalculationResult );

    auto& vertices = context->ge->verticesPlus;

    auto& albedos = context->ge->albedos;
    auto& normals = context->ge->normals;
    auto& triangles = context->ge->triangles;

    auto& token = context->token;

    size_t counter = 0;
    for ( size_t vertexIndx = chunk.from; vertexIndx < chunk.to; vertexIndx++ ) {
        const auto& albedo = albedos[vertexIndx];

        bool hasUpdates = false;

        for ( size_t sampleIndx = 0; sampleIndx < sampleDirections.size(); sampleIndx++ ) {
            if ( token->isCancellationRequested()) return distinctUpdatedVertices;

            if ( occludedSamples[counter] ) {
                hasUpdates = true;
                auto rayResult = context->rayTracer->testRay( vertices[vertexIndx], sampleDirections[sampleIndx] );

                if ( rayResult.geomID != RTC_INVALID_GEOMETRY_ID) {
                    const auto& triangle = triangles[rayResult.primID];

                    float w = 1.0f - ( rayResult.u + rayResult.v );
                    float *ptr0 = sh_buffer[bounceIdx - 1] + triangle.v0 * 3 * totalFunctions;
                    float *ptr1 = sh_buffer[bounceIdx - 1] + triangle.v1 * 3 * totalFunctions;
                    float *ptr2 = sh_buffer[bounceIdx - 1] + triangle.v2 * 3 * totalFunctions;

                    for ( int k = 0; k < 3 * totalFunctions; ++k ) {
                        sh[k] = rayResult.u * ( *ptr0++ ) + rayResult.v * ( *ptr1++ ) + w * ( *ptr2++ );
                    }

                    float Hs = dot( normals[vertexIndx], sampleDirections[sampleIndx].xyz());

//					float pn = dot( normals[triangle.v0], sampleDirections[sampleIndx].xyz() );
                    float pn = dot( normalize( { rayResult.Ng[0], rayResult.Ng[1], rayResult.Ng[2] } ),
                                    sampleDirections[sampleIndx].xyz());
                    if ( pn < 0.0f ) {
                        for ( int k = 0; k < totalFunctions; ++k ) {
                            sh_buffer[bounceIdx][vertexIndx * 3 * totalFunctions + k + 0 * totalFunctions] +=
                                    albedo.x() * Hs * sh[k + 0 * totalFunctions];
                            sh_buffer[bounceIdx][vertexIndx * 3 * totalFunctions + k + 1 * totalFunctions] +=
                                    albedo.y() * Hs * sh[k + 1 * totalFunctions];
                            sh_buffer[bounceIdx][vertexIndx * 3 * totalFunctions + k + 2 * totalFunctions] +=
                                    albedo.z() * Hs * sh[k + 2 * totalFunctions];
                        }
                    }
                }
            }

            counter++;
        }

        if ( hasUpdates )
            distinctUpdatedVertices++;
    }

    return distinctUpdatedVertices;
}

VertexHits
SHCalculator::calculateFirstPassAsync( const ChunkInfo& chunk, const std::shared_ptr<CalculatorContext> ctx ) const {
    //	size_t totalVertices = chunk.to - chunk.from;
    //	size_t totalRays = totalVertices * totalsamples;
    FlatMatrix normalsMatrix = ctx->ge->getNormalsMatrixForChunck( chunk );
    //FlatMatrix hitsMatrix = FlatMatrix( totalVertices, totalsamples, true );

    //ctx->rayTracer->testChunckOfRays( ctx->ge->vertices, sampleDirections, chunk, hitsMatrix.data );

    //{
    //	TASK_PROFILE_BLOCK( "Hits counting", chunk.index );
    //	size_t countHits = hitsMatrix.countValues( 1.0f );
    //	PROFILE_ADD_INFO( " - Hits: " + std::to_string( countHits * 100.f / totalRays ) + "%" );
    //}

    std::ofstream matrixFile;

    if ( ctx->options.dumpMatrices ) {
        std::string tempFolder = "c:/temp/SH/";
        matrixFile.open( tempFolder + "matrices.txt", std::ios::trunc );

        matrixFile << std::setprecision( 4 );
        matrixFile << "Vertices\n";

        for ( auto v : ctx->ge->vertices ) {
            matrixFile << v.x() << '\t' << v.y() << '\t' << v.z() << '\n';
        }
        matrixFile << "\n";

        normalsMatrix.dumpMatrixTo( matrixFile, "Normals" );
        directionMatrix->dumpMatrixTo( matrixFile, "Sample directions" );
        //hitsMatrix.dumpMatrixTo( matrixFile, "Hits" );
        shvaluesMatrix->dumpMatrixTo( matrixFile, "SH functions values" );
    }

    {
        TASK_PROFILE_BLOCK( "SH Calculation", chunk.index );

        if ( ctx->token->isCancellationRequested()) return VertexHits();
        FlatMatrix normPerDirMatrix = normalsMatrix.multiplyTranspose( *( directionMatrix.get()));

        if ( ctx->options.dumpMatrices )
            normPerDirMatrix.dumpMatrixTo( matrixFile, "Normal X Directions" );

        //calculate Max( Normal X Directions,0) .* Hits
        /*float* curVal = normPerDirMatrix.data;
        float* curHit = hitsMatrix.data;
        float threshold = 0.000001f;
        for ( size_t i = 0; i < totalRays; i++ ) {
            if ( *curVal < threshold || *curHit < threshold )
                *curVal = 0.0f;

            curVal++;
            curHit++;
        }*/

        /*float* curVal = normPerDirMatrix.data;
        float threshold = 0.000001f;
        for ( size_t i = 0; i < totalRays; i++ ) {
            if ( *curVal < threshold )
                *curVal = 0.0f;

            curVal++;
        }*/

        if ( ctx->token->isCancellationRequested()) return VertexHits();
        auto hits = ctx->rayTracer->testChunckOfRaysPositive( ctx->ge->verticesPlus, sampleDirections, chunk,
                                                              normPerDirMatrix.data, ctx->token );

        float alpha = 4.0f * M_PI / SSH.NumSamples();

        if ( ctx->token->isCancellationRequested()) return VertexHits();
        FlatMatrix vertPerFuncMatrix = normPerDirMatrix.multiply( *( shvaluesMatrix.get()), alpha );

        if ( ctx->options.dumpMatrices ) {
            normPerDirMatrix.dumpMatrixTo( matrixFile, "SH component = Max( Normal X Directions,0) .* Hits" );
            vertPerFuncMatrix.dumpMatrixTo( matrixFile, "SH component X FunctionValues" );
            matrixFile.close();
        }

        if ( ctx->token->isCancellationRequested()) return VertexHits();
        ctx->ge->update( vertPerFuncMatrix, chunk, totalFunctions );

        return hits;
    }
}
