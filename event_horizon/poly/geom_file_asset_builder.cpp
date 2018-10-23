#include "geom_file_asset_builder.h"
#include <stb/stb_image.h>
#include "core/file_manager.h"
#include "hier_geom.hpp"
#include "geom_builder.h"

bool GeomFileAssetBuilder::makeImpl( DependencyMaker& _md, uint8_p&& _data, const DependencyStatus _status ) {

    AssetManager& sg = static_cast<AssetManager&>(_md);

    if ( _status == DependencyStatus::LoadedSuccessfully ) {
        sg.add( *this, zlibUtil::inflateFromMemory( std::move(_data) ) );
    } else {
        return false;
    }

    return true;
}
