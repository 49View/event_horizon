//
// Created by dado on 14/08/2020.
//

#define TINYGLTF_IMPLEMENTATION
#define TINY_DNG_LOADER_IMPLEMENTATION
#include <iostream>
#include <tinygltf/include/tiny_dng_loader.h>
#include <tinygltf/include/tiny_gltf.h>

#include "gltf2utils.h"

namespace GLTF2Service {

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
        hier->pushData(hashRefName, vData->BBox3d(), "" );
    }

    void addMeshBBoxNode( IntermediateGLTF& _gltf, const tinygltf::Node& node, const GeomSP& hier ) {

        Vector3f pos = Vector3f::ZERO;
        Quaternion rot;
        Vector3f scale = Vector3f::ONE;

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

        return *rootScene->BBox3d();
    }

    V3f GLTFSize( const std::string& _key, const std::string& _path, const SerializableContainer& _array ) {
        return GLTFBBox( _key, _path, _array ).size();
    }

}