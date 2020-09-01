#include "gltf2.h"
#include <iostream>
#include <core/util.h>

#define TINYGLTF_IMPLEMENTATION
#define TINY_DNG_LOADER_IMPLEMENTATION
#include <tinygltf/include/tiny_dng_loader.h>
#include <tinygltf/include/tiny_gltf.h>

#include <core/descriptors/gltf2utils.h>
#include <poly/scene_graph.h>

namespace GLTF2Service {

    GLTF2Service::IntermediateMaterial::IntermediateMaterial() {
        values = std::make_shared<HeterogeneousMap>( S::SH );
    }

    unsigned int accessorTypeToNumberOfComponent( int ty ) {
        if ( ty == TINYGLTF_TYPE_SCALAR) {
            return 1;
        } else if ( ty == TINYGLTF_TYPE_VECTOR) {
            return 1;
        } else if ( ty == TINYGLTF_TYPE_VEC2) {
            return 2;
        } else if ( ty == TINYGLTF_TYPE_VEC3) {
            return 3;
        } else if ( ty == TINYGLTF_TYPE_VEC4) {
            return 4;
        } else if ( ty == TINYGLTF_TYPE_MATRIX) {
            return 1;
        } else if ( ty == TINYGLTF_TYPE_MAT2) {
            return 4;
        } else if ( ty == TINYGLTF_TYPE_MAT3) {
            return 9;
        } else if ( ty == TINYGLTF_TYPE_MAT4) {
            return 16;
        }
        ASSERTV( 0, "Accessor type unknown" );
        return 0;
    }

    unsigned int componentTypeToByteSize( int ct, int type ) {
        auto bsize = 1;
        switch ( ct ) {
            case 5120:
                bsize = 1;
                break;
            case 5121:
                bsize = 1;
                break;
            case 5122:
                bsize = 2;
                break;
            case 5123:
                bsize = 2;
                break;
            case 5124:
                bsize = 4;
                break;
            case 5125:
                bsize = 4;
                break;
            case 5126:
                bsize = 4;
                break;
            default:
                ASSERTV( 0, "Invalid component type" );
        }
        return bsize * accessorTypeToNumberOfComponent( type );
    }

    struct UCharFiller {
        const unsigned char *data;
        size_t size;
        unsigned int stride;
    };

    UCharFiller
    accessorFiller( const tinygltf::Model &model, int _index, GLTF2Service::ExtraAccessorData *_ead = nullptr ) {
        UCharFiller ucf{};

        auto acc = model.accessors[_index];
        if ( !acc.minValues.empty() && _ead ) _ead->min = acc.minValues;
        if ( !acc.maxValues.empty() && _ead ) _ead->max = acc.maxValues;

        auto bvi = model.bufferViews[acc.bufferView];
        auto &bufferContent = model.buffers[bvi.buffer];
        auto bs = componentTypeToByteSize( acc.componentType, acc.type );

        ucf.data = bufferContent.data.data() + acc.byteOffset + bvi.byteOffset;
        ucf.size = acc.count;
        ucf.stride = bs;

        return ucf;
    }

    template<typename RT>
    std::vector<RT>
    fillData( const tinygltf::Model &model, int _index, GLTF2Service::ExtraAccessorData *_ead = nullptr ) {
        auto ucf = accessorFiller( model, _index, _ead );
        std::vector<RT> ret;
        for ( size_t q = 0; q < ucf.size; q++ ) {
            RT elem{ 0 };
            memcpy( &elem, ucf.data + q * ucf.stride, ucf.stride );
            ret.push_back( elem );
        }
        return ret;
    }

    Primitive gltfToPrimitive( int mode ) {
        ASSERT( mode >= 4 );

        if ( mode == 4 ) return PRIMITIVE_TRIANGLES;
        if ( mode == 5 ) return PRIMITIVE_TRIANGLE_STRIP;
        if ( mode == 6 ) return PRIMITIVE_TRIANGLE_FAN;

        return PRIMITIVE_TRIANGLES;
    }

    void fillGeom( std::shared_ptr<VData> geom, tinygltf::Model *model, int meshIndex, int primitiveIndex ) {

        auto mesh = model->meshes[meshIndex];
        tinygltf::Primitive primitive = mesh.primitives[primitiveIndex];

        geom->fillIndices( fillData<uint32_t>( *model, primitive.indices ));

        int64_t NTBFill = 0;
        constexpr uint64_t NORMAL_MASK = 2;
        constexpr uint64_t TANGENT_MASK = 4;
        constexpr uint64_t TEXCOORD_MASK = 1;
        for ( const auto&[k, v] : primitive.attributes ) {
            if ( k == "POSITION" ) {
                GLTF2Service::ExtraAccessorData ead;
                geom->fillCoors3d( fillData<Vector3f>( *model, v, &ead ));
                geom->setMax( ead.max );
                geom->setMin( ead.min );
            } else if ( k == "NORMAL" ) {
                NTBFill |= NORMAL_MASK;
                geom->fillNormals( fillData<Vector3f>( *model, v ));
            } else if ( k == "TANGENT" ) {
                NTBFill |= TANGENT_MASK;
                geom->fillTangents(fillData<Vector4f>(*model, v));
            } else if ( k == "TEXCOORD_0" ) {
                NTBFill |= TEXCOORD_MASK;
                auto fillingUV = fillData<Vector2f>( *model, v );
                geom->fillUV( fillingUV, 0 );
                geom->fillSetUV( fillingUV.size(), V2fc::ZERO, 1 ); // By default fill second uvs anyway
            } else if ( k == "TEXCOORD_1" ) {
//                geom->fillUV( fillData<Vector2f>( *model, v ), 1 );
            }
        }

        if ( !checkBitWiseFlag( NTBFill, TEXCOORD_MASK )) {
            geom->forcePlanarMapping();
        }
        if ( checkBitWiseFlag( NTBFill, NORMAL_MASK ) && checkBitWiseFlag( NTBFill, TANGENT_MASK )) {
            geom->calcBinormalFromNormAndTang();
        } else {
            geom->calcBinormal();
        }
    }

    void addGeom( SceneGraph &_sg, IntermediateGLTF &_gltf, int meshIndex, int primitiveIndex, GeomSP hier ) {

        auto mesh = _gltf.model->meshes[meshIndex];
        tinygltf::Primitive primitive = mesh.primitives[primitiveIndex];
        if ( primitive.indices < 0 ) return;

        auto material = _gltf.model->materials[primitive.material];
        auto matRef = _gltf.matMap.at( material.name );

        auto hashRefName = _gltf.key + _gltf.Name() + std::to_string( meshIndex ) + std::to_string( primitiveIndex );
        auto vdata = std::make_shared<VData>();
        GLTF2Service::fillGeom( vdata, _gltf.model.get(), meshIndex, primitiveIndex );
        _sg.B<VB>( hashRefName ).addIM( vdata );
        hier->pushData( hashRefName, vdata->BBox3d(), matRef );
    }

    template<typename T1, typename T2>
    void readParameterJsonDoubleValue( const tinygltf::Parameter v,
                                       const std::string &s1, const std::string &s2,
                                       std::pair<T1, T2> &baseColorTexture ) {
        if ( auto index = v.json_double_value.find( s1 ); index != v.json_double_value.end()) {
            if ( auto tcoord = v.json_double_value.find( s2 ); tcoord != v.json_double_value.end()) {
                baseColorTexture = { static_cast<int>(index->second), static_cast<int>(tcoord->second) };
            }
        }
    }

    void readParameterJsonDoubleValue( const tinygltf::Parameter v,
                                       const std::string &s1,
                                       int &textureIndex ) {
        if ( auto index = v.json_double_value.find( s1 ); index != v.json_double_value.end()) {
            textureIndex = static_cast<int>(index->second);
        }
    }

    void sigmoidMap( const InternalPBRComponent &ic, const IntermediateMaterial &_im, SigmoidSlope _sg ) {

        std::shared_ptr<RawImage> greyValue = EF::clone( *_im.grayScaleBaseColor.get());

        float lFloor = ( _sg == SigmoidSlope::Positive ? 1.0f - ic.value.x() : ic.value.x()) * 0.5f;

        greyValue->transform<uint8_t>( [&]( uint8_t &_value ) {
            float v = ( static_cast<float>(_value) - 127.0f ) / 127.0f;
            float vn = std::clamp( ic.value.x() + ( v * lFloor ), 0.0f, 1.0f );
            _value = static_cast<uint8_t >(vn * 255.0f);
        } );

//    _im.mb->buffer( ic.baseName, imageUtil::rawToPngMemory( greyValue ) );
    }

    void valueToColorMap( const InternalPBRComponent &ic, const IntermediateMaterial &_im ) {
        auto vc = RawImage{ 1, 1, 3, ic.value.RGBATOI() };
        vc.grayScale();
//    _im.mb->buffer( ic.baseName, imageUtil::rawToPngMemory(vc) );
    }

    void grayscaleToNormalMap( const InternalPBRComponent &ic, const IntermediateMaterial &_im ) {
        RawImage normalMap = _im.grayScaleBaseColor->toNormalMap();
//    _im.mb->buffer( ic.baseName, imageUtil::rawToPngMemory(normalMap) );
    }

    void colorToBasecolorMap( const InternalPBRComponent &ic, const IntermediateMaterial &_im ) {
        _im.grayScaleBaseColor = std::make_shared<RawImage>( 1, 1, 3, ic.value.RGBATOI());

//    _im.mb->buffer( ic.baseName, imageUtil::rawToPngMemory(_im.grayScaleBaseColor) );

        _im.grayScaleBaseColor->grayScale();
    }

    void saveInternalPBRComponent( IntermediateGLTF &_gltf, SceneGraph &_sg, Material &matRef,
                                   const IntermediateMaterial &_im,
                                   const InternalPBRComponent &ic, const std::string &_uniformName ) {
        auto baseFileName = std::string{ _im.name + "_" + ic.baseName };
        if ( ic.textureIndex != -1 ) {
            auto texture = _gltf.model->textures[ic.textureIndex];
            auto image = _gltf.model->images[texture.source];
            uint8_p imgBuffer;
            if ( !image.uri.empty()) {
                auto ext = getFileNameExt( image.name );
                imgBuffer = FM::readLocalFile( _gltf.basePath + image.name );
                auto imRef = _sg.B<IB>( baseFileName ).addIM( RawImage{ std::move( imgBuffer ) } );
                matRef.Values()->assign( _uniformName, imRef );
            } else {
                auto imRef = _sg.B<IB>( baseFileName ).addIM( RawImage{ image.width, image.height, image.component,
                                                                        image.image.data() } );
                matRef.Values()->assign( _uniformName, imRef );
            }
        }
    }

    IntermediateMaterial elaborateMaterial( SceneGraph &_sg, IntermediateGLTF &_gltf, const tinygltf::Material &mat ) {
        IntermediateMaterial im;
        im.name = toLower( mat.name );
        LOGRS( "GLTF2 Material: " << mat.name );
        removeNonAlphaCharFromString( im.name );

        auto mname = mat.name + _gltf.key;
        Material imMat{ S::SH, mname };
        for ( const auto&[k, v] : mat.values ) {
            if ( k == "baseColorFactor" ) {
                im.baseColor.value = v.number_array;
                imMat.setDiffuseColor( im.baseColor.value.xyz());
                imMat.setOpacity( im.baseColor.value.w());
            } else if ( k == "baseColorTexture" ) {
                readParameterJsonDoubleValue( v, "index", im.baseColor.textureIndex );
                saveInternalPBRComponent( _gltf, _sg, imMat, im, im.baseColor, UniformNames::diffuseTexture );
            } else if ( k == "metallicFactor" ) {
                auto lv = static_cast<float>(v.number_value);
                imMat.setMetallicValue( lv );
            } else if ( k == "metallicTexture" ) {
                readParameterJsonDoubleValue( v, "index", im.metallic.textureIndex );
                saveInternalPBRComponent( _gltf, _sg, imMat, im, im.metallic, UniformNames::metallicTexture );
            } else if ( k == "roughnessFactor" ) {
                auto lv = static_cast<float>(v.number_value);
                imMat.setRoughnessValue( lv );
            } else if ( k == "roughnessTexture" ) {
                readParameterJsonDoubleValue( v, "index", im.roughness.textureIndex );
                saveInternalPBRComponent( _gltf, _sg, imMat, im, im.roughness, UniformNames::roughnessTexture );
            }
        }

        for ( const auto&[k, v] : mat.additionalValues ) {
            if ( k == "normalTexture" ) {
                readParameterJsonDoubleValue( v, "index", im.normal.textureIndex );
                saveInternalPBRComponent( _gltf, _sg, imMat, im, im.normal, UniformNames::normalTexture );
            }
        }

        auto matRef = _sg.addMaterialIM( mname, imMat );

        _gltf.matMap[mat.name] = matRef;

        return im;
    }

    void addMeshNode( SceneGraph &_sg, IntermediateGLTF &_gltf, const tinygltf::Node &node, GeomSP hier ) {

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
                addGeom( _sg, _gltf, node.mesh, k, hier );
            }
        }

        for ( const auto &ci : node.children ) {
            auto nextNode = _gltf.model->nodes[ci];
            if ( nextNode.mesh >= 0 || !nextNode.children.empty()) {
                auto c = hier->addChildren( nextNode.name );
                addMeshNode( _sg, _gltf, nextNode, c );
            }
        }
    }

    GeomSP
    load( SceneGraph &_sg, const std::string &_key, const std::string &_path, const SerializableContainer &_array ) {

        IntermediateGLTF gltfScene = loadGLTFAsset( _key, _path, _array );

//        tinygltf::TinyGLTF loader;
//        loader.WriteGltfSceneToFile(gltfScene.model.get(), _key, true, true, true, false);

        for ( const auto &gltfMaterial : gltfScene.model->materials ) {
            elaborateMaterial( _sg, gltfScene, gltfMaterial );
        }
        auto rootScene = EF::create<Geom>( gltfScene.key );
        for ( const auto &scene : gltfScene.model->scenes ) {
            for ( int nodeIndex : scene.nodes ) {
                auto child = rootScene->addChildren( gltfScene.model->nodes[nodeIndex].name );
                addMeshNode( _sg, gltfScene, gltfScene.model->nodes[nodeIndex], child );
            }
        }

        rootScene->calcCompleteBBox3d();

        return rootScene->BBox3d()->isValid() ? rootScene : nullptr;
    }

    void save( SceneGraph &sg, GeomSP asset ) {
        tinygltf::Model model;
        tinygltf::TinyGLTF loader;

        model.asset.minVersion = "2.0";
        model.asset.version = "2.0";
        model.asset.generator = "49view";
        model.asset.copyright = "(C) 2020 49view Ltd";

        model.defaultScene = 0;

        tinygltf::Node node;
        node.name = asset->Name();

        auto vData = sg.VL().get( asset->DataV()[0].vData );

        auto bufferVD = vData->bufferPtr();
        auto bufferID = vData->indexPtr();

        tinygltf::Buffer buffer;
        auto indexData = SerializableContainer{ bufferID.first, bufferID.first + bufferID.second };
        auto soaData = SerializableContainer{ bufferVD.first, bufferVD.first + bufferVD.second };

        buffer.data.insert( buffer.data.end(), indexData.begin(), indexData.end() );
        buffer.data.insert( buffer.data.end(), soaData.begin(), soaData.end() );

        tinygltf::BufferView bufferViewIndex;
        tinygltf::BufferView bufferViewSoa;

        model.bufferViews.emplace_back(bufferViewIndex);
        model.bufferViews.emplace_back(bufferViewSoa);
        model.buffers.emplace_back(buffer);

        tinygltf::Scene defaultScene{};
        defaultScene.name = asset->Name();
        defaultScene.nodes.push_back(0);

        model.nodes.emplace_back(node);
        model.scenes.emplace_back(defaultScene);

        loader.WriteGltfSceneToFile(&model, "ecube.glb", true, true, true, false);
    }

}