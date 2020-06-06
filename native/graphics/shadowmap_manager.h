#pragma once

#include <core/math/matrix4f.h>
#include <core/math/rect2f.h>
#include <core/frame_invalidator.hpp>

namespace JMATH { class AABB; }

class ShadowMapManager : public FrameInvalidator {
public:
	ShadowMapManager();
	void calculateShadowMapMatrices();
	const Matrix4f& ShadowMapMVP() const;
	Matrix4f& ShadowMapMVPBias( bool _useInfiniteHorizon = false );

	Vector3f SunDirection() const;

	void SunPosition( const Vector3f& sunPos );
	Vector3f SunPosition() const { return mShadowMapLightSourcePos; }
	void setFrusom( const Vector2f& xb, const Vector2f& yb, const Vector2f& zb  );
	void setFrusom( const JMATH::AABB& aabb );
	void setFrusomX( const Vector2f& val );
	void setFrusomY( const Vector2f& val );
	void setFrusomZ( const Vector2f& val );

private:
	void updateDepthProjectionMatrix();
	
private:
	Matrix4f mBiasMatrix;
	Matrix4f depthMVP;
	Matrix4f depthBiasMVP;
	Matrix4f depthProjectionMatrix;
	Matrix4f depthViewMatrix;
	Vector3f mShadowMapLightSourcePos;
	Vector3f mShadowMapSunLightDir = Vector3f::Z_AXIS_NEG;
	Vector2f mXFrustom = V2fc::ZERO;
	Vector2f mYFrustom = V2fc::ZERO;
	Vector2f mZFrustom = V2fc::ZERO;
	Vector3f mFrustomCenter = V3f::ZERO;
};

//#define SMM ShadowMapManager::getInstance()
