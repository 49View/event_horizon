//
// Created by Dado on 08/02/2018.
//

#include "scene_graph.h"
#include "core/node.hpp"
#include <poly/geom_builder.h>
#include <poly/ui_shape_builder.h>

auto lambdaUpdateAnimVisitor = [](auto&& arg) { return arg->updateAnim();};
auto lambdaUUID = [](auto&& arg) -> UUID { return arg->Hash();};

void SceneGraph::add( NodeVariants _geom ) {
    addImpl(_geom);
    geoms[std::visit(lambdaUUID, _geom)] = _geom;
}

void SceneGraph::add( const std::vector<std::shared_ptr<MaterialBuilder>> _materials ) {
    for ( const auto& m : _materials ) {
        m->makeDirect( ML() );
    }
}

void SceneGraph::add( GeomAssetSP _geom, const std::vector<std::shared_ptr<MaterialBuilder>> _materials ) {
    for ( const auto& m : _materials ) {
        m->makeDirect( ML() );
    }
    add( _geom );
}

void SceneGraph::update() {
//    if ( mi.isMouseTouchedDown ) {
//        mousePickRay( mi.mousePos, mRayNear, mRayFar );
//        AABB box{ Vector3f::ONE*-0.5f, Vector3f::ONE*0.5f};
//        float tn = std::numeric_limits<float>::lowest();
//        float tf = std::numeric_limits<float>::max();
//        bool bi = box.intersectLine( mRayNear, mRayFar, tn, tf);
//        LOGR( "Camera Ray Near: %s", mRayNear.toString().c_str() );
//        LOGR( "Camera Ray Far: %s", mRayFar.toString().c_str() );
//        if ( bi ) {
//            LOGR( "Intersect: %f, %f", tn, tf );
//
//        }
//    }

    for ( auto& [k,v] : geoms ) {
        std::visit( lambdaUpdateAnimVisitor, v );
    }
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

SceneGraph::SceneGraph( CommandQueue& cq, FontManager& _fm ) : fm(_fm) {
    hcs = std::make_shared<CommandScriptSceneGraph>(*this);
    cq.registerCommandScript(hcs);
    mapGeomType(0, "none");
    mapGeomType(1, "generic");
}

size_t SceneGraph::countGeoms() const {
    return geoms.size();
}

void SceneGraph::mapGeomType( const uint64_t _value, const std::string& _key ) {
    geomTypeMap[_key] = _value;
}

uint64_t SceneGraph::getGeomType( const std::string& _key ) const {
    if ( auto ret = geomTypeMap.find(_key); ret != geomTypeMap.end() ) {
        return ret->second;
    }
    try {
        return static_cast<uint64_t>(std::stoi( _key ));
    }
    catch(...) {
        return 0;
    }
    return 0;
}

NodeGraph& SceneGraph::Nodes() {
    return geoms;
}

void PolySceneGraph::addImpl( [[maybe_unused]] NodeVariants _geom ) {
}

void AssetManager::add( [[maybe_unused]] const std::string& _key, GeomAssetSP _h ) {
    assetsHierList[_h->Name()] = _h;
}

GeomAssetSP AssetManager::findHier( const std::string& _key ) {
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

bool AssetManager::add( GeomFileAssetBuilder& gb, const std::vector<char>& _data ) {
    assetsList[gb.Name()] = _data;
    return true;
}

CommandScriptSceneGraph::CommandScriptSceneGraph( SceneGraph& _hm ) {
    addCommandDefinition("change material", std::bind(&SceneGraph::cmdChangeMaterialTag, &_hm, std::placeholders::_1 ));
    addCommandDefinition("paint", std::bind(&SceneGraph::cmdChangeMaterialColorTag, &_hm, std::placeholders::_1 ));
    addCommandDefinition("build", std::bind(&SceneGraph::cmdCreateGeometry, &_hm, std::placeholders::_1));
    addCommandDefinition("load object", std::bind(&SceneGraph::cmdLoadObject, &_hm, std::placeholders::_1));
    addCommandDefinition("lightmaps", std::bind(&SceneGraph::cmdCalcLightmaps, &_hm, std::placeholders::_1));
}
