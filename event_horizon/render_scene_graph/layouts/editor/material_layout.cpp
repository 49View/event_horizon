//
// Created by Dado on 2018-10-29.
//

#include "material_layout.h"
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene.hpp>
#include <poly/geom_builder.h>
#include <core/tar_util.h>
#include <render_scene_graph/layouts/layout_helper.hpp>

std::shared_ptr<MaterialBuilder> mb;

void ImGuiMatImage( const std::string& name, const ImColor& col, const ImVec2 size, std::shared_ptr<Texture> t,
                    float backup = -1.0f ) {

    if ( t && (t->getWidth() > 4 || backup >= 0.0f) ) {
        if ( t->getWidth() > 4 && name != "Base" ) ImGui::SameLine();
        ImGui::BeginGroup();
        if ( t->getWidth() > 4 ) {
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

void ImGuiMaterials::renderImpl( Scene* p, Rect2f& _r ) {
    float ts = 64.0f;
    ImVec2 textureSize{ ts, ts };

    for ( const auto& mat : p->ML().list()) {
        if ( !mat ) continue;
        ImGui::TextColored( ImVec4{1.0f,0.8f,0.3f,1.0f}, "%s", mat->getName().c_str() );
        float dc[3] = { mat->getColor().x(), mat->getColor().y(), mat->getColor().z() };
        ImGui::PushID( (mat->getName() + "dc").c_str() );
        ImGui::ColorEdit3( "Diffuse", dc );
        ImGui::PopID();
        if ( mat->getType() == MaterialType::PBR ) {
            std::shared_ptr<PBRMaterial> matPBR = std::dynamic_pointer_cast<PBRMaterial>( mat );
            ImGuiMatImage( "Base", ImColor{200, 200, 200}, textureSize, p->TM().TD(matPBR->getBaseColor()) );
            ImGuiMatImage( "Normal", ImColor{100, 100, 250}, textureSize, p->TM().TD( matPBR->getNormal()) );
            ImGuiMatImage( "Roughness", ImColor{250, 250, 100}, textureSize, p->TM().TD( matPBR->getRoughness()),
                           matPBR->getRoughnessValue() );
            ImGuiMatImage( "Metallic", ImColor{240, 240, 240}, textureSize, p->TM().TD( matPBR->getMetallic()),
                           matPBR->getMetallicValue() );
            ImGuiMatImage( "AO", ImColor{100, 100, 100}, textureSize, p->TM().TD( matPBR->getAmbientOcclusion()),
                           matPBR->getAoValue());
            ImGuiMatImage( "Height", ImColor{32, 200, 32}, textureSize, p->TM().TD( matPBR->getHeight()) );
        }
        ImGui::BeginGroup();
        ImGui::EndGroup();
        ImGui::Separator();
    }
}

void callbackMaterial( const std::string& _filename, const std::vector<char>& _data ) {
    mb = std::make_shared<MaterialBuilder>(getFileNameOnly(_filename));
    auto files = tarUtil::untar( _data );
    for ( const auto& fi  : files ) {
        if ( const auto r = MPBRTextures::findTextureInString(fi.name); !r.empty() ) {
            mb->buffer( r, fi.dataPtr );
        }
    }
    Scene::sUpdateCallbacks.emplace_back( []( Scene* p ) {
        mb->makeDirect( p->ML() );
        if ( !p->RR().hasTag(9300) ) {
            GeomBuilder{ShapeType::Sphere, Vector3f::ONE}.g(9300).build( p->RSG() );
        }
        p->RR().changeMaterialOnTags( 9300, std::dynamic_pointer_cast<PBRMaterial>(p->ML().get(mb->Name())) );
    } );
}
