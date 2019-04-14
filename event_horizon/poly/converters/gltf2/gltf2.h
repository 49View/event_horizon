#pragma once

#include <string>
#include <unordered_map>
#include "tiny_gltf.h"
#include <core/math/vector4f.h>
#include <core/name_policy.hpp>
#include <core/descriptors/uniform_names.h>
#include <poly/poly.hpp>

class RawImage;
class HeterogeneousMap;
class Material;
class VData;
template <typename T> class RecursiveTransformation;

struct GeomSceneArtifactData {
    std::shared_ptr<VData>    vdataSP;
    std::shared_ptr<Material> materialSP;
};

using GeomSceneArtifact = RecursiveTransformation<GeomSceneArtifactData>;
using GeomSceneArtifactVector = std::vector<std::shared_ptr<GeomSceneArtifact>>;

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

    using MaterialMap = std::unordered_map<std::string, IntermediateMaterial>;

    explicit GLTF2( const std::string& _path );
    explicit GLTF2( const SerializableContainer& _array, const std::string& _name );
    GeomSceneArtifactVector convert();

private:
    void addGeom( int meshIndex, int primitiveIndex, std::shared_ptr<GeomSceneArtifact> gnode );
    void addMeshNode( const tinygltf::Node& node, std::shared_ptr<GeomSceneArtifact> hier );
    GLTF2::IntermediateMaterial elaborateMaterial( const tinygltf::Material& mat );
    void saveMaterial( const IntermediateMaterial& im );
    void saveInternalPBRComponent( const IntermediateMaterial& _im, const InternalPBRComponent& ic, const std::string& _uniformName );
private:
    MaterialMap matMap;
    std::string basePath;
    tinygltf::Model model;
};
