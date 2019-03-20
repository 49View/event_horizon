//
// Created by Dado on 2018-10-29.
//

#include "image_layout.h"
#include <core/raw_image.h>
#include <core/image_builder.h>
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene_orchestrator.hpp>
#include <render_scene_graph/layouts/layout_helper.hpp>

std::string imageName;
SerializableContainer imageBufferData;

void callbackImage( const std::string& _filename, const SerializableContainer& _fileContent ) {
    imageBufferData = _fileContent;
    imageName = getFileName(_filename);
    SceneOrchestrator::sUpdateCallbacks.emplace_back( [&]( SceneOrchestrator* p ) {
        ImageBuilder{p->SG().TL(), imageName}.create( imageBufferData );
    } );
}

void ImGuiImages::renderImpl( SceneOrchestrator* p, Rect2f& _r ) {
    int ic = 0;
    static bool lShowSystem = false;
    ImGui::Checkbox("Show System", &lShowSystem);
    for ( const auto& it: p->RSG().RR().TM() ) {
        std::string tname = it.first;
        if ( lShowSystem || !FBNames::isPartOf( cubeMapTName(tname) ) ) {
            ImGui::BeginGroup();
            ImGui::TextColored( ImVec4{0.0f,1.0f,1.0f,1.0f}, "%s", tname.c_str());
            ImGui::Image( reinterpret_cast<void *>(it.second->getHandle()), ImVec2{ 100, 100 } );
            ImGui::EndGroup();
            if ( ++ic % 6 != 0 ) { ImGui::SameLine(); }
        }
    }
}
