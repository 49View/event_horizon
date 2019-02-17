//
// Created by Dado on 2018-10-29.
//

#include "material_layout.h"
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene_orchestrator.hpp>
#include <poly/geom_builder.h>
#include <core/tar_util.h>
#include <render_scene_graph/layouts/layout_helper.hpp>

struct CallbackMaterialData {
    CallbackMaterialData( const std::string& filename, const std::vector<char>& data ) : filename( filename ),
                                                                                         data( data ) {}

    std::string filename;
    std::vector<char> data;
};

std::vector<CallbackMaterialData> cmd;

void ImGuiMatImage( const std::string& name, const ImColor& col, const ImVec2 size, std::shared_ptr<Texture> t,
                    float backup = -1.0f ) {

    if ( t && (t->getWidth() > 0|| backup >= 0.0f) ) {
//        if ( t->getWidth() > 4 && name != "Base" ) ImGui::SameLine();
        ImGui::BeginGroup();
        if ( t->getWidth() > 0 ) {
            ImGui::TextColored( col.Value, "%s", name.c_str() );
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

    for ( const auto& mat : p->ML().list()) {
        if ( !mat ) continue;
        ImGui::TextColored( ImVec4{1.0f,0.8f,0.3f,1.0f}, "%s", mat->Name().c_str() );
        float dc[3] = {1.0f, 1.0f, 1.0f};// { mat->getColor().x(), mat->getColor().y(), mat->getColor().z() };
        ImGui::PushID( (mat->Name() + "dc").c_str() );
        ImGui::ColorEdit3( "Diffuse", dc );
        ImGui::PopID();
        for ( const auto& mt : mat->getTextureNames() ) {
            ImGuiMatImage( mt, ImColor{200, 200, 200}, textureSize, p->TM().TD(mt) );
        }
//        if ( mat->getType() == MaterialType::PBR ) {
//            std::shared_ptr<PBRMaterial> matPBR = std::dynamic_pointer_cast<PBRMaterial>( mat );
//            ImGuiMatImage( "Base", ImColor{200, 200, 200}, textureSize, p->TM().TD(matPBR->getBaseColor()) );
//            ImGuiMatImage( "Normal", ImColor{100, 100, 250}, textureSize, p->TM().TD( matPBR->getNormal()) );
//            ImGuiMatImage( "Roughness", ImColor{250, 250, 100}, textureSize, p->TM().TD( matPBR->getRoughness()),
//                           matPBR->getRoughnessValue() );
//            ImGuiMatImage( "Metallic", ImColor{240, 240, 240}, textureSize, p->TM().TD( matPBR->getMetallic()),
//                           matPBR->getMetallicValue() );
//            ImGuiMatImage( "AO", ImColor{100, 100, 100}, textureSize, p->TM().TD( matPBR->getAmbientOcclusion()),
//                           matPBR->getAoValue());
//            ImGuiMatImage( "Height", ImColor{32, 200, 32}, textureSize, p->TM().TD( matPBR->getHeight()) );
//        }
//        if ( mb ) {
//            ImGui::BeginGroup();
            if ( ImGui::Button( "Save", ImVec2( 80, 20 ))) {
                mat->publish();
            }
//            ImGui::EndGroup();
//        }

        ImGui::Separator();
    }
}

void callbackMaterial( const std::string& _filename, const std::vector<char>& _data ) {

    cmd.emplace_back( _filename, _data );

    SceneOrchestrator::sUpdateCallbacks.emplace_back( []( SceneOrchestrator* p ) {
        for ( const auto& elem : cmd ) {
            auto mb = std::make_shared<MaterialBuilder>(getFileNameOnly(elem.filename), elem.data);
            GeomBuilder{ShapeType::Sphere, Vector3f::ONE}.g(9300).m(mb->makeDirect( p->ML() )).build( p->RSG() );
        }
        cmd.clear();
    } );
}
