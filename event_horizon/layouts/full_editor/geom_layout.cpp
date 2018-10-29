//
// Created by Dado on 2018-10-29.
//

#include "geom_layout.h"
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/ui_presenter.hpp>
#include "poly/hier_geom.hpp"
#include "poly/geom_builder.h"

std::shared_ptr<GeomBuilder> gbt;

void ImGuiGeoms( UiPresenter* p, const Rect2f& _r ) {
    ImGui::SetNextWindowPos( ImVec2{ _r.origin().x(), _r.origin().y() } );
    ImGui::SetNextWindowSize( ImVec2{ _r.size().x(), _r.size().y() } );
    ImGui::Begin( "Geometry", nullptr, ImGuiWindowFlags_NoCollapse );

    for ( const auto& it: p->RSG().Geoms() ) {
        auto gname = std::to_string(it->Hash());
        ImGui::BeginGroup();
        ImGui::Text( "Name: %s", it->Name().c_str());
        ImGui::Text( "Hash: %lld", it->Hash());
        ImGui::EndGroup();
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
