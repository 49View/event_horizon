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

    tinygltf::BufferView writeIndexData( const VData* vData, tinygltf::Buffer& buffer, tinygltf::Model& model ) {
        auto bufferID = vData->indexPtr();
        auto indexData = SerializableContainer{ bufferID.first, bufferID.first + bufferID.second };

        buffer.data.insert(buffer.data.end(), indexData.begin(), indexData.end());

        tinygltf::BufferView bufferViewIndex;

        bufferViewIndex.buffer = model.buffers.size();
        bufferViewIndex.byteLength = indexData.size();
        bufferViewIndex.byteOffset = 0;
        bufferViewIndex.target = TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER;

        return bufferViewIndex;
    }

    tinygltf::BufferView writeVertexElementData( const VData* vData, std::size_t byteOffset, tinygltf::Buffer& buffer, tinygltf::Model& model ) {
        SerializableContainer posData{};
        PosTexNorTanBinUV2Col3d vformat;

        posData.resize(vData->numVerts() * vformat.size(0) * sizeof(float));
        vData->flattenStride(posData.data(), 0);
        buffer.data.insert(buffer.data.end(), posData.begin(), posData.end());

        tinygltf::BufferView bufferViewPos;

        bufferViewPos.buffer = model.buffers.size();
        bufferViewPos.byteLength = posData.size() ;
        bufferViewPos.byteOffset = byteOffset;
        bufferViewPos.target = TINYGLTF_TARGET_ARRAY_BUFFER;

        return bufferViewPos;
    }

    tinygltf::BufferView writeVertexData( const VData* vData, std::size_t byteOffset, tinygltf::Buffer& buffer, tinygltf::Model& model ) {
        auto bufferID = vData->bufferPtr();
        auto indexData = SerializableContainer{ bufferID.first, bufferID.first + bufferID.second };
        PosTexNorTanBinUV2Col3d vformat;

        buffer.data.insert(buffer.data.end(), indexData.begin(), indexData.end());

        tinygltf::BufferView bufferViewIndex;

        bufferViewIndex.buffer = model.buffers.size();
        bufferViewIndex.byteLength = indexData.size();
        bufferViewIndex.byteOffset = byteOffset;
        bufferViewIndex.byteStride = vformat.stride();
        bufferViewIndex.target = TINYGLTF_TARGET_ARRAY_BUFFER;

        return bufferViewIndex;
    }

    void writeNode( SceneGraph& sg, GeomSP asset, tinygltf::Model& model ) {
        tinygltf::Buffer buffer;
        PosTexNorTanBinUV2Col3d vformat;
        auto vData = sg.VL().get(asset->DataV()[0].vData);

        auto bufferViewIndex = writeIndexData( vData.get(), buffer, model );
        auto bufferViewSoA = writeVertexData(vData.get(), bufferViewIndex.byteLength, buffer, model );

        std::vector<double> minV = { vData->getMin()[0], vData->getMin()[1], vData->getMin()[2] };
        std::vector<double> maxV = { vData->getMax()[0], vData->getMax()[1], vData->getMax()[2] };

        tinygltf::Accessor indexAccessor = makeAccessor(model.bufferViews.size(), 0,
                                                        TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT, vData->numIndices(),
                                                        TINYGLTF_TYPE_SCALAR);
        tinygltf::Accessor posAccessor = makeAccessor(model.bufferViews.size()+1, 0,
                                                      TINYGLTF_COMPONENT_TYPE_FLOAT, vData->numVerts(),
                                                      TINYGLTF_TYPE_VEC3, &minV, &maxV);
        tinygltf::Accessor tex0Accessor = makeAccessor(model.bufferViews.size()+1, vformat.offset(1),
                                                       TINYGLTF_COMPONENT_TYPE_FLOAT, vData->numVerts(),
                                                       TINYGLTF_TYPE_VEC2);
        tinygltf::Accessor tex1Accessor = makeAccessor(model.bufferViews.size()+1, vformat.offset(2),
                                                       TINYGLTF_COMPONENT_TYPE_FLOAT, vData->numVerts(),
                                                       TINYGLTF_TYPE_VEC2);
        tinygltf::Accessor norAccessor = makeAccessor(model.bufferViews.size()+1, vformat.offset(3),
                                                       TINYGLTF_COMPONENT_TYPE_FLOAT, vData->numVerts(),
                                                       TINYGLTF_TYPE_VEC3);
        tinygltf::Accessor tanAccessor = makeAccessor(model.bufferViews.size()+1, vformat.offset(4),
                                                      TINYGLTF_COMPONENT_TYPE_FLOAT, vData->numVerts(),
                                                      TINYGLTF_TYPE_VEC4);
        tinygltf::Accessor binAccessor = makeAccessor(model.bufferViews.size()+1, vformat.offset(5),
                                                      TINYGLTF_COMPONENT_TYPE_FLOAT, vData->numVerts(),
                                                      TINYGLTF_TYPE_VEC3);
        tinygltf::Accessor colAccessor = makeAccessor(model.bufferViews.size()+1, vformat.offset(6),
                                                      TINYGLTF_COMPONENT_TYPE_FLOAT, vData->numVerts(),
                                                      TINYGLTF_TYPE_VEC4);

        tinygltf::Primitive meshPrimitive;
        meshPrimitive.attributes.emplace("POSITION", model.accessors.size() + 1);
        meshPrimitive.attributes.emplace("TEXCOORD_0", model.accessors.size() + 2);
        meshPrimitive.attributes.emplace("TEXCOORD_1", model.accessors.size() + 3);
        meshPrimitive.attributes.emplace("NORMAL", model.accessors.size() + 4);
        meshPrimitive.attributes.emplace("TANGENT", model.accessors.size() + 5);
        meshPrimitive.attributes.emplace("_BINORMAL", model.accessors.size() + 6);
        meshPrimitive.attributes.emplace("COLOR_0", model.accessors.size() + 7);
        meshPrimitive.indices = model.accessors.size();
        meshPrimitive.mode = TINYGLTF_MODE_TRIANGLES;

        tinygltf::Mesh mesh;
        mesh.primitives.emplace_back(meshPrimitive);

        model.bufferViews.emplace_back(bufferViewIndex);
        model.bufferViews.emplace_back(bufferViewSoA);
        model.buffers.emplace_back(buffer);
        model.accessors.emplace_back(indexAccessor);
        model.accessors.emplace_back(posAccessor);
        model.accessors.emplace_back(tex0Accessor);
        model.accessors.emplace_back(tex1Accessor);
        model.accessors.emplace_back(norAccessor);
        model.accessors.emplace_back(tanAccessor);
        model.accessors.emplace_back(binAccessor);
        model.accessors.emplace_back(colAccessor);

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