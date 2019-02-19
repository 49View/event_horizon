//
// Created by Dado on 2019-02-18.
//

#pragma once

#include <core/math/aabb.h>

struct EmptyBox {
    static constexpr bool IsSerializable() { return false; }

    EmptyBox() = default;
    EmptyBox( [[maybe_unused]] const Vector3f& bmix, [[maybe_unused]] const Vector3f& bmax ) {}
    static EmptyBox MINVALID() { return EmptyBox{}; };
};

template <typename T = JMATH::AABB>
class Boxable {
public:
    inline const T& BBox3d() const { return bbox3d; }
    inline T& BBox3d() { return bbox3d; }
    inline void BBox3d( const Vector3f& bmix, const Vector3f& bmax ) { bbox3d = T{ bmix, bmax }; }
    inline void BBox3d( const T& _value ) { bbox3d = _value; }

protected:
    T bbox3d = T::MINVALID();
};