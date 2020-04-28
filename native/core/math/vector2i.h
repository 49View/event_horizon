#pragma once
#include <math.h>
#include <ostream>

class Vector2i {
public:
    friend std::ostream &operator<<( std::ostream &os, const Vector2i &i ) {
        os << "x: " << i.mX << " y: " << i.mY;
        return os;
    }

    static const Vector2i ZERO;
	static const Vector2i ONE;

	Vector2i() = default;

	explicit Vector2i( int x ) {
		mX = x;
		mY = x;
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

    bool operator!=( const Vector2i& rhs ) const {
        return mX != rhs.mX || mY != rhs.mY;
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

	void operator+=( const Vector2i& rhs ) {
		mX += rhs.x();
		mY += rhs.y();
	}

	void operator-=( const Vector2i& rhs) {
		mX -= rhs.x();
		mY -= rhs.y();
	}

	void operator*=( const Vector2i& rhs ) {
		mX *= rhs.x();
		mY *= rhs.y();
	}

	Vector2i rotate90() const {
		return Vector2i( y(), -x() );
	}

	void rotate90() {
		*this = Vector2i( y(), -x() );
	}

	void rotate90Around( const Vector2i& v ) {
		*this -= v;
		rotate90();
		*this += v;
	}

private:
	int mX;
	int mY;
};

inline Vector2i rotate90Around( const Vector2i& source, const Vector2i& v ) {
	const auto center = source - v;
	return center.rotate90();
}

inline float aspect( const Vector2i& v ) {
	return static_cast<float>( v.x() ) / v.y();
}

using V2i = Vector2i;
