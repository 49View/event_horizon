//
// Created by Dado on 08/02/2018.
//

#pragma once

#include <map>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <typeinfo>
#include "core/image_builder.h"
#include "core/command.hpp"
#include "core/serializebin.hpp"
#include "core/suncalc/sun_builder.h"
#include "core/callback_dependency.h"
#include "profile_builder.h"
#include "material_builder.h"
#include "geom_file_asset_builder.h"

class HierGeom;

typedef std::unordered_map<std::string, std::shared_ptr<HierGeom>> AssetHierContainer;
typedef AssetHierContainer::iterator AssetHierContainerIt;
typedef AssetHierContainer::const_iterator AssetHierContainerCIt;

class AssetManager : public DependencyMaker {
public:
DEPENDENCY_MAKER_EXIST(assetsList);
    bool add( GeomFileAssetBuilder& gb, uint8_p&& _data ) {
        assetsList[gb.Name()] = std::make_shared<uint8_p>( std::move(_data) );
        return true;
    }
    void add( const std::string& _key, std::shared_ptr<HierGeom> _h );
    std::shared_ptr<uint8_p> get( const std::string& _key ) { return assetsList[_key]; }
    std::shared_ptr<HierGeom> findHier( const std::string& _key );

    AssetHierContainerIt begin();
    AssetHierContainerIt end();
    AssetHierContainerCIt begin() const;
    AssetHierContainerCIt end() const;

private:
    std::unordered_map<std::string, std::shared_ptr<uint8_p>> assetsList;
    std::unordered_map<std::string, std::shared_ptr<HierGeom>> assetsHierList;
};

class PolySceneGraphTextureList : public ImageDepencencyMaker {
    bool addImpl( [[maybe_unused]] ImageBuilder& tbd, [[maybe_unused]] std::unique_ptr<uint8_t []>& _data ) override { return true; };
};

class SceneGraph;

class CommandScriptSceneGraph : public CommandScript {
public:
    CommandScriptSceneGraph( SceneGraph& hm );
    virtual ~CommandScriptSceneGraph() {}
};

class SceneGraph : public DependencyMaker {
public:
    SceneGraph(CommandQueue& cq);

DEPENDENCY_MAKER_EXIST(geoms);
    void add(std::shared_ptr<HierGeom> _geom);
    void add( const std::vector<std::shared_ptr<MaterialBuilder>> _materials );
    void add(std::shared_ptr<HierGeom> _geom, const std::vector<std::shared_ptr<MaterialBuilder>> _materials);
    void cmdChangeTime( const std::vector<std::string>& _params );
    void cmdChangeMaterialTag( const std::vector<std::string>& _params );
    void cmdChangeMaterialColorTag( const std::vector<std::string>& _params );
    void cmdCreateGeometry( const std::vector<std::string>& _params );
    void cmdLoadObject( const std::vector<std::string>& _params );
    void cmdCalcLightmaps( const std::vector<std::string>& _params );

    size_t countGeoms() const;
    std::vector<std::shared_ptr<HierGeom>> Geoms();
    virtual DependencyMaker& TL() = 0;
    ProfileManager& PL() { return pl; }
    MaterialManager& ML() { return ml; }
    ColorManager& CL() { return cl; }
    AssetManager& AL() { return al; }
    void mapGeomType( const uint64_t _value, const std::string& _key );
    uint64_t getGeomType( const std::string& _key ) const;
protected:
    virtual void addImpl(std::shared_ptr<HierGeom> _geom) = 0;
    virtual void changeTimeImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
    virtual void cmdloadObjectImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
    virtual void cmdCreateGeometryImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
    virtual void changeMaterialTagImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
    virtual void changeMaterialColorTagImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}
    virtual void cmdCalcLightmapsImpl( [[maybe_unused]] const std::vector<std::string>& _params ) {}

protected:
    std::unordered_map<std::string, std::shared_ptr<HierGeom>> geoms;
    AssetManager   al;
    ProfileManager pl;
    MaterialManager ml;
    ColorManager cl;
    SunBuilder sb;
    std::shared_ptr<CommandScriptSceneGraph> hcs;
    std::unordered_map<std::string, uint64_t> geomTypeMap;
};

class PolySceneGraph : public SceneGraph {
public:
    PolySceneGraph(CommandQueue& cq) : SceneGraph(cq) {
        ml.TL(&tl);
    }

    DependencyMaker& TL() override { return tl; }
protected:
    void addImpl(std::shared_ptr<HierGeom> _geom) override;

private:
    PolySceneGraphTextureList tl;
};
