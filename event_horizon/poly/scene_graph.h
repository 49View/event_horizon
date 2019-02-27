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
#include "core/image_builder.h"
#include "core/uuid.hpp"
#include "core/command.hpp"
#include "core/font_manager.h"
#include "core/serializebin.hpp"
#include "core/suncalc/sun_builder.h"
#include "core/callback_dependency.h"
#include "profile_builder.h"
#include "material_builder.h"
#include <poly/poly.hpp>

class StreamingMediator;

class PolySceneGraphTextureList : public ImageDepencencyMaker {
    bool addImpl( [[maybe_unused]] ImageBuilder& tbd, [[maybe_unused]] std::unique_ptr<uint8_t []>& _data ) override { return true; };
};

using NodeGraph = std::unordered_map<std::string, NodeVariants>;
using SceneRayIntersectCallback = std::function<void(const NodeVariants&, float)>;

class SceneGraph;

class CommandScriptSceneGraph : public CommandScript {
public:
    explicit CommandScriptSceneGraph( SceneGraph& hm );
    virtual ~CommandScriptSceneGraph() = default;
};

class SceneGraph : public DependencyMaker {
public:
    explicit SceneGraph( CommandQueue& cq, FontManager& _fm, SunBuilder& _sb );

DEPENDENCY_MAKER_EXIST(geoms);
    void add( NodeVariants _geom);

    void remove( const UUID& _uuid );

    void cmdChangeMaterialTag( const std::vector<std::string>& _params );
    void cmdChangeMaterialColorTag( const std::vector<std::string>& _params );
    void cmdCreateGeometry( const std::vector<std::string>& _params );
    void cmdRemoveGeometry( const std::vector<std::string>& _params );
    void cmdLoadObject( const std::vector<std::string>& _params );
    void cmdCalcLightmaps( const std::vector<std::string>& _params );

    bool rayIntersect( const V3f& _near, const V3f& _far, SceneRayIntersectCallback _callback );

    size_t countGeoms() const;
    virtual DependencyMaker& TL() = 0;
    ProfileManager& PL() { return pl; }
    MaterialManager& ML() { return ml; }
    ColorManager& CL() { return cl; }
    FontManager& FM() { return fm; }
    SunBuilder& SB() { return sb; }

    void mapGeomType( uint64_t _value, const std::string& _key );
    uint64_t getGeomType( const std::string& _key ) const;

    void update();
    NodeGraph& Nodes();

    template <typename T>
    void visitNode( const UUID& _uuid, T _visitor ) {
        if ( auto it = geoms.find(_uuid); it != geoms.end() ) {
            std::visit(_visitor, it->second);
        }
    }

protected:
    virtual void updateImpl() {};
    virtual void addImpl( NodeVariants _geom) {};
    virtual void removeImpl( const UUID& _uuid ) {};
    virtual void changeTimeImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
    virtual void cmdloadObjectImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
    virtual void cmdCreateGeometryImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
    virtual void cmdRemoveGeometryImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
    virtual void changeMaterialTagImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
    virtual void changeMaterialColorTagImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
    virtual void cmdCalcLightmapsImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}

protected:
    NodeGraph geoms;
    ProfileManager pl;
    MaterialManager ml;
    ColorManager cl;
    FontManager& fm;
    SunBuilder& sb;

    std::shared_ptr<CommandScriptSceneGraph> hcs;
    std::unordered_map<std::string, uint64_t> geomTypeMap;
};

class PolySceneGraph : public SceneGraph {
public:
    PolySceneGraph(CommandQueue& cq, FontManager& _fm, SunBuilder& _sb) : SceneGraph(cq, _fm, _sb) {
        ml.TL(&tl);
    }

    DependencyMaker& TL() override { return tl; }
private:
    PolySceneGraphTextureList tl;
};
