//
// Created by Dado on 2018-10-29.
//

#include "geom_layout.h"
#include <graphics/imgui/imgui_jsonvisit.hpp>
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene.hpp>
#include <render_scene_graph/layouts/layout_helper.hpp>
#include "core/node.hpp"
#include "poly/geom_builder.h"
#include "poly/ui_shape_builder.h"
#include "poly/converters/gltf2/gltf2.h"

std::shared_ptr<GeomBuilder> gbt;
std::shared_ptr<GLTF2> gltf;
std::string svgString;

template <typename T>
struct NodeVisitor {
    void operator()( GeomAssetSP _v ) { _v->visit<T>(); }
    void operator()( UIAssetSP _v ) { _v->visit<T>(); }
};

void loadGeomInGui( Scene* p, std::shared_ptr<GLTF2> _newObject ) {
    auto hierScene = _newObject->convert();
    p->getCamera(Name::Foxtrot)->center(hierScene->BBox3d());
    gbt = std::make_shared<GeomBuilder>( hierScene, _newObject->Materials() );
    gbt->build(p->RSG());
    p->takeScreenShot( hierScene->BBox3d(), gbt->Thumb() );
}

void addGeomToScene() {
    Scene::sUpdateCallbacks.emplace_back( []( Scene* p ) {
        loadGeomInGui( p, gltf );
    } );
}

void callbackGeom( const std::string& _filename, const std::vector<char>& _data ) {
    gltf = std::make_shared<GLTF2>( _data, _filename );
    addGeomToScene();
}

void callbackGeomGLTF( const std::string& _filename ) {
    gltf = std::make_shared<GLTF2>( _filename );
    addGeomToScene();
}

void callbackGeomSVG( const std::string& _svgString ) {
    svgString = _svgString;
    Scene::sUpdateCallbacks.emplace_back( [&]( Scene* p ) {
        GB{GeomBuilderType::svg}.ascii(svgString).pb(ProfileBuilder{0.015f, 6.0f}).buildr(p->RSG());
    } );

}

void ImGuiGeoms( Scene* p, const Rect2f& _r ) {
    LAYOUT_IMGUI_WINDOW_POSSIZE( SceneGraph, _r);

//    ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

    for ( auto& [k,v] : p->RSG().Nodes() ) {
        ImGui::PushID(std::visit(lambdaUUID, v).c_str());
        ImGui::BeginGroup();
        std::visit( NodeVisitor<ImGUIJsonNamed>{}, v );
        ImGui::EndGroup();
        ImGui::PopID();
    }

//    ImGui::EndChild();

    if ( gbt ) {
        ImGui::BeginGroup();
        ImGui::Text( "Name: %s", gbt->Name().c_str());
        if ( ImGui::Button( "Save", ImVec2( 80, 20 ))) {
            gbt->publish();
        }
        ImGui::EndGroup();
    }
    ImGui::End();
}
