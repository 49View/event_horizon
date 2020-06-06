#ifndef D_MATH_MATRIX3_H
#define D_MATH_MATRIX3_H

#include "vector3f.h"

// SHARED CLASS: Any changes to this, should be integrated to /svn/shared, and a mail should be send to everyone



class Matrix3f {
public:

	static const Matrix3f IDENTITY;
	static const Matrix3f ZERO;
	static const Matrix3f ONE;

	Matrix3f() {
	}

	Matrix3f( const Vector3f& row0, const Vector3f& row1, const Vector3f& row2 ) {
		mRows[0] = row0;
		mRows[1] = row1;
		mRows[2] = row2;
	}

	Matrix3f( const float* data ) {
		mRows[0][0] = data[0];
		mRows[0][1] = data[1];
		mRows[0][2] = data[2];
		mRows[1][0] = data[3];
		mRows[1][1] = data[4];
		mRows[1][2] = data[5];
		mRows[2][0] = data[6];
		mRows[2][1] = data[7];
		mRows[2][2] = data[8];
	}

	const Vector3f& getRow( int i ) const {
		ASSERT( i >= 0 && i < 3 );
		return mRows[i];
	}

	void setRow( int i, const Vector3f& row ) {
		ASSERT( i >= 0 && i < 3 );
		mRows[i] = row;
	}

	void setData( const float* data ) {
		mRows[0][0] = data[0];
		mRows[0][1] = data[1];
		mRows[0][2] = data[2];
		mRows[1][0] = data[3];
		mRows[1][1] = data[4];
		mRows[1][2] = data[5];
		mRows[2][0] = data[6];
		mRows[2][1] = data[7];
		mRows[2][2] = data[8];
	}

	void setData( const float* data, const float alpha ) {
		mRows[0][0] = data[0] * alpha;
		mRows[0][1] = data[1] * alpha;
		mRows[0][2] = data[2] * alpha;
		mRows[1][0] = data[3] * alpha;
		mRows[1][1] = data[4] * alpha;
		mRows[1][2] = data[5] * alpha;
		mRows[2][0] = data[6] * alpha;
		mRows[2][1] = data[7] * alpha;
		mRows[2][2] = data[8] * alpha;
	}

	void getData( float * sh_buffer ) const {
		sh_buffer[0] = mRows[0][0];
		sh_buffer[1] = mRows[0][1];
		sh_buffer[2] = mRows[0][2];
		sh_buffer[3] = mRows[1][0];
		sh_buffer[4] = mRows[1][1];
		sh_buffer[5] = mRows[1][2];
		sh_buffer[6] = mRows[2][0];
		sh_buffer[7] = mRows[2][1];
		sh_buffer[8] = mRows[2][2];
	}

	float getElement( int row, int col ) {
		ASSERT( row >= 0 && row < 3 );
		ASSERT( col >= 0 && col < 3 );
		Vector3f& rowV = mRows[row];
		float element = ( col == 0 ) ? rowV.x() : ( col == 1 ) ? rowV.y() : rowV.z();
		return element;
	}

	Matrix3f operator *( const Matrix3f& rhs ) const {
		Matrix3f result;
		result.mRows[0] = rhs.mRows[0] * mRows[0].x() + rhs.mRows[1] * mRows[0].y() + rhs.mRows[2] * mRows[0].z();
		result.mRows[1] = rhs.mRows[0] * mRows[1].x() + rhs.mRows[1] * mRows[1].y() + rhs.mRows[2] * mRows[1].z();
		result.mRows[2] = rhs.mRows[0] * mRows[2].x() + rhs.mRows[1] * mRows[2].y() + rhs.mRows[2] * mRows[2].z();
		return result;
	}

	Matrix3f operator *( float rhs ) const {
		Matrix3f result;
		result.mRows[0] = mRows[0] * rhs;
		result.mRows[1] = mRows[1] * rhs;
		result.mRows[2] = mRows[2] * rhs;
		return result;
	}

	Vector3f operator *( const Vector3f& rhs ) const {
		Vector3f result;
		result[0] = rhs[0] * mRows[0].x() + rhs[1] * mRows[0].y() + rhs[2] * mRows[0].z();
		result[1] = rhs[0] * mRows[1].x() + rhs[1] * mRows[1].y() + rhs[2] * mRows[1].z();
		result[2] = rhs[0] * mRows[2].x() + rhs[1] * mRows[2].y() + rhs[2] * mRows[2].z();
		return result;
	}

	void operator *=( float rhs ) {
		mRows[0] = mRows[0] * rhs;
		mRows[1] = mRows[1] * rhs;
		mRows[2] = mRows[2] * rhs;
	}

	void operator +=( const Matrix3f& rhs ) {
		mRows[0] += rhs.mRows[0];
		mRows[1] += rhs.mRows[1];
		mRows[2] += rhs.mRows[2];
	}

	Matrix3f operator +( const Matrix3f& rhs ) const {
		Matrix3f result;
		result.mRows[0] = mRows[0] + rhs.mRows[0];
		result.mRows[1] = mRows[1] + rhs.mRows[1];
		result.mRows[2] = mRows[2] + rhs.mRows[2];
		return result;
	}

	Matrix3f operator -( const Matrix3f& rhs ) const {
		Matrix3f result;
		result.mRows[0] = mRows[0] - rhs.mRows[0];
		result.mRows[1] = mRows[1] - rhs.mRows[1];
		result.mRows[2] = mRows[2] - rhs.mRows[2];
		return result;
	}

	float operator[]( int index ) const {
		ASSERT( index >= 0 && index < 9 );
		return mRows[index / 3][index % 3];
	}

	float& operator[]( int index ) {
		ASSERT( index >= 0 && index < 9 );
		return *( &mRows[index / 3][index % 3] );
	}

	void setRotation( float angle, const Vector3f& axis ) {
		float c = cos( angle );
		float s = sin( angle );
		float x = axis.x();
		float y = axis.y();
		float z = axis.z();

		mRows[0] = Vector3f( x * x * ( 1 - c ) + c, x * y * ( 1 - c ) + z * s, x * z * ( 1 - c ) - y * s );
		mRows[1] = Vector3f( x * y * ( 1 - c ) - z * s, y * y * ( 1 - c ) + c, y * z * ( 1 - c ) + x * s );
		mRows[2] = Vector3f( x * z * ( 1 - c ) + y * s, y * z * ( 1 - c ) - x * s, z * z * ( 1 - c ) + c );
	}

	inline int size() const { return 9; }

	Vector3f mRows[3];
};

static inline float determinant( const Matrix3f& m ) {
	const Vector3f& row0 = m.getRow( 0 );
	const Vector3f& row1 = m.getRow( 1 );
	const Vector3f& row2 = m.getRow( 2 );
	float det = row0.x() * ( row1.y() * row2.z() - row1.z() * row2.y() );
	det += row0.y() * ( row1.z() * row2.x() - row1.x() * row2.z() );
	det += row0.z() * ( row1.x() * row2.y() - row1.y() * row2.x() );
	return det;
}

static inline Matrix3f inverse( const Matrix3f& m ) {
	Matrix3f result;
	float det = determinant( m );
	if ( det == 0.0f ) {
		ASSERT( false );
		return Matrix3f();
	}
	const Vector3f& xAxis = m.getRow( 0 );
	const Vector3f& yAxis = m.getRow( 1 );
	const Vector3f& zAxis = m.getRow( 2 );
	float recipDet = 1.0f / det;

	Vector3f resultX = Vector3f( //
								 yAxis.y() * zAxis.z() - yAxis.z() * zAxis.y(), //
								 zAxis.y() * xAxis.z() - zAxis.z() * xAxis.y(), //
								 xAxis.y() * yAxis.z() - xAxis.z() * yAxis.y() ) * recipDet;
	Vector3f resultY = Vector3f( //
								 yAxis.z() * zAxis.x() - yAxis.x() * zAxis.z(), //
								 zAxis.z() * xAxis.x() - zAxis.x() * xAxis.z(), //
								 xAxis.z() * yAxis.x() - xAxis.x() * yAxis.z() ) * recipDet;
	Vector3f resultZ = Vector3f( //
								 yAxis.x() * zAxis.y() - yAxis.y() * zAxis.x(), //
								 zAxis.x() * xAxis.y() - zAxis.y() * xAxis.x(), //
								 xAxis.x() * yAxis.y() - xAxis.y() * yAxis.x() ) * recipDet;
	return Matrix3f( resultX, resultY, resultZ );
}

static inline Matrix3f transpose( const Matrix3f& m ) {
	Matrix3f ret;

	ret.setRow( 0, { m.getRow( 0 ).x(), m.getRow( 1 ).x(), m.getRow( 2 ).x() } );
	ret.setRow( 1, { m.getRow( 0 ).y(), m.getRow( 1 ).y(), m.getRow( 2 ).y() } );
	ret.setRow( 2, { m.getRow( 0 ).z(), m.getRow( 1 ).z(), m.getRow( 2 ).z() } );

	return ret;
}

static inline Vector3f transformVector( const Vector3f& v, const Matrix3f& m ) {
	return Vector3f( m.getRow( 0 ) * v.x() + m.getRow( 1 ) * v.y() + m.getRow( 2 ) * v.z() );
}



#endif // D_MATH_MATRIX3_H
