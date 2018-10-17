#include "scene.h"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning( disable : 4324 )
#endif // _MSC_VER

#include "embree2/rtcore_ray.h"
#include "ray_tracing/ray_tracer.h"

Scene::Scene() {
	rayTracer = std::make_shared<RayTracer>();
	rayTracer->createDynamicScene();
}

HitInfo Scene::testRay( const Vector3f &startPosition, const Vector3f &direction ) {
	auto ray = rayTracer->testRay( startPosition, direction );

	if ( ray.geomID == RTC_INVALID_GEOMETRY_ID )
		return HitInfo::NoHit;

	auto obj = internalToExternalMapping.find( ray.geomID );

	if ( obj == internalToExternalMapping.end() )
		return HitInfo::NoHit;

	HitInfo hitInfo;

	hitInfo.objectId = obj->second;
	hitInfo.isHit = true;
	hitInfo.finalPosition = startPosition + direction * ( ray.tfar - .001f );
	hitInfo.normal = normalize( Vector3f( ray.Ng[0], ray.Ng[1], ray.Ng[2] ) );
	hitInfo.distance = ray.tfar;

	return hitInfo;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif