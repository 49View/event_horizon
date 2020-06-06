#ifndef D_MATH_MATRIX4_AFFINE_H
#define D_MATH_MATRIX4_AFFINE_H

#include "vector3f.h"
#include "matrix3f.h"

// SHARED CLASS: Any changes to this, should be integrated to /svn/shared, and a mail should be send to everyone



class Matrix4fAffine {
public:

	static const Matrix4fAffine IDENTITY;

	Matrix4fAffine() {
	}

	Matrix4fAffine( const Vector3f& row0, const Vector3f& row1, const Vector3f& row2, const Vector3f& row3 ) {
		mRows[0] = row0;
		mRows[1] = row1;
		mRows[2] = row2;
		mRows[3] = row3;
	}

	const Vector3f& getRow( int i ) const {
		ASSERT( i >= 0 && i < 4 );
		return mRows[i];
	}

	void setRow( int i, const Vector3f& row ) {
		ASSERT( i >= 0 && i < 4 );
		mRows[i] = row;
	}

private:
	Vector3f mRows[4];
};

static inline Vector3f transformPoint( const Vector3f& v, const Matrix4fAffine& m ) {
	Vector3f result = m.getRow( 0 ) * v.x() + m.getRow( 1 ) * v.y() + m.getRow( 2 ) * v.z() + m.getRow( 3 );
	return result;
}

static inline Vector3f transformVector( const Vector3f& v, const Matrix4fAffine& m ) {
	Vector3f result = m.getRow( 0 ) * v.x() + m.getRow( 1 ) * v.y() + m.getRow( 2 ) * v.z();
	return result;
}

static inline float determinant( const Matrix4fAffine& m ) {
	const Vector3f& xAxis = m.getRow( 0 );
	const Vector3f& yAxis = m.getRow( 1 );
	const Vector3f& zAxis = m.getRow( 2 );
	float det = xAxis.x() * ( yAxis.y() * zAxis.z() - yAxis.z() * zAxis.y() );
	det += xAxis.y() * ( yAxis.z() * zAxis.x() - yAxis.x() * zAxis.z() );
	det += xAxis.z() * ( yAxis.x() * zAxis.y() - yAxis.y() * zAxis.x() );
	return det;
}

static inline Matrix4fAffine inverse( const Matrix4fAffine& m ) {
	float det = determinant( m );
	if ( det == 0.0f ) {
		ASSERT( false );
		return Matrix4fAffine();
	}
	Matrix3f m33( m.getRow( 0 ), m.getRow( 1 ), m.getRow( 2 ) );
	Matrix3f m33Inv = inverse( m33 );
	Vector3f wAxis = transformVector( -m.getRow( 3 ), m33Inv );
	Matrix4fAffine result( m33Inv.getRow( 0 ), m33Inv.getRow( 1 ), m33Inv.getRow( 2 ), wAxis );
	return result;
}

static inline Matrix4fAffine multiply( const Matrix4fAffine& a, const Matrix4fAffine& b ) {
	Matrix4fAffine result( transformVector( a.getRow( 0 ), b ), transformVector( a.getRow( 1 ), b ), transformVector( a.getRow( 2 ), b ), transformPoint( a.getRow( 3 ), b ) );
	return result;
}



#endif // D_MATH_MATRIX4_H
