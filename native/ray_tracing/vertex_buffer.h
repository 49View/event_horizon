#pragma once
#include <vector>
#include "embree2/rtcore.h"
#include "core/math/matrix4f.h"

class VertexBuffer {
public:

	~VertexBuffer();

	int64_t Id() const { return id; }
	void Id( int64_t val ) { id = val; }

	virtual void initialVerticeValues( const std::vector<Vector4f>& vertices );
	virtual void update( const Matrix4f& mTransform );

	virtual unsigned putInScene( RTCScene scene ) = 0;

	bool IsDirty() const { return mIsDirty; }
	void IsDirty( bool val ) { mIsDirty = val; }
protected:
	int64_t id;
	std::vector<Vector4f> mVertices;
	Vector4f *vs = nullptr;
	bool mIsDirty = true;
	RTCScene mScene;
	unsigned geomID;
	Matrix4f mLastTransform;
};

class QuadVertexBuffer : public VertexBuffer {
public:
	virtual unsigned putInScene( RTCScene scene );
private:
};
