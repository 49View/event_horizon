//
// Created by Dado on 2019-02-18.
//

#include "spatial.hpp"

void Spatial::initialiseVolume( const AABB& _value ) {
    bbox3d = _value;
    bbox = bbox3d.topDown();
    size = BBox3d().size();
    centre = BBox3d().centre();
}

void Spatial::invalidateVolume() {
    bbox3d = AABB::MINVALID();
    bbox = Rect2f::INVALID;
}

const JMATH::Rect2f& Spatial::BBox() const {
    return bbox;
}

const AABB& Spatial::BBox3d() const {
    return bbox3d;
}

Spatial Spatial::updateVolume() {
    invalidateVolume();
    updateVolumeInternal();
    return *this;
}

void Spatial::mergeVolume( const Spatial& _spatial ) {
    bbox3d.merge(_spatial.BBox3d());
    bbox.merge(_spatial.BBox());
    centre = bbox3d.centre();
    size = bbox3d.size();
    pos = V3fc::ZERO;
    rotation = Quaternion{};
    scaling = V3fc::ONE;
}

void Spatial::elaborateBBox() {
    V3f scaledHalf = half(size * scaling);
    bbox3d = AABB{ -scaledHalf, scaledHalf };
    bbox3d.translate( pos + centre );
    bbox3d = bbox3d.rotate(rotation);
    bbox = bbox3d.topDown();
}

void Spatial::move( const V3f& _off ) {
    position() += _off;
    elaborateBBox();
}

void Spatial::move( const V2f& _off ) {
    position() += XZY::C(_off, 0.0f);
    elaborateBBox();
}

void Spatial::position( const V3f& _pos ) {
    position() = _pos;
    elaborateBBox();
}

void Spatial::position( const V2f& _pos ) {
    position() = V3f{ _pos.x(), position().y(), _pos.y() };
    elaborateBBox();
}

void Spatial::rotate( const Quaternion& _rot ) {
    rot() = _rot;
    elaborateBBox();
}

void Spatial::scale( const V3f& _scale ) {
    scale() = _scale;
    elaborateBBox();
}

float Spatial::Width() const { return size.x(); }
float Spatial::Height() const { return size.y(); }
float Spatial::Depth() const { return size.z(); }
float Spatial::HalfWidth() const { return Width() * 0.5f; }
float Spatial::HalfHeight() const { return Height() * 0.5f; }
float Spatial::HalfDepth() const { return Depth() * 0.5f; }
V3f Spatial::Position() const { return pos; }
[[maybe_unused]] float Spatial::PositionX() const { return Position().x(); }
float Spatial::PositionY() const { return Position().y(); }
float Spatial::PositionZ() const { return Position().z(); }
V2f Spatial::Position2d() const { return pos.xz(); }
V3f Spatial::Scale() const { return scaling; }
const V3f& Spatial::Size() const { return size; }
const Quaternion& Spatial::Rotation() const { return rotation; }
const std::vector<Triangle2d>& Spatial::Triangles2d() const { return mTriangles2d; }
float& Spatial::w() { return size[0]; }
float& Spatial::h() { return size[1]; }
float& Spatial::d() { return size[2]; }
V3f& Spatial::position() { return pos; }
Quaternion& Spatial::rot() { return rotation; }
V3f& Spatial::scale() { return scaling; }
