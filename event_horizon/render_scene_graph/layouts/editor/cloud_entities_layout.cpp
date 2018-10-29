//
// Created by Dado on 2018-10-29.
//

#include "cloud_entities_layout.h"
#include <graphics/imgui/imgui.h>
#include <render_scene_graph/scene.hpp>

static std::string remoteFilterString;
static std::multimap<std::string, CoreMetaData> cloudEntitiesTypeMap;

void listCloudMaterialCallback( Scene* p ) {
    std::vector<CoreMetaData> newFilteredResult;

    rapidjson::Document document;
    document.Parse<rapidjson::kParseStopWhenDoneFlag>( remoteFilterString.c_str() );
    MegaReader reader( document );
    reader.deserialize( newFilteredResult );

    for ( const auto& elem : newFilteredResult ) {
        std::vector<unsigned char> rd;
        bn::decode_b64( elem.getThumb().begin(), elem.getThumb().end(), std::back_inserter(rd) );
        ImageBuilder{ elem.getName() }.makeDirect( p->RSG().TL(), ucchar_p{ rd.data(), rd.size()} );
        cloudEntitiesTypeMap.insert( {elem.getType(), elem} );
    }
}

void ImGuiCloudEntities( Scene* p, const Rect2f& _r, const std::string _title, const std::string& _entType ) {

    ImGui::SetNextWindowPos( ImVec2{ _r.origin().x(), _r.origin().y() } );
    ImGui::SetNextWindowSize( ImVec2{ _r.size().x(), _r.size().y() } );
    ImGui::Begin( _title.c_str(), nullptr, ImGuiWindowFlags_NoCollapse );

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
                       Scene::sUpdateCallbacks.emplace_back( listCloudMaterialCallback );
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
    ImGui::End();

}
