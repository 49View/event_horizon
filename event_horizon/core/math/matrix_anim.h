#pragma once

#include "vector3f.h"
#include "quaternion.h"

class MatrixAnim {
public:
	MatrixAnim();
	const Vector3f& Pos() const;
	inline void Pos( const Vector3f& val );

	const Quaternion Rot() const;
	inline void Rot( const Quaternion& val );

	inline void RotX( const float& val );
	inline void RotY( const float& val );
	inline void RotZ( const float& val );

	const Vector3f& Scale() const;
	inline void Scale( const Vector3f& val );

	void set( const Vector3f& _pos, const Vector3f& _angleAxis, const Vector3f& _scale );
	void set( const Vector3f& _pos, const Quaternion& _rot, const Vector3f& _scale );

public:
	// Expanded local transform Matrices
	AnimValue<Vector3f>   pos;
	AnimValue<Quaternion> rot;
	AnimValue<Vector3f>   scale;
};
