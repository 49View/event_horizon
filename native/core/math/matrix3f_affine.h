#ifndef D_MATH_Matrix3fAffine_H
#define D_MATH_Matrix3fAffine_H

#include "vector2f.h"
#include "math_util.h"

// SHARED CLASS: Any changes to this, should be integrated to /svn/shared, and a mail should be send to everyone



class Matrix3fAffine {
public:

	static const Matrix3fAffine IDENTITY;

	Matrix3fAffine() {
	}

	Matrix3fAffine( const Vector2f& row0, const Vector2f& row1, const Vector2f& row2 ) {
		mRows[0] = row0;
		mRows[1] = row1;
		mRows[2] = row2;
	}

	Matrix3fAffine( float x0, float y0, float x1, float y1, float x2, float y2 ) {
		mRows[0].set( x0, y0 );
		mRows[1].set( x1, y1 );
		mRows[2].set( x2, y2 );
	}

	explicit Matrix3fAffine( const Vector2f& position ) {
		mRows[0] = Vector2f::X_AXIS;
		mRows[1] = Vector2f::Y_AXIS;
		mRows[2] = position;
	}

	void set( const Vector2f& row0, const Vector2f& row1, const Vector2f& row2 ) {
		mRows[0] = row0;
		mRows[1] = row1;
		mRows[2] = row2;
	}

	void set( float x0, float y0, float x1, float y1, float x2, float y2 ) {
		mRows[0].set( x0, y0 );
		mRows[1].set( x1, y1 );
		mRows[2].set( x2, y2 );
	}

	const Vector2f& getRow( int row ) const {
		return mRows[row];
	}

	void setRow( int row, const Vector2f& pos ) {
		mRows[row] = pos;
	}

	Vector2f transformPoint( const Vector2f& point ) const {
		Vector2f result = mRows[0] * point.x() + mRows[1] * point.y() + mRows[2];
		return result;
	}

	Vector2f transformPoint( float x, float y ) const {
		Vector2f result = mRows[0] * x + mRows[1] * y + mRows[2];
		return result;
	}

	Vector2f transformVector( const Vector2f& vector ) const {
		Vector2f result = mRows[0] * vector.x() + mRows[1] * vector.y();
		return result;
	}

	Vector2f transformVector( float x, float y ) const {
		Vector2f result = mRows[0] * x + mRows[1] * y;
		return result;
	}

	Matrix3fAffine& operator =( const Matrix3fAffine& rhs ) {
		mRows[0] = rhs.mRows[0];
		mRows[1] = rhs.mRows[1];
		mRows[2] = rhs.mRows[2];
		return *this;
	}

	void invert( Matrix3fAffine& result ) {
		float m11 = mRows[0].x();
		float m12 = mRows[0].y();
		float m21 = mRows[1].x();
		float m22 = mRows[1].y();
		float m31 = mRows[2].x();
		float m32 = mRows[2].y();
		float det = m11 * m22 - m12 * m21;
		float recipDet = 1.0f / det;
		result.mRows[0].set( m22 * recipDet, -m12 * recipDet );
		result.mRows[1].set( -m21 * recipDet, m11 * recipDet );
		result.mRows[2].set( ( m32 * m21 - m31 * m22 ) * recipDet, ( m31 * m12 - m32 * m11 ) * recipDet );
	}

	static void multiply( Matrix3fAffine& result, const Matrix3fAffine& a, const Matrix3fAffine& b ) {
		// Will go wrong if "b" and result are the same matrix. ("a" and result pointing to the same matrix is fine though)
		//ASSERT(&result != &b);
		result.mRows[0] = b.mRows[0] * a.mRows[0].x() + b.mRows[1] * a.mRows[0].y();
		result.mRows[1] = b.mRows[0] * a.mRows[1].x() + b.mRows[1] * a.mRows[1].y();
		result.mRows[2] = b.mRows[0] * a.mRows[2].x() + b.mRows[1] * a.mRows[2].y() + b.mRows[2];
	}

	void applyRotation( float angle ) {
		Vector2f oldRow0 = mRows[0];
		Vector2f oldRow1 = mRows[1];

		float c = cosf( angle );
		float s = sinf( angle );

		mRows[0] = +c * oldRow0 + s * oldRow1;
		mRows[1] = -s * oldRow0 + c * oldRow1;
	}

	const Vector2f& getPosition() const {
		return mRows[2];
	}

private:
	Vector2f mRows[3];
};



#endif // D_Matrix3fAffine_H
