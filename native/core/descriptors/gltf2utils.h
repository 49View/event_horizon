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
        Vector3f min = V3fc::ZERO;
        Vector3f max = V3fc::ZERO;
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
        Color4f value = V4fc::ONE;
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

    void geomBBox( const std::shared_ptr<VData>& geom, tinygltf::Model *model, int meshIndex, int primitiveIndex ) {

        auto mesh = model->meshes[meshIndex];
        tinygltf::Primitive primitive = mesh.primitives[primitiveIndex];

        for ( const auto&[k, v] : primitive.attributes ) {
            if ( k == "POSITION" ) {
                GLTF2Service::ExtraAccessorData ead;
                auto acc = model->accessors[v];
                ead.min = acc.minValues;
                ead.max = acc.maxValues;
                geom->setMax(ead.max);
                geom->setMin(ead.min);
            }
        }
    }

    void addGeomBBox( IntermediateGLTF& _gltf, int meshIndex, int primitiveIndex, const GeomSP& hier ) {

        auto mesh = _gltf.model->meshes[meshIndex];
        tinygltf::Primitive primitive = mesh.primitives[primitiveIndex];
        if ( primitive.indices < 0 ) return;

        auto hashRefName = _gltf.key + _gltf.Name() + std::to_string( meshIndex ) + std::to_string( primitiveIndex );
        auto vData = std::make_shared<VData>();
        GLTF2Service::geomBBox(vData, _gltf.model.get(), meshIndex, primitiveIndex );
        hier->pushData(hashRefName, AABB{ vData->getMin(), vData->getMax()}, "" );
    }

    void addMeshBBoxNode( IntermediateGLTF& _gltf, const tinygltf::Node& node, const GeomSP& hier ) {

        Vector3f pos = V3fc::ZERO;
        Quaternion rot;
        Vector3f scale = V3fc::ONE;

        if ( !node.translation.empty()) {
            pos = { node.translation[0], node.translation[1], node.translation[2] };
        }
        if ( !node.rotation.empty()) {
            rot = Quaternion{ node.rotation[0], node.rotation[1], node.rotation[2], -node.rotation[3] };
        }

        if ( !node.scale.empty()) {
            scale = { node.scale[0], node.scale[1], node.scale[2] };
        }

        hier->generateLocalTransformData( pos, rot, scale );
        hier->createLocalHierMatrix( hier->fatherRootTransform());

        if ( node.mesh >= 0 ) {
            for ( size_t k = 0; k < _gltf.model->meshes[node.mesh].primitives.size(); k++ ) {
                addGeomBBox( _gltf, node.mesh, k, hier );
            }
        }

        for ( const auto &ci : node.children ) {
            auto nextNode = _gltf.model->nodes[ci];
            if ( nextNode.mesh >= 0 || !nextNode.children.empty()) {
                auto c = hier->addChildren( nextNode.name );
                addMeshBBoxNode( _gltf, nextNode, c );
            }
        }
    }

    bool handleLoadingMessages( const std::string& err, const std::string& warn, bool ret ) {
        if ( !warn.empty()) {
            LOGR( "Warn: %s", warn.c_str());
        }

        if ( !err.empty()) {
            LOGR( "Err: %s", err.c_str());
        }

        if ( !ret ) {
            LOGR( "Failed to parse glTF" );
            return false;
        }

        return true;
    }

    IntermediateGLTF
    loadGLTFAsset( const std::string& _key, const std::string& _path, const SerializableContainer& _array ) {
        tinygltf::TinyGLTF gltf_ctx;
        std::string err;
        std::string warn;
        std::string ext = getFileNameExt( _path ); //GetFilePathExtension( _path );

        IntermediateGLTF gltfScene;

        gltfScene.basePath = getFileNamePath( _path ) + "/";
        gltfScene.contentHash = _path;
        gltfScene.Name( getFileNameOnly( _path ));
        gltfScene.model = std::make_shared<tinygltf::Model>();
        gltfScene.key = _key;

        bool ret;
        if ( _array.empty()) {
            ret = gltf_ctx.LoadBinaryFromFile( gltfScene.model.get(), &err, &warn, _path );
            if ( !ret ) {
                auto str = std::string( _array.begin(), _array.end());
                ret = gltf_ctx.LoadASCIIFromString( gltfScene.model.get(), &err, &warn, str.c_str(), str.size(), "" );
            }
        } else {
            ret = gltf_ctx.LoadBinaryFromMemory( gltfScene.model.get(), &err, &warn,
                                                 reinterpret_cast<const unsigned char *>(_array.data()),
                                                 _array.size());
            if ( !ret ) {
                ret = gltf_ctx.LoadASCIIFromFile( gltfScene.model.get(), &err, &warn, _path );
            }
        }

        if ( !handleLoadingMessages( err, warn, ret )) {
            LOGRS( "Errors loading GLTF file" )
        }
        return gltfScene;
    }

    AABB GLTFBBox( const std::string& _key, const std::string& _path, const SerializableContainer& _array ) {

        IntermediateGLTF gltfScene = loadGLTFAsset( _key, _path, _array );

        auto rootScene = EF::create<Geom>( gltfScene.key );
        for ( const auto &scene : gltfScene.model->scenes ) {
            for ( int nodeIndex : scene.nodes ) {
                auto child = rootScene->addChildren( gltfScene.model->nodes[nodeIndex].name );
                addMeshBBoxNode( gltfScene, gltfScene.model->nodes[nodeIndex], child );
            }
        }

        rootScene->calcCompleteBBox3d();

        return rootScene->BBox3d();
    }

    V3f GLTFSize( const std::string& _key, const std::string& _path, const SerializableContainer& _array ) {
        return GLTFBBox( _key, _path, _array ).size();
    }
}