#ifndef D_MATH_MATRIX2_H
#define D_MATH_MATRIX2_H

#include "vector2f.h"

// SHARED CLASS: Any changes to this, should be integrated to /svn/shared, and a mail should be send to everyone



class Matrix2f {
public:

	static const Matrix2f IDENTITY;

	Matrix2f() {
	}

	Matrix2f( const Vector2f& row0, const Vector2f& row1 ) {
		mRows[0] = row0;
		mRows[1] = row1;
	}

	const Vector2f& getRow( int i ) {
		return mRows[i];
	}

	void setRow( int i, const Vector2f& row ) {
		mRows[i] = row;
	}

	void setRotation( float angle ) {
		float c = cos( angle );
		float s = sin( angle );

		mRows[0] = Vector2f( c, -s );
		mRows[1] = Vector2f( s, c );
	}

	void setTransform( float angle, Vector2f& scale ) {
		float c = cos( angle );
		float s = sin( angle );

		mRows[0] = Vector2f( c * scale.x(), -s );
		mRows[1] = Vector2f( s, c * scale.y() );
	}

	void mult( const Matrix2f& rhs ) {
		Matrix2f temp;

		for ( int i = 0; i < 2; ++i ) {
			temp.mRows[i].setX( dot( mRows[i], Vector2f( rhs.mRows[0].x(), rhs.mRows[1].x() ) ) );
			temp.mRows[i].setY( dot( mRows[i], Vector2f( rhs.mRows[0].y(), rhs.mRows[1].y() ) ) );
		}

		*this = temp;
	}

	Vector2f transform( const Vector2f& rhs ) const {
		return Vector2f( dot( rhs, Vector2f( mRows[0].x(), mRows[1].x() ) ),
						 dot( rhs, Vector2f( mRows[0].y(), mRows[1].y() ) ) );
	}

	float determinant() {
		return ( mRows[0].x() * mRows[1].y() ) - ( mRows[0].y() * mRows[1].x() );
	}

private:
	Vector2f mRows[2];
};



#endif // D_MATH_MATRIX2_H
