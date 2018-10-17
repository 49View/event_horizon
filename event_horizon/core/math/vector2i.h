#ifndef D_MATH_VECTOR2I_H
#define D_MATH_VECTOR2I_H



#include <math.h>

class Vector2i {
public:

	static const Vector2i ZERO;
	static const Vector2i ONE;

	Vector2i() {
	}

	Vector2i( int x, int y ) {
		mX = x;
		mY = y;
	}

	int x() const {
		return mX;
	}

	int y() const {
		return mY;
	}

	void setX( int x ) {
		mX = x;
	}

	void setY( int y ) {
		mY = y;
	}

	void set( int x, int y ) {
		mX = x;
		mY = y;
	}

	bool operator==( const Vector2i& rhs ) const {
		return mX == rhs.mX && mY == rhs.mY;
	}

	Vector2i operator+( const Vector2i& rhs ) const {
		return Vector2i( mX + rhs.mX, mY + rhs.mY );
	}

	Vector2i operator+( int rhs ) const {
		return Vector2i( mX + rhs, mY + rhs );
	}

	Vector2i operator-( const Vector2i& rhs ) const {
		return Vector2i( mX - rhs.mX, mY - rhs.mY );
	}

	Vector2i operator-( int rhs ) const {
		return Vector2i( mX - rhs, mY - rhs );
	}

	Vector2i operator*( const Vector2i& rhs ) const {
		return Vector2i( mX * rhs.mX, mY * rhs.mY );
	}

	Vector2i operator*( int rhs ) const {
		return Vector2i( mX * rhs, mY * rhs );
	}

	Vector2i operator/( int rhs ) const {
		return Vector2i( mX / rhs, mY / rhs );
	}

	void operator+=( int rhs ) {
		mX += rhs;
		mY += rhs;
	}

	void operator-=( int rhs ) {
		mX -= rhs;
		mY -= rhs;
	}

	void operator*=( int rhs ) {
		mX *= rhs;
		mY *= rhs;
	}

private:
	int mX;
	int mY;
};

inline float aspect( const Vector2i& v ) {
	return static_cast<float>( v.x() ) / v.y();
}



#endif // D_vector2_H
