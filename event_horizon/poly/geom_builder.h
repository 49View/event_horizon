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
#include "hier_geom.hpp"
#include "profile.hpp"
#include "follower.hpp"
#include "poly_helper.h"
#include "scene_graph.h"
#include "di_modules.h"

enum class GeomBuilderType {
    shape,
    follower,
    outline,
    poly,
    mesh,
    asset,
    file,
    import,

    unknown
};

template <typename T>
class GeomBasicBuilder {
public:
    T& inj( std::shared_ptr<HierGeom> _hier ) {
        elem = _hier;
        return static_cast<T&>(*this);
    }

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
    Vector3f axis = Vector3f::ZERO;
    Vector3f scale = Vector3f::ONE;
    MatrixAnim matrixAnim;
    std::shared_ptr<HierGeom> elem;

};

//struct ProfileSchema {
//    ProfileSchema() = default;
//    explicit ProfileSchema( const std::string& name ) : name( name ) {}
//    ProfileSchema( const std::string& name, FollowerFlags flags ) : name( name ), flags( flags ) {}
//    ProfileSchema( const std::string& name, uint32_t _flags ) : name( name ) {
//        flags = static_cast<FollowerFlags>(_flags);
//    }
//    ProfileSchema( const std::string& name, GeomDataFollowerBuilder::Raise raise ) : name( name ), raise( raise ) {}
//    ProfileSchema( const std::string& name, const Vector2f& flipVector ) : name( name ), flipVector( flipVector ) {}
//    ProfileSchema( const std::string& name, FollowerFlags flags, const Vector2f& flipVector ) : name( name ),
//                                                                                                flags( flags ),
//                                                                                                flipVector(
//                                                                                                        flipVector ) {}
//    ProfileSchema( const std::string& name, FollowerFlags flags, GeomDataFollowerBuilder::Raise raise,
//                   const Vector2f& flipVector ) : name( name ), flags( flags ), raise( raise ),
//                                                  flipVector( flipVector ) {}
//
//    ProfileSchema( const Vector2f& p1, const Vector2f& p2 );
//
//    void evaluateDirectBuild( ProfileManager& _PL);
//
//    std::string name;
//    FollowerFlags flags = FollowerFlags::Defaults;
//    GeomDataFollowerBuilder::Raise raise = GeomDataFollowerBuilder::Raise::None;
//    Vector2f flipVector = Vector2f::ZERO;
//    std::shared_ptr<ProfileBuilder> builder;
//};

class GeomBuilder : public DependantBuilder, public GeomBasicBuilder<GeomBuilder> {
public:
    GeomBuilder() = default;
    virtual ~GeomBuilder() = default;
protected:
    void createDependencyList( DependencyMaker& _md ) override;
public:

    GeomBuilder( const GeomBuilderType gbt ) {
        builderType = gbt;
    }

    GeomBuilder( const GeomBuilderType gbt, const std::string& _name ) : DependantBuilder(_name) {
        builderType = gbt;
    }

    GeomBuilder( const GeomBuilderType gbt, const std::initializer_list<std::string>& _tags ) {
        builderType = gbt;
        Name(concatenate( "_", _tags ));
    }

    // Impoorted object
    GeomBuilder( std::shared_ptr<HierGeom>, const std::vector<std::shared_ptr<MaterialBuilder>>& );

    // Polygon list
    explicit GeomBuilder( const std::vector<Vector3f>& _vlist,
                          const Vector3f& _normal = Vector3f::ZERO, ReverseFlag rf = ReverseFlag::False) {
        builderType = GeomBuilderType::poly;
        Vector3f ln = _normal;
        if ( ln == Vector3f::ZERO ) {
            ln = normalize( crossProduct( _vlist.at(0), _vlist.at(2), _vlist.at(1) ));
        }
        polyLines.emplace_back(PolyLine{ _vlist, ln, rf});
    }

    explicit GeomBuilder( const std::vector<Triangle2d>& _tris,
                          float _z = 0.0f,
                          const Vector3f& _normal = Vector3f::ZERO,
                          ReverseFlag rf = ReverseFlag::False ) {
        builderType = GeomBuilderType::poly;
        for ( const auto& [v1,v2,v3] : _tris ) {
            std::vector<Vector3f> plist;
            plist.emplace_back(Vector3f{ v1, _z});
            plist.emplace_back(Vector3f{ v2, _z});
            plist.emplace_back(Vector3f{ v3, _z});
            addPoly( PolyLine{ plist, _normal, rf } );
        }
    }

    GeomBuilder( std::initializer_list<Vector3f>&& arguments_list ) {
        std::vector<Vector3f> vlist;
        for (auto &v: arguments_list) vlist.emplace_back(std::move(v));
        PolyLine p { vlist, normalize( crossProduct( vlist[0], vlist[2], vlist[1] )), ReverseFlag::False};
        polyLines.push_back( p );
    }

    // Outlines
    GeomBuilder( std::initializer_list<Vector3f>&& arguments_list, float _zPull );

    GeomBuilder( std::initializer_list<Vector2f>&& arguments_list, float _zPull ) {
        std::vector<Vector3f> lverts;
        for (auto &v: arguments_list) lverts.emplace_back(v);
        outlineVerts.emplace_back( lverts, _zPull );
        builderType = GeomBuilderType::outline;
    }

    GeomBuilder( const std::vector<Vector3f>& arguments_list, float _zPull ) {
        outlineVerts.emplace_back( arguments_list, _zPull );
        builderType = GeomBuilderType::outline;
    }

    GeomBuilder( const std::vector<Vector2f>& arguments_list, float _zPull ) {
        std::vector<Vector3f> lverts;
        for (auto &v: arguments_list) lverts.emplace_back(v) ;
        outlineVerts.emplace_back( lverts, _zPull );
        builderType = GeomBuilderType::outline;
    }

    GeomBuilder( ShapeType _st, const Vector3f& _size = Vector3f::ONE ) {
        shapeType = _st;
        scale = _size;
        builderType = GeomBuilderType::shape;
    }

    GeomBuilder( const ProfileBuilder& _ps, const std::vector<Vector2f>& _outline,
                 const float _z = 0.0f, const Vector3f& _suggestedAxis = Vector3f::ZERO ) {
        mProfileBuilder = _ps;
        for (auto &v: _outline) profilePath.emplace_back( Vector3f{v, _z} );
        builderType = GeomBuilderType::follower;
    }

    GeomBuilder( const ProfileBuilder& _ps, const std::vector<Vector3f>& _outline,
                 const Vector3f& _suggestedAxis = Vector3f::ZERO ) {
        mProfileBuilder = _ps;
        mFollowerSuggestedAxis = _suggestedAxis;
        for (auto &v: _outline) profilePath.emplace_back( v );
        builderType = GeomBuilderType::follower;
    }

    void publish() const;

    GeomBuilder& bt( const GeomBuilderType _gbt ) {
        builderType = _gbt;
        return *this;
    }

    GeomBuilder& id( const uint64_t _id ) {
        mId = _id;
        return *this;
    }

    GeomBuilder& m( const std::string& _mat ) {
        materialName = _mat;
        return *this;
    }

    GeomBuilder& sh( const std::string& _value ) {
        shaderName = _value;
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

    GeomBuilder& addPoly( const PolyLine& _polyLine );
    GeomBuilder& addPoly( const PolyLine2d& _polyLine2d, const float heightOffset );
    GeomBuilder& addOutline( const std::vector<Vector3f>& _polyLine, const float _raise );

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

    GeomBuilder& addQuad( const QuadVector3fNormal& quad, bool reverseIfTriangulated = false );

    void assemble( DependencyMaker& _md ) override;
    ScreenShotContainerPtr& Thumb();

protected:
    bool validate() const override;
    void deserializeDependencies( DependencyMaker& _md );
    void createFromProcedural( std::shared_ptr<GeomDataBuilder> gb, SceneGraph& sg );
    void createFromAsset( std::shared_ptr<HierGeom> asset );
    std::string toMetaData() const;
    std::string generateThumbnail() const;
    std::string generateRawData() const;

private:
    uint64_t mId = 0;
    uint64_t gt = 1; // This is the generic geom ID, as we reserve 0 as null
    std::string  materialName = "white";
    MaterialType materialType = MaterialType::PBR;
    std::string  shaderName;

    ShapeType shapeType = ShapeType::None;
    subdivisionAccuray subdivAccuracy = accuracyNone;

    ProfileBuilder mProfileBuilder;
    std::vector<Vector3f> profilePath;
    FollowerFlags fflags = FollowerFlags::Defaults;
    PolyRaise fraise = PolyRaise::None;
    Vector2f flipVector = Vector2f::ZERO;
    FollowerGap mGaps = FollowerGap::Empty;
    Vector3f mFollowerSuggestedAxis = Vector3f::ZERO;

    GeomMappingData mapping;

    std::vector<PolyOutLine> outlineVerts;

    std::vector<PolyLine> polyLines;

    QuadVector3fNormalfList quads;

    GeomBuilderType builderType = GeomBuilderType::unknown;

    std::vector<std::shared_ptr<MaterialBuilder>> matBuilders;

    ScreenShotContainerPtr thumb;
    friend class GeomData;
};

