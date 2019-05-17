//
// Created by Dado on 2019-05-17.
//

#pragma once

#include <vector>
#include <poly/converters/obj/tiny_obj_loader.h>
#include <event_horizon/event_horizon/core/lightmap_exchange_format.h>

int xatlasParametrize( std::vector<tinyobj::shape_t>& shapes, scene_t* scene );
