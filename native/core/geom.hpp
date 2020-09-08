//
// Created by Dado on 2019-04-09.
//

#pragma once

#include <core/htypes_shared.hpp>
#include <core/boxable.hpp>

struct GeomData : public Boxable {
    GeomData() = default;
    GeomData( ResourceRef vData, JMATH::AABB _bbox, ResourceRef material );

RESOURCE_CTORS_ONLY(GeomData);

    ResourceRef vData;
    ResourceRef material;
};

