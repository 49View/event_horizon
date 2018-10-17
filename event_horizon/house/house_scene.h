#pragma once
#include "ray_tracing/scene.h"

class ObservableCollection;
class ChangeInfo;

class HouseScene : public Scene {
public:
	HouseScene();

	void watchCollection( std::shared_ptr<ObservableCollection> collection );
	HitInfo testRay( const Vector3f &vertice, const Vector3f &direction );
private:
	void collectionChanged( std::shared_ptr<ChangeInfo> e );

	std::unordered_map<int64_t, std::vector<unsigned>> externalToInternalMapping;
	bool mIsDirty = true;
};
