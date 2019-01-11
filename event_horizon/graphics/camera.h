//
//  camera.h
//  SixthView
//
//  Created by Dado on 11/02/2015.
//  Copyright (c) 2015 JFDP Labs. All rights reserved.
//

#pragma once

#include <ostream>
#include "core/math/matrix4f.h"
#include "core/math/rect2f.h"
#include "core/math/plane3f.h"
#include "core/math/aabb.h"
#include "core/math/quaternion.h"
#include <core/math/math_util.h>
#include <core/math/anim_type.hpp>
#include "graphic_constants.h"

void MultiplyMatrices4by4OpenGL_FLOAT( float *result, float *matrix1, float *matrix2 );
void MultiplyMatrixByVector4by4OpenGL_FLOAT( float *resultvector, const float *matrix, const float *pvector );
int glhInvertMatrixf2( float *m, float *out );
int glhUnProjectf( float winx, float winy, float winz, float *modelview, float *projection, int *viewport, float *objectCoordinate );
int glhProjectf( float objx, float objy, float objz, float *modelview, float *projection, int *viewport, float *windowCoordinate );

enum class CameraControls {
	Edit2d,
	Plan2d,
	Fly,
	Walk
};

struct CameraInputData {
	ViewportTogglesT cvt = ViewportToggles::None;
	Vector2f mousePos = Vector2f::ZERO;
	bool isMouseTouchedDown = false;
	float scrollValue = 0.0f;
	Vector2f moveDiff = Vector2f::ZERO;
	Vector2f moveDiffSS = Vector2f::ZERO;

	float moveForward = 0.0f;
	float strafe = 0.0f;
	float moveUp = 0.0f;
};

struct Frustum {
	Plane3f l;
	Plane3f r;
	Plane3f b;
	Plane3f t;
	Plane3f n;
	Plane3f f; // Frustum-planes

	void calculateFromMVP( const Matrix4f& _mvp );
};

class Camera : public Animable {
public:
	Camera( const std::string& cameraName, CameraState _state, const Rect2f& _viewport );
    virtual ~Camera() = default;

    void setFoV( float fieldOfView );
	void setPosition( const Vector3f& pos );
	void setQuat( const Quaternion& a );
	void setProjectionMatrix( float fovyInDegrees, float aspectRatio, float znear, float zfar );
	void setProjectionMatrix( const Matrix4f & val );

	void setQuatAngles( const Vector3f&a );
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
	void lookAt( const Vector3f& pos );
	void lookAtAngles( const Vector3f& angleAt, const float _time, const float _delay = 0.0f );
	void lookAtRH( const Vector3f& eye, const Vector3f& at, const Vector3f& up );
	void pan( const Vector3f& posDiff );
	void center( const AABB& _bbox );

	void updateFromInputData( const CameraInputData& mi );
	void update();
	friend std::ostream& operator<<( std::ostream& os, const Camera& camera );

	bool isDoom() const {
		return Mode() == CameraMode::Doom;
	}

	ViewportTogglesT getCvt() const {
		return mCvt;
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

	Vector3f getPositionInv() const;

	Vector3f getPositionRH() const;

	Vector3f getYawVector() const {
		return mView.getCol( 0 ).xyz();
	}

	Vector3f getYawVectorInv() const {
		return -mView.getCol( 0 ).xyz();
	}

	Vector3f getUpVector() const {
		return mView.getCol( 1 ).xyz();
	}

	Vector3f getUpVectorInv() const {
		return -mView.getCol( 1 ).xyz();
	}

	Vector3f getDirection() const {
		return mView.getCol( 2 ).xyz();
	}

	Vector3f getDirectionInv() const {
		return -mView.getCol( 2 ).xyz();
	}

	Vector3f getDirectionRH() const {
		Vector3f ret = mView.getCol( 2 ).xzy();
		ret.setX( -ret.x() );
		return ret;
	}

	void AspectRatioMultiplier( float val ) {
		mAspectRatioMultiplier = val;
	}

	float FoV() const;

	floata 		FoVAnim();
	V3fa   		PosAnim();
	Quaterniona QAngleAnim();

	void getViewporti( int* viewport ) const;

	void mousePickRay( const Vector2f& p1, Vector3f& rayNear, Vector3f& rayFar );
	Vector2f mousePickRayOrtho( const Vector2f& _pos );

	void ModeInc();
	void Mode( const CameraMode& val ) {
		mMode = val;
	}
	CameraMode Mode() const {
		return mMode;
	}

	std::string Name() const { return mName; }
	void Name( std::string val ) { mName = val; }

	bool LockAtWalkingHeight() const { return mLockAtWalkingHeight; }
	void LockAtWalkingHeight( bool val ) { mLockAtWalkingHeight = val; }
	void ToggleLockAtWalkingHeight() { mLockAtWalkingHeight = !mLockAtWalkingHeight; }

	void enableInputs( bool _enableInputs ) { mbEnableInputs = _enableInputs; }

	CameraState Status() const { return mStatus; }
	void Status( CameraState val ) { mStatus = val; }

	JMATH::Rect2f ViewPort() const { return mViewPort; }
	void ViewPort( JMATH::Rect2f val );

private:
	void translate( const Vector3f& pos );

	ViewportTogglesT mCvt = ViewportToggles::None;
	std::string mName;
	CameraMode mMode;
	CameraState mStatus;
	Frustum mFrustom;
    Vector3f mRayNear = Vector3f::ZERO;
    Vector3f mRayFar = Vector3f::ZERO;

	float mHAngle;
	float mVAngle;
	float mAspectRatioMultiplier;

	float mNearClipPlaneZ;
	float mFarClipPlaneZ;
	float mNearClipPlaneZClampEdit2d = 0.5f;
	float mFarClipPlaneZClampEdit2d = 30.0f;

	bool mLockAtWalkingHeight;
	bool mbEnableInputs;

	Matrix4f mView;
	Matrix4f mProjection;
	Matrix4f mOrthogonal;
	Matrix4f mAspectRatio;
	Matrix4f mScreenAspectRatio;
	Matrix4f mVP;

	Matrix4f mMVP;
	Matrix4f quatMatrix;

	V3fa mPos;
	Quaterniona qangle; // angles of x,y,z axis to be fed into quaternion math
    floata mFov;

	Vector3f qangleEuler = Vector3f::ZERO;

	JMATH::Rect2f mViewPort;

	bool mbInitialized;
	static const float UI_ZOOM_SCALER;
};
