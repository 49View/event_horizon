//
// Created by Dado on 2019-05-17.
//

#pragma once

#include <vector>
#include <poly/converters/obj/tiny_obj_loader.h>
#include <core/lightmap_exchange_format.h>
#include <poly/node_graph.hpp>
#include "xatlas.h"

class VData;
class SceneGraph;

int xatlasParametrize( std::vector<tinyobj::shape_t>& shapes, scene_t* scene );
int xatlasParametrize( SceneGraph& sg, const NodeGraphContainer& nodes, scene_t* scene );