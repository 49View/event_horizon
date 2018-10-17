#include "scene_manager.h"
#include "core/util.h"

HitInfo SceneManager::testRay( const Vector3f &startPosition, const Vector3f &direction ) {
	for ( auto& scene : scenes ) {
		if ( scene->Enabled() ) {
			auto hitInfo = scene->testRay( startPosition, direction );
			if ( hitInfo.isHit ) {
				//				LOGR("Hitting scene %s", scene->Name().c_str());
				return hitInfo;
			}
		}
	}

	return HitInfo::NoHit;
}

void SceneManager::addScene( std::shared_ptr<Scene> scene ) {
	auto it = scenes.begin();

	for ( ; it < scenes.end(); it++ ) {
		if ( it->get()->Priority() > scene->Priority() )
			break;
	}

	scenes.insert( it, scene );
}