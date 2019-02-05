#pragma once

#include <string>
#include <unordered_map>
#include "tiny_gltf.h"
#include <core/math/vector4f.h>
#include <core/descriptors/material.h>
#include <core/raw_image.h>
#include <poly/poly.hpp>

using ImportMaterialMap = std::vector<std::shared_ptr<Material>>;

class ImportGeomArtifacts {
public:
    const GeomAssetSP& getScene() const {
        return scene;
    }

    void setScene( const GeomAssetSP& scene ) {
        ImportGeomArtifacts::scene = scene;
    }

    const ImportMaterialMap& getMaterials() const {
        return materials;
    }

    void setMaterials( const ImportMaterialMap& materials ) {
        ImportGeomArtifacts::materials = materials;
    }

    void addMaterial( std::shared_ptr<Material> _mat ) {
        materials.emplace_back(_mat);
    }
private:
    GeomAssetSP scene;
    ImportMaterialMap materials;
};

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
        mutable std::shared_ptr<Material> mb;
        mutable RawImage grayScaleBaseColor;
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
    explicit GLTF2( const std::vector<char>& _array, const std::string& _name );
    ImportGeomArtifacts convert();

private:
    void addGeom( int meshIndex, int primitiveIndex, GeomAssetSP father );
    void addNodeToHier( const int nodeIndex, GeomAssetSP& hier );
    std::shared_ptr<Material> elaborateMaterial( const tinygltf::Material& mat );
    void saveMaterial( const IntermediateMaterial& im );
    void saveInternalPBRComponent( const IntermediateMaterial& _im, const InternalPBRComponent& ic, const std::string& _uniformName );
private:
    MaterialMap matMap;
    std::string basePath;
    std::string name;
    tinygltf::Model model;
};
