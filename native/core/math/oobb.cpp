//
// Created by dado on 10/09/2020.
//

#include "oobb.h"

std::ostream& operator<<( std::ostream& os, const OOBB& oobb ) {
    os << "mQuat: " << oobb.mQuat << " mMinPoint: " << oobb.mMinPoint << " mMaxPoint: " << oobb.mMaxPoint;
    return os;
}

OOBB::OOBB( const Vector3f& minPoint, const Vector3f& maxPoint ) {
    mMinPoint = minPoint;
    mMaxPoint = maxPoint;
}

void OOBB::set( const OOBB& _aabb ) {
    *this = _aabb;
}

OOBB::OOBB( AABB& aabb ) {
    mMinPoint = aabb.minPoint();
    mMaxPoint = aabb.maxPoint();
}

OOBB& OOBB::operator=( AABB& aabb ) {
    mMinPoint = aabb.minPoint();
    mMaxPoint = aabb.maxPoint();
    return *this;
}

Vector3f OOBB::center() {
    return (mMaxPoint + mMinPoint)*0.5f;
}

Vector3f OOBB::extent() const {
    return mMaxPoint - mMinPoint;
}

OOBB OOBBc::MIDENTITY() {
    static OOBB a( Vector3f( 0.0f ), Vector3f( 1.0f ) );
    return a;
}

OOBB OOBBc::MIDENTITYCENTER() {
    static OOBB a( Vector3f( -0.5f ), Vector3f( 0.5f ) );
    return a;
}
