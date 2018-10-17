#pragma once

#include "vector3f.h"
#include "quaternion.h"
#include "../util.h"
#include "anim.h"

// SHARED CLASS: Any changes to this, should be integrated to /svn/shared, and a mail should be send to everyone



class MatrixAnim {
public:
	MatrixAnim() {
		pos = std::make_shared<AnimType<Vector3f>>( Vector3f::ZERO );
		rot = std::make_shared<AnimType<Quaternion>>();
		scale = std::make_shared<AnimType<Vector3f>>( Vector3f::ONE );
	}
	const Vector3f& Pos() const { return pos->value; }
	inline void Pos( const Vector3f& val ) { pos->value = val; }

	const Quaternion Rot() const { return rot->value; }
	inline void Rot( const Quaternion& val ) { rot->value = val; }

	inline void RotX( const float& val ) { rot->value = Quaternion{Vector3f::X_AXIS*val}; }
	inline void RotY( const float& val ) { rot->value = Quaternion{Vector3f::Y_AXIS*val}; }
	inline void RotZ( const float& val ) { rot->value = Quaternion{Vector3f::Z_AXIS*val}; }

	const Vector3f& Scale() const { return scale->value; }
	inline void Scale( const Vector3f& val ) { scale->value = val; }

	bool isAnimating() const {
		return pos->isAnimating() || rot->isAnimating() || scale->isAnimating();
	}

	void set( const Vector3f& _pos, const Vector3f& _angleAxis, const Vector3f& _scale ) {
		pos->value = _pos;
		scale->value = _scale;
		rot->value.euler( _angleAxis );
	}

	void set( const Vector3f& _pos, const Quaternion& _rot, const Vector3f& _scale ) {
		pos->value = _pos;
		scale->value = _scale;
		rot->value = _rot;
	}

public:
	// Expanded local transform Matrices
	std::shared_ptr<AnimType<Vector3f>>   pos;
	std::shared_ptr<AnimType<Quaternion>> rot;
	std::shared_ptr<AnimType<Vector3f>>   scale;
};
