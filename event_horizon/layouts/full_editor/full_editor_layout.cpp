//
// Created by Dado on 2018-10-16.
//

#include "full_editor_layout.h"

#include <render_scene_graph/ui_presenter.hpp>
#include <converters/stl/parse_stl.h>
#include <converters/gltf2/gltf2.h>
#include "core/tar_util.h"
#include "poly/hier_geom.hpp"
#include "poly/geom_builder.h"
#include "graphics/ui/imgui_console.h"

struct UIViewLayout {
    float consoleHeight = 0.0f;
    float rightPanelWidth = 0.0f;
    Vector2f main3dWindowSize = Vector2f::ZERO;
    Vector2f timeLinePanelSize = Vector2f::ZERO;
};

std::shared_ptr<MaterialBuilder> mb;
std::shared_ptr<GeomBuilder> gbt;
std::string remoteFilterString;
std::multimap<std::string, CoreMetaData> cloudEntitiesTypeMap;

UIViewLayout uivl;

void materialPBRCallback( const rapidjson::Document& data ) {
    std::string filename = data["name"].GetString();
    if ( filename.find(MDaemonPaths::StoredOutput) != std::string::npos ){
        FM::readRemoteSimpleCallback( filename, [&](const Http::Result& _res) {
            auto inflatedData = zlibUtil::inflateFromMemory( uint8_p{std::move(_res.buffer), _res.length} );
            auto fn = getFileNameOnly(filename);
            mb = std::make_shared<MaterialBuilder>(fn);
            auto files = tarUtil::untar( inflatedData );
            for ( const auto& fi  : files ) {
                if ( const auto r = MPBRTextures::findTextureInString(fi.name); !r.empty() ) {
                    mb->buffer( r, fi.dataPtr );
                }
            }
            UiPresenter::sUpdateCallbacks.push_back( []( UiPresenter* p ) {
                mb->makeDirect( p->ML() );
                if ( !p->RR().hasTag(9300) ) {
                    GeomBuilder{ShapeType::Sphere, Vector3f::ONE}.g(9300).build( p->RSG() );
                }
                p->RR().changeMaterialOnTags( 9300, std::dynamic_pointer_cast<PBRMaterial>(p->ML().get(mb->Name())) );
            } );
        } );
    }
}

void assignMetaValues( const rapidjson::Value* _value ) {
//    std::string type = "aa";//std::string{ ( *_value )["type"].GetString() };
//    rapidjson::Value x = _value->;
}

void listCloudMaterialCallback( UiPresenter* p ) {
    std::vector<CoreMetaData> newFilteredResult;

    rapidjson::Document document;
    document.Parse<rapidjson::kParseStopWhenDoneFlag>( remoteFilterString.c_str() );
    MegaReader reader( document );
    reader.deserialize( newFilteredResult );

    for ( const auto& elem : newFilteredResult ) {
        std::vector<unsigned char> rd;
        bn::decode_b64( elem.getThumb().begin(), elem.getThumb().end(), std::back_inserter(rd) );
        ImageBuilder{ elem.getName() }.makeDirect( p->RSG().TL(), { rd.data(), rd.size()} );
        cloudEntitiesTypeMap.insert( {elem.getType(), elem} );
    }
}

void initLayout( const Rect2f& _screenRect, PresenterLayout* _layout, UiPresenter* p ) {

//    auto gbt = GeomBuilder{}.n("ullala");
//    Http::post( Url{ Http::restEntityPrefix( HierGeom::entityGroup(), gbt.Name() + ".geom" ) }, gbt.toMetaData() );

    uivl.consoleHeight = 0.15f;
    uivl.rightPanelWidth = 0.25f;
    uivl.timeLinePanelSize = { 1.0f - (uivl.rightPanelWidth*2), 0.20f };
    uivl.main3dWindowSize = { _screenRect.size().x() * (1.0-uivl.rightPanelWidth*2.0f),
                              _screenRect.size().y() * (1.0-(uivl.consoleHeight + uivl.timeLinePanelSize.y())) };

    float topX = _screenRect.size().x() * uivl.rightPanelWidth;
    _layout->addBox( Name::Foxtrot, Rect2f{ topX, 0.0f,
                     topX + uivl.main3dWindowSize.x(), uivl.main3dWindowSize.y() },
                     CameraControls::Fly );

//    _layout->addBox( Name::Sierra, Rect2f( Vector2f::ZERO, Vector2f{ 512.0f, 512.0f } ),
//                     CameraControls::Fly );
    _layout->addOffScreenBox( Name::Sierra, { 256.0f, 256.0f } );

    Socket::on( "cloudStorageFileUpdate", materialPBRCallback );

}

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

void ImGuiMaterials( UiPresenter* p ) {
    float ts = (getScreenSizefUI.x()*uivl.rightPanelWidth)/6.5f;
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

void ImGuiImages( UiPresenter* p ) {
    int ic = 0;
    for ( const auto& it: p->TM() ) {
        std::string tname = it.first;
        ImGui::BeginGroup();
        ImGui::TextColored( ImVec4{0.0f,1.0f,1.0f,1.0f}, "%s", tname.c_str());
        ImGui::Image( reinterpret_cast<void *>(it.second->getHandle()), ImVec2{ 100, 100 } );
        ImGui::EndGroup();
        if ( ++ic % 6 != 0 ) { ImGui::SameLine(); }
    }
}

void ImGuiGeoms( UiPresenter* p ) {
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
}

void ImGuiCamera( std::shared_ptr<Camera> cam ) {
    ImGui::BeginGroup();
    ImGui::Text( "Name: %s", cam->Name().c_str());
    ImGui::Text( "Pos: %f, %f, %f", cam->getPosition().x(), cam->getPosition().y(), cam->getPosition().z());
    ImGui::Text( "Angles: %f, %f, %f", cam->quatAngle().x(), cam->quatAngle().y(), cam->quatAngle().z());
    ImGui::Text( "Fov: %f", cam->FoV());
    ImGui::EndGroup();
}

void ImGuiCloudEntities( UiPresenter* p, const std::string& _entType ) {

    static char buf[1024];
    if ( ImGui::InputText( "", buf, 1024,
                      ImGuiInputTextFlags_EnterReturnsTrue|ImGuiInputTextFlags_CallbackCompletion|
                      ImGuiInputTextFlags_CallbackHistory,
                      [](ImGuiTextEditCallbackData* data) -> int {
                        //        if ( data->EventKey == ImGuiKey_Enter) {
                        //        }
                                return 0;
    } ) ) {
        Http::get( Url{ HttpFilePrefix::entities_all + _entType + "/" + std::string(buf) },
            [&](const Http::Result&_res) {
                remoteFilterString = std::string{ reinterpret_cast<char*>(_res.buffer.get()),
                                                  static_cast<std::string::size_type>(_res.length) };
                UiPresenter::sUpdateCallbacks.emplace_back( listCloudMaterialCallback );
        } );
    };

    for ( auto it = cloudEntitiesTypeMap.find(_entType); it != cloudEntitiesTypeMap.end(); ++it ) {
        ImGui::BeginGroup();
        auto& elem = it->second;
        ImGui::Text( "Name: %s", elem.getName().c_str());
        auto tex = p->TM().TD( elem.getName() );
        ImGui::Image( reinterpret_cast<void *>(tex->getHandle()), ImVec2{ 100, 100 } );
        ImGui::EndGroup();
    }

}

void ImGuiTimeline() {

}

void render( UiPresenter* p ) {

    p->Console()->Draw( ImVec2( 0.0, getScreenSizefUI.y()-(getScreenSizefUI.y() * uivl.consoleHeight) ),
                        ImVec2( getScreenSizefUI.x(), getScreenSizefUI.y() * uivl.consoleHeight ) );

    float sceneY = getScreenSizefUI.y() * (1.0f-uivl.consoleHeight);
    float sceneSectionY3 = sceneY / 3.0f;
    float sceneSectionY2 = sceneY / 2.0f;
    float sceneSectionX = getScreenSizefUI.x() * uivl.rightPanelWidth;
    ImGui::SetNextWindowPos( ImVec2{ 0, 0 } );
    ImGui::SetNextWindowSize( ImVec2{ sceneSectionX, sceneSectionY3 } );
    ImGui::Begin( "Geometry",  nullptr, ImGuiWindowFlags_NoCollapse );
    ImGuiGeoms(p);
    ImGui::End();
    ImGui::SetNextWindowPos( ImVec2{ 0, sceneSectionY3 } );
    ImGui::SetNextWindowSize( ImVec2{ sceneSectionX, sceneSectionY3 } );
    ImGui::Begin( "Materials",  nullptr, ImGuiWindowFlags_NoCollapse );
    ImGuiMaterials(p);
    ImGui::End();
    ImGui::SetNextWindowPos( ImVec2{ 0, sceneSectionY3*2.0f } );
    ImGui::SetNextWindowSize( ImVec2{ sceneSectionX, sceneSectionY3 } );
    ImGui::Begin( "Images",  nullptr, ImGuiWindowFlags_NoCollapse );
        ImGuiImages(p);
    ImGui::End();
//    ImGui::Begin( "Camera",  nullptr, ImGuiWindowFlags_NoCollapse );
//    ImGuiCamera(p->getCamera(Name::Foxtrot));
//    ImGui::End();

    // Server side
    ImGui::SetNextWindowPos( ImVec2{ getScreenSizefUI.x() - sceneSectionX, 0.0f } );
    ImGui::SetNextWindowSize( ImVec2{ sceneSectionX, sceneSectionY2 } );
    ImGui::Begin( "Cloud Materials",  nullptr, ImGuiWindowFlags_NoCollapse );
    ImGuiCloudEntities(p, Material::entityGroup());
    ImGui::End();
    ImGui::SetNextWindowPos( ImVec2{ getScreenSizefUI.x() - sceneSectionX, sceneSectionY2 } );
    ImGui::SetNextWindowSize( ImVec2{ sceneSectionX, sceneSectionY2 } );
    ImGui::Begin( "Cloud Geoms",  nullptr, ImGuiWindowFlags_NoCollapse );
    ImGuiCloudEntities(p, HierGeom::entityGroup() );
    ImGui::End();

    // Timeline
    ImGui::SetNextWindowPos( ImVec2{ sceneSectionX,
                                     getScreenSizefUI.y()-(getScreenSizefUI.y() *
                                     (uivl.consoleHeight+uivl.timeLinePanelSize.y() )) } );
    ImGui::SetNextWindowSize( ImVec2{ getScreenSizefUI.x()* uivl.timeLinePanelSize.x(),
                                      getScreenSizefUI.y() * uivl.timeLinePanelSize.y() } );
    ImGui::Begin( "Timeline",  nullptr, ImGuiWindowFlags_NoCollapse );
    ImGuiTimeline();
    ImGui::End();
}

void allConversionsDragAndDropCallback( UiPresenter* p, const std::string& _path ) {
    std::string pathSanitized = url_encode_spacesonly(_path);
    std::string ext = getFileNameExt( pathSanitized );
    std::string extl = toLower(ext);
    std::string finalPath = pathSanitized;
    bool isGeom = false;

    if ( extl == ".fbx" ) {
        // Convert to GLTF
        std::string cmd = "cd " + getFileNamePath(pathSanitized) + "\n FBX2glTF " + pathSanitized;
        std::system( cmd.c_str() );
        finalPath = getFileNamePath(pathSanitized) + "/" + getFileNameOnly(finalPath) + "_out/" + getFileNameOnly(finalPath)
                    + ".gltf";
        isGeom = true;
    } else if ( extl == ".obj" ) {
        // Convert to GLTF
        finalPath = getFileNamePath(pathSanitized) + "/" + getFileNameOnly(finalPath) + ".gltf";
        std::string cmd = "cd " + getFileNamePath(pathSanitized) + "\n obj2gltf -i " + pathSanitized +
                          " -o " + finalPath;
        std::system( cmd.c_str() );
        isGeom = true;
    }
    else if ( extl == ".stl" ) {
        stl::parse_stl(pathSanitized);
        isGeom = true;
    } else if ( extl == ".sbsar" ) {
        FM::copyLocalToRemote( pathSanitized, MDaemonPaths::UploadDir + getFileName(pathSanitized) );
//        SubstanceDriver::elaborateSbsarLayer( pathSanitized, MQSettings::Medium,  512 , p->ML() );
    } else if ( extl == ".zip" ) {
//        SubstanceDriver::elaborateFromTextureSet( getFileNameNoExt(finalPath), p->ML(), ".jpg" );
    }
    else if ( extl == ".gltf") {
        isGeom = true;
    }

    if ( isGeom ) {
        static float ni = 0.0f;
        GLTF2 newObject{ finalPath };
        auto hierScene = newObject.convert();
        p->getCamera(Name::Foxtrot)->center(hierScene->BBox3d(), {0.25f, 0.5f, 0.0f}, {0.0f, 0.25f, 0.0f});
        p->getCamera(Name::Sierra)->center(hierScene->BBox3d(), {0.25f, 0.5f, 0.0f}, {0.0f, 0.25f, 0.0f});
        hierScene->updateTransform( Vector3f::X_AXIS * ni );
        gbt = std::make_shared<GeomBuilder>( hierScene, newObject.Materials() );
        gbt->build(p->RSG());
        p->takeScreenShot( Name::Sierra, gbt->Thumb() );

        ni+=1.0f;
    }
}

std::shared_ptr<PresenterLayout> fullEditor() {
    return std::make_shared<PresenterLayout>(initLayout, render, allConversionsDragAndDropCallback);
}
