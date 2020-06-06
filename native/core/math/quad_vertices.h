#pragma once

#include "vector2f.h"
#include "vector4f.h"
#include "matrix2f.h"
#include "matrix4f.h"
#include "rect2f.h"
#include "aabb.h"

//====================================
// QuadVertices2
//====================================

class QuadVertices2 {
public:
	QuadVertices2() {
		mVertices[0] = Vector2f( 0.0f, 0.0f );
		mVertices[1] = Vector2f( 1.0f, 0.0f );
		mVertices[2] = Vector2f( 0.0f, 1.0f );
		mVertices[3] = Vector2f( 1.0f, 1.0f );
	}

	inline void init( const Vector2f& v0, const Vector2f& v1, const Vector2f& v2, const Vector2f& v3 ) {
		mVertices[0] = v0;
		mVertices[1] = v1;
		mVertices[2] = v2;
		mVertices[3] = v3;
	}

	inline void init( const Vector2f& botomLeft, const Vector2f& topRight ) {
		mVertices[0] = botomLeft;
		mVertices[1] = Vector2f( botomLeft.x(), topRight.y() );
		mVertices[2] = topRight;
		mVertices[3] = Vector2f( topRight.x(), botomLeft.y() );
	}

	QuadVertices2( const Vector2f& v0, const Vector2f& v1, const Vector2f& v2, const Vector2f& v3 ) {
		init( v0, v1, v2, v3 );
	}

	Vector2f operator[]( int i ) const {
		return mVertices[i];
	}

	void setVertex( int i, const Vector2f& v ) {
		mVertices[i] = v;
	}

	const Vector2f& getVertex( int i ) const {
		return mVertices[i];
	}

	void scale( const Vector2f& factor ) {
		for ( int i = 0; i < 4; ++i ) {
			mVertices[i] = mVertices[i] * factor;
		}
	}

	void rotate( const float angle ) {
		Matrix2f mat;

		mat.setRotation( angle );
		for ( int i = 0; i < 4; ++i ) {
			mVertices[i] = mat.transform( mVertices[i] );
		}
	}

	void add( const Vector2f& amount ) {
		for ( int i = 0; i < 4; ++i ) {
			mVertices[i] = mVertices[i] + amount;
		}
	}

	void mirrorTexCoordsX() {
		for ( int i = 0; i < 4; ++i ) {
			mVertices[i] = Vector2f( 1.0f - mVertices[i].x(), mVertices[i].y() );
		}
	}

	void mirrorTexCoordsY() {
		for ( int i = 0; i < 4; ++i ) {
			mVertices[i] = Vector2f( mVertices[i].x(), 1.0f - mVertices[i].y() );
		}
	}

	const float* ptr() const {
		return (float*)mVertices;
	}

	const Vector2f* verts() const {
		return mVertices;
	}

	void rotateTexCoords( int angle ) {
		if ( angle == 90 ) {
			for ( int i = 0; i < 4; ++i ) {
				mVertices[i] = Vector2f( mVertices[i].y(), 1.0f - mVertices[i].x() );
			}
		} else if ( angle == 180 ) {
			for ( int i = 0; i < 4; ++i ) {
				mVertices[i] = Vector2f( 1.0f - mVertices[i].x(), 1.0f - mVertices[i].y() );
			}
		} else if ( angle == 270 ) {
			for ( int i = 0; i < 4; ++i ) {
				mVertices[i] = Vector2f( 1.0f - mVertices[i].y(), mVertices[i].x() );
			}
		}
	}

	void expandToQuadTexCoords() {
		for ( int i = 0; i < 4; ++i ) {
			mVertices[i].setX( mVertices[i].x() > 0.5f ? 1.0f : 0.0f );
			mVertices[i].setY( mVertices[i].y() > 0.5f ? 1.0f : 0.0f );
		}
	}

	bool isInverted() const {
		return mVertices[2].y() < mVertices[0].y();
	}

	void print() const {
		LOGI( "QuadVertices2(%f, %f; %f, %f; %f, %f; %f, %f)", mVertices[0].x(), mVertices[0].y(), mVertices[1].x(), mVertices[1].y(), mVertices[2].x(), mVertices[2].y(),
			  mVertices[3].x(), mVertices[3].y() );
	}

	float getMaxX() const {
		float max = std::numeric_limits<float>::lowest();
		for ( int i = 0; i < 4; ++i ) {
			float x = getVertex( i ).x();
			if ( x > max ) {
				max = x;
			}
		}
		return max;
	}

	float getMaxY() const {
		float max = std::numeric_limits<float>::lowest();
		for ( int i = 0; i < 4; ++i ) {
			float y = getVertex( i ).y();
			if ( y > max ) {
				max = y;
			}
		}
		return max;
	}

	static const QuadVertices2 QUAD_TEX_COORDS;
	static const QuadVertices2 QUAD_INV_TEX_COORDS;
	static const QuadVertices2 QUAD_TEX_COORDS_MIRROR;
	static const QuadVertices2 QUAD_INV_TEX_COORDS_MIRROR;

	static const QuadVertices2 QUAD_TEX_COORDS_CENTERED;
	static const QuadVertices2 QUAD_TEX_STRIP_COORDS;
	static const QuadVertices2 QUAD_TEX_STRIP_INV_Y_COORDS;
	static const QuadVertices2 QUAD_TEX_STRIP_INV_COORDS;

private:
	Vector2f mVertices[4];
};

//====================================
// QuadVertices4
//====================================

class QuadVertices3 {
public:
	QuadVertices3() {
	}

	QuadVertices3( const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3 ) {
		mVertices[0] = v0;
		mVertices[1] = v1;
		mVertices[2] = v2;
		mVertices[3] = v3;
	}

	QuadVertices3( const Rect2f& r, float z ) {
		mVertices[3] = { r.topLeft(), z };
		mVertices[1] = { r.topRight(), z };
		mVertices[2] = { r.bottomLeft(), z };
		mVertices[0] = { r.bottomRight(), z };
	}

	QuadVertices3( const Rect2f& r, const AABB& projection );

	QuadVertices3( const Vector3f& _scale ) {
		switch ( _scale.leastDominantElement() ) {
		case 0:
		*this = QUAD_VERTICES_X;
		break;
		case 1:
		*this = QUAD_VERTICES_Y;
		break;
		case 2:
		*this = QUAD_VERTICES_Z;
		break;

		default:
		break;
		}

		scale( _scale );
	}

	QuadVertices3( const Vector3f& _offset, const Vector3f& _scale ) {
		switch ( _scale.leastDominantElement() ) {
		case 0:
		*this = QUAD_VERTICES_X;
		break;
		case 1:
		*this = QUAD_VERTICES_Y;
		break;
		case 2:
		*this = QUAD_VERTICES_Z;
		break;

		default:
		break;
		}

		scale( _scale );
		translate( _offset );
	}

	QuadVertices3( const Vector2f& v0, const Vector2f& v1, const Vector2f& v2, const Vector2f& v3 ) {
		mVertices[0] = Vector3f( v0, 0.0f );
		mVertices[1] = Vector3f( v1, 0.0f );
		mVertices[2] = Vector3f( v2, 0.0f );
		mVertices[3] = Vector3f( v3, 0.0f );
	}

	void setVertex( int i, const Vector3f& v ) {
		mVertices[i] = v;
	}

	const Vector3f& getVertex( int i ) const {
		return mVertices[i];
	}

	Vector3f operator[]( int i ) const {
		return mVertices[i];
	}

	Vector2f getVertex2( int i ) const {
		return Vector2f( mVertices[i].x(), mVertices[i].y() );
	}

	const float* ptr() const {
		return (float*)mVertices;
	}

	const Vector3f* verts() const {
		return mVertices;
	}

	void transform( const Matrix4f& mat ) {
		for ( int i = 0; i < 4; ++i ) {
			mVertices[i] = ( mat * Vector4f( mVertices[i], 1.0f ) ).xyz();
		}
	}

	void mirrorTexCoordsY() {
		for ( int i = 0; i < 4; ++i ) {
			mVertices[i] = Vector3f( mVertices[i].x(), 1.0f - mVertices[i].y(), mVertices[i].z() );
		}
	}

	void invertY() {
		for ( int i = 0; i < 4; ++i ) {
			mVertices[i] = Vector3f( mVertices[i].x(), -mVertices[i].y(), mVertices[i].z() );
		}
	}

	void scale( float factor ) {
		for ( int i = 0; i < 4; ++i ) {
			mVertices[i] = Vector3f( mVertices[i].x() * factor, mVertices[i].y() * factor, mVertices[i].z() * factor );
		}
	}

	void scale( const Vector3f& factor ) {
		for ( int i = 0; i < 4; ++i ) {
			mVertices[i] *= factor;
		}
	}

	void translate( const Vector3f& factor ) {
		for ( int i = 0; i < 4; ++i ) {
			mVertices[i] += factor;
		}
	}

	static const QuadVertices3 QUAD_VERTICES_X;
	static const QuadVertices3 QUAD_VERTICES_Y;
	static const QuadVertices3 QUAD_VERTICES_Z;

private:
	Vector3f mVertices[4];
};

//====================================
// QuadVertices4
//====================================

class QuadVertices4 {
public:
	QuadVertices4() {
	}

	QuadVertices4( const Vector4f& v0, const Vector4f& v1, const Vector4f& v2, const Vector4f& v3 ) {
		mVertices[0] = v0;
		mVertices[1] = v1;
		mVertices[2] = v2;
		mVertices[3] = v3;
	}

	QuadVertices4( const Vector2f& v0, const Vector2f& v1, const Vector2f& v2, const Vector2f& v3 ) {
		mVertices[0] = Vector4f( v0, 0.0f, 1.0f );
		mVertices[1] = Vector4f( v1, 0.0f, 1.0f );
		mVertices[2] = Vector4f( v2, 0.0f, 1.0f );
		mVertices[3] = Vector4f( v3, 0.0f, 1.0f );
	}

	void setVertex( int i, const Vector4f& v ) {
		mVertices[i] = v;
	}

	const Vector4f& getVertex( int i ) const {
		return mVertices[i];
	}

	Vector4f operator[]( int i ) const {
		return mVertices[i];
	}

	Vector2f getVertex2( int i ) const {
		return Vector2f( mVertices[i].x(), mVertices[i].y() );
	}

	const float* ptr() const {
		return (float*)mVertices;
	}

	const Vector4f* verts() const {
		return mVertices;
	}

	void transform( const Matrix4f& mat ) {
		for ( int i = 0; i < 4; ++i ) {
			mVertices[i] = mat * mVertices[i];
		}
	}

	void mirrorTexCoordsY() {
		for ( int i = 0; i < 4; ++i ) {
			mVertices[i] = Vector4f( mVertices[i].x(), 1.0f - mVertices[i].y(), mVertices[i].y(), mVertices[i].z() );
		}
	}

	void invertY() {
		for ( int i = 0; i < 4; ++i ) {
			mVertices[i] = Vector4f( mVertices[i].x(), -mVertices[i].y(), mVertices[i].y(), mVertices[i].z() );
		}
	}

	void scale( float factor ) {
		for ( int i = 0; i < 4; ++i ) {
			mVertices[i] = Vector4f( mVertices[i].x() * factor, mVertices[i].y() * factor, 0.0f, 1.0f );
		}
	}

	static const QuadVertices4 QUAD_VERTICES;

private:
	Vector4f mVertices[4];
};

class QuadStripUV {
public:
	QuadStripUV() {
		mvs = QuadVertices3::QUAD_VERTICES_Z;
		mvts = QuadVertices2::QUAD_TEX_STRIP_INV_COORDS;
	}
	QuadStripUV( const QuadVertices3& _v, const QuadVertices2& _vt = QuadVertices2::QUAD_TEX_STRIP_INV_COORDS ) {
		mvs = _v;
		mvts = _vt;
	}
	QuadStripUV( const Rect2f& r, float z, const QuadVertices2& _vt = QuadVertices2::QUAD_TEX_STRIP_COORDS ) {
		mvs = QuadVertices3( r, z );
		mvts = _vt;
	}

	const QuadVertices3& vs() const { return mvs; }
	const QuadVertices2& vts() const { return mvts; }
private:
	QuadVertices3 mvs;
	QuadVertices2 mvts;
};
