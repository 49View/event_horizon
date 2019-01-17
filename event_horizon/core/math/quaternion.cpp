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
