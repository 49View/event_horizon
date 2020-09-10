//
// Created by Dado on 2018-12-20.
//

#include "matrix_anim.h"
#include <core/math/anim.h>

MatrixAnim::MatrixAnim() {
    pos = std::make_shared<AnimType<Vector3f>>( V3fc::ZERO, "Pos" );
    rot = std::make_shared<AnimType<Quaternion>>( Quaternion{V3fc::ZERO}, "Rot");
    scale = std::make_shared<AnimType<Vector3f>>( V3fc::ONE, "Scale" );
}

MatrixAnim::MatrixAnim( const V3f& _pos ) {
    pos = std::make_shared<AnimType<Vector3f>>( _pos, "Pos" );
    rot = std::make_shared<AnimType<Quaternion>>( Quaternion{V3fc::ZERO}, "Rot");
    scale = std::make_shared<AnimType<Vector3f>>( V3fc::ONE, "Scale" );
}

MatrixAnim::MatrixAnim( const V3f& _pos, const Quaternion& _rot ) {
    pos = std::make_shared<AnimType<Vector3f>>( _pos, "Pos" );
    rot = std::make_shared<AnimType<Quaternion>>( _rot, "Rot");
    scale = std::make_shared<AnimType<Vector3f>>( V3fc::ONE, "Scale" );
}

MatrixAnim::MatrixAnim( const V3f& _pos, const Quaternion& _rot, const V3f& _scale ) {
    pos = std::make_shared<AnimType<Vector3f>>( _pos, "Pos" );
    rot = std::make_shared<AnimType<Quaternion>>( _rot, "Rot");
    scale = std::make_shared<AnimType<Vector3f>>( _scale, "Scale" );
}

MatrixAnim::MatrixAnim( const MA::Rotate& _rot ) {
    pos = std::make_shared<AnimType<Vector3f>>( V3fc::ZERO, "Pos" );
    rot = std::make_shared<AnimType<Quaternion>>( _rot(), "Rot");
    scale = std::make_shared<AnimType<Vector3f>>( V3fc::ONE, "Scale" );
}

MatrixAnim::MatrixAnim( const MA::Scale& _scale ) {
    pos = std::make_shared<AnimType<Vector3f>>( V3fc::ZERO, "Pos" );
    rot = std::make_shared<AnimType<Quaternion>>( Quaternion{V3fc::ZERO}, "Rot");
    scale = std::make_shared<AnimType<Vector3f>>( _scale(), "Scale" );
}

MatrixAnim MatrixAnim::clone( const MatrixAnim& _source ) const {
    MatrixAnim ret;

    ret.Pos(_source.Pos());
    ret.Rot(_source.Rot());
    ret.Scale(_source.Scale());

    return ret;
}

const Vector3f& MatrixAnim::Pos() const { return pos->value; }
void MatrixAnim::Pos( const Vector3f& val ) { pos->value = val; }

Quaternion MatrixAnim::Rot() const { return rot->value; }
void MatrixAnim::Rot( const Quaternion& val ) { rot->value = val; }

void MatrixAnim::RotX( const float& val ) { rot->value = Quaternion{V3fc::X_AXIS*val}; }
void MatrixAnim::RotY( const float& val ) { rot->value = Quaternion{V3fc::Y_AXIS*val}; }
void MatrixAnim::RotZ( const float& val ) { rot->value = Quaternion{V3fc::Z_AXIS*val}; }

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

Matrix4f MatrixAnim::transform( const Matrix4f& _sourceMatrix ) const {
    Matrix4f ret = _sourceMatrix;
    if ( !rot->isAnimating ) {
        if ( pos->isAnimating ) ret.translate( pos->value );
        if ( scale->isAnimating ) {
            ret.scale( scale->value );
        }
    } else {
        return _sourceMatrix * Matrix4f{ *this };
    }
    return ret;
}
