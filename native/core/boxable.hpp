//
// Created by Dado on 2019-02-18.
//

#pragma once

#include <core/math/aabb.h>

struct EmptyBox {
    static constexpr bool IsSerializable() { return false; }

    EmptyBox() = default;
    static EmptyBox MINVALID() { return EmptyBox{}; };
};

class Boxable {
public:
    [[nodiscard]] const AABB& BBox3d() const { return bbox3d; }
    [[nodiscard]] const AABB* BBox3dPtr() const { return &bbox3d; }
    AABB& BBox3d() { return bbox3d; }
    AABB& BBox3dT() { return bbox3dT; }
    [[maybe_unused]] [[nodiscard]] AABB BBox3dCopy() const { return bbox3d; }
    [[maybe_unused]] const AABB& BBoxTransform( Matrix4f _m ) {
//        if ( std::is_same_v<D, BBoxProjection2d> ) {
//            V3f ssTranslate = _m.getPosition3();
//            ssTranslate.oneMinusY();
//            _m.setTranslation( ssTranslate );
//        }
        bbox3dT.set( bbox3d.getTransform( _m ) );
        return bbox3dT;
    }
    inline void BBox3d( const Vector3f& bMin, const Vector3f& bMax ) { bbox3d = AABB{ bMin, bMax }; }
    inline void BBox3d( const AABB& _value ) { bbox3d = _value; }

protected:
    JMATH::AABB bbox3d{AABB::INVALID};
    JMATH::AABB bbox3dT{AABB::INVALID};
    Rect2f bbox{Rect2f::INVALID};
    Rect2f bboxT{Rect2f::INVALID};
};

template <typename T = JMATH::AABB>
class BoxableRef {
public:
    inline const T& BBox3d() const { return bbox3d; }
    inline T& BBox3d() { return bbox3d; }
    [[maybe_unused]] inline T  BBox3dCopy() const { return bbox3d; }
    inline void BBox3d( const Vector3f& bMin, const Vector3f& bMax ) { bbox3d = T{ bMin, bMax }; }
    inline void BBox3d( const T& _value ) { bbox3d = _value; }

protected:
    T bbox3d = T::MINVALID();
};