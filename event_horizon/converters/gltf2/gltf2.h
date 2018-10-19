#pragma once

#include <string>
#include <unordered_map>
#include "tiny_gltf.h"
#include <core/math/vector4f.h>
#include <core/descriptors/material.h>
#include "opencvutils/cvmatutil.hpp"

class HierGeom;
struct MaterialBuilder;

enum class SigmoidSlope {
    Positive,
    Negative
};

class GLTF2 {
public:
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
        std::string name;
        mutable std::shared_ptr<MaterialBuilder> mb;
        mutable cv::Mat grayScaleBaseColor;
        InternalPBRComponent baseColor{MPBRTextures::basecolorString,
        InternalPBRTextureReconstructionMode::GrayScaleCreate };
        InternalPBRComponent metallic{MPBRTextures::metallicString,
                                      InternalPBRTextureReconstructionMode::SigmoidFloor };
        InternalPBRComponent roughness{MPBRTextures::roughnessString,
                                       InternalPBRTextureReconstructionMode::SigmoidCeiling };
        InternalPBRComponent normal{MPBRTextures::normalString,
                                    InternalPBRTextureReconstructionMode::NormalMap };
    };

    using MaterialMap = std::unordered_map<std::string, IntermediateMaterial>;

    GLTF2( const std::string& _path );
    std::shared_ptr<HierGeom> convert();
    void fixupMaterials();
    std::vector<std::shared_ptr<MaterialBuilder>> Materials();

private:
    void addGeom( int meshIndex, int primitiveIndex, std::shared_ptr<HierGeom> father );
    void addNodeToHier( const int nodeIndex, std::shared_ptr<HierGeom> hier );
    void elaborateMaterial( const tinygltf::Material& mat );
    void saveMaterial( const IntermediateMaterial& im );
    void saveInternalPBRComponent( const IntermediateMaterial& _im, const InternalPBRComponent& ic );
private:
    MaterialMap matMap;
    std::string basePath;
    std::string name;
    tinygltf::Model model;
};
