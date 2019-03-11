#include <utility>

//
// Created by Dado on 29/10/2017.
//

#pragma once

#include <memory>
#include <string>
#include "core/math/vector3f.h"
#include "core/math/matrix_anim.h"
#include "core/service_factory.h"
#include "core/math/poly_shapes.hpp"
#include "core/image_builder.h"
#include "core/descriptors/material.h"
#include <poly/profile.hpp>
#include <poly/follower.hpp>
#include <poly/poly_helper.h>
#include <poly/scene_graph.h>
#include <poly/scene_graph_geom_builder_base.hpp>

enum class GeomBuilderType {
    shape,
    follower,
    outline,
    poly,
    mesh,
    gltf2,
    asset,
    file,
    svg,

    unknown
};

template <typename T>
class GeomBasicBuilder {
public:

    T& r( const Vector3f& _axis ) {
        axis = _axis;
        return static_cast<T&>(*this);
    }

    T& s( const Vector3f& _scaling ) {
        scale = _scaling;
        return static_cast<T&>(*this);
    }

    T& s( const float _scaling ) {
        scale = Vector3f{_scaling};
        return static_cast<T&>(*this);
    }

    T& withScaling( const Vector3f& _scaling ) {
        scale = _scaling;
        return static_cast<T&>(*this);
    }

    T& withScaling( const float _scaling ) {
        scale = Vector3f{_scaling};
        return static_cast<T&>(*this);
    }

    T& at( const Vector3f& _pos ) {
        pos = _pos;
        return static_cast<T&>(*this);
    }

    T& bboff( const Vector3f& _pos ) {
        bboxOffset = _pos;
        return static_cast<T&>(*this);
    }

    T& at( const Vector3f& _pos, const Vector3f& _axis ) {
        pos = _pos;
        axis = _axis;
        return static_cast<T&>(*this);
    }

    T& at( const Vector3f& _pos, const Vector3f& _axis,
           const Vector3f& _scaling ) {
        pos = _pos;
        axis = _axis;
        scale = _scaling;
        return static_cast<T&>(*this);
    }

    T& withMatrix( const MatrixAnim& _m ) {
        matrixAnim = _m;
        return static_cast<T&>(*this);
    }

protected:
    Vector3f pos = Vector3f::ZERO;
    Vector3f bboxOffset = Vector3f::ZERO;
    Vector3f axis = Vector3f::ZERO;
    Vector3f scale = Vector3f::ONE;
    MatrixAnim matrixAnim;
//    GeomAssetSP elem;
};

struct RBUILDER( GeomFileAssetBuilder, geom, geom, Binary, BuilderQueryType::NotExact, GeomData::Version() )

};

using GF = GeomFileAssetBuilder;

class GeomBuilder : public SceneGraphGeomBaseBuilder, public GeomBasicBuilder<GeomBuilder> {
public:
    explicit GeomBuilder( SceneGraph& _sg );
    virtual ~GeomBuilder() = default;

    explicit GeomBuilder( SceneGraph& _sg, GeomBuilderType gbt );
    GeomBuilder( SceneGraph& _sg, GeomBuilderType gbt, const std::string& _name );
    GeomBuilder( SceneGraph& _sg, GeomBuilderType gbt, const std::initializer_list<std::string>& _tags );

    // Polygon list
    explicit GeomBuilder( SceneGraph& _sg, const Rect2f& _rect, float _z = 0.0f );
    explicit GeomBuilder( SceneGraph& _sg, const std::vector<PolyLine>& _plist );
    explicit GeomBuilder( SceneGraph& _sg, const std::vector<Vector3f>& _vlist );
    explicit GeomBuilder( SceneGraph& _sg, const std::vector<Triangle2d>& _tris, float _z = 0.0f );
    explicit GeomBuilder( SceneGraph& _sg, const std::vector<PolyLine2d>& _plines, float _z = 0.0f );

    // Outlines
    GeomBuilder( SceneGraph& _sg, std::initializer_list<Vector3f>&& arguments_list, float _zPull );
    GeomBuilder( SceneGraph& _sg, std::initializer_list<Vector2f>&& arguments_list, float _zPull );
    GeomBuilder( SceneGraph& _sg, const std::vector<Vector3f>& arguments_list, float _zPull );
    GeomBuilder( SceneGraph& _sg, const std::vector<Vector2f>& arguments_list, float _zPull );

    // Shapes
    explicit GeomBuilder( SceneGraph& _sg, ShapeType _st, const Vector3f& _size = Vector3f::ONE );

    // Profile/Follwoers
    GeomBuilder( SceneGraph& _sg, const ProfileBuilder& _ps, const std::vector<Vector2f>& _outline,
                 float _z = 0.0f, const Vector3f& _suggestedAxis = Vector3f::ZERO );
    GeomBuilder( SceneGraph& _sg, const ProfileBuilder& _ps, const std::vector<Vector3f>& _outline,
                 const Vector3f& _suggestedAxis = Vector3f::ZERO );
    GeomBuilder( SceneGraph& _sg, const ProfileBuilder& _ps, const Rect2f& _r, const Vector3f& _suggestedAxis = Vector3f::ZERO );

    GeomBuilder& inj( GeomAssetSP _hier );

    GeomBuilder& bt( const GeomBuilderType _gbt ) {
        builderType = _gbt;
        return *this;
    }

    GeomBuilder& id( const uint64_t _id ) {
        mId = _id;
        return *this;
    }

    GeomBuilder& pb( const ProfileBuilder& _value ) {
        mProfileBuilder = _value;
        return *this;
    }

    GeomBuilder& ascii( const std::string& _value ) {
        asciiText = _value;
        return *this;
    }

    GeomBuilder& md( const MappingDirection _md ) {
        mapping.direction = _md;
        return *this;
    }

    GeomBuilder& n( const std::string& _s ) {
        Name(_s);
        return *this;
    }

    GeomBuilder& g( const uint64_t & _gt ) {
        gt = _gt;
        return *this;
    }

    GeomBuilder& pr( const PolyRaise _profileRaise ) {
        fraise = _profileRaise;
        return *this;
    }

    GeomBuilder& fnp( const Vector3f& _n ) {
        forcingNormalPoly = _n;
        return *this;
    }

    GeomBuilder& prf() {
        rfPoly = ReverseFlag::True;
        return *this;
    }

    GeomBuilder& addPoly( const PolyLine& _polyLine );
    GeomBuilder& addPoly( const PolyLine2d& _polyLine2d, float heightOffset );
    GeomBuilder& addOutline( const std::vector<Vector3f>& _polyLine, float _raise );

    GeomBuilder& accuracy( const subdivisionAccuray _val ) {
        subdivAccuracy = _val;
        return *this;
    }

    GeomBuilder& doNotScaleMapping() {
        mapping.bDoNotScaleMapping = true;
        return *this;
    }

    GeomBuilder& mappingOffset( const Vector2f& _val ) {
        mapping.offset = _val;
        return *this;
    }

    GeomBuilder& mappingMirror( const MappingMirrorE _val ) {
        mapping.mirroring = _val;
        return *this;
    }

    GeomBuilder& unitMapping() {
        mapping.bUnitMapping = true;
        return *this;
    }

    GeomBuilder& ff( const FollowerFlags f ) {
        fflags |= f;
        return *this;
    }

    GeomBuilder& ff( const uint32_t f ) {
        fflags |= static_cast<FollowerFlags>(f);
        return *this;
    }

    GeomBuilder& fflip( const Vector2f& _v ) {
        flipVector = _v;
        return *this;
    }

    GeomBuilder& gaps( const FollowerGap& _gaps ) {
        mGaps = _gaps;
        return *this;
    }

    GeomBuilder& dontAddToSceneGraph() {
        bAddToSceneGraph = false;
        return *this;
    }

    GeomBuilder& addQuad( const QuadVector3fNormal& quad, bool reverseIfTriangulated = false );

// MaterialBuildable policies
    GeomBuilder& m( std::shared_ptr<Material> _value ) {
        materialSet(_value);
        return *this;
    }

    GeomBuilder& m( const std::string& _shader, const std::string& _matName ) {
        materialSet(_shader, _matName);
        return *this;
    }

    template <typename T>
    GeomBuilder& mc( const std::string& _name, T _value ) {
        materialConstant( _name, _value);
        return *this;
    }

    GeomBuilder& c( const Color4f & _color ) {
        materialColor( _color );
        return *this;
    }

    GeomBuilder& c( const std::string& _hexcolor ) {
        materialColor( Vector4f::XTORGBA( _hexcolor ) );
        return *this;
    }

    GeomAssetSP buildr();

    void assemble() override;

protected:
    void elemCreate() override;
    GeomAssetSP Elem() { return elem; }

    void createDependencyList() override;
    bool validate() const override;
    void preparePolyLines();
    void createFromProcedural( std::shared_ptr<GeomDataBuilder> gb );
    void createFromProcedural( std::shared_ptr<GeomDataBuilderList> gb );
    void createFromAsset( GeomAssetSP asset );

private:
    uint64_t mId = 0;
    uint64_t gt = 1; // This is the generic geom ID, as we reserve 0 as null

    bool bMaterialDep = false;
    ShapeType shapeType = ShapeType::None;
    subdivisionAccuray subdivAccuracy = accuracyNone;

    ProfileBuilder mProfileBuilder;
    std::vector<Vector3f> profilePath;
    FollowerFlags fflags = FollowerFlags::Defaults;
    PolyRaise fraise = PolyRaise::None;
    Vector2f flipVector = Vector2f::ZERO;
    FollowerGap mGaps = FollowerGap::Empty;
    Vector3f mFollowerSuggestedAxis = Vector3f::ZERO;

    std::string asciiText;

    GeomMappingData mapping;

    std::vector<PolyOutLine> outlineVerts;

    std::vector<Vector3f> sourcePolysVList;
    std::vector<Triangle3d> sourcePolysTris;
    std::vector<PolyLine> polyLines;
    Vector3f forcingNormalPoly = Vector3f::ZERO;
    ReverseFlag rfPoly = ReverseFlag::False;
    QuadVector3fNormalfList quads;

    GeomBuilderType builderType = GeomBuilderType::unknown;

    std::vector<std::shared_ptr<MaterialBuilder>> matBuilders;

    GeomAssetSP elem = nullptr;
    GeomAssetSP elemInjFather = nullptr;

    bool bAddToSceneGraph = true;

    ScreenShotContainerPtr thumb;
    friend class GeomData;
};

using GB = GeomBuilder;

class GeomBuilderComposer {
public:
    GeomBuilderComposer();

    void add( GeomBuilder _gb );
    void build();

    GeomAssetSP Elem();
private:
    GeomAssetSP elem = nullptr;
    std::vector<GeomBuilder> builders;
};

using GBC = GeomBuilderComposer;

#define bld() build(o()->RSG())
#define bldr() buildr(o()->RSG())
