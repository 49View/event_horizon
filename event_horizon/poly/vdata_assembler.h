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
class SceneGraph;

using GeomDataListBuilderRetType = std::vector<std::shared_ptr<VData>>;

namespace ClipperLib {
    struct IntPoint;
    typedef std::vector< IntPoint > Path;
    typedef std::vector< Path > Paths;
}

template <typename T>
class VDataAssembler : public NamePolicy<> {
public:
    template<typename ...Args>
    explicit VDataAssembler( Args&& ... args ) {
        Name( UUIDGen::make());
        (addParam<T>( std::forward<Args>( args )), ...); // Fold expression (c++17)
    }

    virtual ~VDataAssembler() = default;

    template<typename SGT, typename M>
    VDataAssembler& addParam( const M& _param ) {

        if constexpr ( std::is_same_v<M, GT::M> ) {
            matRef = _param();
            return *this;
        }
        if constexpr ( std::is_same_v<M, GT::A> ) {
            matColor.setW( _param() );
            return *this;
        }
        if constexpr ( std::is_same_v<M, std::string> || std::is_same_v<std::decay_t<M>, char*>) {
            static_assert( std::is_same_v<SGT, GT::Text> );
            dataTypeHolder.text = _param;
            return *this;
        }
        if constexpr ( std::is_same_v<M, Color4f> ) {
            static_assert( std::is_base_of_v<GT::GTPolicyColor, SGT> );
            dataTypeHolder.color = _param;
            matColor = _param;
            return *this;
        }
        if constexpr ( std::is_same_v<M, Vector3f> ) {
            static_assert( std::is_base_of_v<GT::GTPolicyTRS, SGT> );
            dataTypeHolder.pos = _param;
            return *this;
        }
        if constexpr ( std::is_same_v<M, GT::Rotate> ) {
            static_assert( std::is_base_of_v<GT::GTPolicyTRS, SGT> );
            dataTypeHolder.axis = _param();
            return *this;
        }
        if constexpr ( std::is_same_v<M, GT::Scale> ) {
            static_assert( std::is_base_of_v<GT::GTPolicyTRS, SGT> );
            dataTypeHolder.scale = _param();
            return *this;
        }
        if constexpr ( std::is_same_v<M, GT::Direction> ) {
            static_assert( std::is_base_of_v<GT::GTPolicyFollower, SGT> );
            dataTypeHolder.mFollowerSuggestedAxis = _param();
            return *this;
        }
        if constexpr ( std::is_same_v<M, GT::Z> ) {
            static_assert( std::is_base_of_v<GT::GTPolicyExtrusion, SGT> );
            dataTypeHolder.z = _param();
            return *this;
        }
        if constexpr ( std::is_same_v<M, GT::Flip> ) {
            static_assert( std::is_base_of_v<GT::GTPolicyFollower, SGT> );
            dataTypeHolder.flipVector = _param();
            return *this;
        }
        if constexpr ( std::is_same_v<M, FollowerFlags> ) {
            static_assert( std::is_base_of_v<GT::GTPolicyFollower, SGT> );
            dataTypeHolder.fflags = _param;
            return *this;
        }
        if constexpr ( std::is_same_v<M, PolyRaise> ) {
            static_assert( std::is_base_of_v<GT::GTPolicyFollower, SGT> );
            dataTypeHolder.fraise = _param;
            return *this;
        }
        if constexpr ( std::is_same_v<M, ReverseFlag> ) {
            static_assert( std::is_base_of_v<GT::GTReverseNormals, SGT> );
            dataTypeHolder.rfPoly = _param;
            return *this;
        }
        if constexpr ( std::is_same_v<M, GT::ForceNormalAxis> ) {
            static_assert( std::is_base_of_v<GT::GTPolicyPolyline, SGT> ||
                           std::is_base_of_v<GT::GTPolicyFollower, SGT>);
            if constexpr ( std::is_base_of_v<GT::GTPolicyPolyline, SGT> )
                dataTypeHolder.forcingNormalPoly = _param();
            if constexpr ( std::is_base_of_v<GT::GTPolicyFollower, SGT> )
                dataTypeHolder.mFollowerSuggestedAxis = _param();
            return *this;
        }

        if constexpr ( std::is_same<M, ShapeType>::value ) {
            static_assert( std::is_same<SGT, GT::Shape>::value );
            dataTypeHolder.shapeType = _param;
            return *this;
        }
        if constexpr ( std::is_same<M, PolyOutLine>::value ) {
            static_assert( std::is_same<SGT, GT::Extrude>::value );
            dataTypeHolder.extrusionVerts.emplace_back( _param );
            return *this;
        }
        if constexpr ( std::is_same<M, PolyLine2d>::value ) {
            static_assert( std::is_same<SGT, GT::Extrude>::value );
            dataTypeHolder.extrusionVerts.emplace_back( _param );
            return *this;
        }
        if constexpr ( std::is_same_v<M, std::shared_ptr<Profile>> ) {
            static_assert( std::is_same_v<SGT, GT::Follower> );
            dataTypeHolder.profile = _param;
            return *this;
        }
        if constexpr ( std::is_same<M, std::shared_ptr<Cloth>>::value ) {
            static_assert( std::is_base_of_v<GT::GTPolicyCloth, SGT> );
            dataTypeHolder.cloth = _param;
            return *this;
        }
        if constexpr ( std::is_same<M, std::vector<Vector3f>>::value ) {
            static_assert( std::is_same_v<SGT, GT::Poly> || std::is_same_v<SGT, GT::Follower> );
            if constexpr ( std::is_same_v<SGT, GT::Poly> )
                dataTypeHolder.sourcePolysVList = _param;
            if constexpr ( std::is_same_v<SGT, GT::Follower> )
                dataTypeHolder.profilePath = _param;
            return *this;
        }

        if constexpr ( std::is_same_v<M, std::vector<Vector2f>> ) {
            static_assert( std::is_same_v<SGT, GT::Poly> || std::is_same_v<SGT, GT::Follower> );
            if constexpr ( std::is_same_v<SGT, GT::Poly> ) {
                for ( const auto& v : _param ) {
                    dataTypeHolder.sourcePolysVList.emplace_back( XZY::C(v) );
                }
            }
            if constexpr ( std::is_same_v<SGT, GT::Follower> )
                dataTypeHolder.profilePath2d = _param;
            return *this;
        }

        if constexpr ( std::is_same_v<M, std::vector<Triangle2d>> ) {
            static_assert( std::is_same_v<SGT, GT::Poly> );
            for ( const auto& [v1,v2,v3] : _param ) {
                dataTypeHolder.sourcePolysTris.emplace_back(Triangle3d( v1, v2, v3 ) );
            }
            return *this;
        }

        if constexpr ( std::is_same_v<M, std::vector<Triangle3d>> ) {
            static_assert( std::is_same_v<SGT, GT::Poly> );
            dataTypeHolder.sourcePolysTris = _param;
            return *this;
        }

        if constexpr ( std::is_same_v<M, Rect2f> ) {
            static_assert( std::is_same_v<SGT, GT::Poly> || std::is_same_v<SGT, GT::Follower> );
            if constexpr ( std::is_same_v<SGT, GT::Poly> )
                dataTypeHolder.sourcePolysVList = XZY::C(_param.points3d(0.0f));
            if constexpr ( std::is_same_v<SGT, GT::Follower> )
                for ( auto &v: _param.points3dcw() ) dataTypeHolder.profilePath.emplace_back( v );
            return *this;
        }

        if constexpr ( std::is_same<M, QuadVector3fNormalfList>::value ) {
            static_assert( std::is_same<SGT, GT::Mesh>::value );
            dataTypeHolder.quads = _param;
            return *this;
        }

        if constexpr ( std::is_same_v<M, tinygltf::Model*> ) {
            static_assert( std::is_same_v<SGT, GT::GLTF2> );
            dataTypeHolder.model = _param;
            return *this;
        }

        if constexpr ( std::is_same_v<M, GT::GLTF2PrimitiveIndex> ) {
            static_assert( std::is_same_v<SGT, GT::GLTF2> );
            dataTypeHolder.primitiveIndex = _param();
            return *this;
        }

        if constexpr ( std::is_same_v<M, GT::GLTF2MeshIndex> ) {
            static_assert( std::is_same_v<SGT, GT::GLTF2> );
            dataTypeHolder.meshIndex = _param();
            return *this;
        }

        if constexpr ( std::is_same_v<M, GeomSP> ) {
            elemInjFather = _param;
            return *this;
        }

//        static_assert( std::false_type::value, "VData assembly params type not mapped " );
//        return *this;
    }

    T dataTypeHolder;
    ResourceRef matRef = S::WHITE_PBR;
    Color4f matColor = C4f::WHITE;
    GeomSP elemInjFather = nullptr;
};

namespace VDataServices {

    void prepare( SceneGraph& sg, GT::Shape& _d );
    void buildInternal( const GT::Shape& _d, std::shared_ptr<VData> _ret );
    ResourceRef refName( const GT::Shape& _d );

    void prepare( SceneGraph& sg, GT::Poly& _d );
    void buildInternal( const GT::Poly& _d, std::shared_ptr<VData> _ret );
    ResourceRef refName( const GT::Poly& _d );

    void prepare( SceneGraph& sg, GT::Extrude& _d );
    void buildInternal( const GT::Extrude& _d, std::shared_ptr<VData> _ret );
    ResourceRef refName( const GT::Extrude& _d );

    void prepare( SceneGraph& sg, GT::Text& _d );
    void buildInternal( const GT::Text& _d, std::shared_ptr<VData> _ret );
    ResourceRef refName( const GT::Text& _d );

    void prepare( SceneGraph& sg, GT::Mesh& _d );
    void buildInternal( const GT::Mesh& _d, std::shared_ptr<VData> _ret );
    ResourceRef refName( const GT::Mesh& _d );

    void prepare( SceneGraph& sg, GT::ClothMesh& _d );
    void buildInternal( const GT::ClothMesh& _d, std::shared_ptr<VData> _ret );
    ResourceRef refName( const GT::ClothMesh& _d );

    void prepare( SceneGraph& sg, GT::Follower& _d );
    void buildInternal( const GT::Follower& _d, std::shared_ptr<VData> _ret );
    ResourceRef refName( const GT::Follower& _d );

    void prepare( SceneGraph& sg, GT::GLTF2& _d );
    void buildInternal( const GT::GLTF2& _d, std::shared_ptr<VData> _ret );
    ResourceRef refName( const GT::GLTF2& _d );

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
