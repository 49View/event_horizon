#pragma once

#include <mutex>
#include <atomic>
#include "core/safe_queues.h"
#include "house/observable_collection.h"

class HierGeom;

class GeometryOffset;

class SHRelightCollection : public ObservableCollection {
public:
    void checkAnyRelight();
    void addBatch( std::shared_ptr<GeometryOffset> go, size_t howMany );
    void ensureScheduledForRelighting( std::shared_ptr<GeometryOffset> go );

private:
    ConcurrentQueue<HierGeom> toBeRelighted;
};