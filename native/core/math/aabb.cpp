
#include "aabb.h"
#include <core/math/quaternion.h>

AABB& AABB::MIDENTITY() {
    static AABB a(Vector3f(0.0f), Vector3f(1.0f));
    return a;
}

AABB& AABB::MIDENTITYCENTER() {
    static AABB a(Vector3f(-0.5f), Vector3f(0.5f));
    return a;
}

AABB& AABB::ZERO() {
    static AABB a(V3f{ 0.0f }, V3f{ 0.0f });
    return a;
}

AABB& AABB::MINVALID() {
    static AABB a(V3f(std::numeric_limits<float>::max()), V3f(std::numeric_limits<float>::lowest()));
    return a;
}

// *********************************************************************************************************************
// Constructors and operators
// *********************************************************************************************************************

AABB::AABB( const Vector3f& minPoint, const Vector3f& maxPoint ) {
    mMinPoint = minPoint;
    mMaxPoint = maxPoint;
}

AABB::AABB( const Vector3f& minPoint, const Vector3f& maxPoint, [[maybe_unused]] bool bt ) {
    set(minPoint, maxPoint);
}

AABB::AABB( const std::vector<Vector3f>& points ) {
    *this = AABB::MINVALID();
    for ( const auto& p : points ) expand(p);
}

namespace JMATH {
    std::ostream& operator<<( std::ostream& os, const JMATH::AABB& f ) {
        os << "Min: " << f.mMinPoint << " Max: " << f.mMaxPoint;
        return os;
    }
}

bool AABB::operator==( const AABB& rhs ) const {
    if ( minPoint() != rhs.minPoint() || maxPoint() != rhs.maxPoint() ) return false;
    return true;
}

bool AABB::operator!=( const AABB& rhs ) const {
    if ( minPoint() != rhs.minPoint() || maxPoint() != rhs.maxPoint() ) return true;
    return false;
}

AABB AABB::operator-( const Vector3f& rhs ) const {
    return AABB(mMinPoint - rhs, mMaxPoint - rhs);
}

AABB AABB::operator+( const Vector3f& rhs ) const {
    return AABB(mMinPoint + rhs, mMaxPoint + rhs);
}

void AABB::operator-=( const Vector3f& rhs ) {
    mMinPoint -= rhs;
    mMaxPoint -= rhs;
}

void AABB::operator+=( const Vector3f& rhs ) {
    mMinPoint += rhs;
    mMaxPoint += rhs;
}

AABB AABB::operator*( const Vector3f& rhs ) const {
    AABB r = *this;
    r.scaleX(rhs.x());
    r.scaleY(rhs.y());
    return r;
}


void JMATH::AABB::merge( const AABB& val ) {
    expandMax(val.mMaxPoint);
    expandMin(val.mMinPoint);
}

JMATH::Rect2f JMATH::AABB::demoteTo2d() const {
    Vector3f diff = mMaxPoint - mMinPoint;
    if ( diff.leastDominantElement() == 0 ) {
        return Rect2f(mMinPoint.yz(), mMaxPoint.yz());
    }
    if ( diff.leastDominantElement() == 1 ) {
        return Rect2f(mMinPoint.xz(), mMaxPoint.xz());
    }
    return Rect2f(mMinPoint.xy(), mMaxPoint.xy());
}

JMATH::Rect2f JMATH::AABB::topDown() const {
    return Rect2f(mMinPoint.xz(), mMaxPoint.xz());
}

JMATH::Rect2f JMATH::AABB::front() const {
    return Rect2f(mMinPoint.xy(), mMaxPoint.xy());
}

bool AABB::intersectLine( const Vector3f& linePos, const Vector3f& lineDir, float& tNear, float& tFar ) const {
    //assume2(lineDir.IsNormalized(), lineDir, lineDir.LengthSq());
    //assume2(tNear <= tFar && "AABB::IntersectLineAABB: User gave a degenerate line as input for the intersection test!", tNear, tFar);
    // The user should have inputted values for tNear and tFar to specify the desired sub-range [tNear, tFar] of the line
    // for this intersection test.
    // For a Line-AABB test, pass in
    //    tNear = -FLOAT_INF;
    //    tFar = FLOAT_INF;
    // For a Ray-AABB test, pass in
    //    tNear = 0.f;
    //    tFar = FLOAT_INF;
    // For a LineSegment-AABB test, pass in
    //    tNear = 0.f;
    //    tFar = LineSegment.Length();

    // Test each cardinal plane (X, Y and Z) in turn.
    if ( !isScalarEqual(lineDir.x(), 0.f) ) {
        float recipDir = 1.0f / lineDir.x(); //RecipFast(lineDir.x);
        float t1 = ( mMinPoint.x() - linePos.x() ) * recipDir;
        float t2 = ( mMaxPoint.x() - linePos.x() ) * recipDir;

        // tNear tracks distance to intersect (enter) the AABB.
        // tFar tracks the distance to exit the AABB.
        if ( t1 < t2 ) {
            tNear = std::max(t1, tNear);
            tFar = std::min(t2, tFar);
        } else { // Swap t1 and t2.
            tNear = std::max(t2, tNear);
            tFar = std::min(t1, tFar);
        }

        if ( tNear > tFar )
            return false;//false; // Box is missed since we "exit" before entering it.
    } else if ( linePos.x() < mMinPoint.x() || linePos.x() > mMaxPoint.x() )
        return false; // The ray can't possibly enter the box, abort.

    if ( !isScalarEqual(lineDir.y(), 0.f) ) {
        float recipDir = 1.0f / lineDir.y();
        float t1 = ( mMinPoint.y() - linePos.y() ) * recipDir;
        float t2 = ( mMaxPoint.y() - linePos.y() ) * recipDir;

        if ( t1 < t2 ) {
            tNear = std::max(t1, tNear);
            tFar = std::min(t2, tFar);
        } else {// Swap t1 and t2.
            tNear = std::max(t2, tNear);
            tFar = std::min(t1, tFar);
        }
        if ( tNear > tFar )
            return false; // Box is missed since we "exit" before entering it.
    } else if ( linePos.y() < mMinPoint.y() || linePos.y() > mMaxPoint.y() )
        return false; // The ray can't possibly enter the box, abort.

    if ( !isScalarEqual(lineDir.z(), 0.f) ) // ray is parallel to plane in question
    {
        float recipDir = 1.0f / lineDir.z();
        float t1 = ( mMinPoint.z() - linePos.z() ) * recipDir;
        float t2 = ( mMaxPoint.z() - linePos.z() ) * recipDir;

        if ( t1 < t2 ) {
            tNear = std::max(t1, tNear);
            tFar = std::min(t2, tFar);
        } else { // Swap t1 and t2.
            tNear = std::max(t2, tNear);
            tFar = std::min(t1, tFar);
        }
    } else if ( linePos.z() < mMinPoint.z() || linePos.z() > mMaxPoint.z() )
        return false; // The ray can't possibly enter the box, abort.

    return tNear <= tFar;
}

std::vector<Vector3f> AABB::topDownOutline( CompositeWrapping _wrap ) const {
    std::vector<Vector3f> ret;
    ret.emplace_back(mMinPoint.xy());
    ret.push_back(Vector2f{ mMaxPoint.x(), mMinPoint.y() });
    ret.emplace_back(mMaxPoint.xy());
    ret.push_back(Vector2f{ mMinPoint.x(), mMaxPoint.y() });
    if ( _wrap == CompositeWrapping::Wrap ) {
        ret.emplace_back(mMinPoint.xy());
    }
    return ret;
}

[[maybe_unused]] bool AABB::containsXZ( const V2f& _point ) const {
    return _point.x() > minPoint().x() && _point.x() < maxPoint().x() &&
           _point.y() > minPoint().z() && _point.y() < maxPoint().z();
}

bool AABB::containsXY( const V2f& _point ) const {
    return _point.x() > minPoint().x() && _point.x() < maxPoint().x() &&
           _point.y() > minPoint().y() && _point.y() < maxPoint().y();
}

void AABB::identity() {
    *this = MIDENTITY();
}

void AABB::identityCentered() {
    *this = MIDENTITYCENTER();
}

bool AABB::containsX( float _value ) const {
    return isbetween(_value, mMinPoint.x(), mMaxPoint.x());
}

bool AABB::containsY( float _value ) const {
    return isbetween(_value, mMinPoint.y(), mMaxPoint.y());
}

bool AABB::containsZ( float _value ) const {
    return isbetween(_value, mMinPoint.z(), mMaxPoint.z());
}

Vector3f AABB::centreFront() const {
    V3f ret = calcCentre();
    ret.setX(mMaxPoint.x());
    return ret;
}

Vector3f AABB::centreBack() const {
    V3f ret = calcCentre();
    ret.setX(mMinPoint.x());
    return ret;
}

Vector3f AABB::centreTop() const {
    V3f ret = calcCentre();
    ret.setY(mMaxPoint.y());
    return ret;
}

Vector3f AABB::centreBottom() const {
    V3f ret = calcCentre();
    ret.setY(mMinPoint.y());
    return ret;
}

Vector3f AABB::centreLeft() const {
    V3f ret = calcCentre();
    ret.setZ(mMaxPoint.z());
    return ret;
}

Vector3f AABB::centreRight() const {
    V3f ret = calcCentre();
    ret.setZ(mMinPoint.z());
    return ret;
}

Vector3f AABB::bottomFront() const {
    return Vector3f{ calcCentre().xy(), minPoint().z() } + V3fc::Y_AXIS * calcHeight() * 0.5f;
}

Vector3fList AABB::bottomFace() const {
    Vector3fList ret;
    ret.push_back(mMinPoint);
    ret.push_back(V3f{ mMinPoint.x(), mMinPoint.y(), mMaxPoint.z() });
    ret.push_back(V3f{ mMaxPoint.x(), mMinPoint.y(), mMaxPoint.z() });
    ret.push_back(V3f{ mMaxPoint.x(), mMinPoint.y(), mMinPoint.z() });
    return ret;
}

void AABB::calc( const Rect2f& bbox, float minHeight, float maxHeight, const Matrix4f& tMat ) {
    V3f b3dMin = XZY::C(bbox.topLeft(), minHeight);
    V3f b3dMax = XZY::C(bbox.bottomRight(), maxHeight);
//    V3f b3dMin = V3f(bbox.topLeft(), minHeight);
//    V3f b3dMax = V3f(bbox.bottomRight(), maxHeight);

    b3dMin = tMat.transform(b3dMin);
    b3dMax = tMat.transform(b3dMax);

    set(b3dMin, b3dMax);
}

void AABB::set( const AABB& _aabb ) {
    *this = _aabb;
}

bool AABB::isValid() const {
    return *this != AABB::MINVALID();
}

float *AABB::rawPtr() {
    return reinterpret_cast<float *>( &mMinPoint[0] );
}

void AABB::set( const Vector3f& minPoint, const Vector3f& maxPoint ) {
    *this = AABB::MINVALID();
    expand(minPoint);
    expand(maxPoint);
}

void AABB::calc( const std::initializer_list<Vector3f>& iList, const Matrix4f& tMat ) {
    std::vector<Vector3f> vList;
    for ( auto& v : iList ) vList.push_back(v);
    calc(vList, tMat);
}

void AABB::calc( const std::vector<Vector3f>& vList, const Matrix4f& tMat ) {
    Vector3f b3dMin = V3fc::HUGE_VALUE_POS;
    Vector3f b3dMax = V3fc::HUGE_VALUE_NEG;

    for ( auto& v : vList ) {
        b3dMin = min(b3dMin, v);
        b3dMax = max(b3dMax, v);
    }

    b3dMin = tMat.transform(b3dMin);
    b3dMax = tMat.transform(b3dMax);

    set(b3dMin, b3dMax);
}

void AABB::setCenterAndSize( const Vector3f& _center, const Vector3f& _size ) {
    mMinPoint = _center;
    mMaxPoint = _center;
    expand(_center + _size * 0.5f);
    expand(_center + _size * -0.5f);
}

void AABB::expand( const Vector3f& p ) {
    if ( p.x() < mMinPoint.x() ) mMinPoint.setX(p.x());
    if ( p.x() > mMaxPoint.x() ) mMaxPoint.setX(p.x());

    if ( p.y() < mMinPoint.y() ) mMinPoint.setY(p.y());
    if ( p.y() > mMaxPoint.y() ) mMaxPoint.setY(p.y());

    if ( p.z() < mMinPoint.z() ) mMinPoint.setZ(p.z());
    if ( p.z() > mMaxPoint.z() ) mMaxPoint.setZ(p.z());
}
void AABB::expandMin( const Vector3f& p ) {
    if ( p.x() < mMinPoint.x() ) mMinPoint.setX(p.x());
    if ( p.y() < mMinPoint.y() ) mMinPoint.setY(p.y());
    if ( p.z() < mMinPoint.z() ) mMinPoint.setZ(p.z());
}
void AABB::expandMax( const Vector3f& p ) {
    if ( p.x() > mMaxPoint.x() ) mMaxPoint.setX(p.x());
    if ( p.y() > mMaxPoint.y() ) mMaxPoint.setY(p.y());
    if ( p.z() > mMaxPoint.z() ) mMaxPoint.setZ(p.z());
}
int AABB::leastDominantAxis() const {
    Vector3f diff = mMaxPoint - mMinPoint;
    return diff.leastDominantElement();
}
float AABB::calcWidth() const {
    return fabs(mMaxPoint.x() - mMinPoint.x());
}
float AABB::calcHeight() const {
    return fabs(mMaxPoint.y() - mMinPoint.y());
}
float AABB::calcDepth() const {
    return fabs(mMaxPoint.z() - mMinPoint.z());
}
float AABB::calcDiameter() const {
    Vector3f s = size();
    return s[s.dominantElement()];
}
Vector3f AABB::calcCentre() const {
    Vector3f centre = ( mMinPoint + mMaxPoint ) * 0.5f;
    return centre;
}
float AABB::calcRadius() const {
    return calcDiameter() * 0.5f;
}
float AABB::pivotHeight() const {
    return mMaxPoint.y();
}
float AABB::pivotWidth() const {
    return mMaxPoint.x();
}

const Vector3f& AABB::minPoint() const { return mMinPoint; }
const Vector3f& AABB::maxPoint() const { return mMaxPoint; }
void AABB::setMinPoint( const Vector3f& val ) { mMinPoint = val; }
void AABB::setMaxPoint( const Vector3f& val ) { mMaxPoint = val; }
Vector3f AABB::size() const { return mMaxPoint - mMinPoint; }
Vector3f AABB::centre() const { return calcCentre(); }

// *********************************************************************************************************************
// Transformations
// *********************************************************************************************************************

void AABB::translate( const Vector3f& offset ) {
    mMinPoint += offset;
    mMaxPoint += offset;
}

JMATH::AABB JMATH::AABB::rotate( const Quaternion& axisAngle ) const {
    Matrix3f mat = axisAngle.rotationMatrix3();

    Vector3f mi = mat * ( mMinPoint - centre() );
    Vector3f ma = mat * ( mMaxPoint - centre() );

    mi += centre();
    ma += centre();

    AABB ret;
    ret.set(mi, ma);

    return ret;
}

void AABB::scaleX( float x ) {
    mMinPoint.setX(mMinPoint.x() * x);
    mMaxPoint.setX(mMaxPoint.x() * x);
}

void AABB::scaleY( float x ) {
    mMinPoint.setY(mMinPoint.y() * x);
    mMaxPoint.setY(mMaxPoint.y() * x);
}

void AABB::scaleZ( float x ) {
    mMinPoint.setZ(mMinPoint.z() * x);
    mMaxPoint.setZ(mMaxPoint.z() * x);
}

void AABB::scale( float x ) {
    mMinPoint *= 1.0f / x;
    mMaxPoint *= x;
}

void AABB::transform( const Matrix4f& mat ) {
    Vector3f mi = mat.transform(mMinPoint);
    Vector3f ma = mat.transform(mMaxPoint);

    set(mi, ma);
}

AABB AABB::getTransform( const Matrix4f& mat ) const {
    Vector3f mi = mat.transform(mMinPoint);
    Vector3f ma = mat.transform(mMaxPoint);

    return AABB(mi, ma, true);
}
