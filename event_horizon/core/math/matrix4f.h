#pragma once

#include <utility>
#include <cstring>

#include "math_util.h"
#include "vector2f.h"
#include "vector3f.h"
#include "vector4f.h"
#include "matrix3f.h"

// SHARED CLASS: Any changes to this, should be integrated to /svn/shared, and a mail should be send to everyone

class MatrixAnim;

enum class MatrixRotationOrder {
	xyz,
	xzy,
	yxz,
	yzx,
	zxy,
	zyx
};

class Quaternion;

class Matrix4f {
public:

	static const Matrix4f IDENTITY;
	static const Matrix4f IDENTITY_YINV;
	static const Matrix4f BEEF;
    static const Matrix4f ZERO;
    static const Matrix4f ONE;

	Matrix4f() {
	}

	Matrix4f( const Vector3f& pos, float angle, const Vector3f& axis ) {
		setRotation( angle, axis );
		translate( pos );
	}

	Matrix4f( const Vector3f& pos, float angle, const Vector3f& axis, const Vector3f& zoom ) {
		Matrix4f rotM = Matrix4f::IDENTITY;
		Matrix4f scaleM = Matrix4f::IDENTITY;
		Matrix4f posM = Matrix4f::IDENTITY;
		rotM.setRotation( angle, axis );
		scaleM.scale( zoom );
		posM.translate( pos );
		Matrix4f conc = rotM * scaleM * posM;
		for ( int t = 0; t < 4; t++ ) {
			setRow( t, conc.getRow( t ) );
		}
	}

	Matrix4f( const Vector3f& pos, const Vector3f& axis, const Vector3f& zoom, MatrixRotationOrder mro = MatrixRotationOrder::xyz ) {
		setFromRTS( pos, axis, zoom, mro );
	}

	Matrix4f( const MatrixAnim& rts );
    Matrix4f( const Vector3f& pos, const Quaternion& axis, const Vector3f& zoom );
	// This just sets the translation
	Matrix4f( const Vector3f& row3 ) {
		mRows[0] = Matrix4f::IDENTITY.getRow( 0 );
		mRows[1] = Matrix4f::IDENTITY.getRow( 1 );
		mRows[2] = Matrix4f::IDENTITY.getRow( 2 );
		mRows[3] = Vector4f( row3 );
	}

	Matrix4f( const Vector4f& row3 ) {
		mRows[0] = Matrix4f::IDENTITY.getRow( 0 );
		mRows[1] = Matrix4f::IDENTITY.getRow( 1 );
		mRows[2] = Matrix4f::IDENTITY.getRow( 2 );
		mRows[3] = row3;
	}

	// In case of quaternion, set rotation
	Matrix4f( const Quaternion& rot );

	Matrix4f( const Vector4f& row0, const Vector4f& row1, const Vector4f& row2, const Vector4f& row3 ) {
		mRows[0] = row0;
		mRows[1] = row1;
		mRows[2] = row2;
		mRows[3] = row3;
	}

	Matrix4f( float v[4][4] ) {
		setRow( 0, Vector4f( v[0][0], v[1][0], v[2][0], v[3][0] ) );
		setRow( 1, Vector4f( v[0][1], v[1][1], v[2][1], v[3][1] ) );
		setRow( 2, Vector4f( v[0][2], v[1][2], v[2][2], v[3][2] ) );
		setRow( 3, Vector4f( v[0][3], v[1][3], v[2][3], v[3][3] ) );
	}

	Matrix4f( const float* v ) {
		setRow( 0, Vector4f( v[0], v[1], v[2], v[3] ) );
		setRow( 1, Vector4f( v[4], v[5], v[6], v[7] ) );
		setRow( 2, Vector4f( v[8], v[9], v[10], v[11] ) );
		setRow( 3, Vector4f( v[12], v[13], v[14], v[15] ) );
	}

	Matrix4f( float v0, float v1, float v2, float v3, float v4, float v5, float v6, float v7, float v8, float v9, float v10, float v11, float v12, float v13, float v14, float v15 ) {
		setRow( 0, Vector4f( v0, v1, v2, v3 ) );
		setRow( 1, Vector4f( v4, v5, v6, v7 ) );
		setRow( 2, Vector4f( v8, v9, v10, v11 ) );
		setRow( 3, Vector4f( v12, v13, v14, v15 ) );
	}

	void setFromRotationAnglePos( const Vector3f& normal, const Vector3f& originAxis, const Vector3f& pos );

	void setFromRTS( const Vector3f& pos, const Vector3f& axis, const Vector3f& zoom, MatrixRotationOrder mro = MatrixRotationOrder::xyz );
	static Matrix4f& MIDENTITY()  // return reference.
	{
		static Matrix4f a( Vector4f( 1.0f, 0.0f, 0.0f, 0.0f ), Vector4f( 0.0f, 1.0f, 0.0f, 0.0f ), Vector4f( 0.0f, 0.0f, 1.0f, 0.0f ),
						   Vector4f( 0.0f, 0.0f, 0.0f, 1.0f ) );
		return a;
	}

	static Matrix4f& MIDENTITY_YINV()  // return reference.
	{
		static Matrix4f a( Vector4f( 1.0f, 0.0f, 0.0f, 0.0f ), Vector4f( 0.0f, -1.0f, 0.0f, 0.0f ), Vector4f( 0.0f, 0.0f, 1.0f, 0.0f ),
						   Vector4f( 0.0f, 0.0f, 0.0f, 1.0f ) );
		return a;
	}

	int size() const { return 16; }

	const Vector4f& getRow( int i ) const {
		ASSERT( i >= 0 && i < 4 );
		return mRows[i];
	}

	Vector4f getCol( int i ) const {
		ASSERT( i >= 0 && i < 4 );
		return Vector4f( mRows[0][i], mRows[1][i], mRows[2][i], mRows[3][i] );
	}

	void setRow( int i, const Vector4f& row ) {
		ASSERT( i >= 0 && i < 4 );
		mRows[i] = row;
	}

	//float operator[](int element) const {
	//	ASSERT(element >= 0 && element < 3);
	//	return (&mX)[element];
	//}

	//float& operator[](int element) {
	//	ASSERT(element >= 0 && element < 3);
	//	return (&mX)[element];
	//}

	float operator[]( int index ) const {
		ASSERT( index >= 0 && index < size() );
		return mRows[index / 4][index % 4];
	}

	float& operator[]( int index ) {
		ASSERT( index >= 0 && index < size() );
		return *( &mRows[index / 4][index % 4] );
	}

	//const Vector4f& operator[](int row) const {
	//	return getRow(row);
	//}

	float operator()( int row, int col ) const {
		ASSERT( row >= 0 && row < 4 );
		return mRows[row][col];
	}

	float* rawPtr() {
		return reinterpret_cast<float*>( &mRows[0] );
	}

	const float* rawPtr() const {
		return reinterpret_cast<const float*>( &mRows[0] );
	}

	void set( int row, int col, float val ) {
		mRows[row][col] = val;
	}

	Matrix3f matrix3f() const {
		Matrix3f m;

		m.setRow( 0, Vector3f( mRows[0].x(), mRows[1].x(), mRows[2].x() ) );
		m.setRow( 1, Vector3f( mRows[0].y(), mRows[1].y(), mRows[2].y() ) );
		m.setRow( 2, Vector3f( mRows[0].z(), mRows[1].z(), mRows[2].z() ) );

		return m;
	}

	Vector4f operator*( const Vector4f& rhs ) const {
		Vector4f ret;
		ret[0] = getRow( 0 ).x()*rhs.x() + getRow( 1 ).x()*rhs.y() + getRow( 2 ).x()*rhs.z() + getRow( 3 ).x()*rhs.w();
		ret[1] = getRow( 0 ).y()*rhs.x() + getRow( 1 ).y()*rhs.y() + getRow( 2 ).y()*rhs.z() + getRow( 3 ).y()*rhs.w();
		ret[2] = getRow( 0 ).z()*rhs.x() + getRow( 1 ).z()*rhs.y() + getRow( 2 ).z()*rhs.z() + getRow( 3 ).z()*rhs.w();
		ret[3] = getRow( 0 ).w()*rhs.x() + getRow( 1 ).w()*rhs.y() + getRow( 2 ).w()*rhs.z() + getRow( 3 ).w()*rhs.w();
		return ret;
	}

	Vector3f operator*( const Vector3f& rhs3d ) const {
		Vector4f rhs = Vector4f( rhs3d, 1.0f );
		Vector4f ret;
		ret[0] = getRow( 0 ).x()*rhs.x() + getRow( 1 ).x()*rhs.y() + getRow( 2 ).x()*rhs.z() + getRow( 3 ).x()*rhs.w();
		ret[1] = getRow( 0 ).y()*rhs.x() + getRow( 1 ).y()*rhs.y() + getRow( 2 ).y()*rhs.z() + getRow( 3 ).y()*rhs.w();
		ret[2] = getRow( 0 ).z()*rhs.x() + getRow( 1 ).z()*rhs.y() + getRow( 2 ).z()*rhs.z() + getRow( 3 ).z()*rhs.w();
		ret[3] = getRow( 0 ).w()*rhs.x() + getRow( 1 ).w()*rhs.y() + getRow( 2 ).w()*rhs.z() + getRow( 3 ).w()*rhs.w();
		return ret.xyz();
	}

	inline bool isSimilarTo( const Matrix4f& m ) const {
		for ( size_t i = 0; i < 4; i++ ) {
			if ( !mRows[i].isSimilarTo( m.mRows[i] ) )
				return false;
		}

		return true;
	}

	bool operator==( const Matrix4f& rhs ) const {
		for ( int t = 0; t < 4; t++ ) if ( getRow( t ) != rhs.getRow( t ) ) return false;
		return true;
	}

    bool operator!=( const Matrix4f& rhs ) const {
	    return !operator==(rhs);
    }

    float operator()( int64_t index ) const {
		ASSERT( index < 16 );
		if ( index == 0 ) return mRows[0].x();
		if ( index == 1 ) return mRows[0].y();
		if ( index == 2 ) return mRows[0].z();
		if ( index == 3 ) return mRows[0].w();
		if ( index == 4 ) return mRows[1].x();
		if ( index == 5 ) return mRows[1].y();
		if ( index == 6 ) return mRows[1].z();
		if ( index == 7 ) return mRows[1].w();
		if ( index == 8 ) return mRows[2].x();
		if ( index == 9 ) return mRows[2].y();
		if ( index == 10 ) return mRows[2].z();
		if ( index == 11 ) return mRows[2].w();
		if ( index == 12 ) return mRows[3].x();
		if ( index == 13 ) return mRows[3].y();
		if ( index == 14 ) return mRows[3].z();
		if ( index == 15 ) return mRows[3].w();

		return 0.0f;
	}

	void setPerspective( float fovyInDegrees, float aspectRatio, float znear, float zfar );
	void setOrthogonalProjection( float left, float right, float bottom, float top );

	void setOrthogonalProjection();
	void setOrthogonalProjection( float leftv, float rightv, float topv, float bottomv, float nearv, float farv );
	void setAspectRatioMatrix( float aspectRatio );
	void setAspectRatioMatrixScreenSpace( float aspectRatio );

	void make3x3NormalizedRotationMatrix();

	void setRotationFromAngle( float angle, const Vector3f& axis ) {
		float c = angle;
		float s = sqrtf( 1.0f - c*c );
		float C = 1.0f - c;

		float x = axis.x();
		float y = axis.y();
		float z = axis.z();

		mRows[0] = Vector4f( x*x*C + c, x*y*C - z*s, x*z*C + y*s, 0.0f );
		mRows[1] = Vector4f( y*x*C + z*s, y*y*C + c, y*z*C - x*s, 0.0f );
		mRows[2] = Vector4f( z*x*C - y*s, z*y*C + x*s, z*z*C + c, 0.0f );
		mRows[3] = Vector4f( 0.0f, 0.0f, 0.0f, 1.0f );
	}

	void setRotation( float angle, const Vector3f& axis ) {
		float c = cos( angle );
		float s = sin( angle );
		float x = axis.x();
		float y = axis.y();
		float z = axis.z();

		mRows[0] = Vector4f( x * x * ( 1 - c ) + c, x * y * ( 1 - c ) + z * s, x * z * ( 1 - c ) - y * s, 0.0f );
		mRows[1] = Vector4f( x * y * ( 1 - c ) - z * s, y * y * ( 1 - c ) + c, y * z * ( 1 - c ) + x * s, 0.0f );
		mRows[2] = Vector4f( x * z * ( 1 - c ) + y * s, y * z * ( 1 - c ) - x * s, z * z * ( 1 - c ) + c, 0.0f );
		mRows[3] = Vector4f( 0.0f, 0.0f, 0.0f, 1.0f );
	}

	void rotate( float angle, const Vector3f& axis ) {
		float c = cos( angle );
		float s = sin( angle );
		float x = axis.x();
		float y = axis.y();
		float z = axis.z();
		Matrix4f m;

		m.setRow( 0, Vector4f( x * x * ( 1 - c ) + c, x * y * ( 1 - c ) + z * s, x * z * ( 1 - c ) - y * s, 0.0f ) );
		m.setRow( 1, Vector4f( x * y * ( 1 - c ) - z * s, y * y * ( 1 - c ) + c, y * z * ( 1 - c ) + x * s, 0.0f ) );
		m.setRow( 2, Vector4f( x * z * ( 1 - c ) + y * s, y * z * ( 1 - c ) - x * s, z * z * ( 1 - c ) + c, 0.0f ) );
		m.setRow( 3, Vector4f( 0.0f, 0.0f, 0.0f, 1.0f ) );

		mult( m );
	}

	void mult( const Matrix4f& rhs ) {
		Matrix4f temp;

		for ( int i = 0; i < 4; ++i ) {
			temp.mRows[i].setX( mRows[i].dot( Vector4f( rhs.mRows[0].x(), rhs.mRows[1].x(), rhs.mRows[2].x(), rhs.mRows[3].x() ) ) );
			temp.mRows[i].setY( mRows[i].dot( Vector4f( rhs.mRows[0].y(), rhs.mRows[1].y(), rhs.mRows[2].y(), rhs.mRows[3].y() ) ) );
			temp.mRows[i].setZ( mRows[i].dot( Vector4f( rhs.mRows[0].z(), rhs.mRows[1].z(), rhs.mRows[2].z(), rhs.mRows[3].z() ) ) );
			temp.mRows[i].setW( mRows[i].dot( Vector4f( rhs.mRows[0].w(), rhs.mRows[1].w(), rhs.mRows[2].w(), rhs.mRows[3].w() ) ) );
		}

		*this = temp;
	}

	Matrix4f operator*( const Matrix4f& rhs ) const {
		Matrix4f t = *this;
		t.mult( rhs );
		return t;
	}

	void identityWithPos( const Vector3f& offset ) {
		std::memcpy(this,  &Matrix4f::IDENTITY, sizeof(Matrix4f));
		setTranslation( offset );
	}

	void translate( const Vector2f& offset ) {
		mRows[3].setX( mRows[3].x() + offset.x() );
		mRows[3].setY( mRows[3].y() + offset.y() );
	}

	void translate( const Vector3f& offset ) {
		mRows[3].setX( mRows[3].x() + offset.x() );
		mRows[3].setY( mRows[3].y() + offset.y() );
		mRows[3].setZ( mRows[3].z() + offset.z() );
	}

	void setTranslation( const Vector2f& offset ) {
		mRows[3].setX( offset.x() );
		mRows[3].setY( offset.y() );
	}

	void setTranslation( const Vector3f& offset ) {
		mRows[3].setX( offset.x() );
		mRows[3].setY( offset.y() );
		mRows[3].setZ( offset.z() );
	}

    void setScale( const Vector3f& scale ) {
        mRows[0].setX( scale.x() );
        mRows[1].setY( scale.y() );
        mRows[2].setZ( scale.z() );
    }

    void scale( const float scale ) {
		mRows[0].setX( mRows[0].x() * scale );
		mRows[1].setY( mRows[1].y() * scale );
		mRows[2].setZ( mRows[2].z() * scale );
	}

	void scale( const Vector2f& scale ) {
		mRows[0].setX( mRows[0].x() * scale.x() );
		mRows[1].setY( mRows[1].y() * scale.y() );
	}

	void scale( const Vector3f& scale ) {
		mRows[0].setX( mRows[0].x() * scale.x() );
		mRows[1].setY( mRows[1].y() * scale.y() );
		mRows[2].setZ( mRows[2].z() * scale.z() );
	}

	void lookAt( const Vector3f& eye, const Vector3f& at, const Vector3f& up ) {
		Vector3f z = normalize( at - eye );  // Forward
		Vector3f x = normalize( cross( up, z ) ); // Right
		Vector3f y = cross( z, x );

		setRow( 0, Vector4f( x.x(), y.x(), z.x(), 0.0f ) );
		setRow( 1, Vector4f( x.y(), y.y(), z.y(), 0.0f ) );
		setRow( 2, Vector4f( x.z(), y.z(), z.z(), 0.0f ) );
		setRow( 3, Vector4f( -( dot( x, eye ) ), -( dot( y, eye ) ), -( dot( z, eye ) ), 1.0f ) );
	}

	Vector2f getXAxis2() const {
		return Vector2f( mRows[0].x(), mRows[0].y() );
	}

	Vector2f getYAxis2() const {
		return Vector2f( mRows[1].x(), mRows[1].y() );
	}

	Vector2f getZAxis2() const {
		return Vector2f( mRows[2].x(), mRows[2].y() );
	}

	Vector2f getPosition2() const {
		return Vector2f( mRows[3].x(), mRows[3].y() );
	}

	Vector3f getPosition3() const {
		return Vector3f( mRows[3].xyz() );
	}

	Vector4f transform( const Vector4f& position ) const {
		return Vector4f( position.dot( Vector4f( mRows[0].x(), mRows[1].x(), mRows[2].x(), mRows[3].x() ) ),
						 position.dot( Vector4f( mRows[0].y(), mRows[1].y(), mRows[2].y(), mRows[3].y() ) ), position.dot( Vector4f( mRows[0].z(), mRows[1].z(), mRows[2].z(), mRows[3].z() ) ),
						 position.dot( Vector4f( mRows[0].w(), mRows[1].w(), mRows[2].w(), mRows[3].w() ) ) );
	}

	Vector3f transform( const Vector3f& position3 ) const {
		Vector4f position = Vector4f( position3, 1.0f );
		return Vector4f( position.dot( Vector4f( mRows[0].x(), mRows[1].x(), mRows[2].x(), mRows[3].x() ) ),
						 position.dot( Vector4f( mRows[0].y(), mRows[1].y(), mRows[2].y(), mRows[3].y() ) ),
						 position.dot( Vector4f( mRows[0].z(), mRows[1].z(), mRows[2].z(), mRows[3].z() ) ),
						 position.dot( Vector4f( mRows[0].w(), mRows[1].w(), mRows[2].w(), mRows[3].w() ) ) ).xyz();
	}

	Vector3f transform3x3( const Vector3f& position3 ) const {
		return Vector3f( dot( position3, { mRows[0].x(), mRows[1].x(), mRows[2].x() } ),
						 dot( position3, { mRows[0].y(), mRows[1].y(), mRows[2].y() } ),
						 dot( position3, { mRows[0].z(), mRows[1].z(), mRows[2].z() } ) );
	}

	Vector2f transformPosition( const Vector2f& position ) const {
		Vector4f position4 = Vector4f( position.x(), position.y(), 0.0f, 1.0f );
		Vector4f transfPos = transform( position4 );
		return Vector2f( transfPos.x(), transfPos.y() );
	}

	Vector2f transformVector( const Vector2f& v ) const {
		Vector4f v4 = Vector4f( v.x(), v.y(), 0.0f, 0.0f );
		Vector4f transformedVector = transform( v4 );
		return Vector2f( transformedVector.x(), transformedVector.y() );
	}

	Vector4f transformWithPivotUE4Profile( const Vector2f& position, const Vector2f& pivot, float z ) const {
		Vector4f p4 = Vector4f( 0.0f, position.x(), position.y() + z, 1.0f );
		Vector4f p4Pivot = Vector4f( 0.0f, pivot.x(), pivot.y(), 1.0f );
		p4 = p4 - p4Pivot;
		return transform( p4 );
	}

	Matrix4f transpose() {
		Matrix4f m;

		m.setRow( 0, Vector4f( mRows[0].x(), mRows[1].x(), mRows[2].x(), mRows[3].x() ) );
		m.setRow( 1, Vector4f( mRows[0].y(), mRows[1].y(), mRows[2].y(), mRows[3].y() ) );
		m.setRow( 2, Vector4f( mRows[0].z(), mRows[1].z(), mRows[2].z(), mRows[3].z() ) );
		m.setRow( 3, Vector4f( mRows[0].w(), mRows[1].w(), mRows[2].w(), mRows[3].w() ) );

		return m;
	}

	void print() const {
		LOGI( "%0.2f, %0.2f, %0.2f, %0.2f\n%0.2f, %0.2f, %0.2f, %0.2f\n%0.2f, %0.2f, %0.2f, %0.2f\n%0.2f, %0.2f, %0.2f, %0.2f\n", mRows[0].x(), mRows[0].y(), mRows[0].z(),
			  mRows[0].w(), mRows[1].x(), mRows[1].y(), mRows[1].z(), mRows[1].w(), mRows[2].x(), mRows[2].y(), mRows[2].z(), mRows[2].w(), mRows[3].x(), mRows[3].y(),
			  mRows[3].z(), mRows[3].w() );
	}

	inline bool invert( Matrix4f& ret ) const {
		float inv[16], det;
		int i;

		inv[0] = ( *this )( 5 )  * ( *this )( 10 ) * ( *this )( 15 ) -
			( *this )( 5 )  * ( *this )( 11 ) * ( *this )( 14 ) -
			( *this )( 9 )  * ( *this )( 6 )  * ( *this )( 15 ) +
			( *this )( 9 )  * ( *this )( 7 )  * ( *this )( 14 ) +
			( *this )( 13 ) * ( *this )( 6 )  * ( *this )( 11 ) -
			( *this )( 13 ) * ( *this )( 7 )  * ( *this )( 10 );

		inv[4] = -( *this )( 4 )  * ( *this )( 10 ) * ( *this )( 15 ) +
			( *this )( 4 )  * ( *this )( 11 ) * ( *this )( 14 ) +
			( *this )( 8 )  * ( *this )( 6 )  * ( *this )( 15 ) -
			( *this )( 8 )  * ( *this )( 7 )  * ( *this )( 14 ) -
			( *this )( 12 ) * ( *this )( 6 )  * ( *this )( 11 ) +
			( *this )( 12 ) * ( *this )( 7 )  * ( *this )( 10 );

		inv[8] = ( *this )( 4 )  * ( *this )( 9 ) * ( *this )( 15 ) -
			( *this )( 4 )  * ( *this )( 11 ) * ( *this )( 13 ) -
			( *this )( 8 )  * ( *this )( 5 ) * ( *this )( 15 ) +
			( *this )( 8 )  * ( *this )( 7 ) * ( *this )( 13 ) +
			( *this )( 12 ) * ( *this )( 5 ) * ( *this )( 11 ) -
			( *this )( 12 ) * ( *this )( 7 ) * ( *this )( 9 );

		inv[12] = -( *this )( 4 )  * ( *this )( 9 ) * ( *this )( 14 ) +
			( *this )( 4 )  * ( *this )( 10 ) * ( *this )( 13 ) +
			( *this )( 8 )  * ( *this )( 5 ) * ( *this )( 14 ) -
			( *this )( 8 )  * ( *this )( 6 ) * ( *this )( 13 ) -
			( *this )( 12 ) * ( *this )( 5 ) * ( *this )( 10 ) +
			( *this )( 12 ) * ( *this )( 6 ) * ( *this )( 9 );

		inv[1] = -( *this )( 1 )  * ( *this )( 10 ) * ( *this )( 15 ) +
			( *this )( 1 )  * ( *this )( 11 ) * ( *this )( 14 ) +
			( *this )( 9 )  * ( *this )( 2 ) * ( *this )( 15 ) -
			( *this )( 9 )  * ( *this )( 3 ) * ( *this )( 14 ) -
			( *this )( 13 ) * ( *this )( 2 ) * ( *this )( 11 ) +
			( *this )( 13 ) * ( *this )( 3 ) * ( *this )( 10 );

		inv[5] = ( *this )( 0 )  * ( *this )( 10 ) * ( *this )( 15 ) -
			( *this )( 0 )  * ( *this )( 11 ) * ( *this )( 14 ) -
			( *this )( 8 )  * ( *this )( 2 ) * ( *this )( 15 ) +
			( *this )( 8 )  * ( *this )( 3 ) * ( *this )( 14 ) +
			( *this )( 12 ) * ( *this )( 2 ) * ( *this )( 11 ) -
			( *this )( 12 ) * ( *this )( 3 ) * ( *this )( 10 );

		inv[9] = -( *this )( 0 )  * ( *this )( 9 ) * ( *this )( 15 ) +
			( *this )( 0 )  * ( *this )( 11 ) * ( *this )( 13 ) +
			( *this )( 8 )  * ( *this )( 1 ) * ( *this )( 15 ) -
			( *this )( 8 )  * ( *this )( 3 ) * ( *this )( 13 ) -
			( *this )( 12 ) * ( *this )( 1 ) * ( *this )( 11 ) +
			( *this )( 12 ) * ( *this )( 3 ) * ( *this )( 9 );

		inv[13] = ( *this )( 0 )  * ( *this )( 9 ) * ( *this )( 14 ) -
			( *this )( 0 )  * ( *this )( 10 ) * ( *this )( 13 ) -
			( *this )( 8 )  * ( *this )( 1 ) * ( *this )( 14 ) +
			( *this )( 8 )  * ( *this )( 2 ) * ( *this )( 13 ) +
			( *this )( 12 ) * ( *this )( 1 ) * ( *this )( 10 ) -
			( *this )( 12 ) * ( *this )( 2 ) * ( *this )( 9 );

		inv[2] = ( *this )( 1 )  * ( *this )( 6 ) * ( *this )( 15 ) -
			( *this )( 1 )  * ( *this )( 7 ) * ( *this )( 14 ) -
			( *this )( 5 )  * ( *this )( 2 ) * ( *this )( 15 ) +
			( *this )( 5 )  * ( *this )( 3 ) * ( *this )( 14 ) +
			( *this )( 13 ) * ( *this )( 2 ) * ( *this )( 7 ) -
			( *this )( 13 ) * ( *this )( 3 ) * ( *this )( 6 );

		inv[6] = -( *this )( 0 )  * ( *this )( 6 ) * ( *this )( 15 ) +
			( *this )( 0 )  * ( *this )( 7 ) * ( *this )( 14 ) +
			( *this )( 4 )  * ( *this )( 2 ) * ( *this )( 15 ) -
			( *this )( 4 )  * ( *this )( 3 ) * ( *this )( 14 ) -
			( *this )( 12 ) * ( *this )( 2 ) * ( *this )( 7 ) +
			( *this )( 12 ) * ( *this )( 3 ) * ( *this )( 6 );

		inv[10] = ( *this )( 0 )  * ( *this )( 5 ) * ( *this )( 15 ) -
			( *this )( 0 )  * ( *this )( 7 ) * ( *this )( 13 ) -
			( *this )( 4 )  * ( *this )( 1 ) * ( *this )( 15 ) +
			( *this )( 4 )  * ( *this )( 3 ) * ( *this )( 13 ) +
			( *this )( 12 ) * ( *this )( 1 ) * ( *this )( 7 ) -
			( *this )( 12 ) * ( *this )( 3 ) * ( *this )( 5 );

		inv[14] = -( *this )( 0 )  * ( *this )( 5 ) * ( *this )( 14 ) +
			( *this )( 0 )  * ( *this )( 6 ) * ( *this )( 13 ) +
			( *this )( 4 )  * ( *this )( 1 ) * ( *this )( 14 ) -
			( *this )( 4 )  * ( *this )( 2 ) * ( *this )( 13 ) -
			( *this )( 12 ) * ( *this )( 1 ) * ( *this )( 6 ) +
			( *this )( 12 ) * ( *this )( 2 ) * ( *this )( 5 );

		inv[3] = -( *this )( 1 ) * ( *this )( 6 ) * ( *this )( 11 ) +
			( *this )( 1 ) * ( *this )( 7 ) * ( *this )( 10 ) +
			( *this )( 5 ) * ( *this )( 2 ) * ( *this )( 11 ) -
			( *this )( 5 ) * ( *this )( 3 ) * ( *this )( 10 ) -
			( *this )( 9 ) * ( *this )( 2 ) * ( *this )( 7 ) +
			( *this )( 9 ) * ( *this )( 3 ) * ( *this )( 6 );

		inv[7] = ( *this )( 0 ) * ( *this )( 6 ) * ( *this )( 11 ) -
			( *this )( 0 ) * ( *this )( 7 ) * ( *this )( 10 ) -
			( *this )( 4 ) * ( *this )( 2 ) * ( *this )( 11 ) +
			( *this )( 4 ) * ( *this )( 3 ) * ( *this )( 10 ) +
			( *this )( 8 ) * ( *this )( 2 ) * ( *this )( 7 ) -
			( *this )( 8 ) * ( *this )( 3 ) * ( *this )( 6 );

		inv[11] = -( *this )( 0 ) * ( *this )( 5 ) * ( *this )( 11 ) +
			( *this )( 0 ) * ( *this )( 7 ) * ( *this )( 9 ) +
			( *this )( 4 ) * ( *this )( 1 ) * ( *this )( 11 ) -
			( *this )( 4 ) * ( *this )( 3 ) * ( *this )( 9 ) -
			( *this )( 8 ) * ( *this )( 1 ) * ( *this )( 7 ) +
			( *this )( 8 ) * ( *this )( 3 ) * ( *this )( 5 );

		inv[15] = ( *this )( 0 ) * ( *this )( 5 ) * ( *this )( 10 ) -
			( *this )( 0 ) * ( *this )( 6 ) * ( *this )( 9 ) -
			( *this )( 4 ) * ( *this )( 1 ) * ( *this )( 10 ) +
			( *this )( 4 ) * ( *this )( 2 ) * ( *this )( 9 ) +
			( *this )( 8 ) * ( *this )( 1 ) * ( *this )( 6 ) -
			( *this )( 8 ) * ( *this )( 2 ) * ( *this )( 5 );

		det = ( *this )( 0 ) * inv[0] + ( *this )( 1 ) * inv[4] + ( *this )( 2 ) * inv[8] + ( *this )( 3 ) * inv[12];

		if ( det == 0 )
			return false;

		det = 1.0f / det;

		for ( i = 0; i < 4; i++ ) {
			ret.setRow( i, Vector4f( inv[i * 4 + 0] * det, inv[i * 4 + 1] * det, inv[i * 4 + 2] * det, inv[i * 4 + 3] * det ) );
		}

		return true;
	}

private:
	Vector4f mRows[4];
};
