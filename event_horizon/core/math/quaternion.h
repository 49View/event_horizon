#pragma once

#include <iostream>
#include <math.h>
#include "vector4f.h"
#include "matrix4f.h"

class Quaternion {
	float mData[4];

public:

	Quaternion() {
		mData[0] = mData[1] = mData[2] = 0;
		mData[3] = 1;
	}

	Quaternion( const Vector3f& v ) {
		euler( v );
	}

	Quaternion( const Vector3f& v, float w ) {
		mData[0] = v[0];
		mData[1] = v[1];
		mData[2] = v[2];
		mData[3] = w;
	}

	// This makes a rotation quaterion
	Quaternion( float angle, const Vector3f& axis ) {
		mData[0] = axis.x() * sinf( angle / 2.0f );
		mData[1] = axis.y() * sinf( angle / 2.0f );
		mData[2] = axis.z() * sinf( angle / 2.0f );
		mData[3] = cosf( angle / 2.0f );
	}

	Quaternion( const Vector4f& v ) {
		mData[0] = v[0];
		mData[1] = v[1];
		mData[2] = v[2];
		mData[3] = v[3];
	}

	Quaternion( const float* array ) {
		if ( !array ) {
			//			UAV_EXCEPTION("Constructing quaternion from 0 array.");
		}
		for ( uint32_t i = 0; i < 4; i++ ) {
			mData[i] = array[i];
		}
	}

	Quaternion( float x, float y, float z, float w ) {
		mData[0] = x;
		mData[1] = y;
		mData[2] = z;
		mData[3] = w;
	}

	Quaternion( double x, double y, double z, double w ) {
		mData[0] = static_cast<float>(x);
		mData[1] = static_cast<float>(y);
		mData[2] = static_cast<float>(z);
		mData[3] = static_cast<float>(w);
	}

	float x() const { return mData[0]; }
	float y() const { return mData[1]; }
	float z() const { return mData[2]; }
	float w() const { return real(); }

	friend std::ostream& operator<<( std::ostream& os, const Quaternion& quaternion ) {
		os << quaternion.mData;
		return os;
	}

	Vector3f complex() const { return Vector3f( mData ); }
	void complex( const Vector3f& c ) { mData[0] = c[0]; mData[1] = c[1];  mData[2] = c[2]; }

	float real() const { return mData[3]; }
	void real( float r ) { mData[3] = r; }
	constexpr size_t size() const { return 4; }

	Quaternion conjugate( void ) const {
		return Quaternion( -complex(), real() );
	}

	/**
	* @brief Computes the inverse of this quaternion.
	*
	* @note This is a general inverse.  If you know a priori
	* that you're using a unit quaternion (i.e., norm() == 1),
	* it will be significantly faster to use conjugate() instead.
	*
	* @return The quaternion q such that q * (*this) == (*this) * q
	* == [ 0 0 0 1 ]<sup>T</sup>.
	*/
	Quaternion inverse( void ) const {
		return conjugate() / norm();
	}

	/**
	* @brief Computes the product of this quaternion with the
	* quaternion 'rhs'.
	*
	* @param rhs The right-hand-side of the product operation.
	*
	* @return The quaternion product (*this) x @p rhs.
	*/
	Quaternion product( const Quaternion& rhs ) const {
		return Quaternion( y()*rhs.z() - z()*rhs.y() + x()*rhs.w() + w()*rhs.x(),
						   z()*rhs.x() - x()*rhs.z() + y()*rhs.w() + w()*rhs.y(),
						   x()*rhs.y() - y()*rhs.x() + z()*rhs.w() + w()*rhs.z(),
						   w()*rhs.w() - x()*rhs.x() - y()*rhs.y() - z()*rhs.z() );
	}

	/**
	* @brief Quaternion product operator.
	*
	* The result is a quaternion such that:
	*
	* result.real() = (*this).real() * rhs.real() -
	* (*this).complex().dot(rhs.complex());
	*
	* and:
	*
	* result.complex() = rhs.complex() * (*this).real
	* + (*this).complex() * rhs.real()
	* - (*this).complex().cross(rhs.complex());
	*
	* @return The quaternion product (*this) x rhs.
	*/
	Quaternion operator*( const Quaternion& rhs ) const {
		return product( rhs );
	}

	/**
	* @brief Quaternion scalar product operator.
	* @param s A scalar by which to multiply all components
	* of this quaternion.
	* @return The quaternion (*this) * s.
	*/
	Quaternion operator*( float s ) const {
		return Quaternion( complex()*s, real()*s );
	}

	/**
	* @brief Produces the sum of this quaternion and rhs.
	*/
	Quaternion operator+( const Quaternion& rhs ) const {
		return Quaternion( x() + rhs.x(), y() + rhs.y(), z() + rhs.z(), w() + rhs.w() );
	}

	/**
	* @brief Produces the difference of this quaternion and rhs.
	*/
	Quaternion operator-( const Quaternion& rhs ) const {
		return Quaternion( x() - rhs.x(), y() - rhs.y(), z() - rhs.z(), w() - rhs.w() );
	}

	/**
	* @brief Unary negation.
	*/
	Quaternion operator-() const {
		return Quaternion( -x(), -y(), -z(), -w() );
	}

	/**
	* @brief Quaternion scalar division operator.
	* @param s A scalar by which to divide all components
	* of this quaternion.
	* @return The quaternion (*this) / s.
	*/
	Quaternion operator/( float s ) const {
		if ( s == 0 ) std::clog << "Dividing quaternion by 0." << std::endl;
		return Quaternion( complex() / s, real() / s );
	}

	/**
	* @brief Returns a matrix representation of this
	* quaternion.
	*
	* Specifically this is the matrix such that:
	*
	* this->matrix() * q.vector() = (*this) * q for any quaternion q.
	*
	* Note that this is @e NOT the rotation matrix that may be
	* represented by a unit quaternion.
	*/
	Matrix4f matrix() const {
		float m[16] = {
			w(), -z(),  y(), x(),
			z(),  w(), -x(), y(),
			-y(),  x(),  w(), z(),
			-x(), -y(), -z(), w()
		};
		return Matrix4f( m );
	}

	/**
	* @brief Returns a matrix representation of this
	* quaternion for right multiplication.
	*
	* Specifically this is the matrix such that:
	*
	* q.vector().transpose() * this->matrix() = (q *
	* (*this)).vector().transpose() for any quaternion q.
	*
	* Note that this is @e NOT the rotation matrix that may be
	* represented by a unit quaternion.
	*/
	Matrix4f rightMatrix() const {
		float m[16] = {
			+w(), -z(),  y(), -x(),
			+z(),  w(), -x(), -y(),
			-y(),  x(),  w(), -z(),
			+x(),  y(),  z(),  w()
		};
		return Matrix4f( m );
	}

	/**
	* @brief Returns this quaternion as a 4-vector.
	*
	* This is simply the vector [x y z w]<sup>T</sup>
	*/
	Vector4f vector() const { return Vector4f( mData ); }

	/**
	* @brief Returns the norm ("magnitude") of the quaternion.
	* @return The 2-norm of [ w(), x(), y(), z() ]<sup>T</sup>.
	*/
	float norm() const {
		return sqrt( mData[0] * mData[0] + mData[1] * mData[1] +
					 mData[2] * mData[2] + mData[3] * mData[3] );
	}

	void normalise() {
		float n = norm();
		mData[0] /= n;
		mData[1] /= n;
		mData[2] /= n;
		mData[3] /= n;
	}

	/**
	* @brief Computes the rotation matrix represented by a unit
	* quaternion.
	*
	* @note This does normalizes the quaternion
	*/
	Matrix4f rotationMatrix() const {
		float n = norm();
		float qx = mData[0] / n;
		float qy = mData[1] / n;
		float qz = mData[2] / n;
		float qw = mData[3] / n;

		Vector4f r0( 1 - 2 * qy*qy - 2 * qz*qz, 2 * qx*qy - 2 * qz*qw    , 2 * qx*qz + 2 * qy*qw, 0.0f);
		Vector4f r1( 2 * qx*qy + 2 * qz*qw    , 1 - 2 * qx*qx - 2 * qz*qz, 2 * qy*qz - 2 * qx*qw, 0.0f);
		Vector4f r2( 2 * qx*qz - 2 * qy*qw    , 2 * qy*qz + 2 * qx*qw    , 1 - 2 * qx*qx - 2 * qy*qy, 0.0f);
		Vector4f r3( 0.0f, 0.0f, 0.0f, 1.0f );

		return Matrix4f( r0, r1, r2, r3 );
	}

	/**
	* @brief Returns the scaled-axis representation of this
	* quaternion rotation.
	*/
	Vector3f scaledAxis( void ) {
		if ( mData[3] > 1 ) normalise(); // if w>1 acos and sqrt will produce errors, this cant happen if quaternion is normalised
//		float angle = 2.0f * acosf(mData[3]);
		double s = sqrt( 1 - mData[3] * mData[3] ); // assuming quaternion normalised then w is less than 1, so term always positive.
		if ( s < 0.001 ) { // test to avoid divide by zero, s is always positive due to sqrt
						 // if s close to zero then direction of axis not important
			return Vector3f( mData[0], mData[1], mData[2] ); // if it is important that axis is normalised then replace with x=1; y=z=0;
		}

		return Vector3f( mData[0] / s, mData[1] / s, mData[2] / s ); // if it is important that axis is normalised then replace with x=1; y=z=0;
	}

	/**
	* @brief Sets quaternion to be same as rotation by scaled axis w.
	*/
	void scaledAxis( const Vector3f& w ) {
		float theta = length( w );
		if ( theta > 0.0001f ) {
			float s = sinf( theta / 2.0f );
			Vector3f W( w / theta * s );
			mData[0] = W[0];
			mData[1] = W[1];
			mData[2] = W[2];
			mData[3] = cosf( theta / 2.0f );
		} else {
			mData[0] = mData[1] = mData[2] = 0;
			mData[3] = 1.0f;
		}
	}

	/**
	* @brief Returns a vector rotated by this quaternion.
	*
	* Functionally equivalent to:  (rotationMatrix() * v)
	* or (q * Quaternion(0, v) * q.inverse()).
	*
	* @warning conjugate() is used instead of inverse() for better
	* performance, when this quaternion must be normalized.
	*/
	Vector3f rotatedVector( const Vector3f& v ) const {
		return ( ( ( *this ) * Quaternion( v, 0 ) ) * conjugate() ).complex();
	}

	/**
	* @brief Computes the quaternion that is equivalent to a given
	* euler angle rotation.
	* @param euler A 3-vector in order:  roll-pitch-yaw.
	*/
	void euler( const Vector3f& euler );

	/**
* @brief Computes the quaternion that is equivalent to a given
* euler angle rotation.
* @param euler A 3-vector in order:  roll-pitch-yaw.
*/
	void euler2( const Vector3f& euler );

	/** @brief Returns an equivalent euler angle representation of
	* this quaternion.
	* @return Euler angles
	*/
	Vector3f euler( void ) const;
    Vector3f euler2( void ) const;

	/**
	* @brief Computes a special representation that decouples the Z
	* rotation.
	*
	* The decoupled representation is two rotations, Qxy and Qz,
	* so that Q = Qxy * Qz.
	*/
	void decoupleZ( Quaternion* Qxy, Quaternion* Qz ) const {
		Vector3f ztt( 0.0f, 0.0f, 1.0f );
		Vector3f zbt = this->rotatedVector( ztt );
		Vector3f axis_xy = cross( ztt, zbt );
		float axis_norm = length( axis_xy );

		float axis_theta = acosf( clamp( zbt[2], -1.0f, +1.0f ) );
		if ( axis_norm > 0.00001f ) {
			axis_xy = axis_xy * ( axis_theta / axis_norm ); // limit is *1
		}

		Qxy->scaledAxis( axis_xy );
		*Qz = ( Qxy->conjugate() * ( *this ) );
	}

	/**
	* @brief Returns the quaternion slerped between this and q1 by fraction 0 <= t <= 1.
	*/
	Quaternion slerp( const Quaternion& q1, float t ) {
		return slerp( *this, q1, t );
	}

	/// Returns quaternion that is slerped by fraction 't' between q0 and q1.
	static Quaternion slerp( const Quaternion& q0, const Quaternion& q1, float t ) {
		float omega = acosf( clamp( q0.mData[0] * q1.mData[0] +
							 q0.mData[1] * q1.mData[1] +
							 q0.mData[2] * q1.mData[2] +
							 q0.mData[3] * q1.mData[3], -1.0f, 1.0f ) );
		if ( fabs( omega ) < 1e-10 ) {
			omega = 1e-10f;
		}
		float som = sinf( omega );
		float st0 = sinf( ( 1 - t ) * omega ) / som;
		float st1 = sinf( t * omega ) / som;

		return Quaternion( q0.mData[0] * st0 + q1.mData[0] * st1,
						   q0.mData[1] * st0 + q1.mData[1] * st1,
						   q0.mData[2] * st0 + q1.mData[2] * st1,
						   q0.mData[3] * st0 + q1.mData[3] * st1 );
	}

	/**
	* @brief Returns pointer to the internal array.
	*
	* Array is in order x,y,z,w.
	*/
	float* row( uint32_t i ) { return mData + i; }
	// Const version of the above.
	const float* row( uint32_t i ) const { return mData + i; }

	float operator[]( size_t i ) const {
		return mData[i];
	}

	float& operator[]( size_t i ) {
		return mData[i];
	}

};

/**
* @brief Global operator allowing left-multiply by scalar.
*/
Quaternion operator*( float s, const Quaternion& q );

