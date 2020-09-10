//
// Created by Dado on 2019-01-17.
//

#include "quaternion.h"

void Quaternion::euler( const Vector3f& euler ) {

    V3f eulerNorm = euler.fmod(M_PI_2);
    float cr = cosf( eulerNorm[0] * 0.5f );
    float sr = sinf( eulerNorm[0] * 0.5f );

    float cy = cosf( eulerNorm[2] * 0.5f );
    float sy = sinf( eulerNorm[2] * 0.5f );

    float cp = cosf( eulerNorm[1] * 0.5f );
    float sp = sinf( eulerNorm[1] * 0.5f );

    mData[3] = cy * cr * cp + sy * sr * sp;
    mData[0] = cy * sr * cp - sy * cr * sp;
    mData[1] = cy * cr * sp + sy * sr * cp;
    mData[2] = sy * cr * cp - cy * sr * sp;
}

void Quaternion::euler2( const Vector3f& euler ) {

    V3f eulerNorm = euler.fmod(M_PI_2);
    float cr = cosf( eulerNorm[0] * 0.5f );
    float sr = sinf( eulerNorm[0] * 0.5f );

    float cy = cosf( eulerNorm[2] * 0.5f );
    float sy = sinf( eulerNorm[2] * 0.5f );

    float cp = cosf( eulerNorm[1] * 0.5f );
    float sp = sinf( eulerNorm[1] * 0.5f );

    mData[3] = cy * cr * cp + sy * sr * sp;
    mData[0] = cy * sr * cp - sy * cr * sp;
    mData[1] = cy * cr * sp + sy * sr * cp;
    mData[2] = sy * cr * cp - cy * sr * sp;
}

Vector3f Quaternion::euler( void ) const {
    double roll = 0.0f;
    double pitch = 0.0f;
    double yaw = 0.0f;

    // roll (x-axis rotation)
    double sinr = +2.0 * (w() * x() + y() * z());
    double cosr = +1.0 - 2.0 * (x() * x() + y() * y());
    roll = atan2(sinr, cosr);

    // pitch (y-axis rotation)
    double sinp = +2.0 * (w() * y() - z() * x());
    if (fabs(sinp) >= 1)
        pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    else
        pitch = asin(sinp);

    // yaw (z-axis rotation)
    double siny = +2.0 * (w() * z() + x() * y());
    double cosy = +1.0 - 2.0 * (y() * y() + z() * z());
    yaw = atan2(siny, cosy);

    return {roll, yaw, pitch};
}

Vector3f Quaternion::euler2( void ) const {

    double roll = 0.0f;
    double pitch = 0.0f;
    double yaw = 0.0f;

    // roll (x-axis rotation)
    double sinr = +2.0 * (w() * x() + y() * z());
    double cosr = +1.0 - 2.0 * (x() * x() + y() * y());
    roll = atan2(sinr, cosr);

    // pitch (y-axis rotation)
    double sinp = +2.0 * (w() * y() - z() * x());
    if (fabs(sinp) >= 1)
        pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    else
        pitch = asin(sinp);

    // yaw (z-axis rotation)
    double siny = +2.0 * (w() * z() + x() * y());
    double cosy = +1.0 - 2.0 * (y() * y() + z() * z());
    yaw = atan2(siny, cosy);

    return {roll, pitch, yaw};
}

void Quaternion::fromAxis( const Vector4f& w ) {
    float theta = w.w();
    float s = sinf( theta / 2.0f );
    Vector3f W( w.xyz() * s );
    mData[0] = W[0];
    mData[1] = W[1];
    mData[2] = W[2];
    mData[3] = cosf( theta / 2.0f );
}

Matrix4f Quaternion::rotationMatrix() const {
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

Matrix4f Quaternion::matrix4( const V3f& _pos ) const {
    float n = norm();
    float qx = mData[0] / n;
    float qy = mData[1] / n;
    float qz = mData[2] / n;
    float qw = mData[3] / n;

    Vector4f r0( 1 - 2 * qy*qy - 2 * qz*qz, 2 * qx*qy - 2 * qz*qw    , 2 * qx*qz + 2 * qy*qw, 0.0f);
    Vector4f r1( 2 * qx*qy + 2 * qz*qw    , 1 - 2 * qx*qx - 2 * qz*qz, 2 * qy*qz - 2 * qx*qw, 0.0f);
    Vector4f r2( 2 * qx*qz - 2 * qy*qw    , 2 * qy*qz + 2 * qx*qw    , 1 - 2 * qx*qx - 2 * qy*qy, 0.0f);
    Vector4f r3( _pos, 1.0f );

    return Matrix4f( r0, r1, r2, r3 );
}

Matrix3f Quaternion::rotationMatrix3() const {
    float n = norm();
    float qx = mData[0] / n;
    float qy = mData[1] / n;
    float qz = mData[2] / n;
    float qw = mData[3] / n;

    Vector3f r0( 1 - 2 * qy*qy - 2 * qz*qz, 2 * qx*qy - 2 * qz*qw    , 2 * qx*qz + 2 * qy*qw);
    Vector3f r1( 2 * qx*qy + 2 * qz*qw    , 1 - 2 * qx*qx - 2 * qz*qz, 2 * qy*qz - 2 * qx*qw);
    Vector3f r2( 2 * qx*qz - 2 * qy*qw    , 2 * qy*qz + 2 * qx*qw    , 1 - 2 * qx*qx - 2 * qy*qy);

    return Matrix3f( r0, r1, r2 );
}

Matrix4f Quaternion::rotationMatrixNotNormalised() const {
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

void Quaternion::rotateFromAxis( const Vector3f& fromAxis, const Vector3f& rotateToAxis ) {

    // both vectors needs to be normalised
    V4f q{cross(fromAxis, rotateToAxis), 1.0f + dot(fromAxis, rotateToAxis)};
    for ( auto t = 0u; t < 4; t++ ) mData[t] = q[t];
    normalise();
}

Quaternion quatFromAxis( const Vector4f& w ) {
    float theta = w.w();
    float s = sinf( theta / 2.0f );
    Vector3f W( w.xyz() * s );
    return Quaternion{ V4f{W[0],W[1],W[2],cosf( theta / 2.0f )} };
}

Quaternion quatCompose( const Vector3f& a ) {
    Quaternion qz( a.z(), V3fc::Z_AXIS );
    Quaternion qy( a.y(), V3fc::Y_AXIS );
    Quaternion qx( a.x(), V3fc::X_AXIS );

    return  qx * qy * qz;
}
