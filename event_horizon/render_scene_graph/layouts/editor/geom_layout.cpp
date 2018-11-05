//
// Created by Dado on 2018-10-29.
//

#include "geom_layout.h"
#include <graphics/imgui/imgui_jsonvisit.hpp>
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene.hpp>
#include "poly/hier_geom.hpp"
#include "poly/geom_builder.h"
#include "poly/converters/gltf2/gltf2.h"

std::shared_ptr<GeomBuilder> gbt;

void loadGeomInGui( Scene* p, std::shared_ptr<GLTF2> _newObject ) {
    auto hierScene = _newObject->convert();
    p->getCamera(Name::Foxtrot)->center(hierScene->BBox3d(), {0.35f, 0.5f, 0.0f}, {0.0f, 0.25f, 0.0f});
    gbt = std::make_shared<GeomBuilder>( hierScene, _newObject->Materials() );
    gbt->build(p->RSG());
    p->takeScreenShot( hierScene->BBox3d(), gbt->Thumb() );
}

void ImGuiGeoms( Scene* p, const Rect2f& _r ) {
    ImGui::SetNextWindowPos( ImVec2{ _r.origin().x(), _r.origin().y() } );
    ImGui::SetNextWindowSize( ImVec2{ _r.size().x(), _r.size().y() } );
    ImGui::Begin( "Geometry", nullptr, ImGuiWindowFlags_NoCollapse );

    for ( const auto& it: p->RSG().Geoms() ) {
        it->visit<ImGUIJsonNamed>();
    }
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
