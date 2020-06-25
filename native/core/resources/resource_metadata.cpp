//
// Created by dado on 14/06/2020.
//

#include "resource_metadata.hpp"
#include <core/http/webclient.h>

void ResourceMetaData::getListOf( const std::string& entityGroup, const std::string& tags, const ResourceMetadataListCallback& ccf ) {
    Http::get(Url{ "/entities/list/" + entityGroup + "/" + tags }, [ccf]( HttpResponeParams params ) {
		ccf(deserializeVector<EntityMetaData>(params.BufferString()));
    });
}
