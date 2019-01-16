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

typedef std::unordered_map<std::string, GeomAssetSP> AssetHierContainer;
typedef AssetHierContainer::iterator AssetHierContainerIt;
typedef AssetHierContainer::const_iterator AssetHierContainerCIt;

struct GeomFileAssetBuilder;

class AssetManager : public DependencyMaker {
public:
DEPENDENCY_MAKER_EXIST(assetsList);
    bool add( GeomFileAssetBuilder& gb, const std::vector<char>& _data );
    void add( const std::string& _key, GeomAssetSP _h );
    std::vector<char> get( const std::string& _key ) { return assetsList[_key]; }
    GeomAssetSP findHier( const std::string& _key );

    AssetHierContainerIt begin();
    AssetHierContainerIt end();
    AssetHierContainerCIt begin() const;
    AssetHierContainerCIt end() const;

private:
    std::unordered_map<std::string, std::vector<char>> assetsList;
    AssetHierContainer assetsHierList;
};

class PolySceneGraphTextureList : public ImageDepencencyMaker {
    bool addImpl( [[maybe_unused]] ImageBuilder& tbd, [[maybe_unused]] std::unique_ptr<uint8_t []>& _data ) override { return true; };
};

using NodeVariants = std::variant<GeomAssetSP, UIAssetSP>;
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
    explicit SceneGraph( CommandQueue& cq, FontManager& _fm );

DEPENDENCY_MAKER_EXIST(geoms);
    void add( NodeVariants _geom);
    void add( GeomAssetSP _geom, std::vector<std::shared_ptr<MaterialBuilder>> _materials);

    void add( std::vector<std::shared_ptr<MaterialBuilder>> _materials );
    void cmdChangeMaterialTag( const std::vector<std::string>& _params );
    void cmdChangeMaterialColorTag( const std::vector<std::string>& _params );
    void cmdCreateGeometry( const std::vector<std::string>& _params );
    void cmdLoadObject( const std::vector<std::string>& _params );
    void cmdCalcLightmaps( const std::vector<std::string>& _params );

    void rayIntersect( const V3f& _near, const V3f& _far, SceneRayIntersectCallback _callback );

    size_t countGeoms() const;
    virtual DependencyMaker& TL() = 0;
    ProfileManager& PL() { return pl; }
    MaterialManager& ML() { return ml; }
    ColorManager& CL() { return cl; }
    AssetManager& AL() { return al; }
    SunBuilder& SB() { return sb; }
    FontManager& FM() { return fm; }
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
    virtual void addImpl( NodeVariants _geom) = 0;
    virtual void changeTimeImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
    virtual void cmdloadObjectImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
    virtual void cmdCreateGeometryImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
    virtual void changeMaterialTagImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
    virtual void changeMaterialColorTagImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
    virtual void cmdCalcLightmapsImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}

protected:
    NodeGraph geoms;
    AssetManager   al;
    ProfileManager pl;
    MaterialManager ml;
    ColorManager cl;
    SunBuilder sb;
    FontManager& fm;
    std::shared_ptr<CommandScriptSceneGraph> hcs;
    std::unordered_map<std::string, uint64_t> geomTypeMap;
};

class PolySceneGraph : public SceneGraph {
public:
    PolySceneGraph(CommandQueue& cq, FontManager& _fm) : SceneGraph(cq, _fm) {
        ml.TL(&tl);
    }

    DependencyMaker& TL() override { return tl; }
protected:
    void addImpl( NodeVariants _geom) override;

private:
    PolySceneGraphTextureList tl;
};
