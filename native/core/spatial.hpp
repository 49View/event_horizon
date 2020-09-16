//
// Created by Dado on 2019-02-18.
//

#pragma once

#include <core/math/aabb.h>
#include <core/math/oobb.h>

class Spatial {
public:
    [[nodiscard]] const JMATH::Rect2f& BBox() const;
    [[nodiscard]] const AABB& BBox3d() const;

    void initialiseVolume( const Rect2f& _value );
    void initialiseVolume( const AABB& _value );
    void initialiseVolume( const V3f& _size, const V3f& _center );

    [[nodiscard]] float Width() const;
    [[nodiscard]] float Height() const;
    [[nodiscard]] float Depth() const;

    [[nodiscard]] float HalfWidth() const;
    [[nodiscard]] float HalfHeight() const;
    [[nodiscard]] float HalfDepth() const;

    [[nodiscard]] V3f Position() const;
    [[maybe_unused]] [[nodiscard]] float PositionX() const;
    [[maybe_unused]] [[nodiscard]] float PositionY() const;
    [[maybe_unused]] [[nodiscard]] float PositionZ() const;
    [[nodiscard]] V2f Position2d() const;
    [[nodiscard]] V3f Center() const;
//    [[nodiscard]] V2f Center2d() const { return centre.xz(); }
    [[nodiscard]] const V3f& Size() const;
    [[nodiscard]] const V3f& Scale() const;

    [[nodiscard]] const Quaternion& Rotation() const;

    [[nodiscard]] const std::vector<Triangle2d>& Triangles2d() const;

    virtual void move( const V3f& _off );
    virtual void move( const V2f& _off );
    virtual void position( const V3f& _pos );
    virtual void position( const V2f& _pos );
    virtual void center( const V3f& _pos );
    virtual void rotate( const Quaternion& _rot );
    virtual void scale( const V3f& _scale );

    Spatial updateVolume();

protected:
    [[nodiscard]] float& w();
    [[nodiscard]] float& h();
    [[nodiscard]] float& d();
    [[nodiscard]] V3f& position();
//    [[nodiscard]] V3f& center() { return centre; }
    [[nodiscard]] Quaternion& rot();
    [[nodiscard]] V3f& scale();

private:
    virtual void updateVolumeInternal();
    void invalidateVolume();
    void posBBox();
    void rotateBBox( const Quaternion& _rot );
    void scaleBBox( const V3f& _scale );
    void internalUpdate();

protected:
    void mergeVolume( const Spatial& _spatial );

protected:
    JMATH::AABB bbox3d{AABB::MINVALID()};
    Rect2f bbox{Rect2f::INVALID};
    V3f size{ V3fc::ZERO };
    V3f centre{ V3fc::ZERO };

    V3f pos{ V3fc::ZERO };
    Quaternion rotation{ V3fc::ZERO, 1.0f };
    Vector3f scaling = V3fc::ONE;
    std::vector<Triangle2d> mTriangles2d;
};
