//
// Created by Dado on 2018-10-29.
//

#include "cloud_entities_layout.h"
#include <core/raw_image.h>
#include <graphics/imgui/imgui.h>
#include <graphics/texture_manager.h>
#include <render_scene_graph/scene_orchestrator.hpp>
#include <poly/resources/resource_builder.hpp>

static std::string remoteFilterString;
static std::multimap<std::string, CoreMetaData> cloudEntitiesTypeMap;

void listCloudCallback( SceneOrchestrator* p ) {

    if ( remoteFilterString.empty() ) return;

    std::vector<CoreMetaData> newFilteredResult;

    rapidjson::Document document;
    document.Parse<rapidjson::kParseStopWhenDoneFlag>( remoteFilterString.c_str() );
    MegaReader reader( document );
    reader.deserialize( newFilteredResult );

    for ( const auto& elem : newFilteredResult ) {
        SerializableContainer rd;
        bn::decode_b64( elem.getThumb().begin(), elem.getThumb().end(), std::back_inserter(rd) );
        IB{ p->SG(), elem.getName() }.make( rd );
        cloudEntitiesTypeMap.insert( {elem.getGroup(), elem} );
    }
}

void ImGuiCloudEntities( SceneOrchestrator* p, Rect2f& _r, const std::string& _title, const std::string& _entType ) {

    static char buf1[1024];
    static char buf2[1024];
    char* buf = _title == "Cloud Geometry" ? buf1 : buf2;
    ImGui::PushID( static_cast<int>(std::hash<std::string>{}( _title)));
    if ( ImGui::InputText( ("##" + _title).c_str(), buf, 1024, ImGuiInputTextFlags_EnterReturnsTrue ) ) {
        Http::get( Url::entityMetadata( _entType, std::string(buf)),
                   [&](const Http::Result&_res) {
                       remoteFilterString = _res.bufferString;
                       SceneOrchestrator::sUpdateCallbacks.emplace_back( listCloudCallback );
                   } );
    };
    ImGui::PopID();

    for ( auto it = cloudEntitiesTypeMap.find(_entType); it != cloudEntitiesTypeMap.end(); ++it ) {
        ImGui::BeginGroup();
        auto& elem = it->second;
        ImGui::Text( "Name: %s", elem.getName().c_str());
        auto tex = p->RSG().RR().TM().TD( elem.getName() );
        ImGui::Image( reinterpret_cast<void *>(tex->getHandle()), ImVec2{ 100, 100 } );
        ImGui::EndGroup();
    }
}
