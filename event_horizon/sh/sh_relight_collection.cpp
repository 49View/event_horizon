#include "sh_relight_collection.h"
#include "core/profiler.h"
//#include "cpp_common/core/task_utils.h"
#include "core/node.hpp"
#include "ray_tracing/geometry_offset.h"

void SHRelightCollection::checkAnyRelight() {
    if ( toBeRelighted.empty()) return;
    PROFILE_BLOCK( "Checking relight" );

    size_t count = 0;

    std::vector<GeomAssetSP> modifiedGeoms;

    while ( auto hgToProcess = toBeRelighted.dequeue()) {
        hgToProcess->relightSH( false );
        modifiedGeoms.push_back( hgToProcess );
        count++;
    }

    PROFILE_ADD( "Items relighted", count );

    auto ev = std::make_shared<ChangeInfo>();
    ev->modifiedObjects = modifiedGeoms;
    notifyChanges( ev );

    return;
}

void SHRelightCollection::addBatch( std::shared_ptr<GeometryOffset> go, size_t howMany ) {
    if ( go->incrementDone( howMany ))
        toBeRelighted.enqueue( go->geometry );
}

void SHRelightCollection::ensureScheduledForRelighting( std::shared_ptr<GeometryOffset> go ) {
    if ( go->ensureScheduledForRelighting())
        toBeRelighted.enqueue( go->geometry );
}