//
// Created by Dado on 2019-02-18.
//

#include "boxable.hpp"

const JMATH::Rect2f& Boxable::BBox() const {
    return bbox;
}

const AABB& Boxable::BBox3d() const {
    return bbox3d;
}

const AABB *Boxable::BBox3dPtr() const {
    return &bbox3d;
}

//AABB& Boxable::BBox3d() { return bbox3d; }
//AABB& Boxable::BBox3dT() { return bbox3dT; }

void Boxable::transform( const Matrix4f& _m ) {
//        if ( std::is_same_v<D, BBoxProjection2d> ) {
//            V3f ssTranslate = _m.getPosition3();
//            ssTranslate.oneMinusY();
//            _m.setTranslation( ssTranslate );
//        }
    bbox3dT = bbox3d;
    bbox3dT.transform(_m);
}

void Boxable::BBox3d( const Vector3f& bMin, const Vector3f& bMax ) {
    bbox3d = AABB{ bMin, bMax };
    bbox3dT = bbox3d;
}

void Boxable::BBox3d( const AABB& _value ) {
    bbox3d = _value;
    bbox3dT = bbox3d;
}

void Boxable::expandVolume( const V3f& _value ) {
    bbox3d.expand( _value );
    bbox.expand( _value.xz() );
    // ### TODO OOBB expand
}

void Boxable::setMinPoint( const V3f& _value ) {
    bbox3d.setMinPoint( _value );
    bbox.setBottomRight( _value.xz() );
    // ### TODO OOBB expand

}

void Boxable::setMaxPoint( const V3f& _value ) {
    bbox3d.setMaxPoint( _value );
    bbox.setTopLeft( _value.xz() );
    // ### TODO OOBB expand

}

void Boxable::invalidateVolume() {
    bbox3d = AABB::MINVALID();;
    bbox = Rect2f::INVALID;
    oobb = OOBBc::INVALID;
}
