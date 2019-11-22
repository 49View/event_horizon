#pragma once
#include <string>
#include <unordered_map>
#include "ray_tracing/hit_info.h"

class RayTracer;

class Scene {
public:
	Scene();
	virtual HitInfo testRay( const Vector3f &vertice, const Vector3f &direction );

	int Priority() const { return priority; }
	std::string Name() const { return name; }
	bool Enabled() const { return enabled; }
	void Enabled( bool val ) { enabled = val; }

protected:
	std::shared_ptr<RayTracer> rayTracer;
	std::unordered_map<unsigned, ExternalId> internalToExternalMapping;
	int priority;
	bool enabled;
	std::string name;
};
