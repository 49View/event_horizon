//
// Created by Dado on 2018-10-29.
//

#include "material_layout.h"
#include <core/tar_util.h>
#include <poly/geom_builder.h>
#include <poly/material_builder.h>
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/layouts/layout_helper.hpp>
#include <render_scene_graph/scene_orchestrator.hpp>

struct CallbackMaterialData {
    CallbackMaterialData( const std::string& filename, const SerializableContainer& data ) : filename( filename ),
                                                                                         data( data ) {}

    std::string filename;
    SerializableContainer data;
};

std::vector<CallbackMaterialData> cmd;

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

void ImGuiMaterials::renderImpl( SceneOrchestrator* p, Rect2f& _r ) {
    float ts = 64.0f;
    ImVec2 textureSize{ ts, ts };

    for ( const auto& mat : p->SG().ML().list()) {
        if ( !mat ) continue;
        ImGui::BeginGroup();
        auto matName = mat->Name().substr(0, 10);
        ImGui::TextColored( ImVec4{1.0f,0.8f,0.3f,1.0f}, "%s", matName.c_str() );
        bool bHasTexture = false;
        for ( const auto& [k, mt] : mat->getTextureNameMap() ) {
            if ( k == UniformNames::diffuseTexture || k == UniformNames::colorTexture ) {
                ImGuiMatImage( mt, ImColor{200, 200, 200}, textureSize, p->RSG().RR().TM().TD(mt) );
                bHasTexture = true;
                break;
            }
        }
        if (!bHasTexture) {
            V3f cv3 = Vector3f::ONE;
            if ( mat->hasVector3f(UniformNames::diffuseColor) ) {
                mat->get( UniformNames::diffuseColor, cv3 );
            }
            ImGui::PushID( (mat->Name() + "dc").c_str() );
            if ( ImGui::ColorEdit3( "Diffuse", cv3.rawPtr(), ImGuiColorEditFlags_NoInputs ) ) {
                mat->assign( UniformNames::diffuseColor, cv3 );
            }
            ImGui::PopID();
        }
        ImGui::EndGroup();
        ImGui::SameLine();
    }
}

void callbackMaterial( const std::string& _filename, const SerializableContainer& _data ) {

    cmd.emplace_back( _filename, _data );

    SceneOrchestrator::sUpdateCallbacks.emplace_back( []( SceneOrchestrator* p ) {
//        ### re-enable create mesh from material imported
//        for ( const auto& elem : cmd ) {
//            auto mb = std::make_shared<MaterialBuilder>(getFileNameOnly(elem.filename), elem.data);
//            GB{p->RSG(), ShapeType::Sphere, Vector3f::ONE}.g(9300).m(mb->makeDirect( p->RSG().ML() )).build();
//        }
        cmd.clear();
    } );
}
