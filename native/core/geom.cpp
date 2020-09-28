//
// Created by Dado on 2019-04-09.
//

#include "geom.hpp"
#include <core/math/aabb.h>

void GeomData::bufferDecode(  [[maybe_unused]] const unsigned char* rawData,  [[maybe_unused]]  size_t length ) {

}

GeomData::GeomData( ResourceRef vData, ResourceRef material ) :
                    vData(std::move( vData )),
                    material(std::move( material )) {
}

GeomData::GeomData( const AABB& _nodeAABB, ResourceRef vData, ResourceRef material,
                    ResourceRef _program ) :
        vData(std::move( vData )),
        material(std::move( material )),
        program(std::move( _program )) {
    BBox3d(_nodeAABB);
}
