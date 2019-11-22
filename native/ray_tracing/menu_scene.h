#pragma once
#include "ray_tracing/scene.h"

class VertexBuffer;

class MenuScene : public Scene {
public:
	MenuScene();
	HitInfo testRay( const Vector3f &vertice, const Vector3f &direction );
	void addBuffers( const std::vector<std::shared_ptr<VertexBuffer>>& buffers );
	void reset();
private:
	std::vector<std::shared_ptr<VertexBuffer>> mBuffers;
};
