//
// Created by Dado on 08/02/2018.
//

#include "scene_graph.h"
#include "core/node.hpp"
#include <poly/geom_builder.h>
#include <poly/ui_shape_builder.h>

void SceneGraph::add( NodeVariants _geom ) {
    geoms[std::visit(lambdaUUID, _geom)] = _geom;
    addImpl(_geom);
    geoms[std::visit(lambdaUUID, _geom)] = _geom;
}

void SceneGraph::remove( const UUID& _uuid ) {
    if ( auto it = geoms.find(_uuid); it != geoms.end() ) {
        // Remove all child
        removeImpl(_uuid);
        geoms.erase( it );
    }
}

void SceneGraph::update() {
    for ( auto& [k,v] : geoms ) {
        std::visit( lambdaUpdateAnimVisitor, v );
    }
    updateImpl();
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

void SceneGraph::cmdRemoveGeometry( const std::vector<std::string>& _params ) {
    cmdRemoveGeometryImpl( _params );
}

void SceneGraph::cmdLoadObject( const std::vector<std::string>& _params ) {
    cmdloadObjectImpl( _params );
}

void SceneGraph::cmdCalcLightmaps( const std::vector<std::string>& _params ) {
    cmdCalcLightmapsImpl( _params );
}

SceneGraph::SceneGraph( CommandQueue& cq, FontManager& _fm, SunBuilder& _sb ) : fm(_fm), sb(_sb) {
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

bool SceneGraph::rayIntersect( const V3f& _near, const V3f& _far, SceneRayIntersectCallback _callback ) {

    bool ret = false;

    for ( const auto& [k, n] : geoms ) {

        AABB box = AABB::INVALID;
        UUID uuid{};
        bool bPerformeOnNode = false;
        if ( auto as = std::get_if<GeomAssetSP>(&n); as != nullptr ) {
            box = (*as)->BBox3d();
            uuid = (*as)->Hash();
            bPerformeOnNode = true;
        } else if ( auto as = std::get_if<UIAssetSP>(&n); as != nullptr ) {
            box = (*as)->BBox3d();
            uuid = (*as)->Hash();
            bPerformeOnNode = true;
        }

        if ( bPerformeOnNode ) {
            float tn = 0.0f;
            float tf = std::numeric_limits<float>::max();
            auto ldir = normalize( _far - _near );
            if ( box.intersectLine( _near, ldir, tn, tf) ) {
                _callback( n, tn );
                ret = true;
                break;
            }
        }
    }

    return ret;
}

CommandScriptSceneGraph::CommandScriptSceneGraph( SceneGraph& _hm ) {
    addCommandDefinition("change material", std::bind(&SceneGraph::cmdChangeMaterialTag, &_hm, std::placeholders::_1 ));
    addCommandDefinition("paint", std::bind(&SceneGraph::cmdChangeMaterialColorTag, &_hm, std::placeholders::_1 ));
    addCommandDefinition("add", std::bind(&SceneGraph::cmdCreateGeometry, &_hm, std::placeholders::_1));
    addCommandDefinition("remove", std::bind(&SceneGraph::cmdRemoveGeometry, &_hm, std::placeholders::_1));
    addCommandDefinition("load object", std::bind(&SceneGraph::cmdLoadObject, &_hm, std::placeholders::_1));
    addCommandDefinition("lightmaps", std::bind(&SceneGraph::cmdCalcLightmaps, &_hm, std::placeholders::_1));
}
