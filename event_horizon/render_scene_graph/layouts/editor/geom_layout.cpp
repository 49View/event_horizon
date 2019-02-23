//
// Created by Dado on 2018-10-29.
//

#include "geom_layout.h"
#include <graphics/imgui/imgui_jsonvisit.hpp>
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene_orchestrator.hpp>
#include <render_scene_graph/layouts/layout_helper.hpp>
#include "core/node.hpp"
#include "poly/geom_builder.h"
#include "poly/ui_shape_builder.h"
#include "poly/converters/gltf2/gltf2.h"

//std::shared_ptr<GeomBuilder> gbt;
std::shared_ptr<GLTF2> gltf;
std::string svgString;
std::string svgName;
std::string glftString;
SerializableContainer gltfBufferData;

template <typename T>
struct NodeVisitor {
    void operator()( GeomAssetSP _v ) { _v->visit<T>(); }
    void operator()( UIAssetSP _v ) { _v->visit<T>(); }
};

void loadGeomInGui( SceneOrchestrator* p, std::shared_ptr<GLTF2> _newObject ) {
    auto imported = _newObject->convert();
    auto hierScene = imported.getScene();
    p->getCamera(Name::Foxtrot)->center(hierScene->BBox3d());

    p->RSG().add( imported.getMaterials() );
    p->RSG().add( hierScene );

//    auto gbt = std::make_shared<GeomBuilder>( hierScene );
//    gbt->build(p->RSG());

//    p->takeScreenShot( hierScene->BBox3d(), gbt->Thumb() );
}

void addGeomToScene() {
    SceneOrchestrator::sUpdateCallbacks.emplace_back( []( SceneOrchestrator* p ) {
        loadGeomInGui( p, std::make_shared<GLTF2>( glftString ) );
    } );
}

void addGeomToSceneData() {
    SceneOrchestrator::sUpdateCallbacks.emplace_back( []( SceneOrchestrator* p ) {
        loadGeomInGui( p, std::make_shared<GLTF2>( gltfBufferData, glftString ) );
    } );
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
    SceneOrchestrator::sUpdateCallbacks.emplace_back( [&]( SceneOrchestrator* p ) {
        GB{GeomBuilderType::svg}.n(svgName).ascii(svgString).pb(ProfileBuilder{0.015f, 6.0f}).buildr(p->RSG());
    } );

}

void ImGuiGeoms::renderImpl( SceneOrchestrator* p, Rect2f& _r ) {
    for ( auto& [k,v] : p->RSG().Nodes() ) {
        ImGui::PushID(std::visit(lambdaUUID, v).c_str());
        ImGui::BeginGroup();
        std::visit( NodeVisitor<ImGUIJson>{}, v );
        if ( ImGui::Button( "Save", ImVec2( 80, 20 ))) {
            VisitLambda( publish, v );
        }
        ImGui::EndGroup();
        ImGui::PopID();
    }

//    ImGui::EndChild();

//    if ( gbt ) {
//        ImGui::BeginGroup();
//        ImGui::Text( "Name: %s", gbt->Name().c_str());
//        ImGui::EndGroup();
//    }
}
