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

    depthProjectionMatrix.setOrthogonalProjection(mXFrustum.x(), mXFrustum.y(), mYFrustum.y(), mYFrustum.x(),
                                                  mZFrustum.x(), mZFrustum.y());
    calculateShadowMapMatrices();
    invalidate();
}

[[maybe_unused]] void ShadowMapManager::setFrustumX( const Vector2f& val ) {
    mXFrustum = val;
    updateDepthProjectionMatrix();
}

[[maybe_unused]] void ShadowMapManager::setFrustumY( const Vector2f& val ) {
    mYFrustum = val;
    updateDepthProjectionMatrix();
}

[[maybe_unused]] void ShadowMapManager::setFrustumZ( const Vector2f& val ) {
    mZFrustum = val;
    updateDepthProjectionMatrix();
}

void ShadowMapManager::setFrustum( const Vector2f& xb, const Vector2f& yb, const Vector2f& zb ) {
    mXFrustum = xb;
    mYFrustum = yb;
    mZFrustum = zb;
    mFrustumCenter = V3f::ZERO();
    updateDepthProjectionMatrix();
}

void ShadowMapManager::SunPosition( const Vector3f& sunPos, float _dayDelta, float _artificialWorldRotationAngle ) {
    if ( mShadowMapLightSourcePos != sunPos ) {
        optimisedSunAngle = _dayDelta;
        mShadowMapLightSourcePos = sunPos;
        mShadowMapSunLightDir = normalize(mShadowMapLightSourcePos);
        Matrix4f mat{};
        mat.setRotation(_artificialWorldRotationAngle, V3fc::UP_AXIS);
        mShadowMapSunLightDir = mat.transform(mShadowMapSunLightDir);
        calculateShadowMapMatrices();
        invalidate();
    }
}

void ShadowMapManager::calculateShadowMapMatrices() {
    // Compute the MVP matrix from the light's point of view
    if ( mZFrustum.y() != 0.0f ) {
        depthViewMatrix.lookAt2(mShadowMapSunLightDir * half( mZFrustum.y()), mShadowMapSunLightDir, V3f{ 0.0f, 1.0f, 0.000001f });
        depthMVP = depthViewMatrix * depthProjectionMatrix;
        depthBiasMVP = depthMVP * mBiasMatrix;
    }
}

const Matrix4f& ShadowMapManager::ShadowMapMVP() const {
    return depthMVP;
}

Matrix4f& ShadowMapManager::ShadowMapMVPBias( bool _useInfiniteHorizon ) {
    if ( mShadowMapLightSourcePos.y() < 0.0f && _useInfiniteHorizon ) {
        return mBiasMatrix;
    }
    return depthBiasMVP;
}

Vector3f ShadowMapManager::SunDirection() const {
    return mShadowMapSunLightDir;
}

void ShadowMapManager::setFrustum( const JMATH::AABB& aabb ) {
    float aabbDiameter = aabb.calcDiameter()*1.75f;
    setFrustum({ -aabbDiameter, aabbDiameter }, { -half(aabb.calcDiameter()), aabbDiameter }, { 0.0f, aabbDiameter*2.0f });
}
