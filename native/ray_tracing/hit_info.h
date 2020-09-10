#pragma once
#include <memory>
#include "core/math/vector3f.h"

typedef int64_t ExternalId;

struct HitInfo {
	HitInfo() {
		objectId = -1;
		isHit = false;
		finalPosition = V3fc::ZERO;
		normal = V3fc::ZERO;
		distance = 0.0f;
	}

	ExternalId objectId;
	bool isHit;
	Vector3f finalPosition;
	Vector3f normal;
	float distance;

	static const HitInfo NoHit;
};
