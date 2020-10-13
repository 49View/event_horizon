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
#include <core/resources/material_and_color_property.hpp>
#include <poly/poly_services.hpp>
#include <poly/poly.hpp>

class Profile;
class SceneGraph;
struct OSMData;

using GeomDataListBuilderRetType = std::vector<std::shared_ptr<VData>>;

struct V2nff {
    V2f v2;
    V3f normal;
    float f;
};

template <typename T>
class VDataAssembler : public NamePolicy<> {
public:
    template<typename ...Args>
    explicit VDataAssembler( Args&& ... args ) {
        Name(UUIDGen::make());
        (addParam<T>( std::forward<Args>( args )), ...); // Fold expression (c++17)
    }

    virtual ~VDataAssembler() = default;

    template<typename SGT, typename M>
    VDataAssembler& addParam( const M& _param ) {

        if constexpr ( std::is_same_v<M, GT::M> ) {
            matRef = _param();
            return *this;
        }
        if constexpr ( std::is_same_v<M, MaterialAndColorProperty> ) {
            matRef = _param.materialHash;
            matColor = _param.color;
            return *this;
        }
        if constexpr ( std::is_same_v<M, GT::Program> ) {
            programRef = _param();
            return *this;
        }
        if constexpr ( std::is_same_v<M, GT::A> ) {
            matColor.setW( _param() );
            return *this;
        }
        if constexpr ( std::is_same_v<M, GT::Tag> ) {
            tag = _param();
            return *this;
        }
        if constexpr ( std::is_same_v<M, GT::Bucket> ) {
            bucketIndex = _param();
            return *this;
        }
        if constexpr ( std::is_same_v<M, std::string> || std::is_same_v<std::decay_t<M>, char*>) {
            if constexpr ( std::is_same_v<SGT, GT::Text> )
                dataTypeHolder.text = _param;
            if constexpr ( std::is_same_v<SGT, GT::Asset> || std::is_same_v<SGT, GT::GLTF2> )
                dataTypeHolder.nameId = _param;
            Name( _param );
            return *this;
        }
        if constexpr ( std::is_same_v<M, Color4f> ) {
            static_assert( std::is_base_of_v<GT::GTPolicyColor, SGT> );
            dataTypeHolder.color = _param;
            matColor = _param;
            return *this;
        }
        if constexpr ( std::is_same_v<M, Vector2f> ) {
            if constexpr ( std::is_same_v<SGT, GT::OSMBuildings> || std::is_same_v<SGT, GT::OSMTile> ) {
                dataTypeHolder.locationLatLon = _param;
            }
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
        if constexpr ( std::is_same_v<M, AABB> ) {
            static_assert( std::is_same_v<GT::Shape, SGT> );
            dataTypeHolder.aabb = _param;
            return *this;
        }
        if constexpr ( std::is_same_v<M, MatrixAnim> ) {
            static_assert( std::is_base_of_v<GT::GTPolicyTRS, SGT> );
            mTRS = _param;
            return *this;
        }
        if constexpr ( std::is_same_v<M, GeomMappingData> ) {
            static_assert( std::is_base_of_v<GT::GTPolicyMapping, SGT> );
            dataTypeHolder.mappingData = _param;
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
        if constexpr ( std::is_same_v<M, GT::FlipNormal> ) {
            static_assert( std::is_base_of_v<GT::GTPolicyModifiers, SGT> );
            dataTypeHolder.flipNormals = _param();
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
        if constexpr ( std::is_same<M, std::vector<PolyOutLine>>::value ) {
            static_assert( std::is_same<SGT, GT::Extrude>::value );
            for ( const auto& ol : _param ) {
                dataTypeHolder.extrusionVerts.emplace_back( ol );
            }
            return *this;
        }
        if constexpr ( std::is_same<M, V2nff>::value ) {
            static_assert( std::is_same<SGT, GT::Extrude>::value );
            dataTypeHolder.extrusionVerts.emplace_back(
                    PolyOutLine{ Rect2f{_param.v2, RectV2f::Centered }.points3dcw_xzy(), _param.normal, _param.f} );
            return *this;
        }
        if constexpr ( std::is_same<M, std::vector<PolyLine2d>>::value ) {
            static_assert( std::is_base_of_v<GT::GTPolicyPolyline, SGT> );
            dataTypeHolder.sourcePolylines2d = _param;
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
        if constexpr ( std::is_same<M, QuadVector3fNormal>::value ) {
            static_assert( std::is_same<SGT, GT::Mesh>::value );
            dataTypeHolder.quads.emplace_back(_param);
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

        if constexpr ( std::is_same_v<M, OSMData*> ) {
            static_assert( std::is_same_v<SGT, GT::OSMBuildings> || std::is_same_v<SGT, GT::OSMTile> );
            dataTypeHolder.osmData = _param;
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
    ResourceRef programRef = S::SH;
    ResourceRef matRef = S::WHITE_PBR;
    Color4f matColor = C4fc::WHITE;
    uint64_t tag = 0;
    int bucketIndex = GTBucket::Near;
    MatrixAnim mTRS;
    GeomSP elemInjFather = nullptr;
};

namespace VDataServices {

    bool prepare( SceneGraph& sg, GT::Shape& _d, Material* matPtr );
    void buildInternal( const GT::Shape& _d, const std::shared_ptr<VData>& _ret );
    ResourceRef refName( const GT::Shape& _d );

    bool prepare( SceneGraph& sg, GT::Poly& _d, Material* matPtr );
    void buildInternal( const GT::Poly& _d, const std::shared_ptr<VData>& _ret );
    ResourceRef refName( const GT::Poly& _d );

    bool prepare( SceneGraph& sg, GT::Extrude& _d, Material* matPtr );
    void buildInternal( const GT::Extrude& _d, const std::shared_ptr<VData>& _ret );
    ResourceRef refName( const GT::Extrude& _d );

    bool prepare( SceneGraph& sg, GT::Text& _d, Material* matPtr );
    void buildInternal( const GT::Text& _d, const std::shared_ptr<VData>& _ret );
    ResourceRef refName( const GT::Text& _d );

    bool prepare( SceneGraph& sg, GT::Mesh& _d, Material* matPtr );
    void buildInternal( const GT::Mesh& _d, const std::shared_ptr<VData>& _ret );
    ResourceRef refName( const GT::Mesh& _d );

    bool prepare( SceneGraph& sg, GT::ClothMesh& _d, Material* matPtr );
    void buildInternal( const GT::ClothMesh& _d, const std::shared_ptr<VData>& _ret );
    ResourceRef refName( const GT::ClothMesh& _d );

    bool prepare( SceneGraph& sg, GT::Follower& _d, Material* matPtr );
    void buildInternal( const GT::Follower& _d, std::shared_ptr<VData> _ret );
    ResourceRef refName( const GT::Follower& _d );

    bool prepare( SceneGraph& sg, GT::GLTF2& _d, Material* matPtr );
    void buildInternal( const GT::GLTF2& _d, std::shared_ptr<VData> _ret );
    ResourceRef refName( const GT::GLTF2& _d );

    bool prepare( SceneGraph& sg, GT::OSMTile& _d, Material* matPtr );
    void buildInternal( const GT::OSMTile& _d, const std::shared_ptr<VData>& _ret );
    ResourceRef refName( const GT::OSMTile& _d );

    bool prepare( SceneGraph& sg, GT::OSMBuildings& _d, Material* matPtr );
    void buildInternal( const GT::OSMBuildings& _d, const std::shared_ptr<VData>& _ret );
    ResourceRef refName( const GT::OSMBuildings& _d );

    bool prepare( SceneGraph& sg, GT::Asset& _d, Material* matPtr );
    void buildInternal( const GT::Asset& _d, const std::shared_ptr<VData>& _ret );
    ResourceRef refName( const GT::Asset& _d );

    template <typename DT>
    std::shared_ptr<VData> build( const DT& _d ) {
        auto ret = std::make_shared<VData>();
        buildInternal( _d, ret);
        return ret;
    }
}
