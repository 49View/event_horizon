#include <utility>

//
//  profile.hpp
//  6thViewImporter
//
//  Created by Dado on 13/10/2015.
//
//

#pragma once
#include <vector>
#include <array>
#include "core/callback_dependency.h"
#include "core/htypes_shared.hpp"
#include "core/math/vector3f.h"

namespace JMATH { class Rect2f; };

class Profile {
public:
	Profile() = default;
	Profile( const std::string& _name, uint8_p&& _data );
	explicit Profile( std::string _name ) : mName( std::move( _name )) {}
	void createWire( float radius, int numSubDivs );
	void createLine( const Vector2f& a, const Vector2f& b, WindingOrderT wo = WindingOrder::CCW );
	void createRect( const Vector2f& size, WindingOrderT wo = WindingOrder::CCW );
	void createRect( const JMATH::Rect2f& _rect );
	void createArc( float startAngle, float angle, float radius, float numSegments = 10 );
	void createArbitrary(const std::vector<Vector2f>& points );
	void raise( const Vector2f& v );

	void mirror( const Vector2f& axis );
	void flip( const Vector2f& axis );
	void move( const Vector2f& pos );
	void centered();
	std::vector<Vector3f> rotatePoints( const Vector3f& nx, const Vector3f& ny,
										const Vector3f& offset = Vector3f::ZERO ) const;

	inline int32_t numVerts() const { return static_cast<int32_t>( mPoints.size() ); }
	inline float width() const { return mBBox.x(); }
	inline float height() const { return mBBox.y(); }

	inline Vector2f pointAt( uint64_t index ) const { return mPoints[index]; }
	inline float lengthAt( uint64_t index ) { return mLengths[index]; }

	std::vector<Vector2f> Points() const { return mPoints; }
	std::vector<Vector3f> Points3d( const Vector3f& mainAxis ) const;
	void Points( const std::vector<Vector2f>& val ) { mPoints = val; }

	std::vector<float> Lengths() const { return mLengths; }
	void Lengths( std::vector<float> val ) { mLengths = val; }

	Vector3f Normal() const { return mNormal; }
	void Normal( Vector3f val ) { mNormal = val; }

	const std::string& Name() const {
		return mName;
	}

	void Name( const std::string& name ) {
		mName = name;
	}

    static std::shared_ptr<Profile> makeLine(const std::string& _name, const std::vector<Vector2f>& vv2fs, const std::vector<float>& vfs);
    static std::shared_ptr<Profile> makeWire(const std::string& _name, const std::vector<Vector2f>& vv2fs, const std::vector<float>& vfs);

private:
	void calculatePerimeter();
	void calcBBox();

private:
	std::string				mName;
	Vector2f				mBBox = Vector2f::ZERO;
	std::vector<Vector2f>	mPoints;
	std::vector<float>		mLengths;
	float					mPerimeter = 0.0f;
	Vector3f				mNormal = Vector3f::X_AXIS;
};
