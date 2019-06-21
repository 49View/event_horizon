//
// Created by Dado on 2018-10-29.
//

#include "material_layout.h"
#include <core/tar_util.h>
#include <core/raw_image.h>
#include <core/TTF.h>
#include <core/resources/material.h>
#include <core/descriptors/uniform_names.h>
#include <core/resources/resource_builder.hpp>

#include <graphics/imgui/imgui.h>
#include <graphics/texture_manager.h>

#include <render_scene_graph/layouts/layout_helper.hpp>
#include <render_scene_graph/render_orchestrator.h>

struct CallbackMaterialData {
    CallbackMaterialData( const std::string& filename, const SerializableContainer& data ) : filename( filename ),
                                                                                         data( data ) {}

    std::string filename;
    SerializableContainer data;
};

std::vector<std::string> cmdKeys;

void ImGuiMatImage( const std::string& name, const ImColor& col, const ImVec2 size, std::shared_ptr<Texture> t,
                    float backup = -1.0f ) {

    if ( t && (t->getWidth() > 0|| backup >= 0.0f) ) {
//        if ( t->getWidth() > 4 && name != "Base" ) ImGui::SameLine();
        ImGui::BeginGroup();
        if ( t->getWidth() > 0 ) {
//            ImGui::TextColored( col.Value, "%s", name.c_str() );
            ImGui::Image( reinterpret_cast<void *>(t->getHandle()), size );
        } else {
            ImGui::PushID( t->getHandle() );
            ImGui::SliderFloat( name.c_str(), &backup, 0.0f, 1.0f );
            ImGui::PopID();
        }
        ImGui::EndGroup();
    }
}

void ImGuiMaterials::renderImpl( SceneGraph& _sg, RenderOrchestrator& _rsg, Rect2f& _r ) {
    float ts = 64.0f;
    ImVec2 textureSize{ ts, ts };

    for ( const auto& mate : _sg.ML().listResources()) {
        ImGui::BeginGroup();
        auto mat = mate.second;
        auto matName = mate.first.substr(0, 10);
        ImGui::TextColored( ImVec4{1.0f,0.8f,0.3f,1.0f}, "%s", matName.c_str() );
        bool bHasTexture = false;
        for ( const auto& [k, mt] : mat->Values()->getMap<std::string>() ) {
            if ( k == UniformNames::diffuseTexture || k == UniformNames::colorTexture ) {
                ImGuiMatImage( mt, ImColor{200, 200, 200}, textureSize, _rsg.RR().TM()->TD(mt) );
                bHasTexture = true;
                break;
            }
        }
        if (!bHasTexture) {
            V3f cv3 = mat->Values()->getDef<V3f>(UniformNames::diffuseColor);
            ImGui::PushID( (mate.first + "dc").c_str() );
            if ( ImGui::ColorEdit3( "Diffuse", cv3.rawPtr(), ImGuiColorEditFlags_NoInputs ) ) {
                mat->Values()->assign( UniformNames::diffuseColor, cv3 );
            }
            ImGui::PopID();
        }
        ImGui::EndGroup();
        ImGui::SameLine();
    }
}

void callbackMaterial( const std::string& _filename, const SerializableContainer& _data ) {

}
