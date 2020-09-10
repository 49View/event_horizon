#pragma once

#include <ostream>
#include "vector3f.h"
#include "matrix4f.h"
#include "aabb.h"
#include "quaternion.h"

// SHARED CLASS: Any changes to this, should be integrated to /svn/shared, and a mail should be send to everyone
 
class OOBB {
public:
    static constexpr bool IsSerializable() { return true; }

    friend std::ostream& operator<<( std::ostream& os, const OOBB& oobb );

    OOBB() = default;
    OOBB(const OOBB &) = default;

    OOBB( const Vector3f& minPoint, const Vector3f& maxPoint );
//    explicit OOBB( const std::vector<Vector3f> points ) {
//        for ( auto& p : points ) expand(p);
//    }
    void set( const OOBB& _aabb );
    
    explicit OOBB(AABB & aabb);

    OOBB & operator=(AABB & aabb);

    inline Vector3f center();

    [[nodiscard]] inline Vector3f extent() const;

//    inline PREC maxExtent() const{
//        return (mMaxPoint - mMinPoint).maxCoeff();
//    }
//
//    inline PREC maxExtent(Vector3f::Index & i) const{
//        return (mMaxPoint - mMinPoint).maxCoeff(&i);
//    }

//    inline bool isEmpty() const {
//        return mMaxPoint(0) <= mMinPoint(0) || mMaxPoint(1) <= mMinPoint(1) || mMaxPoint(2) <= mMinPoint(2);
//    }

//    template<typename Derived, bool coordinateSystemIsI = true>
//    inline bool overlaps(const MatrixBase<Derived> &p) const {
//        if(coordinateSystemIsI){
//            // p is in I frame
//            Vector3f t = mQuat.inverse() * p; // A_IK^T * I_p
//            return ((t.array() >= mMinPoint.array()) && (t.array() <= mMaxPoint.array())).all();
//        }else{
//            // p is in K Frame!!
//            return ((p.array() >= mMinPoint.array()) && (p.array() <= mMaxPoint.array())).all();
//        }
//    }
//
//    void expandToMinExtentRelative(PREC p = 0.1, PREC defaultExtent = 0.1, PREC eps = 1e-10);
//
//    void expandToMinExtentAbsolute(PREC minExtent);
//
//    inline void expand(Vector3f d) {
//        ApproxMVBB_ASSERTMSG(d(0)>=0 && d(1)>=0 && d(2)>=0,"d>=0")
//        mMinPoint -= d;
//        mMaxPoint += d;
//    }
//
//    inline PREC volume() const {
//        Vector3f d = mMaxPoint- mMinPoint;
//        return d(0) * d(1) * d(2);
//    }
//
//    inline Vector3f getDirection(unsigned int i) const{
////        ApproxMVBB_ASSERTMSG(i<3,"Index wrong: " << i)
//        Vector3f d = Vector3f::ZERO;
//        d[i] = 1.0;
//        return mQuat * d; // A_IK* d;
//    }
//
//    template<bool coordinateSystemIsI = true>
//    inline Vector3fList getCornerPoints() const{
//        Vector3fList points(8);
//        Vector3f ex = extent();
//        points[0] =   mMinPoint /*+ Vector3f(0,0,0) * extent*/ ;
//        points[1] =   mMinPoint + (Vector3f(1.0, 0.0, 0.0) * ex).matrix();
//        points[2] =   mMinPoint + (Vector3f(0.0, 1.0, 0.0) * ex).matrix();
//        points[3] =   mMinPoint + (Vector3f(1.0, 1.0, 0.0) * ex).matrix();
//
//        points[4] =   mMinPoint + (Vector3f(0.0, 0.0, 1.0) * ex).matrix();
//        points[5] =   mMinPoint + (Vector3f(1.0, 0.0, 1.0) * ex).matrix();
//        points[6] =   mMinPoint + (Vector3f(0.0, 1.0, 1.0) * ex).matrix();
//        points[7] =   mMaxPoint /*+ Vector3f(1,1,1) * extent */;
//
//        if(coordinateSystemIsI){
//            for(auto & p : points){
//                p = (mQuat * p).eval(); //    I_p = A_IK * K_p
//            }
//        }
//
//        return points;
//    }

    Quaternion mQuat{};
    Vector3f mMinPoint;
    Vector3f mMaxPoint;
};

namespace OOBBc {
    static const OOBB IDENTITY;
    static const OOBB INVALID;
    static const OOBB ZERO;

    OOBB MIDENTITY();

    OOBB MIDENTITYCENTER();
}

