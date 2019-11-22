#include "ray_tracer.h"

#include "core/chunks.h"
#include "core/profiler.h"
#include "core/math/vector3f.h"
#include "core/task_utils.h"
#include "core/node.hpp"
#include "ray_tracing/geometry_offset.h"
#include "ray_tracing/vertex_buffer.h"
#include "core/util.h"

RayTracer::RayTracer() {
	device = rtcNewDevice( NULL );
	errorHandler( nullptr, rtcDeviceGetError( device ) );
	rtcDeviceSetErrorFunction2( device, &errorHandler, nullptr );
}

RayTracer::~RayTracer() {
	rtcDeleteScene( scene );
	rtcDeleteDevice( device );
}

void RayTracer::errorHandler( void* /*userPtr*/, const RTCError code, const char* str ) {
	if ( code == RTC_NO_ERROR )
		return;

	LOGR( "Embree error %s", str );

	switch ( code ) {
	case RTC_UNKNOWN_ERROR: LOGR( "RTC_UNKNOWN_ERROR" ); break;
	case RTC_INVALID_ARGUMENT: LOGR( "RTC_INVALID_ARGUMENT" ); break;
	case RTC_INVALID_OPERATION: LOGR( "RTC_INVALID_OPERATION" ); break;
	case RTC_OUT_OF_MEMORY: LOGR( "RTC_OUT_OF_MEMORY" ); break;
	case RTC_UNSUPPORTED_CPU: LOGR( "RTC_UNSUPPORTED_CPU" ); break;
	case RTC_CANCELLED: LOGR( "RTC_CANCELLED" ); break;
	default: LOGR( "invalid error code" ); break;
	}

	//exit(1);
}

void RayTracer::createScene( const std::vector<Triangle> &triangles, const std::vector<Vector4f> &vertices ) {
	PROFILE_BLOCK( "Creating scene" );

	int scene_flags = RTC_SCENE_STATIC | RTC_SCENE_COHERENT;
	//int scene_aflags = RTC_INTERSECT1 | RTC_INTERSECT_STREAM | RTC_INTERPOLATE;
	int scene_aflags = RTC_INTERSECT1; // | RTC_INTERPOLATE;
	scene = rtcDeviceNewScene( device, (RTCSceneFlags)scene_flags, (RTCAlgorithmFlags)scene_aflags );

	RTCGeometryFlags geomFlags = RTC_GEOMETRY_STATIC;
	size_t numTriangles = triangles.size();
	size_t numVertices = vertices.size();

	unsigned geomID = rtcNewTriangleMesh( scene, geomFlags, numTriangles, numVertices );

	Vector4f* vs = (Vector4f*)rtcMapBuffer( scene, geomID, RTC_VERTEX_BUFFER );

	std::memcpy( vs, vertices.data(), sizeof( Vector4f ) * numVertices );

	rtcUnmapBuffer( scene, geomID, RTC_VERTEX_BUFFER );

	Triangle* ts = (Triangle*)rtcMapBuffer( scene, geomID, RTC_INDEX_BUFFER );

	for ( size_t i = 0; i < numTriangles; i++ ) {
		ts[i].v0 = triangles[i].v0;
		ts[i].v1 = triangles[i].v1;
		ts[i].v2 = triangles[i].v2;
	}

	rtcUnmapBuffer( scene, geomID, RTC_INDEX_BUFFER );
	rtcCommit( scene );
}

void RayTracer::createScene( const std::vector<std::shared_ptr< GeometryOffset>>& geometryOffsets, const std::vector<Vector4f>& vertices ) {
	PROFILE_BLOCK( "Creating scene" );

	int scene_flags = RTC_SCENE_STATIC | RTC_SCENE_COHERENT;
	//int scene_aflags = RTC_INTERSECT1 | RTC_INTERSECT_STREAM | RTC_INTERPOLATE;
	int scene_aflags = RTC_INTERSECT1; // | RTC_INTERPOLATE;
	scene = rtcDeviceNewScene( device, (RTCSceneFlags)scene_flags, (RTCAlgorithmFlags)scene_aflags );

	RTCGeometryFlags geomFlags = RTC_GEOMETRY_STATIC;

	for ( auto go : geometryOffsets ) {
		auto gd = go->geometry->BakedGeom();
		const int32_t* indices = gd->Indices();
		int offset = static_cast<int>( go->offset );

		size_t numTriangles = gd->numIndices() / 3;
		size_t numVertices = go->length;
		unsigned geomID = rtcNewTriangleMesh( scene, geomFlags, numTriangles, numVertices );

		Vector4f* vs = (Vector4f*)rtcMapBuffer( scene, geomID, RTC_VERTEX_BUFFER );

		std::memcpy( vs, vertices.data() + offset, sizeof( Vector4f ) * numVertices );

		rtcUnmapBuffer( scene, geomID, RTC_VERTEX_BUFFER );

		Triangle* ts = (Triangle*)rtcMapBuffer( scene, geomID, RTC_INDEX_BUFFER );

		size_t index = 0;
		for ( size_t i = 0; i < gd->numIndices(); i += 3 ) {
			ts[index].v0 = indices[i]; // +offset;
			ts[index].v1 = indices[i + 1]; // +offset;
			ts[index].v2 = indices[i + 2]; // +offset;

			index++;
		}

		rtcUnmapBuffer( scene, geomID, RTC_INDEX_BUFFER );
	}

	rtcCommit( scene );
}

void RayTracer::createDynamicScene() {
	int scene_flags = RTC_SCENE_DYNAMIC | RTC_SCENE_COHERENT;
	int scene_aflags = RTC_INTERSECT1;
	scene = rtcDeviceNewScene( device, (RTCSceneFlags)scene_flags, (RTCAlgorithmFlags)scene_aflags );
}

void RayTracer::refreshDynamicScene() {
	rtcCommit( scene );
}

unsigned RayTracer::addMesh( std::shared_ptr<GeomData> data ) {
	RTCGeometryFlags geomFlags = RTC_GEOMETRY_STATIC;

	std::vector<Vector3f>& coords = data->Coords3d();

	unsigned geomID = rtcNewTriangleMesh( scene, geomFlags, data->numIndices() / 3, coords.size() );

	Vector4f* vs = (Vector4f*)rtcMapBuffer( scene, geomID, RTC_VERTEX_BUFFER );

	for ( size_t i = 0; i < coords.size(); i++ ) {
		vs[i] = coords[i];
	}

	rtcUnmapBuffer( scene, geomID, RTC_VERTEX_BUFFER );

	Triangle* ts = (Triangle*)rtcMapBuffer( scene, geomID, RTC_INDEX_BUFFER );

	const int32_t* indices = data->Indices();

	size_t index = 0;
	for ( size_t i = 0; i < data->numIndices(); i += 3 ) {
		ts[index].v0 = indices[i]; // +offset;
		ts[index].v1 = indices[i + 1]; // +offset;
		ts[index].v2 = indices[i + 2]; // +offset;

		index++;
	}

	rtcUnmapBuffer( scene, geomID, RTC_INDEX_BUFFER );

	return geomID;
}

unsigned RayTracer::addMesh( std::shared_ptr<VertexBuffer> buffer ) {
	return buffer->putInScene( scene );
}

unsigned RayTracer::addQuadMesh( std::vector<Vector4f> verts ) const {
	RTCGeometryFlags geomFlags = RTC_GEOMETRY_STATIC;

	unsigned geomID = rtcNewQuadMesh( scene, geomFlags, 1, 4 );

	Vector4f* vs = (Vector4f*)rtcMapBuffer( scene, geomID, RTC_VERTEX_BUFFER );

	for ( size_t i = 0; i < verts.size(); i++ ) {
		vs[i] = verts[i];
	}

	rtcUnmapBuffer( scene, geomID, RTC_VERTEX_BUFFER );

	Quad* ts = (Quad *)rtcMapBuffer( scene, geomID, RTC_INDEX_BUFFER );

	size_t index = 0;
	for ( size_t i = 0; i < 1; i++ ) {
		ts[index].v0 = 0;
		ts[index].v1 = 1;
		ts[index].v2 = 2;
		ts[index].v3 = 3;
		index++;
	}

	rtcUnmapBuffer( scene, geomID, RTC_INDEX_BUFFER );

	return geomID;
}

void RayTracer::removeMesh( unsigned id ) {
	rtcDeleteGeometry( scene, id );
}

void RayTracer::setMeshVisibility( unsigned id, bool visible ) {
	if ( visible )
		rtcEnable( scene, id );
	else
		rtcDisable( scene, id );
}

void RayTracer::testChunckOfRays( const std::vector<Vector4f> &vertices, const std::vector<Vector4f> &sampleDirections, const ChunkInfo& chunk, float *hits ) {
	TASK_PROFILE_BLOCK( "Testing rays", chunk.index );

	float inf = std::numeric_limits<float>::infinity();

	RTCRay emptyRay;
	emptyRay.tnear = 0.0f;
	emptyRay.tfar = inf;
	emptyRay.instID = RTC_INVALID_GEOMETRY_ID;
	emptyRay.geomID = RTC_INVALID_GEOMETRY_ID;
	emptyRay.primID = RTC_INVALID_GEOMETRY_ID;
	emptyRay.mask = 0xFFFFFFFF;
	emptyRay.time = 0.0f;

	size_t raySize = sizeof( RTCRay );
	size_t cSize = sizeof( float ) * 3;

	size_t totalColumns = sampleDirections.size();

	//vertices are on rows and samples on columns
	for ( size_t vi = chunk.from; vi < chunk.to; vi++ ) {
		for ( size_t di = 0; di < sampleDirections.size(); di++ ) {
			RTCRay ray;

			std::memcpy( &ray, &emptyRay, raySize );
			std::memcpy( &ray.org, &vertices[vi], cSize );
			std::memcpy( &ray.dir, &sampleDirections[di], cSize );

			//rtcIntersect( scene, ray );
			rtcOccluded( scene, ray ); //=> ray.geomID == 0 if occuluded
			hits[( vi - chunk.from ) * totalColumns + di] = ray.geomID == 0 ? 0.0f : 1.0f;

			/*if ( ray.geomID == 0 )
			std::cout << "hit\n";
			else
			std::cout << "missed\n";*/
		}
	}
}

RTCRay RayTracer::testRay( const Vector3f &vertice, const Vector3f &direction ) {
	float inf = std::numeric_limits<float>::infinity();

	RTCRay ray;
	ray.org[0] = vertice.x();
	ray.org[1] = vertice.y();
	ray.org[2] = vertice.z();

	ray.dir[0] = direction.x();
	ray.dir[1] = direction.y();
	ray.dir[2] = direction.z();

	ray.tnear = 0.0f;
	ray.tfar = inf;
	ray.instID = RTC_INVALID_GEOMETRY_ID;
	ray.geomID = RTC_INVALID_GEOMETRY_ID;
	ray.primID = RTC_INVALID_GEOMETRY_ID;
	ray.mask = 0xFFFFFFFF;
	ray.time = 0.0f;

	rtcIntersect( scene, ray );

	return ray;
}

RTCRay RayTracer::testRay( const Vector4f & vertice, const Vector4f & direction ) {
	float inf = std::numeric_limits<float>::infinity();

	RTCRay ray;
	ray.org[0] = vertice.x();
	ray.org[1] = vertice.y();
	ray.org[2] = vertice.z();

	ray.dir[0] = direction.x();
	ray.dir[1] = direction.y();
	ray.dir[2] = direction.z();

	ray.tnear = 0.0f;
	ray.tfar = inf;
	ray.instID = RTC_INVALID_GEOMETRY_ID;
	ray.geomID = RTC_INVALID_GEOMETRY_ID;
	ray.primID = RTC_INVALID_GEOMETRY_ID;
	ray.mask = 0xFFFFFFFF;
	ray.time = 0.0f;

	rtcIntersect( scene, ray );

	return ray;
}

VertexHits RayTracer::testChunckOfRaysPositive( const std::vector<Vector4f>& vertices, const std::vector<Vector4f>& sampleDirections, const ChunkInfo& chunk, float *dotAngles, const std::shared_ptr<CancellationToken> token ) {
	TASK_PROFILE_BLOCK( "Testing rays", chunk.index );

	float inf = std::numeric_limits<float>::infinity();

	RTCRay emptyRay;
	emptyRay.tnear = 0.0f;
	emptyRay.tfar = inf;
	emptyRay.instID = RTC_INVALID_GEOMETRY_ID;
	emptyRay.geomID = RTC_INVALID_GEOMETRY_ID;
	emptyRay.primID = RTC_INVALID_GEOMETRY_ID;
	emptyRay.mask = 0xFFFFFFFF;
	emptyRay.time = 0.0f;

	size_t raySize = sizeof( RTCRay );
	size_t cSize = sizeof( float ) * 3;

	size_t totalColumns = sampleDirections.size();
	const float threshold = 0.00001f;

	//auto vectorSize = sizeof(Vector4f);

	RTCRay ray;
	std::memcpy( &ray, &emptyRay, raySize );

	std::vector<bool> occludedSamples( ( chunk.to - chunk.from ) * totalColumns );

	size_t counter = 0;
	//vertices are on rows and samples on columns
	for ( size_t vertexCounter = chunk.from; vertexCounter < chunk.to; vertexCounter++ ) {
		if ( token->isCancellationRequested() ) break;;

		std::memcpy( &ray.org, &vertices[vertexCounter], cSize );

		for ( size_t sampleCounter = 0; sampleCounter < totalColumns; sampleCounter++ ) {
			if ( *dotAngles <= threshold ) {
				*dotAngles = 0.0f;
				// doesnt matter for the angle < 0
				occludedSamples[counter] = false;
			} else {
				//std::memcpy(&ray, &emptyRay, raySize);
				std::memcpy( &ray.dir, &sampleDirections[sampleCounter], cSize );

				ray.geomID = RTC_INVALID_GEOMETRY_ID;
				ray.instID = RTC_INVALID_GEOMETRY_ID;
				ray.tnear = 0.0f;
				ray.tfar = inf;

				//USE intersect to get full info
				//rtcIntersect( scene, ray ); //=> ray.geomID == 0 if occuluded
				rtcOccluded( scene, ray ); //=> ray.geomID == 0 if occuluded

				//ID 0 if for the entire scene because we dont split vertices in different geometries

				if ( ray.geomID != RTC_INVALID_GEOMETRY_ID ){
					*dotAngles = 0.0f;
					occludedSamples[counter] = true;
				} else
					occludedSamples[counter] = false;
			}

			dotAngles++;
			counter++;
		}
	}

	VertexHits result = std::make_tuple( chunk, occludedSamples );
	return result;
}
