#pragma once

#include <string>
#include <unordered_map>
#include "tiny_gltf.h"
#include <core/math/vector4f.h>
#include <core/name_policy.hpp>
#include <core/descriptors/uniform_names.h>
#include <core/heterogeneous_map.hpp>
#include <poly/poly.hpp>
#include <poly/import_artifacts.hpp>

class RawImage;

enum class SigmoidSlope {
    Positive,
    Negative
};

class GLTF2 : public NamePolicy<> {
public:
    virtual ~GLTF2() = default;
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
        mutable std::unordered_map<std::string, RawImage> mb;
        HeterogeneousMap values;
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

    using MaterialMap = std::unordered_map<std::string, IntermediateMaterial>;

    explicit GLTF2( const std::string& _path );
    explicit GLTF2( const SerializableContainer& _array, const std::string& _name );
    ImportGeomArtifacts convert();

private:
    void addGeom( int meshIndex, int primitiveIndex, GeomAssetSP father );
    void addNodeToHier( int nodeIndex, GeomAssetSP& hier );
    GLTF2::IntermediateMaterial elaborateMaterial( const tinygltf::Material& mat );
    void saveMaterial( const IntermediateMaterial& im );
    void saveInternalPBRComponent( const IntermediateMaterial& _im, const InternalPBRComponent& ic, const std::string& _uniformName );
private:
    MaterialMap matMap;
    std::string basePath;
    tinygltf::Model model;
};
