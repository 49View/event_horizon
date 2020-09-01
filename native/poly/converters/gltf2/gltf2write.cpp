#include "gltf2.h"
#include <core/util.h>

#include <tinygltf/include/tiny_gltf.h>

#include <poly/scene_graph.h>

namespace GLTF2Service {

    void writeModelAssetInfo( tinygltf::Model& model ) {
        model.asset.minVersion = "2.0";
        model.asset.version = "2.0";
        model.asset.generator = "49view";
        model.asset.copyright = "(C) 2020 49view Ltd";
    }

    tinygltf::Accessor makeAccessor( int bwIndex, std::size_t byteOffset, int componentType, size_t count, int type, std::vector<double>* minV = nullptr, std::vector<double>* maxV = nullptr  ) {
        tinygltf::Accessor accessor;

        accessor.bufferView = bwIndex;
        accessor.byteOffset = byteOffset;
        accessor.componentType = componentType;
        accessor.count = count;
        accessor.type = type;
        if ( minV ) {
            accessor.minValues = *minV;
        }
        if ( maxV ) {
            accessor.maxValues = *maxV;
        }

        return accessor;
    }

    void writeNode( SceneGraph& sg, GeomSP asset, tinygltf::Model& model ) {
        auto vData = sg.VL().get(asset->DataV()[0].vData);

        auto bufferID = vData->indexPtr();

        tinygltf::Buffer buffer;
        auto indexData = SerializableContainer{ bufferID.first, bufferID.first + bufferID.second };
        SerializableContainer posData{};
        SerializableContainer tex0Data{};
//        size_t bufferViewStride = sizeof(V3f) + sizeof(V2f);
        posData.resize(vData->numVerts() * sizeof(V3f));
        tex0Data.resize(vData->numVerts() * sizeof(V2f));
        vData->flattenStride(posData.data(), 0);
        vData->flattenStride(tex0Data.data(), 1);

        buffer.data.insert(buffer.data.end(), indexData.begin(), indexData.end());
        buffer.data.insert(buffer.data.end(), posData.begin(), posData.end());
        buffer.data.insert(buffer.data.end(), tex0Data.begin(), tex0Data.end());

        tinygltf::BufferView bufferViewIndex;
        tinygltf::BufferView bufferViewPos;
        tinygltf::BufferView bufferViewTex0;

        bufferViewIndex.buffer = model.buffers.size();
        bufferViewIndex.byteLength = indexData.size();
        bufferViewIndex.byteOffset = 0;
        bufferViewIndex.target = TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER;

        bufferViewPos.buffer = model.buffers.size();
        bufferViewPos.byteLength = posData.size() ;
        bufferViewPos.byteOffset = bufferViewIndex.byteLength;
        bufferViewPos.target = TINYGLTF_TARGET_ARRAY_BUFFER;

        bufferViewTex0.buffer = model.buffers.size();
        bufferViewTex0.byteLength = tex0Data.size();
        bufferViewTex0.byteOffset = posData.size() + bufferViewIndex.byteLength;
        bufferViewTex0.target = TINYGLTF_TARGET_ARRAY_BUFFER;

        std::vector<double> minV = { vData->getMin()[0], vData->getMin()[1], vData->getMin()[2] };
        std::vector<double> maxV = { vData->getMax()[0], vData->getMax()[1], vData->getMax()[2] };

        tinygltf::Accessor indexAccessor = makeAccessor(model.bufferViews.size(), 0,
                                                        TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT, vData->numIndices(),
                                                        TINYGLTF_TYPE_SCALAR);
        tinygltf::Accessor posAccessor = makeAccessor(model.bufferViews.size()+1, 0,
                                                      TINYGLTF_COMPONENT_TYPE_FLOAT, vData->numVerts(),
                                                      TINYGLTF_TYPE_VEC3, &minV, &maxV);
        tinygltf::Accessor tex0Accessor = makeAccessor(model.bufferViews.size()+2, 0,
                                                       TINYGLTF_COMPONENT_TYPE_FLOAT, vData->numVerts(),
                                                       TINYGLTF_TYPE_VEC2);

        tinygltf::Mesh mesh;
        tinygltf::Primitive meshPrimitive;
        meshPrimitive.attributes.emplace("POSITION", model.accessors.size() + 1);
        meshPrimitive.attributes.emplace("TEXCOORD_0", model.accessors.size() + 2);
        meshPrimitive.indices = model.accessors.size();
        meshPrimitive.mode = TINYGLTF_MODE_TRIANGLES;
        mesh.primitives.emplace_back(meshPrimitive);

        model.bufferViews.emplace_back(bufferViewIndex);
        model.bufferViews.emplace_back(bufferViewPos);
        model.bufferViews.emplace_back(bufferViewTex0);
        model.buffers.emplace_back(buffer);
        model.accessors.emplace_back(indexAccessor);
        model.accessors.emplace_back(posAccessor);
        model.accessors.emplace_back(tex0Accessor);

        model.meshes.emplace_back(mesh);
    }

    void writeScene( GeomSP asset, tinygltf::Model& model ) {
        tinygltf::Scene defaultScene{};
        defaultScene.name = asset->Name();
        defaultScene.nodes.push_back(0);

        tinygltf::Node node;
        node.name = asset->Name();
        node.mesh = 0;

        model.nodes.emplace_back(node);
        model.scenes.emplace_back(defaultScene);
        model.defaultScene = 0;
    }

    void save( SceneGraph& sg, GeomSP asset ) {
        tinygltf::Model model;
        tinygltf::TinyGLTF loader;

        writeModelAssetInfo(model);

        writeNode(sg, asset, model);

        writeScene(asset, model);
        loader.WriteGltfSceneToFile(&model, "ecube.glb", true, true, true, false);
    }

}