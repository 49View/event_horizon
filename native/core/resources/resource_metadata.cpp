//
// Created by dado on 14/06/2020.
//

#include "resource_metadata.hpp"
#include <core/http/webclient.h>

void ResourceMetaData::getListOf( const std::string& entityGroup, const std::string& tags, const ResourceMetadataListCallback& ccf ) {
    Http::get(Url{ "/entities/list/" + entityGroup + "/" + tags }, [ccf]( HttpResponeParams params ) {
        std::vector<EntityMetaData> el{};
        if ( !params.bufferString.empty() ) {
            rapidjson::Document document;
            document.Parse<rapidjson::kParseStopWhenDoneFlag>( params.bufferString.c_str() );
            MegaReader reader( document );
            reader.deserialize(el);
        }
		ccf(el);
    });
}
