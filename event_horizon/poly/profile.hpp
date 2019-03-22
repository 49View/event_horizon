//
//  profile.hpp
//  6thViewImporter
//
//  Created by Dado on 13/10/2015.
//
//

#pragma once
#include <utility>
#include <vector>
#include <array>
#include <core/htypes_shared.hpp>
#include <core/math/vector3f.h>
#include <poly/resources/publisher.hpp>

namespace JMATH { class Rect2f; }

class Profile {
public:
	Profile() = default;
	explicit Profile( uint8_p&& _data );
    explicit Profile( const SerializableContainer& _data );

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

    static std::shared_ptr<Profile> makeLine(const std::string& _name, const std::vector<Vector2f>& vv2fs, const std::vector<float>& vfs);
    static std::shared_ptr<Profile> makeWire(const std::string& _name, const std::vector<Vector2f>& vv2fs, const std::vector<float>& vfs);

    static std::shared_ptr<Profile> fromPoints( const std::string& name, const std::vector<Vector2f>& points );

private:
    void readSVGfromMemory( const char* _buffer, size_t _length );
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

class ProfileMaker {
public:
	ProfileMaker() = default;
	explicit ProfileMaker( std::string name ) : name( std::move( name )) {}

	ProfileMaker& o();
	ProfileMaker& s( float _s );
	ProfileMaker& sd( uint32_t _sd );
	ProfileMaker& ay( float radius, int32_t subdivs = -1 );
	ProfileMaker& l( const V2f& _p1 );
	ProfileMaker& ly( float _y1 );
	ProfileMaker& lx( float _x1 );

	std::shared_ptr<Profile> make() const { return Profile::fromPoints( name, points ); }

private:
	void add( const V2f& _p );
	int setPointerSubdivs( int _sd ) const;
	V2f pointer() const { return points.empty() ? V2f::ZERO : points.back(); }

private:
	std::string name;
	std::vector<V2f> points;
	int32_t gsubdivs = 4;
	float scale = 1.0f;
};
