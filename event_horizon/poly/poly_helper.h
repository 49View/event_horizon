#include <utility>

//
// Created by Dado on 02/01/2018.
//

#pragma once

#include <utility>
#include <core/v_data.hpp>
#include <core/math/poly_shapes.hpp>
#include <poly/poly_services.hpp>

class Profile;

using GeomDataListBuilderRetType = std::vector<std::shared_ptr<VData>>;

namespace ClipperLib {
    struct IntPoint;
    typedef std::vector< IntPoint > Path;
    typedef std::vector< Path > Paths;
}

struct QuadVector3fNormal {
    QuadVector3f quad;
    Vector3f normal;
};

struct PolyLineBase3d {
    explicit PolyLineBase3d( std::vector<Vector3f> verts ) : verts( std::move( verts )) {}
    explicit PolyLineBase3d( const Triangle3d& _verts ) {
        const auto& [v1,v2,v3] = _verts;
        verts.emplace_back(v1);
        verts.emplace_back(v2);
        verts.emplace_back(v3);
    }

    std::vector<Vector3f> verts;
};

struct PolyLineBase2d  {
    explicit PolyLineBase2d( std::vector<Vector2f> verts ) : verts( std::move( verts )) {}

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
    PolyLine( const Triangle3d& _verts, const Vector3f& _normal,
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
//    MappingDirection getMappingDirection() const { return mapping.direction; }
//    void setMappingDirection( MappingDirection val ) { mapping.direction = val; }
//    Vector2f MappingOffset() const { return mapping.offset; }
//    void MappingOffset( const Vector2f& val ) { mapping.offset = val; }
//    MappingMirrorE MappingMirror() const { return mapping.mirroring; }
//    void MappingMirror( MappingMirrorE val ) { mapping.mirroring = val; }
//    bool UnitMapping() const { return mapping.bUnitMapping; }
//    void UnitMapping( bool val ) { mapping.bUnitMapping = val; }
//    subdivisionAccuray SubdivAccuracy() const { return mSubdivAccuracy; }
//    void SubdivAccuracy( subdivisionAccuray val ) { mSubdivAccuracy = val; }
//    const std::vector<Vector2f>& WrapMappingCoords() const { return wrapMappingCoords; }
//    void WrapMappingCoords( const std::vector<Vector2f>& val ) { wrapMappingCoords = val; }

    // Mapping constants
    MappingDirection direction = MappingDirection::X_POS;
    bool bDoNotScaleMapping = false;
    Vector2f offset = Vector2f::ZERO;
    MappingMirrorE mirroring = MappingMirrorE::None;
    bool bUnitMapping = false;
    subdivisionAccuray subdivAccuracy = accuracyNone;
    WindingOrderT windingOrder = WindingOrder::CCW;

    JSONSERIALBIN( direction, bDoNotScaleMapping, offset, mirroring, bUnitMapping, subdivAccuracy, windingOrder)
    // Mappping computed
    float fuvScale = 1.0f;
    Vector2f uvScale = Vector2f::ONE;
    Vector2f uvScaleInv = Vector2f::ONE;
    std::vector<Vector2f> wrapMappingCoords;
    Vector2f pullMappingCoords = Vector2f::ZERO;
};

class GeomDataBuilder {
public:
    std::shared_ptr<VData> build() {
        auto ret = std::make_shared<VData>();
        if ( mappingData.bDoNotScaleMapping ) MappingServices::doNotScaleMapping(mappingData);
        buildInternal(ret);
        return ret;
    }
    std::string refName() const { return mRefName; };
    virtual void setupRefName() = 0;

protected:
    virtual void buildInternal( std::shared_ptr<VData> _ret ) = 0;

protected:
    std::string mRefName;
    GeomMappingData mappingData;
};

class GeomDataShapeBuilder : public GeomDataBuilder {
public:
    explicit GeomDataShapeBuilder( ShapeType shapeType );
    virtual ~GeomDataShapeBuilder() = default;
    void setupRefName() override;
protected:
    void buildInternal( std::shared_ptr<VData> _ret ) override;
protected:
    ShapeType shapeType;
};

class GeomDataOutlineBuilder : public GeomDataBuilder {
public:
    explicit GeomDataOutlineBuilder( std::vector<PolyOutLine> outlineVerts ) : outlineVerts( std::move( outlineVerts )) {}
    virtual ~GeomDataOutlineBuilder() = default;
    void setupRefName() override;
protected:
    void buildInternal( std::shared_ptr<VData> _ret ) override;
protected:
    std::vector<PolyOutLine> outlineVerts;
};

class GeomDataPolyBuilder : public GeomDataBuilder {
public:
    explicit GeomDataPolyBuilder( std::vector<PolyLine> _polyLine ) : polyLine( std::move( _polyLine )) {}
    virtual ~GeomDataPolyBuilder() = default;
    void setupRefName() override;
protected:
    void buildInternal( std::shared_ptr<VData> _ret ) override;
protected:
    std::vector<PolyLine> polyLine;
};

class GeomDataQuadMeshBuilder : public GeomDataBuilder {
public:
    explicit GeomDataQuadMeshBuilder( QuadVector3fNormalfList _quads ) : quads( std::move( _quads )) {}
    virtual ~GeomDataQuadMeshBuilder() = default;
    void setupRefName() override;
protected:
    void buildInternal( std::shared_ptr<VData> _ret ) override;
protected:
    QuadVector3fNormalfList quads;
};

class GeomDataFollowerBuilder : public GeomDataBuilder {
public:
    GeomDataFollowerBuilder( std::shared_ptr<Profile> _profile,
                             std::vector<Vector3f> _verts,
                             const FollowerFlags f = FollowerFlags::Defaults,
                             const PolyRaise _r = PolyRaise::None,
                             const Vector2f& _flipVector = Vector2f::ZERO,
                             FollowerGap _gaps = FollowerGap::Empty,
                             const Vector3f& _suggestedAxis = Vector3f::ZERO ) :
                             mProfile( std::move( _profile )), mVerts( std::move( _verts )),
                             followersFlags(f), mRaiseEnum(_r),
                             mFlipVector(_flipVector), mGaps( std::move( _gaps )), mSuggestedAxis(_suggestedAxis) {}
    virtual ~GeomDataFollowerBuilder() = default;
    void setupRefName() override;
protected:
    void buildInternal( std::shared_ptr<VData> _ret ) override;
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

template <typename T>
class GeomDataBuilderBaseList {
public:
    virtual std::vector<std::shared_ptr<T>> build() = 0;
};

class GeomDataBuilderList : public GeomDataBuilderBaseList<VData> {
public:
    virtual ~GeomDataBuilderList() = default;
};

class GeomDataSVGBuilder : public GeomDataBuilderList {
public:
    GeomDataSVGBuilder( std::string _svgString, std::shared_ptr<Profile> _profile ) :
                        svgAscii(std::move( _svgString )), mProfile(std::move(_profile)) {}
    GeomDataListBuilderRetType build() override;
protected:
    std::string svgAscii;
    std::shared_ptr<Profile> mProfile;
};

void clipperToPolylines( std::vector<PolyLine2d>& ret, const ClipperLib::Paths& solution, const Vector3f& _normal,
                         ReverseFlag rf = ReverseFlag::False );
std::vector<PolyLine2d> clipperToPolylines( const ClipperLib::Paths& source, const ClipperLib::Path& clipAgainst,
                                            const Vector3f& _normal, ReverseFlag rf = ReverseFlag::False );
ClipperLib::Path getPerimeterPath( const std::vector<Vector2f>& _values );
