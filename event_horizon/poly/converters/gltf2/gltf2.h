#pragma once

#include <string>
#include <unordered_map>
#include "tiny_gltf.h"
#include <core/math/vector4f.h>
#include <core/name_policy.hpp>
#include <core/descriptors/uniform_names.h>
#include <poly/poly.hpp>
#include <core/resources/resource_types.hpp>
#include <core/recursive_transformation.hpp>

class RawImage;
class HeterogeneousMap;
class Material;
class VData;
class SceneGraph;

enum class SigmoidSlope {
    Positive,
    Negative
};

namespace GLTF2Service {

    struct ExtraAccessorData {
        Vector3f min = Vector3f::ZERO;
        Vector3f max = Vector3f::ZERO;
    };

    enum class InternalPBRTextureReconstructionMode {
        None,
        GrayScaleCreate,
        SigmoidCeiling,
        SigmoidFloor,
        NormalMap
    };

    struct InternalPBRComponent {
        std::string baseName;
        InternalPBRTextureReconstructionMode textureReconstructionMode = InternalPBRTextureReconstructionMode::None;
        Color4f value = Vector4f::ONE;
        std::pair<int, int> texture{-1,-1};
    };

    struct IntermediateMaterial {
        IntermediateMaterial();
        std::string name;
        mutable std::unordered_map<std::string, RawImage> mb;
        std::shared_ptr<HeterogeneousMap> values;
        mutable std::shared_ptr<RawImage> grayScaleBaseColor;
        InternalPBRComponent baseColor{MPBRTextures::basecolorString,
        InternalPBRTextureReconstructionMode::GrayScaleCreate };
        InternalPBRComponent metallic{MPBRTextures::metallicString,
                                      InternalPBRTextureReconstructionMode::SigmoidFloor };
        InternalPBRComponent roughness{MPBRTextures::roughnessString,
                                       InternalPBRTextureReconstructionMode::SigmoidCeiling };
        InternalPBRComponent normal{MPBRTextures::normalString,
                                    InternalPBRTextureReconstructionMode::NormalMap };
    };

    using MaterialMap = std::unordered_map<std::string, std::string>;
    struct IntermediateGLTF : public NamePolicy<> {
        std::shared_ptr<tinygltf::Model> model;
        MaterialMap matMap;
        std::string basePath;
    };

    GeomSP load( SceneGraph& _sg, const std::string& _path, const SerializableContainer& _array = {} );

    void fillGeom( std::shared_ptr<VData> geom, tinygltf::Model* model, int meshIndex, int primitiveIndex );

};
