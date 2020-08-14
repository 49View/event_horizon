#pragma once

#include <core/raw_image.h>
#include <core/math/quaternion.h>
#include <core/image_util.h>
#include <core/names.hpp>
#include <core/heterogeneous_map.hpp>
#include <core/file_manager.h>
#include <core/v_data.hpp>
#include <core/geom.hpp>
#include <core/recursive_transformation.hpp>
#include <core/resources/material.h>
#include <core/resources/entity_factory.hpp>
#include <core/resources/resource_builder.hpp>
#include <core/descriptors/uniform_names.h>

namespace tinygltf {
    class Model;
    class Node;
}

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
        int textureIndex = -1;
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
        std::string contentHash;
        std::string key;
    };

    void geomBBox( const std::shared_ptr <VData>& geom, tinygltf::Model *model, int meshIndex, int primitiveIndex );

    void addGeomBBox( IntermediateGLTF &_gltf, int meshIndex, int primitiveIndex, const GeomSP& hier );

    void addMeshBBoxNode( IntermediateGLTF &_gltf, const tinygltf::Node &node, const GeomSP& hier );

    bool handleLoadingMessages( const std::string &err, const std::string &warn, bool ret );

    IntermediateGLTF
    loadGLTFAsset( const std::string &_key, const std::string &_path, const SerializableContainer &_array );

    AABB GLTFBBox( const std::string &_key, const std::string &_path, const SerializableContainer &_array );

    [[maybe_unused]] V3f GLTFSize( const std::string &_key, const std::string &_path, const SerializableContainer &_array );

}