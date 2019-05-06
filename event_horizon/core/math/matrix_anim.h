#pragma once

#include "vector3f.h"
#include "quaternion.h"
#include <core/math/anim_type.hpp>

class MatrixAnim {
public:
	MatrixAnim();
	const Vector3f& Pos() const;
	void Pos( const Vector3f& val );

	const Quaternion Rot() const;
	Vector3f Euler() const;
	void Rot( const Quaternion& val );

	void RotX( const float& val );
	void RotY( const float& val );
	void RotZ( const float& val );

	const Vector3f& Scale() const;
	void Scale( const Vector3f& val );

	void set( const Vector3f& _pos, const Vector3f& _angleAxis, const Vector3f& _scale );
	void set( const Vector3f& _pos, const Vector4f& _angleAxis, const Vector3f& _scale );
	void set( const Vector3f& _pos, const Quaternion& _rot, const Vector3f& _scale );

    MatrixAnim clone( const MatrixAnim& _source);

	bool isAnimating() const;
public:
	// Expanded local transform Matrices
	AnimValue<Vector3f>   pos;
	AnimValue<Quaternion> rot;
	AnimValue<Vector3f>   scale;
};
