#pragma once

#include <mutex>
#include <atomic>
#include <memory>

class HierGeom;

class  GeometryOffset {
public:
	GeometryOffset( GeomAssetSP g, size_t os, size_t l ) :geometry{ g }, offset{ os }, length{ l }, totalDone{ 0 } {}

	bool incrementDone( size_t howMany );
	bool ensureScheduledForRelighting();

public:
	GeomAssetSP geometry;
	size_t offset;
	size_t length;

private:

	bool scheduledToBeRelighted = false;
	std::atomic_size_t totalDone;
};
