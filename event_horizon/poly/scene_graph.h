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

protected:
//    virtual void addImpl( NodeVariants _geom) {};
//    virtual void removeImpl( const UUID& _uuid ) {};
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
