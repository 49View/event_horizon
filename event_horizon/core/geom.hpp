//
// Created by Dado on 2019-04-09.
//

#pragma once

#include <core/htypes_shared.hpp>

struct GeomData {
    GeomData() = default;
    GeomData( ResourceRef vData, ResourceRef material );

RESOURCE_CTORS_ONLY(GeomData);

    ResourceRef vData;
    ResourceRef material;
};

