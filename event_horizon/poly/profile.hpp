//
//  profile.hpp
//  6thViewImporter
//
//  Created by Dado on 13/10/2015.
//
//

#pragma once
#include <vector>
#include "core/callback_dependency.h"
#include "core/htypes_shared.hpp"
#include "core/math/vector3f.h"

namespace JMATH { class Rect2f; };

class Profile {
public:
	Profile() {}
	Profile( const std::string& _name, uint8_p&& _data );
	void createWire( const float radius, int numSubDivs );
	void createLine( const Vector2f& a, const Vector2f& b, PivotPointPosition ppp = PivotPointPosition::PPP_CENTER, const Vector2f& customPivotPoint = Vector2f::ZERO, WindingOrderT wo = WindingOrder::CCW );
	void createRect( const Vector2f& size, PivotPointPosition ppp = PivotPointPosition::PPP_CENTER, const Vector2f& customPivotPoint = Vector2f::ZERO, WindingOrderT wo = WindingOrder::CCW );
	void createRect( const JMATH::Rect2f& _rect, PivotPointPosition ppp = PivotPointPosition::PPP_CENTER, const Vector2f& customPivotPoint = Vector2f::ZERO, WindingOrderT wo = WindingOrder::CCW );
	void createArc( float startAngle, float angle, float radius, float numSegments = 10, PivotPointPosition ppp = PivotPointPosition::PPP_CENTER, const Vector2f& customPivotPoint = Vector2f::ZERO, WindingOrderT wo = WindingOrder::CCW );
	void createArbitrary( const Vector2f& size, const std::vector<Vector2f>& points, PivotPointPosition ppp = PivotPointPosition::PPP_CENTER, const Vector2f& customPivotPoint = Vector2f::ZERO );
	void raise( const Vector2f& v );

	void mirror( const Vector2f& axis );
	void flip( const Vector2f& axis );
	void move( const Vector2f& pos );
	void centered();
	void invertCCW();
	void forceWindingOrder( WindingOrderT wo ) { mbForceWindingOrder = true; mForcedWindingOrder = wo; }
	bool hasForcedWindingOrder() { return mbForceWindingOrder; }
	WindingOrderT windingOrder() { return mForcedWindingOrder; }
	std::vector<Vector3f> rotatePoints( const Vector3f& nx, const Vector3f& ny,
										const Vector3f& offset = Vector3f::ZERO );

	inline int32_t numVerts() const { return static_cast<int32_t>( mPoints.size() ); }
	inline float width() const { return mBBox.x(); }
	inline float height() const { return mBBox.y(); }

	inline Vector3f PivotPoint() const { return Vector3f( mPivotPoint.x(), 0.0f, mPivotPoint.y() ); }
	inline Vector2f PivotPoint2d() const { return mPivotPoint; }

	inline Vector2f pointAt( uint64_t index ) const { return mPoints[index]; }
	inline float lengthAt( uint64_t index ) { return mLengths[index]; }

	std::vector<Vector2f> Points() const { return mPoints; }
	std::vector<Vector3f> Points3d( const Vector3f& mainAxis ) const;
	void Points( const std::vector<Vector2f>& val ) { mPoints = val; }

	std::vector<float> Lengths() const { return mLengths; }
	void Lengths( std::vector<float> val ) { mLengths = val; }

	Vector3f Normal() const { return mNormal; }
	void Normal( Vector3f val ) { mNormal = val; }

	PivotPointPosition PPP() const {
		return mPPP;
	};

	const std::string& Name() const {
		return mName;
	}

	void Name( const std::string& name ) {
		mName = name;
	}

private:
	void calculatePerimeter();
	void calculatePPP();

private:
	std::string				mName;
	Vector2f				mBBox = Vector2f::ZERO;
	std::vector<Vector2f>	mPoints;
	std::vector<float>		mLengths;
	Vector2f				mPivotPoint = Vector2f::ZERO;
	float					mPerimeter = 0.0f;
	Vector3f				mNormal = Vector3f::X_AXIS;
	bool					mbForceWindingOrder = false;
	WindingOrderT			mForcedWindingOrder = WindingOrder::CW;
	PivotPointPosition		mPPP = PivotPointPosition::PPP_CENTER;
	Vector2f				mCustomPivotPoint = Vector2f::ZERO;
};
