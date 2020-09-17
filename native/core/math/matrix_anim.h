#pragma once

#include "vector3f.h"
#include "quaternion.h"
#include <core/math/anim_type.hpp>

namespace MA {

    struct Scale {
        template<typename ...Args>
        explicit Scale( Args&& ... args ) : data(std::forward<Args>( args )...) {}
        V3f operator()() const noexcept {
            return data;
        }
        V3f data;
    };

    struct Rotate {
        template<typename ...Args>
        explicit Rotate( Args&& ... args ) : data(std::forward<Args>( args )...) {}
        Quaternion operator()() const noexcept {
            return data;
        }
        Quaternion data;
    };

}

class MatrixAnim {
public:
	MatrixAnim();
	explicit MatrixAnim( const V3f& _pos );
	MatrixAnim( const V3f& _pos, const Quaternion& _rot );
	MatrixAnim( const V3f& _pos, const Quaternion& _rot, const V3f& _scale );
	explicit MatrixAnim( const MA::Rotate& _rot );
	explicit MatrixAnim( const MA::Scale& _scale );

	[[nodiscard]] const Vector3f& Pos() const;
	void Pos( const Vector3f& val );

	[[nodiscard]] Quaternion Rot() const;
	[[nodiscard]] Vector3f Euler() const;
	void Rot( const Quaternion& val );

	void RotX( const float& val );
	void RotY( const float& val );
	void RotZ( const float& val );

	[[nodiscard]] const Vector3f& Scale() const;
	void Scale( const Vector3f& val );

	void set( const Vector3f& _pos, const Vector3f& _angleAxis, const Vector3f& _scale );
	void set( const Vector3f& _pos, const Vector4f& _angleAxis, const Vector3f& _scale );
	void set( const Vector3f& _pos, const Quaternion& _rot, const Vector3f& _scale );

    [[nodiscard]] MatrixAnim clone() const;
	[[nodiscard]] bool isAnimating() const;
	[[nodiscard]] Matrix4f transform( const Matrix4f& _sourceMatrix ) const;

public:
	// Expanded local transform Matrices
	AnimValue<Vector3f>   pos;
	AnimValue<Quaternion> rot;
	AnimValue<Vector3f>   scale;
};
