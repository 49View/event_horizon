//
// Created by Dado on 2019-04-09.
//

#include "geom.hpp"

bool Geom::empty() const {
    return vData.empty();
}

const ResourceRef& Geom::MaterialRef() const {
    return material;
}

void Geom::MaterialRef( const ResourceRef& _material ) {
    Geom::material = _material;
}

const ResourceRef& Geom::VDataRef() const {
    return vData;
}

void Geom::VDataRef( const ResourceRef& _vData ) {
    Geom::vData = _vData;
}
