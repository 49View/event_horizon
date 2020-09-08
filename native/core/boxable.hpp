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
    [[nodiscard]] const JMATH::Rect2f& BBox() const;
    [[nodiscard]] const AABB& BBox3d() const;
    [[nodiscard]] const AABB* BBox3dPtr() const;
    AABB& BBox3d();
    AABB& BBox3dT();
    [[maybe_unused]] [[nodiscard]] AABB BBox3dCopy() const;
    [[maybe_unused]] const AABB& BBoxTransform( Matrix4f _m );
    void BBox3d( const Vector3f& bMin, const Vector3f& bMax );
    void BBox3d( const AABB& _value );

protected:
    JMATH::AABB bbox3d{AABB::INVALID};
    JMATH::AABB bbox3dT{AABB::INVALID};
    Rect2f bbox{Rect2f::INVALID};
    Rect2f bboxT{Rect2f::INVALID};
};
