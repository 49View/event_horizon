//
// Created by Dado on 2018-10-29.
//

#include "geom_layout.h"
#include <graphics/imgui/imgui_jsonvisit.hpp>
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/render_orchestrator.h>
#include <render_scene_graph/layouts/layout_helper.hpp>
#include <core/node.hpp>
#include <core/geom.hpp>
#include <core/camera.h>
#include <core/raw_image.h>
#include <core/geom.hpp>
#include <core/camera_rig.hpp>
#include <poly/resources/geom_builder.h>
#include <poly/resources/ui_shape_builder.h>
#include <poly/converters/gltf2/gltf2.h>

//std::shared_ptr<GeomBuilder> gbt;
std::string svgString;
std::string svgName;
std::string glftString;
SerializableContainer gltfBufferData;

void loadGeomInGui( SceneGraph& _sg, RenderOrchestrator& _rsg, std::shared_ptr<GLTF2> _newObject ) {

    // convert returns a vector of scenes read from GLTF file
    for ( const auto& scene : _newObject->convert() ) {
        auto geom = std::make_shared<Geom>(scene->Name());
    }
}

void addGeomToScene() {
//    SceneOrchestrator::sUpdateCallbacks.emplace_back( []( SceneOrchestrator* p ) {
//        loadGeomInGui( p, std::make_shared<GLTF2>( glftString ) );
//    } );
}

void addGeomToSceneData() {
//    SceneOrchestrator::sUpdateCallbacks.emplace_back( []( SceneOrchestrator* p ) {
//        loadGeomInGui( p, std::make_shared<GLTF2>( gltfBufferData, glftString ) );
//        gltfBufferData.clear();
//        glftString = "";
//    } );
}

void callbackGeom( const std::string& _filename, const SerializableContainer& _data ) {
    glftString = _filename;
    gltfBufferData = _data;
    addGeomToSceneData();
}

void callbackGeomGLTF( const std::string& _filename ) {
    glftString = _filename;
    addGeomToScene();
}

void callbackGeomSVG( const std::string& _filename, const std::string& _svgString ) {
    svgString = _svgString;
    svgName = getFileNameOnly(_filename);
//    SceneOrchestrator::sUpdateCallbacks.emplace_back( [&]( SceneOrchestrator* p ) {
//        GB{p->SG(),GeomBuilderType::svg}.n(svgName).ascii(svgString).pb(0.015f, 6.0f).buildr();
//    } );
}

void ImGuiGeoms::renderImpl( SceneGraph& _sg, RenderOrchestrator& _rsg, Rect2f& _r ) {
}
