#include "mapping_geom_extensions.h"
#include <stack>

#include "ray_tracing/geometry_offset.h"
#include "ray_tracing/shapes.h"
//#include "graphics/render_list.h"
#include "core/profiler.h"
#include "core/service_factory.h"
#include "sh/sh_relight_collection.h"
#include "core/node.hpp"

MappingGeomExtensions::MappingGeomExtensions() {
    shRelightCollection = ServiceFactory::get<SHRelightCollection>();
}

void MappingGeomExtensions::initialize( std::vector<GeomSP>& sceneGeoms ) {
    PROFILE_BLOCK( "Initializing geom extensions" );

    //std::queue<std::shared_ptr< HierGeom>> q;
    std::stack<std::tuple<int, GeomSP>> s;

    //for ( auto& g : RL.SceneGeoms() ) q.push( g.second );
    for ( auto& g : sceneGeoms ) {
        g->bake();
        s.push( std::make_tuple( 0, g ));
    }

    std::vector<std::string> exclude;
//    exclude.push_back( "Mesh8" ); //hinges
//    exclude.push_back( "Maxlight_ES_200811_plafon_FORM:Sphere002" ); //spotlights
//    exclude.push_back( "Maxlight_ES_200811_plafon_FORM:Cylinder004" ); //spotlights
//    exclude.push_back( "Maxlight_ES_200811_plafon_FORM:Sphere001" ); //spotlights

    //while ( !q.empty() ) {
    while ( !s.empty()) {
        //std::shared_ptr< HierGeom> g = q.front();
        //q.pop();

        auto item = s.top();
        auto g = std::get<1>( item );
        auto level = std::get<0>( item );
        s.pop();

        /*std::string receiver = " ->R";
        std::string indent = "  ";
        std::string totalIndent;
        for ( size_t i = 0; i < level; i++ ) totalIndent = totalIndent + indent;*/

        bool toInclude = g->hasDataBacked() && !g->SHReceiver();

        if ( toInclude ) {
            for ( auto str : exclude ) {
                if ( g->Name() == str ) {
                    toInclude = false;
                    break;
                }
            }
        }

        if ( toInclude ) {
            float opactity = g->BakedGeom()->getOpacity();

            if ( opactity > 0.2f ) {
                std::vector<Vector3f>& coords = g->BakedGeom()->Coords3d();
                std::vector<Vector3f>& normalVector = g->BakedGeom()->Normals3d();

                int32_t offset = static_cast<int32_t>( vertices.size());

                geometryOffsets.push_back( std::make_shared<GeometryOffset>( g, vertices.size(), coords.size()));

                for ( int32_t i = 0; i < coords.size(); i++ ) {
                    vertices.push_back( Vector4f( coords[i] ));

                    verticesPlus.push_back( Vector4f( coords[i] ));
                    //verticesPlus.push_back( Vector4f( coords[i] + normalVector[i] * 0.0001f ) );
                    normals.push_back( normalVector[i] );
                }

                const int32_t *indices = g->BakedGeom()->Indices();

                float deltainsideAngle = 0.00f;
                float deltaUpNormal = 0.00f;
                //float u = 0.01f;

                for ( size_t i = 0; i < g->BakedGeom()->numIndices(); i += 3 ) {
                    auto v0 = coords[indices[i]];
                    auto v1 = coords[indices[i + 1]];
                    auto v2 = coords[indices[i + 2]];

                    auto vBar = (( v0 + v1 + v2 ) / 3 );

                    //u can be a fix amount
                    float u0 = deltainsideAngle / distance<Vector3f>( v0, vBar );
                    float u1 = deltainsideAngle / distance<Vector3f>( v1, vBar );
                    float u2 = deltainsideAngle / distance<Vector3f>( v2, vBar );

                    ////or can be in percent
                    //float u0 = u;
                    //float u1 = u;
                    //float u2 = u;

                    auto newV0 = v0 * ( 1 - u0 ) + vBar * u0;
                    auto newV1 = v1 * ( 1 - u1 ) + vBar * u1;
                    auto newV2 = v2 * ( 1 - u2 ) + vBar * u2;

                    auto i0 = ( indices[i] ) + offset;
                    auto i1 = ( indices[i + 1] ) + offset;
                    auto i2 = ( indices[i + 2] ) + offset;

                    triangles.push_back( Triangle( i0, i1, i2 ));

                    //normal can be
                    //re-calculated:
                    //auto normal = normalize( crossProduct( newV0, newV1, newV2 ) );

                    //normals[i0] = normal;
                    //normals[i1] = normal;
                    //normals[i2] = normal;

                    ////all like vertex 0
                    //normals[i1] = normals[i0];
                    //normals[i2] = normals[i0];

                    //or keep the same
                    //[nothing to do as normals contains the right value]

                    //ASSERT( normal == normalVector[indices[i]] );

                    //auto deltaNorm = normal * deltaUpNormal;
                    auto deltaNorm0 = normalVector[indices[i]] * deltaUpNormal;
                    auto deltaNorm1 = normalVector[indices[i + 1]] * deltaUpNormal;
                    auto deltaNorm2 = normalVector[indices[i + 2]] * deltaUpNormal;

                    verticesPlus[i0] = Vector4f( newV0 + deltaNorm0 );
                    verticesPlus[i1] = Vector4f( newV1 + deltaNorm1 );
                    verticesPlus[i2] = Vector4f( newV2 + deltaNorm2 );
                }

                //PROFILE_ADD( totalIndent + g->Name() + receiver, coords.size() );
            } else {
                //LOGR( "Opacity under threshold %f", opactity );
            }
        } else {
            //PROFILE_ADD( totalIndent + g->Name(), 0 );
        }

        //for ( auto& c : g->Children() ) q.push( c );
        for ( auto& c : g->Children()) s.push( std::make_tuple( level + 1, c ));
    }

    PROFILE_ADD( "Vertices", vertices.size());
}

void MappingGeomExtensions::update( const FlatMatrix& vertPerFuncMatrix, const ChunkInfo& chunk,
                                    const int& numberOfSHFunctions ) const {
    if ( chunk.from >= chunk.to || geometryOffsets.size() == 0 ) return;

    TASK_PROFILE_BLOCK( "SH update", chunk.index );

    float *currentRow = vertPerFuncMatrix.data;

    size_t current = chunk.from;
    size_t currentGeomIdx = getGeometryIndexAt( current );

    do {
        auto go = geometryOffsets[currentGeomIdx];
        auto geom = go->geometry->Geom();

        Vector3f albedo = geom->getColor() / M_PI;

        int32_t fromItem = static_cast<int32_t>( current - go->offset );
        int32_t howMany = static_cast<int32_t>( min( chunk.to - current, go->length - fromItem ));

        for ( int32_t i = fromItem; i < fromItem + howMany; i++ ) {
            geom->shCoeffRAt( i ).setData( currentRow, albedo.x());
            geom->shCoeffGAt( i ).setData( currentRow, albedo.y());
            geom->shCoeffBAt( i ).setData( currentRow, albedo.z());

            currentRow += numberOfSHFunctions;
        }

        shRelightCollection->addBatch( go, howMany );
        currentGeomIdx++;
        current += howMany;
    } while ( current < chunk.to );
}

void MappingGeomExtensions::update( float *sh_buffer, int numberOfSHFunctions ) const {
    for ( auto& go : geometryOffsets ) {
        const auto& gd = go->geometry->Geom();
        for ( int32_t i = 0; i < gd->numVerts(); i++ ) {
            gd->shCoeffRAt( i ).setData( sh_buffer );
            sh_buffer += numberOfSHFunctions;
            gd->shCoeffGAt( i ).setData( sh_buffer );
            sh_buffer += numberOfSHFunctions;
            gd->shCoeffBAt( i ).setData( sh_buffer );
            sh_buffer += numberOfSHFunctions;
        }

        shRelightCollection->addBatch( go, go->length );
    }
}

size_t MappingGeomExtensions::getGeometryIndexAt( size_t t ) const {
    if ( geometryOffsets.size() == 1 || t == 0 ) return 0;
    if ( geometryOffsets.back()->offset + geometryOffsets.back()->length < t ) return geometryOffsets.size() - 1;

    size_t from = 0;
    size_t to = geometryOffsets.size() - 1;

    while ( t < geometryOffsets[from]->offset || t >= geometryOffsets[from]->offset + geometryOffsets[from]->length ) {
        size_t middle = from + ( to - from + 1 ) / 2;

        if ( geometryOffsets[middle]->offset > t ) {
            to = middle - 1;
        } else {
            from = middle;
        }
    }

    return from;
}

FlatMatrix MappingGeomExtensions::getNormalsMatrixForChunck( const ChunkInfo& chunk ) const {
    TASK_PROFILE_BLOCK( "Creating normals matrix", chunk.index );

    if ( chunk.from >= chunk.to || geometryOffsets.size() == 0 ) return FlatMatrix( 0, 0 );

    size_t totalVertices = chunk.to - chunk.from;
    FlatMatrix matrix( totalVertices, 3 );
    int rowStartingIndex = 0;
    float *m = matrix.data;

    size_t current = chunk.from;
    size_t currentGeomIdx = getGeometryIndexAt( current );

    do {
        auto go = geometryOffsets[currentGeomIdx];
        auto normalsVector = go->geometry->BakedGeom()->Normals3d();
        size_t fromItem = current - go->offset;

        size_t howMany = min( chunk.to - current, go->length - fromItem );

        for ( size_t i = fromItem; i < fromItem + howMany; i++ ) {
            //std::memcpy( &m[rowStartingIndex], &normalsVector[i], sizeof( Vector3f ) );
            std::memcpy( &m[rowStartingIndex], &normals[i + go->offset], sizeof( Vector3f ));

            rowStartingIndex += 3;
        }

        currentGeomIdx++;
        current += howMany;
    } while ( current < chunk.to );

    //matrix.dumpMatrixTo( std::cout, "" );
    return matrix;
}

void MappingGeomExtensions::finalize() {
    for ( auto& go : geometryOffsets ) {
        shRelightCollection->ensureScheduledForRelighting( go );
    }
}

void MappingGeomExtensions::fillSHValues( float *sh_buffer, const int& numberOfSHFunctions ) const {
    for ( auto& go : geometryOffsets ) {
        const auto& gd = go->geometry->Geom();
        for ( int32_t i = 0; i < gd->numVerts(); i++ ) {
            gd->shCoeffRAt( i ).getData( sh_buffer );
            sh_buffer += numberOfSHFunctions;
            gd->shCoeffGAt( i ).getData( sh_buffer );
            sh_buffer += numberOfSHFunctions;
            gd->shCoeffBAt( i ).getData( sh_buffer );
            sh_buffer += numberOfSHFunctions;
        }
    }
}

void MappingGeomExtensions::createAlbedos( float k ) {
    PROFILE_BLOCK( "Get albedos for bouncing" );

    albedos.clear();
    albedos.reserve( vertices.size());

    for ( auto& go : geometryOffsets ) {
        const auto& color = ( go->geometry->Geom()->getColor() * k );
        auto numVerts = go->geometry->BakedGeom()->numVerts();

        for ( int32_t i = 0; i < numVerts; i++ ) {
            albedos.emplace_back( color );
        }
    }
}

void MappingGeomExtensions::ensureInitialized( std::vector<GeomSP>& sceneGeoms ) {
    if ( !isInitialized ) {
        initialize( sceneGeoms );
        isInitialized = true;
    }
}
