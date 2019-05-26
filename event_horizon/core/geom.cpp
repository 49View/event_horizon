//
// Created by Dado on 2019-04-09.
//

#include "geom.hpp"

void GeomData::bufferDecode( const unsigned char* rawData, size_t length ) {

}

GeomData::GeomData( ResourceRef vData, JMATH::AABB _bbox, ResourceRef material ) :
                    vData(std::move( vData )),
                    material(std::move( material )) {
    BBox3d(std::move( _bbox ));
}
