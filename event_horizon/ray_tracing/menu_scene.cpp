#include "menu_scene.h"
#include "ray_tracing/ray_tracer.h"
#include "ray_tracing/vertex_buffer.h"

MenuScene::MenuScene() {
	priority = 2;
	enabled = false;
	name = "menu";
}

void MenuScene::addBuffers( const std::vector<std::shared_ptr<VertexBuffer>>& buffers ) {
	mBuffers = buffers;

	for ( auto buffer : buffers ) {
		auto sceneObjectId = rayTracer->addMesh( buffer );
		internalToExternalMapping[sceneObjectId] = buffer->Id();
	}
}

void MenuScene::reset() {
	if ( internalToExternalMapping.size() == 0 ) return;

	for ( auto& item : internalToExternalMapping )
		rayTracer->removeMesh( item.first );

	internalToExternalMapping.clear();
}

HitInfo MenuScene::testRay( const Vector3f &startPosition, const Vector3f &direction ) {
	bool requireRefresh = false;
	for ( auto buffer : mBuffers ) {
		if ( buffer->IsDirty() ) {
			buffer->IsDirty( false );
			requireRefresh = true;
		}
	}

	if ( requireRefresh )
		rayTracer->refreshDynamicScene();

	return Scene::testRay( startPosition, direction );
}