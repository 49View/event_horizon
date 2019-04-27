//
// Created by Dado on 02/01/2018.
//

#pragma once

#include <utility>
#include <core/v_data.hpp>
#include <core/uuid.hpp>
#include <core/names.hpp>
#include <core/name_policy.hpp>
#include <core/math/matrix_anim.h>
#include <core/math/poly_shapes.hpp>
#include <core/resources/resource_types.hpp>
#include <poly/poly_services.hpp>
#include <poly/poly.hpp>

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

template <typename T>
class VDataBaseAssembler {
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

//protected:
    Vector3f pos = Vector3f::ZERO;
    Vector3f bboxOffset = Vector3f::ZERO;
    Vector3f axis = Vector3f::ZERO;
    Vector3f scale = Vector3f::ONE;
    MatrixAnim matrixAnim;
};

template <typename T>
class VDataAssembler : public VDataBaseAssembler<VDataAssembler<T>>, public NamePolicy<> {
public:
    template<typename ...Args>
    explicit VDataAssembler( Args&& ... args ) {
        Name( UUIDGen::make());
        (addParam<T>( std::forward<Args>( args )), ...); // Fold expression (c++17)
    }

    virtual ~VDataAssembler() = default;

    template<typename SGT, typename M>
    VDataAssembler& addParam( const M& _param ) {

        if constexpr ( std::is_same<M, std::string>::value ) {
            static_assert( std::is_same<SGT, GT::Text2d>::value ||
                           std::is_same<SGT, GT::Text3d>::value ||
                           std::is_same<SGT, GT::TextUI>::value );
            dataTypeHolder.text = _param;
        }

        if constexpr ( std::is_same<M, ShapeType>::value ) {
            static_assert( std::is_same<SGT, GT::Shape>::value );
            dataTypeHolder.shapeType = _param;
        }

        if constexpr ( std::is_same<M, PolyOutLine>::value ) {
            static_assert( std::is_same<SGT, GT::Extrude>::value );
            dataTypeHolder.extrusionVerts.emplace_back( _param );
        }

        if constexpr ( std::is_same<M, std::vector<Vector3f>>::value ) {
            static_assert( std::is_same<SGT, GT::Poly>::value );
            dataTypeHolder.sourcePolysVList = _param;
        }

        return *this;
    }

    T dataTypeHolder;
    ResourceRef matRef = S::WHITE_PBR;
    GeomSP elemInjFather = nullptr;
};

namespace VDataServices {

    void prepare( GT::Shape& _d );
    void buildInternal( const GT::Shape& _d, std::shared_ptr<VData> _ret );
    ResourceRef refName( const GT::Shape& _d );

    void prepare( GT::Poly& _d );
    void buildInternal( const GT::Poly& _d, std::shared_ptr<VData> _ret );
    ResourceRef refName( const GT::Poly& _d );

    void prepare( GT::Extrude& _d );
    void buildInternal( const GT::Extrude& _d, std::shared_ptr<VData> _ret );
    ResourceRef refName( const GT::Extrude& _d );

    template <typename DT>
    std::shared_ptr<VData> build( const DT& _d ) {
        auto ret = std::make_shared<VData>();
        buildInternal( _d, ret);
        return ret;
    }
}

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

//class GeomDataQuadMeshBuilder : public GeomDataBuilder {
//public:
//    explicit GeomDataQuadMeshBuilder( QuadVector3fNormalfList _quads ) : quads( std::move( _quads )) {}
//    virtual ~GeomDataQuadMeshBuilder() = default;
//    void setupRefName() override;
//protected:
//    void buildInternal( std::shared_ptr<VData> _ret ) override;
//protected:
//    QuadVector3fNormalfList quads;
//};
//
//class GeomDataFollowerBuilder : public GeomDataBuilder {
//public:
//    GeomDataFollowerBuilder( std::shared_ptr<Profile> _profile,
//                             std::vector<Vector3f> _verts,
//                             const FollowerFlags f = FollowerFlags::Defaults,
//                             const PolyRaise _r = PolyRaise::None,
//                             const Vector2f& _flipVector = Vector2f::ZERO,
//                             FollowerGap _gaps = FollowerGap::Empty,
//                             const Vector3f& _suggestedAxis = Vector3f::ZERO ) :
//                             mProfile( std::move( _profile )), mVerts( std::move( _verts )),
//                             followersFlags(f), mRaiseEnum(_r),
//                             mFlipVector(_flipVector), mGaps( std::move( _gaps )), mSuggestedAxis(_suggestedAxis) {}
//    virtual ~GeomDataFollowerBuilder() = default;
//    void setupRefName() override;
//protected:
//    void buildInternal( std::shared_ptr<VData> _ret ) override;
//protected:
//    std::shared_ptr<Profile> mProfile;
//    std::vector<Vector3f> mVerts;
//    FollowerFlags followersFlags = FollowerFlags::Defaults;
//    Vector2f mRaise = Vector2f::ZERO;
//    PolyRaise mRaiseEnum = PolyRaise::None;
//    Vector2f mFlipVector = Vector2f::ZERO;
//    FollowerGap mGaps = FollowerGap::Empty;
//    Vector3f mSuggestedAxis = Vector3f::ZERO;
//};
//
//template <typename T>
//class GeomDataBuilderBaseList {
//public:
//    virtual std::vector<std::shared_ptr<T>> build() = 0;
//};
//
//class GeomDataBuilderList : public GeomDataBuilderBaseList<VData> {
//public:
//    virtual ~GeomDataBuilderList() = default;
//};
//
//class GeomDataSVGBuilder : public GeomDataBuilderList {
//public:
//    GeomDataSVGBuilder( std::string _svgString, std::shared_ptr<Profile> _profile ) :
//                        svgAscii(std::move( _svgString )), mProfile(std::move(_profile)) {}
//    GeomDataListBuilderRetType build() override;
//protected:
//    std::string svgAscii;
//    std::shared_ptr<Profile> mProfile;
//};

void clipperToPolylines( std::vector<PolyLine2d>& ret, const ClipperLib::Paths& solution, const Vector3f& _normal,
                         ReverseFlag rf = ReverseFlag::False );
std::vector<PolyLine2d> clipperToPolylines( const ClipperLib::Paths& source, const ClipperLib::Path& clipAgainst,
                                            const Vector3f& _normal, ReverseFlag rf = ReverseFlag::False );
ClipperLib::Path getPerimeterPath( const std::vector<Vector2f>& _values );
