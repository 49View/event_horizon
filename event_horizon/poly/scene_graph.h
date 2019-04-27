//
// Created by Dado on 08/02/2018.
//

#pragma once

#include <map>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <typeinfo>
#include <variant>

#include <core/uuid.hpp>
#include <core/command.hpp>
#include <core/resources/resource_utils.hpp>
#include <poly/poly.hpp>
#include <poly/node_graph.hpp>
#include <poly/resources/geom_builder.h>

class SceneGraph;

class CommandScriptSceneGraph : public CommandScript {
public:
    explicit CommandScriptSceneGraph( SceneGraph& hm );
    virtual ~CommandScriptSceneGraph() = default;
};

class SceneGraph : public NodeGraph {
public:
    explicit SceneGraph( CommandQueue& cq,
                         VDataManager& _vl,
                         ImageManager& _tl,
                         ProfileManager& _pm,
                         MaterialManager& _ml,
                         ColorManager& _cl,
                         FontManager& _fm,
                         CameraManager& _cm,
                         GeomManager& _gm);

    void init();
    UUID addNode( const ResourceRef& _hash );
    void removeNode( const UUID& _uuid );

    void cmdChangeMaterialTag( const std::vector<std::string>& _params );
    void cmdChangeMaterialColorTag( const std::vector<std::string>& _params );
    void cmdCreateGeometry( const std::vector<std::string>& _params );
    void cmdRemoveGeometry( const std::vector<std::string>& _params );
    void cmdLoadObject( const std::vector<std::string>& _params );
    void cmdCalcLightmaps( const std::vector<std::string>& _params );
    void cmdChangeTime( const std::vector<std::string>& params );
    void cmdReloadShaders( const std::vector<std::string>& _params );

    bool rayIntersect( const V3f& _near, const V3f& _far, SceneRayIntersectCallback _callback );

    size_t countGeoms() const;

    ImageManager& TL() { return tl; }
    VDataManager& VL() { return vl; }
    ProfileManager& PL() { return pl; }
    MaterialManager& ML() { return ml; }
    ColorManager& CL() { return cl; }
    CameraManager& CM() { return cm; }
    FontManager& FM() { return fm; }
    ColorManager& MC() { return cl; }
    GeomManager& GM() { return gm; }

    template <typename R>
    auto& M() {
        if constexpr ( std::is_same<R, VData>::value )                  return VL();
        if constexpr ( std::is_same<R, RawImage>::value )               return TL();
        if constexpr ( std::is_same<R, Material>::value )               return ML();
        if constexpr ( std::is_same<R, Utility::TTFCore::Font>::value ) return FM();
        if constexpr ( std::is_same<R, Profile>::value )                return PL();
        if constexpr ( std::is_same<R, MaterialColor>::value )          return MC();
        if constexpr ( std::is_same<R, CameraRig>::value )              return CM();
        if constexpr ( std::is_same<R, Geom>::value )                   return GM();
    }

    template <typename T>
    T B( const std::string& _name ) {
        return T{ *this, _name };
    }

    void mapGeomType( uint64_t _value, const std::string& _key );
    uint64_t getGeomType( const std::string& _key ) const;

    void update();
    NodeGraphContainer& Nodes();

    template <typename T>
    void visitNode( const UUID& _uuid, T _visitor ) {
        if ( auto it = nodes.find(_uuid); it != nodes.end() ) {
            std::visit(_visitor, it->second);
        }
    }

    template <typename T, typename ...Args>
    UUID GB( Args&&... args ) {
        GeomBuilder<T> gb{std::forward<Args>(args)...};
        return buildGeom<T>(gb);
    }

protected:
    template <typename T>
    UUID buildGeom( GeomBuilder<T>& gb ) {
        auto elem   = createGBGeom(gb.Name());
        auto matRef = createGBMatRef(gb.matRef);

        if constexpr ( std::is_same<T, GT::Cube>::value ) {
            createFromProcedural( std::make_shared<GeomDataShapeBuilder>( ShapeType::Cube ), elem, matRef );
        } else if constexpr ( std::is_same<T, GT::Poly>::value ) {
            preparePolyLines( gb );
            createFromProcedural( std::make_shared<GeomDataPolyBuilder>( gb.polyLines ), elem, matRef );
        } else if constexpr ( std::is_same<T, GT::Extrude>::value ) {
            createFromProcedural( std::make_shared<GeomDataOutlineBuilder>( gb.outlineVerts ), elem, matRef );
        }

//        case GeomBuilderType::poly:
//            preparePolyLines();
//            createFromProcedural( std::make_shared<GeomDataPolyBuilder>( polyLines ) );
//            break;
//        case GeomBuilderType::mesh:
//            createFromProcedural( std::make_shared<GeomDataQuadMeshBuilder>( quads ) );
//            break;
//        case GeomBuilderType::follower: {
//            createFromProcedural( std::make_shared<GeomDataFollowerBuilder>( sg.PL().get( mDepResourceName ),
//                                                                             profilePath,
//                                                                             fflags,
//                                                                             fraise,
//                                                                             flipVector,
//                                                                             mGaps,
//                                                                             mFollowerSuggestedAxis ) );
//        }
//            break;
//        case GeomBuilderType::svg:
//            createFromProcedural( std::make_shared<GeomDataSVGBuilder>( asciiText, sg.PL().get( mDepResourceName ) ) );
//            break;
//        case GeomBuilderType::unknown:
//            LOGE( "Unknown builder type" );
//            return {};
//        default:
//            break;
//    }
        return finaliseGB( elem, gb.elemInjFather, gb.Name(), gb.pos, gb.axis, gb.scale );
    }

    void internalCheckPolyNormal( Vector3f& ln, const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, ReverseFlag rf ) {
        if ( ln == Vector3f::ZERO ) {
            ln = normalize( crossProduct( v1, v2, v3 ));
            if ( rf == ReverseFlag::True ) ln *= -1.0f;
        }
    }

    template <typename T>
    void preparePolyLines( GeomBuilder<T>& gb ) {
        if ( gb.polyLines.empty() ) {
            Vector3f ln = gb.forcingNormalPoly;
            if ( !gb.sourcePolysTris.empty() ) {
                auto [v1,v2,v3] = gb.sourcePolysTris[0];
                internalCheckPolyNormal( ln, v1, v2, v3, gb.rfPoly );
                for ( const auto& tri : gb.sourcePolysTris ) {
                    gb.polyLines.emplace_back(PolyLine{ tri, ln, gb.rfPoly});
                }
            }
            if ( !gb.sourcePolysVList.empty() ) {
                internalCheckPolyNormal( ln, gb.sourcePolysVList.at(0), gb.sourcePolysVList.at(1), gb.sourcePolysVList.at(2), gb.rfPoly );
                gb.polyLines.emplace_back( PolyLine{ gb.sourcePolysVList, ln, gb.rfPoly } );
            }
        }
    }

    GeomSP createGBGeom( const std::string& _name );
    ResourceRef createGBMatRef( const ResourceRef& _sourceMatRef );
    void createFromProcedural( std::shared_ptr<GeomDataBuilder> gb, GeomSP elem, const ResourceRef& matRef );
    UUID finaliseGB( const GeomSP& elem, GeomSP elemInjFather, const ResourceRef& gbName,
                     const Vector3f& pos, const Vector3f& rot, const Vector3f& scale );

//    virtual void cmdChangeTimeImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
//    virtual void cmdloadObjectImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
//    virtual void cmdCreateGeometryImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
//    virtual void cmdRemoveGeometryImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
//    virtual void changeMaterialTagImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
//    virtual void changeMaterialColorTagImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
//    virtual void cmdCalcLightmapsImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}

protected:
    VDataManager& vl;
    ImageManager& tl;
    ProfileManager& pl;
    MaterialManager& ml;
    ColorManager& cl;
    FontManager& fm;
    CameraManager& cm;
    GeomManager& gm;

    std::shared_ptr<CommandScriptSceneGraph> hcs;
};
