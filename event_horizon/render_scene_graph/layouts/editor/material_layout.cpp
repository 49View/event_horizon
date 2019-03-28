//
// Created by Dado on 2018-10-29.
//

#include "material_layout.h"
#include <core/tar_util.h>
#include <core/raw_image.h>
#include <poly/geom_builder.h>
#include <poly/material_builder.h>
#include <poly/resources/image_builder.h>
#include <poly/resources/material_file.hpp>
#include <graphics/imgui/imgui.h>
#include <graphics/texture_manager.h>
#include <render_scene_graph/layouts/layout_helper.hpp>
#include <render_scene_graph/scene_orchestrator.hpp>

struct CallbackMaterialData {
    CallbackMaterialData( const std::string& filename, const SerializableContainer& data ) : filename( filename ),
                                                                                         data( data ) {}

    std::string filename;
    SerializableContainer data;
};

std::vector<CallbackMaterialData> cmd;
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

void ImGuiMaterials::renderImpl( SceneOrchestrator* p, Rect2f& _r ) {
    float ts = 64.0f;
    ImVec2 textureSize{ ts, ts };

    for ( const auto& mate : p->SG().ML().listResources()) {
        ImGui::BeginGroup();
        auto mat = mate.second;
        auto matName = mate.first.substr(0, 10);
        ImGui::TextColored( ImVec4{1.0f,0.8f,0.3f,1.0f}, "%s", matName.c_str() );
        bool bHasTexture = false;
        for ( const auto& [k, mt] : mat->Values()->getTextureNameMap() ) {
            if ( k == UniformNames::diffuseTexture || k == UniformNames::colorTexture ) {
                ImGuiMatImage( mt, ImColor{200, 200, 200}, textureSize, p->RSG().RR().TM().TD(mt) );
                bHasTexture = true;
                break;
            }
        }
        if (!bHasTexture) {
            V3f cv3 = Vector3f::ONE;
            if ( mat->Values()->hasVector3f(UniformNames::diffuseColor) ) {
                mat->Values()->get( UniformNames::diffuseColor, cv3 );
            }
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

    auto fk = getFileNameNoExt(_filename);
    cmd.emplace_back( fk, _data );
    cmdKeys.emplace_back( fk );

    SceneOrchestrator::sUpdateCallbacks.emplace_back( []( SceneOrchestrator* p ) {
        for ( const auto& elem : cmd ) {
            auto files = tarUtil::untar( elem.data );
            for ( const auto& [k,v] : files ) {
                p->SG().TL().addDependencyHook( elem.filename, k);
            }
            p->SG().TL().connect( [&](const ResourceSignalsAddSignature<RawImage>& _val ) {
                const std::string& resName = std::get<1>(_val);
                for ( const auto& k : cmdKeys ) {
                    p->SG().TL().tagDependencyLoaded( k, resName );
                    ResourceDependencyMap retDM;
                    if ( p->SG().TL().checkDependencyCompleted(k, retDM) ) {
                        LOGRS( "ALL IMAGES PUBLISHED ");
                        auto values = std::make_shared<HeterogeneousMap>();
                        std::vector<ResourceRef> imageRefs;
                        for ( const auto& [k2,v] : retDM ) {
                            LOGRS( "Key: " << k2 << " Name: " << v );
                            imageRefs.emplace_back( v );
                            auto pbrStr = MPBRTextures::mapToTextureUniform( k2 );
                            if ( !pbrStr.empty() ) {
                                values->assign( pbrStr, v );
                            }
                        }
                        MB{ p->SG().ML(), k }.create( { S::SH, imageRefs, values } );

                        cmdKeys.erase(std::remove(cmdKeys.begin(), cmdKeys.end(), k), cmdKeys.end());
                    }
                }
            });

            for ( const auto& [k,v] : files ) {
                IB{ p->SG().TL(), k }.create( v );
            }
//            auto mb = std::make_shared<MaterialBuilder>(getFileNameOnly(elem.filename), elem.data);
//            GB{p->RSG(), ShapeType::Sphere, Vector3f::ONE}.g(9300).m(mb->makeDirect( p->RSG().ML() )).build();
        }
        cmd.clear();
    } );
}
