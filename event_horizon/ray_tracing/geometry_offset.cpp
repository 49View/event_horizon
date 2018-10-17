#include "geometry_offset.h"
#include "core/profiler.h"
#include "core/task_utils.h"
#include "poly/hier_geom.hpp"

bool GeometryOffset::incrementDone( size_t howMany ) {
	totalDone += howMany;
	if ( totalDone >= length ) {
		scheduledToBeRelighted = true;
		return true;
	}

	return false;
}

bool GeometryOffset::ensureScheduledForRelighting() {
	if ( !scheduledToBeRelighted ) {
		scheduledToBeRelighted = true;
		return true;
	}
	return false;
}