//
// Created by Dado on 08/02/2018.
//

#include "scene_graph.h"
#include "hier_geom.hpp"

void SceneGraph::add( std::shared_ptr<HierGeom> _geom ) {
    addImpl(_geom);
    geoms[std::to_string(_geom->Hash())] = _geom;
}

void SceneGraph::add( const std::vector<std::shared_ptr<MaterialBuilder>> _materials ) {
    for ( const auto& m : _materials ) {
        m->makeDirect( ML() );
    }
}

void
SceneGraph::add( std::shared_ptr<HierGeom> _geom, const std::vector<std::shared_ptr<MaterialBuilder>> _materials ) {
    for ( const auto& m : _materials ) {
        m->makeDirect( ML() );
    }
    add( _geom );
}

void SceneGraph::cmdChangeTime( const std::vector<std::string>& _params ) {
    sb.buildFromString( concatenate( " ", {_params.begin(), _params.end()}) );
    changeTimeImpl( _params );
}

void SceneGraph::cmdChangeMaterialTag( const std::vector<std::string>& _params ) {
    changeMaterialTagImpl( _params );
}

void SceneGraph::cmdChangeMaterialColorTag( const std::vector<std::string>& _params ) {
    changeMaterialColorTagImpl( _params );
}

void SceneGraph::cmdCreateGeometry( const std::vector<std::string>& _params ) {
    cmdCreateGeometryImpl( _params );
}

void SceneGraph::cmdLoadObject( const std::vector<std::string>& _params ) {
    cmdloadObjectImpl( _params );
}

void SceneGraph::cmdCalcLightmaps( const std::vector<std::string>& _params ) {
    cmdCalcLightmapsImpl( _params );
}

SceneGraph::SceneGraph(CommandQueue& cq) {
    hcs = std::make_shared<CommandScriptSceneGraph>(*this);
    cq.registerCommandScript(hcs);
    mapGeomType(0, "none");
    mapGeomType(1, "generic");
}

size_t SceneGraph::countGeoms() const {
    return geoms.size();
}

std::vector<std::shared_ptr<HierGeom>> SceneGraph::Geoms() {
    std::vector<std::shared_ptr<HierGeom>> ret;
    for ( auto& [k,v] : geoms ) ret.emplace_back( v );
    return ret;
}

void SceneGraph::mapGeomType( const uint64_t _value, const std::string& _key ) {
    geomTypeMap[_key] = _value;
}

uint64_t SceneGraph::getGeomType( const std::string& _key ) const {
    if ( auto ret = geomTypeMap.find(_key); ret != geomTypeMap.end() ) {
        return ret->second;
    }
    try {
        return std::stoi( _key );
    }
    catch(...) {
        return 0;
    }
    return 0;
}

void PolySceneGraph::addImpl( [[maybe_unused]] std::shared_ptr<HierGeom> _geom ) {
}

void AssetManager::add( [[maybe_unused]] const std::string& _key, std::shared_ptr<HierGeom> _h ) {
    assetsHierList[_h->Name()] = _h;
}

std::shared_ptr<HierGeom> AssetManager::findHier( const std::string& _key ) {
    if ( auto h = assetsHierList.find(_key); h != assetsHierList.end() ) {
        return h->second;
    }
    return nullptr;
}

AssetHierContainerIt AssetManager::begin() {
    return assetsHierList.begin();
}

AssetHierContainerIt AssetManager::end() {
    return assetsHierList.end();
}

AssetHierContainerCIt AssetManager::begin() const {
    return assetsHierList.cbegin();
}

AssetHierContainerCIt AssetManager::end() const {
    return assetsHierList.cend();
}

CommandScriptSceneGraph::CommandScriptSceneGraph( SceneGraph& _hm ) {
    addCommandDefinition("change time", std::bind(&SceneGraph::cmdChangeTime, &_hm, std::placeholders::_1 ));
    addCommandDefinition("change material", std::bind(&SceneGraph::cmdChangeMaterialTag, &_hm, std::placeholders::_1 ));
    addCommandDefinition("paint", std::bind(&SceneGraph::cmdChangeMaterialColorTag, &_hm, std::placeholders::_1 ));
    addCommandDefinition("build", std::bind(&SceneGraph::cmdCreateGeometry, &_hm, std::placeholders::_1));
    addCommandDefinition("load object", std::bind(&SceneGraph::cmdLoadObject, &_hm, std::placeholders::_1));
    addCommandDefinition("lightmaps", std::bind(&SceneGraph::cmdCalcLightmaps, &_hm, std::placeholders::_1));
}
