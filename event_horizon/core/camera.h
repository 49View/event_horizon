//
//  camera.h
//  SixthView
//
//  Created by Dado on 11/02/2015.
//  Copyright (c) 2015 JFDP Labs. All rights reserved.
//

#pragma once

#include <ostream>
#include <core/math/matrix4f.h>
#include <core/math/rect2f.h>
#include <core/math/plane3f.h>
#include <core/math/aabb.h>
#include <core/math/quaternion.h>
#include <core/math/math_util.h>
#include <core/math/anim_type.hpp>
#include <core/camera_utils.hpp>
#include <core/name_policy.hpp>

void MultiplyMatrices4by4OpenGL_FLOAT( float *result, const float *matrix1, const float *matrix2 );
void MultiplyMatrixByVector4by4OpenGL_FLOAT( float *resultvector, const float *matrix, const float *pvector );
int glhInvertMatrixf2( float *m, float *out );
int glhUnProjectf( float winx, float winy, float winz, float *modelview, float *projection, int *viewport, float *objectCoordinate );
int glhProjectf( float objx, float objy, float objz, float *modelview, float *projection, int *viewport, float *windowCoordinate );

struct PickRayData;

struct Frustum {
	Plane3f l;
	Plane3f r;
	Plane3f b;
	Plane3f t;
	Plane3f n;
	Plane3f f; // Frustum-planes

	void calculateFromMVP( const Matrix4f& _mvp );
};

enum class CameraCenterAngle {
    Front,
    Back,
    Halfway,
    HalfwayOpposite
};

class Camera : public Animable, public NamePolicy<> {
public:
	Camera( const std::string& cameraName, const Rect2f& _viewport );
    virtual ~Camera() = default;

    void setFoV( float fieldOfView );
	void setPosition( const Vector3f& pos );
	void setQuat( const Quaternion& a );
	void setProjectionMatrix( float fovyInDegrees, float aspectRatio, float znear, float zfar );
	void setProjectionMatrix( const Matrix4f & val );

	void setQuatAngles( const Vector3f&a );
	void incrementQuatAngles( const Vector3f& a );
    void incrementSphericalAngles( const V2f& _sph );
    void incrementOrbitDistance( float _d );

	Quaternion quatAngle() const;
	Vector3f centerScreenOn( const Vector2f& area, const float bMiddleIsCenter = true, const float slack = 0.0f );
	Vector3f centerScreenOnWithinArea( Vector2f area, const Rect2f& targetArea, const float padding = 1.0f, const float slack = 0.0f );

    TimelineSet addKeyFrame( const std::string& name, float _time ) override;

    void zoom2d( float amount );
	void moveUp( float amount );
	void moveForward( float amount );
	void strafe( float amount );
	void setViewMatrix( const Vector3f&pos, const Quaternion& q );
	void setViewMatrixVR( const Vector3f&pos, const Quaternion& q, const Matrix4f& origRotMatrix );
	void lookAt( const Vector3f& _at );
	void pan( const Vector3f& posDiff );
	void center( const AABB& _bbox, CameraCenterAngle cca = CameraCenterAngle::Front );

	void update();

	friend std::ostream& operator<<( std::ostream& os, const Camera& camera );

	bool isDoom() const {
		return Mode() == CameraMode::Doom;
	}

	Matrix4f& MVP( const Matrix4f& model = Matrix4f::IDENTITY, CameraProjectionType cpType =
	CameraProjectionType::Perspective );

	bool frustomClipping( const AABB& bbox ) const;

	Matrix4f getViewMatrix() const {
		return mView;
	}

	Matrix4f getProjectionMatrix() const {
		return mProjection;
	}

	Matrix4f AspectRatio() const { return mAspectRatio; }
	const Matrix4f& ScreenAspectRatio() const { return mScreenAspectRatio; }

	void setViewMatrix( const Matrix4f& val ) {
		mView = val;
	}

	void setNearClipPlaneZClampEdit2d( float _value ) { mNearClipPlaneZClampEdit2d = _value; }
	void setFarClipPlaneZClampEdit2d( float _value ) { mFarClipPlaneZClampEdit2d = _value;}

	Vector3f getPosition() const;
	Vector4f getNearFar() const;
	Vector3f getPositionInv() const;
	Vector3f getPositionRH() const;
	Vector3f getYawVector() const;
	Vector3f getYawVectorInv() const;
	Vector3f getUpVector() const;
	Vector3f getUpVectorInv() const;
	Vector3f getDirection() const;
	Vector3f getDirectionInv() const;
	Vector3f getDirectionRH() const;

	void AspectRatioMultiplier( float val );

	float FoV() const;
	floata& 		FoVAnim();
	V3fa&   		PosAnim();
	Quaterniona& QAngleAnim();

	void getViewporti( int* viewport ) const;

	PickRayData rayViewportPickIntersection( const Vector2f& p1 ) const;
	Vector2f mousePickRayOrtho( const Vector2f& _pos );

	void ModeInc();
	void Mode( const CameraMode& val );
	CameraMode Mode() const;

	bool LockAtWalkingHeight() const { return mLockAtWalkingHeight; }
	void LockAtWalkingHeight( bool val ) { mLockAtWalkingHeight = val; }
	void ToggleLockAtWalkingHeight() { mLockAtWalkingHeight = !mLockAtWalkingHeight; }
    void UpdateIncrementalEulerFromQangle();
    void UpdateIncrementalEulerFromQangle( const Quaternion& _qtarget);
	void enableInputs( bool _enableInputs ) { mbLocked = _enableInputs; }

	JMATH::Rect2f ViewPort() const { return mViewPort; }
	void ViewPort( JMATH::Rect2f val );

private:
    void lookAtCalc();
    void translate( const Vector3f& pos );
    void computeOrbitPosition();

private:
	CameraMode mMode = CameraMode::Edit2d;
	Frustum mFrustom{};

    float mAspectRatioMultiplier = 1.0f;
    float mNearClipPlaneZ = 0.01f;
    float mFarClipPlaneZ = 100.0f;

	float mNearClipPlaneZClampEdit2d = 0.5f;
	float mFarClipPlaneZClampEdit2d = 30.0f;

	bool mLockAtWalkingHeight = false;
	bool mbLocked = false;

	Matrix4f mView              = Matrix4f::MIDENTITY();
	Matrix4f mProjection        = Matrix4f::MIDENTITY();
	Matrix4f mOrthogonal        = Matrix4f::MIDENTITY();
	Matrix4f mAspectRatio       = Matrix4f::MIDENTITY();
	Matrix4f mScreenAspectRatio = Matrix4f::MIDENTITY();
	Matrix4f mVP                = Matrix4f::MIDENTITY();

	Matrix4f mMVP               = Matrix4f::MIDENTITY();
	Matrix4f quatMatrix         = Matrix4f::MIDENTITY();

	V3fa mPos;
	V3fa mTarget;
	Quaterniona qangle; // angles of x,y,z axis to be fed into quaternion math
    floata mFov;

    V3f incrementalEulerQuatAngle = Vector3f::ZERO;
    V2f sphericalAcc = V2f::ZERO;
    V3f mOrbitStrafe = V3f::ZERO;
    float mOrbitDistance = 2.0f;

    JMATH::Rect2f mViewPort{};
};
