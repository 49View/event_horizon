//
// Created by Dado on 2018-12-20.
//

#include "matrix_anim.h"
#include <core/math/anim.h>

MatrixAnim::MatrixAnim() {
    pos = std::make_shared<AnimType<Vector3f>>( Vector3f::ZERO, "Pos" );
    rot = std::make_shared<AnimType<Quaternion>>( Quaternion{Vector3f::ZERO}, "Rot");
    scale = std::make_shared<AnimType<Vector3f>>( Vector3f::ONE, "Scale" );
}

MatrixAnim MatrixAnim::clone( const MatrixAnim& _source ) {
    MatrixAnim ret;

    ret.Pos(_source.Pos());
    ret.Rot(_source.Rot());
    ret.Scale(_source.Scale());

    return ret;
}

const Vector3f& MatrixAnim::Pos() const { return pos->value; }
void MatrixAnim::Pos( const Vector3f& val ) { pos->value = val; }

const Quaternion MatrixAnim::Rot() const { return rot->value; }
void MatrixAnim::Rot( const Quaternion& val ) { rot->value = val; }

void MatrixAnim::RotX( const float& val ) { rot->value = Quaternion{Vector3f::X_AXIS*val}; }
void MatrixAnim::RotY( const float& val ) { rot->value = Quaternion{Vector3f::Y_AXIS*val}; }
void MatrixAnim::RotZ( const float& val ) { rot->value = Quaternion{Vector3f::Z_AXIS*val}; }

const Vector3f& MatrixAnim::Scale() const { return scale->value; }
void MatrixAnim::Scale( const Vector3f& val ) { scale->value = val; }

void MatrixAnim::set( const Vector3f& _pos, const Vector4f& _angleAxis, const Vector3f& _scale ) {
    pos->value = _pos;
    scale->value = _scale;
    rot->value.fromAxis( _angleAxis );
}

void MatrixAnim::set( const Vector3f& _pos, const Vector3f& _angleAxis, const Vector3f& _scale ) {
    pos->value = _pos;
    scale->value = _scale;
    rot->value.euler2( _angleAxis );
}

void MatrixAnim::set( const Vector3f& _pos, const Quaternion& _rot, const Vector3f& _scale ) {
    pos->value = _pos;
    scale->value = _scale;
    rot->value = _rot;
}

bool MatrixAnim::isAnimating() const {
    return pos->isAnimating || rot->isAnimating || scale->isAnimating;
}

Vector3f MatrixAnim::Euler() const {
    return rot->value.euler2();
}

