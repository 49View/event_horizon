#include "shadowmap_manager.h"
#include <core/math/quaternion.h>
#include <core/suncalc/sun_builder.h>

ShadowMapManager::ShadowMapManager() {
	mBiasMatrix = Matrix4f(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f
	);

	float bs = 2.50f;

	setFrusom( { -bs, bs}, { -bs, bs}, { 0.0f, bs} );
}

void ShadowMapManager::updateDepthProjectionMatrix() {
	depthProjectionMatrix.setOrthogonalProjection( mXFrustom.x(), mXFrustom.y(), mYFrustom.x(), mYFrustom.y(), mZFrustom.x(), mZFrustom.y() );
	invalidate();
}

void ShadowMapManager::setFrusomX( const Vector2f& val ) {
	mXFrustom = val;
	
	updateDepthProjectionMatrix();
}

void ShadowMapManager::setFrusomY( const Vector2f& val ) {
	mYFrustom = val;
	
	updateDepthProjectionMatrix();
}

void ShadowMapManager::setFrusomZ( const Vector2f& val ) {
	mZFrustom = val;
	
	updateDepthProjectionMatrix();
}

void ShadowMapManager::setFrusom( const Vector2f& xb, const Vector2f& yb, const Vector2f& zb ) {
	mXFrustom = xb;
	mYFrustom = yb;
	mZFrustom = zb;
	
	updateDepthProjectionMatrix();
}

void ShadowMapManager::SunPosition( const Vector3f& sunPos ) {
	if ( mShadowMapLightSourcePos != sunPos ) {
		mShadowMapLightSourcePos = sunPos;
		mShadowMapSunLightDir = normalize( mShadowMapLightSourcePos );
		mShadowMapLightSourcePos = mShadowMapSunLightDir * ( mYFrustom.y() * 0.5f );
		calculateShadowMapMatrices();
		invalidate();
	}
}

void ShadowMapManager::calculateShadowMapMatrices() {
	// Compute the MVP matrix from the light's point of view
	depthViewMatrix.lookAt( mShadowMapLightSourcePos, mShadowMapLightSourcePos + mShadowMapSunLightDir, V3f::Y_AXIS );
//    depthViewMatrix = Matrix4f::IDENTITY;
	depthMVP = depthViewMatrix * depthProjectionMatrix;
	depthBiasMVP = depthMVP * mBiasMatrix;
}

const Matrix4f& ShadowMapManager::ShadowMapMVP() const {
	return depthMVP;
}

Matrix4f & ShadowMapManager::ShadowMapMVPBias( bool _useInfiniteHorizon ) {
	if ( mShadowMapLightSourcePos.y() < 0.0f && _useInfiniteHorizon ) {
		return mBiasMatrix;
	}
	return depthBiasMVP;
}

Vector3f ShadowMapManager::SunDirection() const {
	return mShadowMapSunLightDir;
}
