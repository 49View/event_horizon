#include "house_scene.h"
#include <stack>
#include "core/util.h"
#include "house/observable_collection.h"
#include "core/node.hpp"
#include "ray_tracing/ray_tracer.h"

HouseScene::HouseScene() {
	priority = 10;
	enabled = true;
	name = "geoms";
}

void HouseScene::watchCollection( std::shared_ptr<ObservableCollection> collection ) {
	collection->registerCallback( std::bind( &HouseScene::collectionChanged, this, std::placeholders::_1 ) );
}

void HouseScene::collectionChanged( std::shared_ptr<ChangeInfo> e ) {
	if ( e->removedHashes.size() == 0 && e->addedObjects.size() == 0 ) return;

	mIsDirty = true;

	for ( auto hashToRemove : e->removedHashes ) {
		//remove
		auto meshIds = externalToInternalMapping[hashToRemove];
		externalToInternalMapping.erase( hashToRemove );

		for ( auto meshId : meshIds )
			rayTracer->removeMesh( meshId );
	}

	for ( auto ancestorGeom : e->addedObjects ) {
		//add
		std::stack<std::shared_ptr<HierGeom>> stack;
		std::vector<unsigned> meshIDs;
		stack.push( ancestorGeom );

		while ( !stack.empty() ) {
			auto g = stack.top();
			stack.pop();

			if ( g->hasDataBacked() ) {
				auto sceneObjectId = rayTracer->addMesh( g->BakedGeom() );
				internalToExternalMapping[sceneObjectId] = ancestorGeom->Hash();
				meshIDs.push_back( sceneObjectId );
			}

			for ( auto& c : g->Children() ) stack.push( c );
		}

		externalToInternalMapping[ancestorGeom->Hash()] = meshIDs;
	}
}

HitInfo HouseScene::testRay( const Vector3f &startPosition, const Vector3f &direction ) {
	if ( mIsDirty ) {
		rayTracer->refreshDynamicScene();
		mIsDirty = false;
	}

	return Scene::testRay( startPosition, direction );
}