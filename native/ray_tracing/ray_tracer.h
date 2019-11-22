#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning( disable : 4324 )
#endif // _MSC_VER

#include <vector>
#include <memory>
#include <tuple>

#include "embree2/rtcore.h"
#include "embree2/rtcore_ray.h"

#include "ray_tracing/shapes.h"

class Vector4f;
class GeometryOffset;
class GeomData;
class VertexBuffer;
struct ChunkInfo;
class Vector3f;
class CancellationToken;

typedef std::tuple<ChunkInfo, std::vector<bool>> VertexHits;

class RayTracer {
public:
	RayTracer();
	~RayTracer();

	void createScene( const std::vector<Triangle> &triangles, const std::vector<Vector4f> &vertices );
	void createScene( const std::vector<std::shared_ptr<GeometryOffset>>& geometryOffsets, const std::vector<Vector4f> &vertices );

	void createDynamicScene();
	void refreshDynamicScene();
	unsigned addMesh( std::shared_ptr<GeomData> data );
	unsigned addMesh( std::shared_ptr<VertexBuffer> buffer );

	void removeMesh( unsigned id );
	void setMeshVisibility( unsigned id, bool visible );

	void testChunckOfRays( const std::vector<Vector4f>& vertices, const std::vector<Vector4f>& sampleDirections, const ChunkInfo& chunk, float *hits );
	VertexHits testChunckOfRaysPositive( const std::vector<Vector4f> &vertices, const std::vector<Vector4f>& sampleDirections, const ChunkInfo& chunk, float *dotAngles, const std::shared_ptr<CancellationToken> token );

	RTCRay testRay( const Vector3f &vertice, const Vector3f &direction );
	RTCRay testRay( const Vector4f &vertice, const Vector4f &direction );

	unsigned addQuadMesh( std::vector<Vector4f> verts ) const;
private:
	static void errorHandler( void* userPtr, const RTCError code, const char* str = nullptr );

private:
	RTCScene scene;
	RTCDevice device;
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif