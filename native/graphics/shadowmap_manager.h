#pragma once

#include <core/math/matrix4f.h>
#include <core/math/rect2f.h>
#include <core/frame_invalidator.hpp>

namespace JMATH { class AABB; }

class ShadowMapManager : public FrameInvalidator {
public:
	ShadowMapManager();
	void calculateShadowMapMatrices();
	[[nodiscard]] const Matrix4f& ShadowMapMVP() const;
	Matrix4f& ShadowMapMVPBias( bool _useInfiniteHorizon = false );

	[[nodiscard]] Vector3f SunDirection() const;

	void SunPosition( const Vector3f& sunPos, float _dayDelta, float _artificialWorldRotationAngle );
	[[nodiscard]] Vector3f SunPosition() const { return mShadowMapLightSourcePos; }
	void setFrustum( const Vector2f& xb, const Vector2f& yb, const Vector2f& zb );
	void setFrustum( const JMATH::AABB& aabb );
    [[maybe_unused]] void setFrustumX( const Vector2f& val );
    [[maybe_unused]] void setFrustumY( const Vector2f& val );
    [[maybe_unused]] void setFrustumZ( const Vector2f& val );
    [[maybe_unused, nodiscard]] V2f getFrustumX() const { return mXFrustum; };
    [[maybe_unused, nodiscard]] V2f getFrustumY() const { return mYFrustum; };
    [[maybe_unused, nodiscard]] V2f getFrustumZ() const { return mZFrustum; };

private:
	void updateDepthProjectionMatrix();
	
private:
    float optimisedSunAngle = 0.5f;
	Matrix4f mBiasMatrix{Matrix4f::MIDENTITY()};
	Matrix4f depthMVP{Matrix4f::MIDENTITY()};
	Matrix4f depthBiasMVP{Matrix4f::MIDENTITY()};
	Matrix4f depthProjectionMatrix{Matrix4f::MIDENTITY()};
	Matrix4f depthViewMatrix{Matrix4f::MIDENTITY()};
	Vector3f mShadowMapLightSourcePos{V3fc::ZERO};
	Vector3f mShadowMapSunLightDir = V3fc::Z_AXIS_NEG;
	Vector2f mXFrustum = V2fc::ZERO;
	Vector2f mYFrustum = V2fc::ZERO;
	Vector2f mZFrustum = V2fc::ZERO;
	Vector3f mFrustumCenter = V3fc::ZERO;
};

//#define SMM ShadowMapManager::getInstance()
