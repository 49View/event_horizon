#pragma once

#include <string>
#include <unordered_map>
#include <iostream>
#include <tinygltf/include/tiny_gltf.h>
#include <core/math/vector4f.h>
#include <core/raw_image.h>
#include <core/name_policy.hpp>
#include <core/descriptors/tiny_dng_loader.h>
#include <core/descriptors/uniform_names.h>
#include <poly/poly.hpp>
#include <core/resources/resource_types.hpp>
#include <core/recursive_transformation.hpp>

class RawImage;
class HeterogeneousMap;
class Material;
class VData;
class SceneGraph;

namespace GLTF2Service {

    GeomSP load( SceneGraph& _sg, const std::string& _key, const std::string& _path, const SerializableContainer& _array = {} );

    void fillGeom( std::shared_ptr<VData> geom, tinygltf::Model* model, int meshIndex, int primitiveIndex );
};
