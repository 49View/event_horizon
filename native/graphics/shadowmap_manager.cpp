#include "shadowmap_manager.h"
#include <core/math/quaternion.h>
#include <core/math/aabb.h>

ShadowMapManager::ShadowMapManager() {
//    mBiasMatrix = Matrix4f::MIDENTITY();
    mBiasMatrix = Matrix4f(
            0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f
    );
}

void ShadowMapManager::updateDepthProjectionMatrix() {

	depthProjectionMatrix.setOrthogonalProjection( mXFrustom.x(), mXFrustom.y(), mYFrustom.y(), mYFrustom.x(), mZFrustom.x(), mZFrustom.y() );
    calculateShadowMapMatrices();
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
		calculateShadowMapMatrices();
		invalidate();
	}
}

void ShadowMapManager::calculateShadowMapMatrices() {
	// Compute the MVP matrix from the light's point of view
	if ( mZFrustom.y() != 0.0f ) {
//        depthViewMatrix.lookAt2( V3f{0.0f,  10.7f, 0.0f}, V3f::ZERO, V3f{0.0f, 1.0f, 0.000001f} );
        depthViewMatrix.lookAt2( mShadowMapSunLightDir*mZFrustom.y()*0.5f, mFrustomCenter, V3f{0.0f, 1.0f, 0.000001f} );
        depthMVP = depthViewMatrix * depthProjectionMatrix;
        depthBiasMVP = depthMVP * mBiasMatrix;
	}
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

void ShadowMapManager::setFrusom( const JMATH::AABB& aabb ) {
    float aabbDiameter = aabb.calcDiameter();
    mFrustomCenter = aabb.centre();

    setFrusom( { -aabbDiameter, aabbDiameter}, { -aabbDiameter, aabbDiameter}, { 0.0f, aabbDiameter*4.0f} );
}
