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

struct BBoxProjection3d {};
struct BBoxProjection2d {};

template <typename T = JMATH::AABB, typename D = BBoxProjection3d>
class Boxable {
public:
    Boxable() {
        bbox3d = std::make_shared<T>();
        bbox3dT = std::make_shared<T>();
    }

    inline const std::shared_ptr<T>& BBox3d() const { return bbox3d; }
    inline std::shared_ptr<T>& BBox3d() { return bbox3d; }
    inline std::shared_ptr<T>& BBox3dT() { return bbox3dT; }
    inline T BBox3dCopy() const { return *bbox3d.get(); }
    inline const AABB& BBoxTransform( Matrix4f _m ) {
        if ( std::is_same_v<D, BBoxProjection2d> ) {
            V3f ssTranslate = _m.getPosition3();
            ssTranslate.oneMinusY();
            _m.setTranslation( ssTranslate );
        }
        bbox3dT->set( bbox3d->gettransform( _m ) );
        return *bbox3dT;
    }
    inline const T* BBox3dPtr() const { return bbox3d.get(); }
    inline const T* BBox3dTPtr() const { return bbox3dT.get(); }
    inline void BBox3d( const Vector3f& bmix, const Vector3f& bmax ) { *bbox3d.get() = T{ bmix, bmax }; }
    inline void BBox3d( const T& _value ) { *bbox3d.get() = _value; }

protected:
    std::shared_ptr<T> bbox3d;
    std::shared_ptr<T> bbox3dT;
};

template <typename T = JMATH::AABB>
class BoxableRef {
public:
    inline const T& BBox3d() const { return bbox3d; }
    inline T& BBox3d() { return bbox3d; }
    inline T  BBox3dCopy() const { return bbox3d; }
    inline void BBox3d( const Vector3f& bmix, const Vector3f& bmax ) { bbox3d = T{ bmix, bmax }; }
    inline void BBox3d( const T& _value ) { bbox3d = _value; }

protected:
    T bbox3d = T::MINVALID();
};