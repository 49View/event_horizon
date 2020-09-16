//
// Created by Dado on 2019-02-18.
//

#pragma once

#include <core/math/aabb.h>
#include <core/math/oobb.h>

struct EmptyBox {
    static constexpr bool IsSerializable() { return false; }

    EmptyBox() = default;
    static EmptyBox MINVALID() { return EmptyBox{}; };
};

class Boxable {
public:
    [[nodiscard]] const JMATH::Rect2f& BBox() const;
    [[nodiscard]] const AABB& BBox3d() const;

    void BBox3d( const Vector3f& bMin, const Vector3f& bMax );
    void BBox3d( const AABB& _value );
    void BBox( const Rect2f& _value );

    void transform( const Matrix4f& _m );
    void expandVolume( const V3f& _value );
    void invalidateVolume();
    void setMinPoint( const V3f& _value );
    void setMaxPoint( const V3f& _value );

private:
    OOBB oobb{};
    JMATH::AABB bbox3d{AABB::MINVALID()};
    JMATH::AABB bbox3dT{AABB::MINVALID()};
    Rect2f bbox{Rect2f::INVALID};
    Rect2f bboxT{Rect2f::INVALID};
};
