//
// Created by Dado on 02/01/2018.
//

#pragma once

#include "core/math/poly_shapes.hpp"
#include "core/callback_dependency.h"

class GeomData;
class Profile;

struct QuadVector3fNormal {
    QuadVector3f quad;
    Vector3f normal;
};

struct PolyLineBase3d {
    PolyLineBase3d( const std::vector<Vector3f>& verts ) : verts( verts ) {}

    std::vector<Vector3f> verts;
};

struct PolyLineBase2d  {
    PolyLineBase2d( const std::vector<Vector2f>& verts ) : verts( verts ) {}

    std::vector<Vector2f> verts;
};

struct PolyLineCommond {
    PolyLineCommond( const Vector3f& normal, ReverseFlag reverseFlag ) : normal( normal ), reverseFlag( reverseFlag ) {}

    Vector3f normal = Vector3f::ZERO;
    ReverseFlag reverseFlag = ReverseFlag::False;
};

struct PolyLine : public PolyLineBase3d, public PolyLineCommond {
    PolyLine( const std::vector<Vector3f>& _verts, const Vector3f& _normal,
              const ReverseFlag _reverseFlag = ReverseFlag::False ) :
            PolyLineBase3d(_verts), PolyLineCommond(_normal, _reverseFlag) {}
};

struct PolyOutLine : public PolyLineBase3d {
    PolyOutLine( const std::vector<Vector3f>& _verts, const float _zPull,
                 const ReverseFlag _reverseFlag = ReverseFlag::False ) :
                 PolyLineBase3d(_verts), zPull(_zPull), reverseFlag(_reverseFlag) {}

    float zPull;
    ReverseFlag reverseFlag = ReverseFlag::False;
};

struct PolyLine2d : public PolyLineBase2d, public PolyLineCommond {
    PolyLine2d( const std::vector<Vector2f>& _verts, const Vector3f& _normal,
                const ReverseFlag _reverseFlag = ReverseFlag::False ) :
            PolyLineBase2d(_verts), PolyLineCommond(_normal, _reverseFlag) {}
};

using QuadVector3fNormalfList = std::vector<QuadVector3fNormal>;

struct GeomMappingData {
    MappingDirection direction = MappingDirection::X_POS;
    bool bDoNotScaleMapping = false;
    Vector2f offset = Vector2f::ZERO;
    MappingMirrorE mirroring = MappingMirrorE::None;
    bool bUnitMapping = false;
};

class GeomDataBuilder : public ConcreteBuilder<GeomData> {
public:
    virtual ~GeomDataBuilder() {}
protected:
    std::shared_ptr<PBRMaterial> material;

    GeomDataBuilder& m( std::shared_ptr<PBRMaterial> _material ) {
        material = _material;
        return *this;
    }
    GeomMappingData mappingData;

    friend class GeomBuilder;
    friend class GeomData;
};

class GeomDataShapeBuilder : public GeomDataBuilder {
public:
    GeomDataShapeBuilder( ShapeType shapeType, const Vector3f& pos, const Vector3f& axis, const Vector3f& scale )
            : shapeType( shapeType ), pos( pos ), axis( axis ), scale( scale ) {}

    std::shared_ptr<GeomData> build() override;

protected:
    ShapeType shapeType;
    Vector3f  pos;
    Vector3f  axis;
    Vector3f  scale;
};

class GeomDataOutlineBuilder : public GeomDataBuilder {
public:
    GeomDataOutlineBuilder( const std::vector<PolyOutLine>& outlineVerts ) : outlineVerts( outlineVerts ) {}

    std::shared_ptr<GeomData> build() override;

protected:
    std::vector<PolyOutLine> outlineVerts;
};

class GeomDataPolyBuilder : public GeomDataBuilder {
public:
    GeomDataPolyBuilder( const std::vector<PolyLine>& _polyLine ) : polyLine( _polyLine ) {}
    std::shared_ptr<GeomData> build() override;

protected:
    std::vector<PolyLine> polyLine;
};

class GeomDataQuadMeshBuilder : public GeomDataBuilder {
public:
    GeomDataQuadMeshBuilder( QuadVector3fNormalfList _quads ) : quads( std::move( _quads )) {}
    std::shared_ptr<GeomData> build() override;

protected:
    QuadVector3fNormalfList quads;
};

class GeomDataFollowerBuilder : public GeomDataBuilder {
public:
    GeomDataFollowerBuilder( std::shared_ptr<Profile> _profile,
                             std::vector<Vector3f> _verts,
                             const FollowerFlags f,
                             const PolyRaise _r,
                             const Vector2f& _flipVector,
                             FollowerGap _gaps,
                             const Vector3f& _suggestedAxis ) :
                             mProfile(_profile), mVerts( std::move( _verts )), followersFlags(f), mRaiseEnum(_r),
                             mFlipVector(_flipVector), mGaps( std::move( _gaps )), mSuggestedAxis(_suggestedAxis) {}
    std::shared_ptr<GeomData> build() override;

    GeomDataFollowerBuilder& raise( const Vector2f& _r ) {
        mRaise = _r;
        return *this;
    }

    GeomDataFollowerBuilder& raise( const PolyRaise _r ) {
        mRaiseEnum = _r;
        return *this;
    }

    GeomDataFollowerBuilder& flip( const Vector2f& _flipVector ) {
        mFlipVector = _flipVector;
        return *this;
    }

    GeomDataFollowerBuilder& gaps( const FollowerGap& _gaps ) {
        mGaps = _gaps;
        return *this;
    }

    GeomDataFollowerBuilder& ff( const FollowerFlags f ) {
        followersFlags |= f;
        return *this;
    }

protected:
    std::shared_ptr<Profile> mProfile;
    std::vector<Vector3f> mVerts;
    FollowerFlags followersFlags = FollowerFlags::Defaults;
    Vector2f mRaise = Vector2f::ZERO;
    PolyRaise mRaiseEnum = PolyRaise::None;
    Vector2f mFlipVector = Vector2f::ZERO;
    FollowerGap mGaps = FollowerGap::Empty;
    Vector3f mSuggestedAxis = Vector3f::ZERO;
};

//std::shared_ptr<GeomData> GeomDataBuilder::build() {
//
//    elem = std::make_shared<GeomData>();
//
//    switch ( builderType ) {
//        case file:break;
//        case shape:
//            elem->addShape( shapeType, Vector3f::ZERO, Vector3f::ONE, 3 );
//            break;
//        case outline:
//            elem->pull( outlineVerts, zPull );
//            break;
//        case follower: {
//            //FollowerBuilder{ baseGeom, profile, outlineVerts }.gaps(mGaps).ff(followersFlags).build();
//        } break;
//        case unknown:
//            break;
//    }
//    return elem;
//}
