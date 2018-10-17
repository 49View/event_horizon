#pragma once
#include <memory>
#include "ray_tracing/hit_info.h"
#include "ray_tracing/scene.h"
#include "core/math/vector3f.h"

class SceneManager {
public:
	HitInfo testRay( const Vector3f &vertice, const Vector3f &direction );
	void addScene( std::shared_ptr<Scene> scene );
private:
	std::vector<std::shared_ptr<Scene>> scenes;
};
