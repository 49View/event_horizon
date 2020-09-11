#include "vertex_buffer.h"
#include <cstring>
#include "ray_tracing/shapes.h"

VertexBuffer::~VertexBuffer() {
}

void VertexBuffer::initialVerticeValues( const std::vector<Vector4f>& vertices ) {
	mVertices = vertices;
}

void VertexBuffer::update( const Matrix4f& transform ) {
	if ( !vs ) return;

	if ( transform.isSimilarTo( mLastTransform ) ) return;

	mLastTransform = transform;

	for ( size_t i = 0; i < mVertices.size(); i++ ) {
		vs[i] = transform.transform( mVertices[i] );
	}

	rtcUpdate( mScene, geomID );
	IsDirty( true );
}

unsigned QuadVertexBuffer::putInScene( RTCScene scene ) {
	mScene = scene;
	mLastTransform = Matrix4f::IDENTITY();
	RTCGeometryFlags geomFlags = RTC_GEOMETRY_DEFORMABLE;

	geomID = rtcNewQuadMesh( scene, geomFlags, 1, mVertices.size() );

	vs = (Vector4f*)rtcMapBuffer( scene, geomID, RTC_VERTEX_BUFFER );

	std::memcpy( vs, mVertices.data(), sizeof( Vector4f ) * mVertices.size() );

	rtcUnmapBuffer( scene, geomID, RTC_VERTEX_BUFFER );

	Quad* ts = (Quad *)rtcMapBuffer( scene, geomID, RTC_INDEX_BUFFER );

	ts[0].v0 = 0;
	ts[0].v1 = 1;
	ts[0].v2 = 2;
	ts[0].v3 = 3;

	rtcUnmapBuffer( scene, geomID, RTC_INDEX_BUFFER );

	IsDirty( true );

	return geomID;
}