#include "program_uniform_set.h"
#include "texture_manager.h"
#include "renderer.h"
#include "core/serializebin.hpp"

std::map<std::string, int> ProgramUniformSet::mUBONames;
std::map<std::string, int> ProgramUniformSet::mUBOOffsetMap;
std::map<std::string, unsigned int> ProgramUniformSet::mUBOHandles;

//ProgramUniformSet& ProgramUniformSet::globalStaticUniform() {
//    static ProgramUniformSet globalSU( PUSType::Global );
//    return globalSU;
//}

//ProgramUniformSet::ProgramUniformSet( const ProgramUniformSet *source ) {
//    mTextures = source->mTextures;
//    mInts = source->mInts;
//    mFloats = source->mFloats;
//    mV2fs = source->mV2fs;
//    mV3fs = source->mV3fs;
//    mV3fvs = source->mV3fvs;
//    mV4fs = source->mV4fs;
//    mM3fs = source->mM3fs;
//    mM4fs = source->mM4fs;
//    mNumUniforms = source->NumUniforms();
//}

//ProgramUniformSet::ProgramUniformSet( PUSType pt ) {
//    switch ( pt ) {
//        case PUSType::Color:
//        case PUSType::UI_3d:
//            assign( UniformNames::diffuseColor, Color4f::WHITE.xyz());
//            break;
//        case PUSType::Alpha:
//            break;
//        case PUSType::Color_alpha:
//            assign( UniformNames::diffuseColor, Color4f::WHITE.xyz());
//            break;
//        case PUSType::UI:
//            assign( UniformNames::diffuseColor, Vector3f::ONE );
//            assign( UniformNames::ui_z, 1.0f );
//            break;
//        case PUSType::Full_3d:
//        case PUSType::Full_3d_NoTexture: {
//            assign( UniformNames::ambient, Vector3f( 0.05f ));
//            assign( UniformNames::diffuseColor, Vector3f::ONE );
//            assign( UniformNames::diffuse, 1.0f );
//            assign( UniformNames::incandescenceColor, Color3f::ZERO );
//            assign( UniformNames::incandescenceFactor, 1.0f );
//            assign( UniformNames::opacity, 1.0f );
//            assign( UniformNames::alpha, 1.0f );
//            assign( UniformNames::reflectivity, 0.0f );
//            assign( UniformNames::specularColor, Vector3f::ZERO );
//            assign( UniformNames::cosinePower, 20.0f );
//            assign( UniformNames::metallic, 0.0f );
//            assign( UniformNames::roughness, 0.5f );
//            assign( UniformNames::ao, 0.000001f );
//            if ( pt == PUSType::Full_3d ) {
//                assign( UniformNames::colorTexture, TM.T( "white.png" ));
//                assign( UniformNames::normalTexture, TM.T( "white_n.png" ));
//            }
//        }
//            break;
//        case PUSType::Font:
//            assign( UniformNames::diffuseColor, Color4f::PASTEL_GRAY.xyz());
//            break;
//        case PUSType::Global:
//            break;
//        default:
//            break;
//    }
//    if ( pt != PUSType::Global ) {
//        assign( UniformNames::opacity, 1.0f );
//        assign( UniformNames::alpha, 1.0f );
//    }
//}
//
//ProgramUniformSet::ProgramUniformSet( const std::string& name ) {
//    assign( UniformNames::colorTexture, TM.T( name ));
//    assign( UniformNames::diffuseColor, Color4f::WHITE.xyz());
//}
//
//ProgramUniformSet::ProgramUniformSet( std::shared_ptr<Texture> tex ) {
//    assign( UniformNames::colorTexture, tex );
//    assign( UniformNames::diffuseColor, Color4f::WHITE.xyz());
//}
//
//ProgramUniformSet::ProgramUniformSet( std::shared_ptr<Texture> tex, const float opacity ) {
//    assign( UniformNames::colorTexture, tex );
//    assign( UniformNames::diffuseColor, Color3f::ONE );
//    assign( UniformNames::opacity, opacity );
//}
//
//ProgramUniformSet::ProgramUniformSet( std::shared_ptr<Texture> tex, const Color3f& color ) {
//    assign( UniformNames::colorTexture, tex );
//    assign( UniformNames::diffuseColor, color );
//}
//
//ProgramUniformSet::ProgramUniformSet( std::shared_ptr<Texture> tex, const Color3f& color, const float opacity ) {
//    assign( UniformNames::colorTexture, tex );
//    assign( UniformNames::diffuseColor, color );
//    assign( UniformNames::opacity, opacity );
//}
//
//ProgramUniformSet::ProgramUniformSet( const float opacity ) {
//    assign( UniformNames::opacity, opacity );
//}
//
//ProgramUniformSet::ProgramUniformSet( const Vector3f& color, ColorMatType cmt ) {
//    if ( cmt == ColorMatType::Color ) {
//        assign( UniformNames::colorTexture, TM.T( "white.png" ));
//        assign( UniformNames::diffuseColor, color );
//    }
//    if ( cmt == ColorMatType::Diffuse ) {
//        assign( UniformNames::diffuseColor, color );
//    }
//}
//
//ProgramUniformSet::ProgramUniformSet( const Vector3f& color, const float opacity, ColorMatType cmt ) {
//    if ( cmt == ColorMatType::Color ) {
//        assign( UniformNames::colorTexture, TM.T( "white.png" ));
//        assign( UniformNames::diffuseColor, color );
//    }
//    if ( cmt == ColorMatType::Diffuse ) {
//        assign( UniformNames::diffuseColor, color );
//    }
//    assign( UniformNames::opacity, opacity );
//}
//
//ProgramUniformSet::ProgramUniformSet( const Vector4f& color, ColorMatType cmt ) {
//    if ( cmt == ColorMatType::Color ) {
//        assign( UniformNames::colorTexture, TM.T( "white.png" ));
//        assign( UniformNames::diffuseColor, color.xyz());
//    }
//    if ( cmt == ColorMatType::Diffuse ) {
//        assign( UniformNames::diffuseColor, color.xyz());
//    }
//    assign( UniformNames::opacity, color.w());
//}
//
//ProgramUniformSet::ProgramUniformSet( const std::string& name, const Vector3f& color ) {
//    assign( UniformNames::colorTexture, TM.T( name ));
//    assign( UniformNames::diffuseColor, color );
//}
//
//ProgramUniformSet::ProgramUniformSet( const std::string& name, const TextureSetFlags tfs, const Vector3f& color ) {
//    assign( UniformNames::colorTexture, TM.T( name ));
//    if ( tfs & TextureSetFlags::TSF_Normal ) assign( UniformNames::normalTexture, TM.T( name + "_n" ));
//    assign( UniformNames::diffuseColor, color );
//}
//
//ProgramUniformSet::ProgramUniformSet( const std::string& name, const float opacity ) {
//    assign( UniformNames::colorTexture, TM.T( name ));
//    assign( UniformNames::diffuseColor, Color3f::ONE );
//    assign( UniformNames::opacity, opacity );
//}
//
//ProgramUniformSet::ProgramUniformSet( const std::string& name, const Vector4f& color ) {
//    assign( UniformNames::colorTexture, TM.T( name ));
//    assign( UniformNames::diffuseColor, color );
//    assign( UniformNames::opacity, color.w());
//}
//
//ProgramUniformSet::ProgramUniformSet( const Vector4f& color, const float metallic, const float roughness,
//                                      const float ao ) {
//
////    assign( UniformNames::aoTexture, TM.TD( "white.png", TSLOT_AO ));
////    assign( UniformNames::roughnessTexture, TM.TD( "black.png", TSLOT_ROUGHNESS ));
////    assign( UniformNames::metallicTexture, TM.TD( "black.png", TSLOT_METALLIC ));
//
//    assign( UniformNames::ibl_irradianceMap, TM.TD( FBNames::convolution, TSLOT_IBL_IRRADIANCE ));
//    assign( UniformNames::ibl_specularMap, TM.TD( FBNames::specular_prefilter, TSLOT_IBL_PREFILTER ));
//    assign( UniformNames::ibl_brdfLUTMap, TM.TD( FBNames::ibl_brdf, TSLOT_IBL_BRDFLUT ));
//
//    assign( UniformNames::diffuseColor, color.xyz());
//    assign( UniformNames::metallic, metallic );
//    assign( UniformNames::roughness, roughness );
//    assign( UniformNames::ao, ao );
//}
//
//ProgramUniformSet::ProgramUniformSet( const std::string& name, const Vector4f& color, [[maybe_unused]] float metallic,
//                                       [[maybe_unused]] float roughness, [[maybe_unused]] float ao ) {
//    std::string base = getFileNameNoExt( name );
//    std::string ext = getFileNameExt( name );
//
//    assign( UniformNames::colorTexture, TM.T( ImageBuilder{ base + "_basecolor" + ext }.GPUSlot( TSLOT_COLOR )
//                                                      .backup( "white.png" ) ));
//    assign( UniformNames::normalTexture, TM.T( ImageBuilder{ base + "_normal" + ext }.GPUSlot( TSLOT_NORMAL )
//                                                       .backup( "white_n.png" )));
//    assign( UniformNames::aoTexture,
//            TM.T( ImageBuilder{ base + "_ambient_occlusion" + ext }.GPUSlot( TSLOT_AO ).backup( "white.png" )));
//    assign( UniformNames::roughnessTexture, TM.T( ImageBuilder{ base + "_roughness" + ext }.GPUSlot(
//            TSLOT_ROUGHNESS ).backup( "white.png" )));
//    assign( UniformNames::metallicTexture,
//            TM.T( ImageBuilder{ base + "_metallic" + ext }.GPUSlot( TSLOT_METALLIC ).backup( "black.png" )
//            ));
//    assign( UniformNames::heightTexture,
//            TM.T( ImageBuilder{ base + "_height" + ext }.GPUSlot( TSLOT_HEIGHT ).backup( "black.png" )
//            ));
//
//    assign( UniformNames::ibl_irradianceMap, TM.TD( FBNames::convolution, TSLOT_IBL_IRRADIANCE ));
//    assign( UniformNames::ibl_specularMap, TM.TD( FBNames::specular_prefilter, TSLOT_IBL_PREFILTER ));
//    assign( UniformNames::ibl_brdfLUTMap, TM.TD( FBNames::ibl_brdf, TSLOT_IBL_BRDFLUT ));
//
//    assign( UniformNames::diffuseColor, color.xyz());
//    assign( UniformNames::metallic, 1.0f );
//    assign( UniformNames::roughness, 1.0f );
//    assign( UniformNames::ao, 1.0f );
//}

void ProgramUniformSet::serialize( std::shared_ptr<SerializeBin> writer ) {
    writer->write( mNumUniforms );

    // We cannot serialize pointers so we create a temp map with texture names instead
//    std::map<std::string, std::string> textureNames;
//    for ( auto& u : mTextures ) {
//        if ( u.first == UniformNames::colorTexture || u.first == UniformNames::normalTexture ) {
//            textureNames.insert( std::make_pair( u.first, u.second->FullName()));
//        }
//    }
//    writer->write( textureNames );

    writer->write( mFloats );
    writer->write( mInts );
    writer->write( mV2fs );
    writer->write( mV3fs );
    writer->write( mV3fvs );
    writer->write( mV4fs );
    writer->write( mM3fs );
    writer->write( mM4fs );
}

void ProgramUniformSet::deserialize( std::shared_ptr<DeserializeBin> reader ) {
    reader->read( mNumUniforms );

    // We cannot deserialize pointers so we create a temp map with texture names instead
    std::map<std::string, std::string> textureNames;
    reader->read( textureNames );
    // TODO: How do we deal with this de-serialization? Needs the extra function to complete the loading after
    // deserialization has finished
//    for ( auto& u : textureNames ) {
//        assign( u.first, TM.T( u.second ));
//    }

    reader->read( mFloats );
    reader->read( mInts );
    reader->read( mV2fs );
    reader->read( mV3fs );
    reader->read( mV3fvs );
    reader->read( mV4fs );
    reader->read( mM3fs );
    reader->read( mM4fs );

    calcHash();
}

int ProgramUniformSet::getUBOPoint( const std::string& ubo_name ) {
    auto n = mUBONames.find( ubo_name );
    if ( n == mUBONames.end()) {
        auto s = static_cast<int>( mUBONames.size());
        mUBONames[ubo_name] = s;
        return s;
    }
    return n->second;
}

void ProgramUniformSet::calcTotalNumUniforms() {
    size_t lNumUniforms = 0;
    lNumUniforms += mTextures.size();
    lNumUniforms += mInts.size();
    lNumUniforms += mFloats.size();
    lNumUniforms += mV2fs.size();
    lNumUniforms += mV3fs.size();
    lNumUniforms += mV3fvs.size();
    lNumUniforms += mV4fs.size();
    lNumUniforms += mM3fs.size();
    lNumUniforms += mM4fs.size();

    mNumUniforms = static_cast<int>( lNumUniforms );

    calcHash();
}

void ProgramUniformSet::calcHash() {
    mHash = mNumUniforms;

    for ( auto& i : mTextures ) mHash += std::hash<std::string>{}( i.second->getId());
    for ( auto& i : mInts ) mHash += i.second;
    for ( auto& i : mFloats ) mHash += static_cast<int64_t>( i.second * 1000.0f );
    for ( auto& i : mV2fs ) mHash += i.second.hash();
    for ( auto& i : mV3fs ) mHash += i.second.hash();
    for ( auto& i : mV4fs ) mHash += i.second.hash();
}

void ProgramUniformSet::setUBOStructure( const std::string& uniformName, int off ) {
    mUBOOffsetMap[uniformName] = mUBOSize;
    mUBOSize+=off;
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, int value ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &value, 4 );
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, float value ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &value, 4 );
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, float x, float y ) {
    Vector2f v{ x, y };
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &v, sizeof( Vector2f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, float x, float y, float z ) {
    Vector3f v{ x, y, z };
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &v, sizeof( Vector3f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, float x, float y, float z, float w ) {
    Vector4f v{ x, y, z, w };
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &v, sizeof( Vector4f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, const Vector2f& v ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &v, sizeof( Vector2f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, const Vector3f& v ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &v, sizeof( Vector3f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, const Vector4f& v ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &v, sizeof( Vector4f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, const Matrix2f& matrix ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &matrix, sizeof( Matrix2f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, const Matrix3f& matrix ) {
    int accOff = 0;
    for ( int t = 0; t < 3; t++ ) {
        std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName] + accOff, &matrix.getRow( t ), sizeof( Vector3f ));
        accOff += 16;
    }
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, const Matrix4f& matrix ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &matrix, sizeof( Matrix4f ));
}

void ProgramUniformSet::assign( const ProgramUniformSet *source ) {
    for ( auto& pu : source->mTextures ) assign( pu.first, pu.second );
    for ( auto& pu : source->mInts ) assign( pu.first, pu.second );
    for ( auto& pu : source->mFloats ) assign( pu.first, pu.second );
    for ( auto& pu : source->mV2fs ) assign( pu.first, pu.second );
    for ( auto& pu : source->mV3fs ) assign( pu.first, pu.second );
    for ( auto& pu : source->mV3fvs ) assign( pu.first, pu.second );
    for ( auto& pu : source->mV4fs ) assign( pu.first, pu.second );
    for ( auto& pu : source->mM3fs ) assign( pu.first, pu.second );
    for ( auto& pu : source->mM4fs ) assign( pu.first, pu.second );
}

void ProgramUniformSet::assign( const std::string& uniformName, int data ) {
    mInts[uniformName] = data;
    calcTotalNumUniforms();
}

void ProgramUniformSet::assign( const std::string& uniformName, std::shared_ptr<Texture> data ) {
    mTextures[uniformName] = data;
    calcTotalNumUniforms();
}

void ProgramUniformSet::assign( const std::string& uniformName, float data ) {
    mFloats[uniformName] = data;
    calcTotalNumUniforms();
}

void ProgramUniformSet::assign( const std::string& uniformName, double data ) {
    mFloats[uniformName] = static_cast<float>( data );
    calcTotalNumUniforms();
}

void ProgramUniformSet::assign( const std::string& uniformName, const Vector2f& data ) {
    mV2fs[uniformName] = data;
    calcTotalNumUniforms();
}

void ProgramUniformSet::assign( const std::string& uniformName, const Vector3f& data ) {
    mV3fs[uniformName] = data;
    calcTotalNumUniforms();
}

void ProgramUniformSet::assign( const std::string& uniformName, const std::vector<Vector3f>& data ) {
    mV3fvs[uniformName] = data;
    calcTotalNumUniforms();
}

void ProgramUniformSet::assign( const std::string& uniformName, const Vector4f& data ) {
    mV4fs[uniformName] = data;
    calcTotalNumUniforms();
}

void ProgramUniformSet::assign( const std::string& uniformName, const Matrix4f& data ) {
    mM4fs[uniformName] = data;
    calcTotalNumUniforms();
}

void ProgramUniformSet::assign( const std::string& uniformName, const Matrix3f& data ) {
    mM3fs[uniformName] = data;
    calcTotalNumUniforms();
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, int data ) {
    mInts[uniformName] = data;
    mNumUniforms = 1;
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, std::shared_ptr<Texture> data ) {
    mTextures[uniformName] = data;
    mNumUniforms = 1;
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, float data ) {
    mFloats[uniformName] = data;
    mNumUniforms = 1;
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, double data ) {
    mFloats[uniformName] = static_cast<float>( data );
    mNumUniforms = 1;
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, const Vector2f& data ) {
    mV2fs[uniformName] = data;
    mNumUniforms = 1;
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, const Vector3f& data ) {
    mV3fs[uniformName] = data;
    mNumUniforms = 1;
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, const std::vector<Vector3f>& data ) {
    mV3fvs[uniformName] = data;
    mNumUniforms = 1;
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, const Vector4f& data ) {
    mV4fs[uniformName] = data;
    mNumUniforms = 1;
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, const Matrix4f& data ) {
    mM4fs[uniformName] = data;
    mNumUniforms = 1;
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, const Matrix3f& data ) {
    mM3fs[uniformName] = data;
    mNumUniforms = 1;
}

//void ProgramUniformSet::setStatic( const std::string& uniformName, std::shared_ptr<Texture> data, const Program *_p ) {
//    //mTextures[uniformName] = data;
//    //calcTotalNumUniforms();
//    if ( _p != nullptr ) {
//        _p->beginRender();
//        setUniform( uniformName.c_str(), static_cast<int>( data->textureSlot()), _p->handle());
//        data->bind( data->textureSlot());
//    } else {
//        for ( const auto& i : SM.Programs()) {
//            i->beginRender();
//            setUniform( uniformName.c_str(), static_cast<int>( data->textureSlot()), i->handle());
//            data->bind( data->textureSlot());
//        }
//    }
//}
//
//void ProgramUniformSet::setStatic( const std::string& uniformName, int data ) {
//    for ( const auto& i : SM.Programs()) {
//        i->beginRender();
//        setUniform( uniformName.c_str(), data, i->handle());
//    }
//}
//
//void ProgramUniformSet::setStatic( const std::string& uniformName, float data ) {
//    for ( const auto& i : SM.Programs()) {
//        i->beginRender();
//        setUniform( uniformName.c_str(), data, i->handle());
//    }
//}
//
//void ProgramUniformSet::setStatic( const std::string& uniformName, double data ) {
//    for ( const auto& i : SM.Programs()) {
//        i->beginRender();
//        setUniform( uniformName.c_str(), static_cast<float>( data ), i->handle());
//    }
//}
//
//void ProgramUniformSet::setStatic( const std::string& uniformName, const Vector2f& data ) {
//    for ( const auto& i : SM.Programs()) {
//        i->beginRender();
//        setUniform( uniformName.c_str(), data, i->handle());
//    }
//}
//
//void ProgramUniformSet::setStatic( const std::string& uniformName, const Vector3f& data ) {
//    for ( const auto& i : SM.Programs()) {
//        i->beginRender();
//        setUniform( uniformName.c_str(), data, i->handle());
//    }
//}
//
//void ProgramUniformSet::setStatic( const std::string& uniformName, const std::vector<Vector3f>& data ) {
//    for ( const auto& i : SM.Programs()) {
//        i->beginRender();
//        setUniform( uniformName.c_str(), data, i->handle());
//    }
//}
//
//void ProgramUniformSet::setStatic( const std::string& uniformName, const Vector4f& data ) {
//    for ( const auto& i : SM.Programs()) {
//        i->beginRender();
//        setUniform( uniformName.c_str(), data, i->handle());
//    }
//}
//
//void ProgramUniformSet::setStatic( const std::string& uniformName, const Matrix4f& data ) {
//    for ( const auto& i : SM.Programs()) {
//        i->beginRender();
//        setUniform( uniformName.c_str(), data, i->handle());
//    }
//}
//
//void ProgramUniformSet::setStatic( const std::string& uniformName, const Matrix3f& data ) {
//    for ( const auto& i : SM.Programs()) {
//        i->beginRender();
//        setUniform( uniformName.c_str(), data, i->handle());
//    }
//}

void ProgramUniformSet::assign( ProgramUniformTextureBuilder& pub ) {
    if ( pub.program->hasUniform( pub.uniformName) ) {
        assign( pub.uniformName, pub.rr.TD( pub.textureName, pub.tslot ) );
    }
}

void ProgramUniformSet::clearTextures() {
    mTextures.clear();
    calcTotalNumUniforms();
}

std::shared_ptr<Texture> ProgramUniformSet::getTexture( const std::string& uniformName ) const {
    ASSERT( mTextures.find( uniformName ) != mTextures.end());
    return mTextures.at( uniformName );
}

float ProgramUniformSet::getInt( const std::string& uniformName ) const {
    ASSERT( mInts.find( uniformName ) != mInts.end());
    return static_cast<float>( mInts.at( uniformName ));
}

float ProgramUniformSet::getFloatWithDefault( const std::string& uniformName, const float def ) const {
    if ( auto it = mFloats.find( uniformName ); it != mFloats.end() ) {
        return it->second;
    }
    return def;
}

float ProgramUniformSet::getFloat( const std::string& uniformName ) const {
    ASSERT( mFloats.find( uniformName ) != mFloats.end());
    return mFloats.at( uniformName );
}

Vector2f ProgramUniformSet::getVector2f( const std::string& uniformName ) const {
    ASSERT( mV2fs.find( uniformName ) != mV2fs.end());
    return mV2fs.at( uniformName );
}

Vector3f ProgramUniformSet::getVector3f( const std::string& uniformName ) const {
    ASSERT( mV3fs.find( uniformName ) != mV3fs.end());
    return mV3fs.at( uniformName );
}

Vector4f ProgramUniformSet::getVector4f( const std::string& uniformName ) const {
    ASSERT( mV4fs.find( uniformName ) != mV4fs.end());
    return mV4fs.at( uniformName );
}

Matrix4f ProgramUniformSet::getMatrix4f( const std::string& uniformName ) const {
    ASSERT( mM4fs.find( uniformName ) != mM4fs.end());
    return mM4fs.at( uniformName );
}

Matrix3f ProgramUniformSet::getMatrix3f( const std::string& uniformName ) const {
    ASSERT( mM3fs.find( uniformName ) != mM3fs.end());
    return mM3fs.at( uniformName );
}

bool ProgramUniformSet::hasTexture( const std::string& uniformName ) const {
    return ( mTextures.find( uniformName ) != mTextures.end());
}

bool ProgramUniformSet::hasInt( const std::string& uniformName ) const {
    return ( mInts.find( uniformName ) != mInts.end());
}

bool ProgramUniformSet::hasFloat( const std::string& uniformName ) const {
    return ( mFloats.find( uniformName ) != mFloats.end());
}

bool ProgramUniformSet::hasVector2f( const std::string& uniformName ) const {
    return ( mV2fs.find( uniformName ) != mV2fs.end());
}

bool ProgramUniformSet::hasVector3f( const std::string& uniformName ) const {
    return ( mV3fs.find( uniformName ) != mV3fs.end());
}

bool ProgramUniformSet::hasVector4f( const std::string& uniformName ) const {
    return ( mV4fs.find( uniformName ) != mV4fs.end());
}

bool ProgramUniformSet::hasMatrix4f( const std::string& uniformName ) const {
    return ( mM4fs.find( uniformName ) != mM4fs.end());
}

bool ProgramUniformSet::hasMatrix3f( const std::string& uniformName ) const {
    return ( mM3fs.find( uniformName ) != mM3fs.end());
}

void ProgramUniformSet::get( const std::string& uniformName, std::shared_ptr<Texture> ret ) const {
    ASSERT( mTextures.find( uniformName ) != mTextures.end());
    ret = mTextures.at( uniformName );
}

void ProgramUniformSet::get( const std::string& uniformName, int& ret ) const {
    ASSERT( mInts.find( uniformName ) != mInts.end());
    ret = mInts.at( uniformName );
}

void ProgramUniformSet::get( const std::string& uniformName, float& ret ) const {
    ASSERT( mFloats.find( uniformName ) != mFloats.end());
    ret = mFloats.at( uniformName );
}

void ProgramUniformSet::get( const std::string& uniformName, Vector2f& ret ) const {
    ASSERT( mV2fs.find( uniformName ) != mV2fs.end());
    ret = mV2fs.at( uniformName );
}

void ProgramUniformSet::get( const std::string& uniformName, Vector3f& ret ) const {
    ASSERT( mV3fs.find( uniformName ) != mV3fs.end());
    ret = mV3fs.at( uniformName );
}

void ProgramUniformSet::get( const std::string& uniformName, Vector4f& ret ) const {
    ASSERT( mV4fs.find( uniformName ) != mV4fs.end());
    ret = mV4fs.at( uniformName );
}

void ProgramUniformSet::get( const std::string& uniformName, Matrix4f& ret ) const {
    ASSERT( mM4fs.find( uniformName ) != mM4fs.end());
    ret = mM4fs.at( uniformName );
}

void ProgramUniformSet::get( const std::string& uniformName, Matrix3f& ret ) const {
    ASSERT( mM3fs.find( uniformName ) != mM3fs.end());
    ret = mM3fs.at( uniformName );
}

void ProgramUniformSet::setOn( unsigned int handle ) {
    if ( mNumUniforms == 0 ) return;

    // Textures
    for ( auto& u : mTextures ) {
        u.second->bind( u.second->textureSlot());
        setUniform( u.first.c_str(), static_cast<int>( u.second->textureSlot()), handle );
    }
    // Normal uniforms
    for ( auto& u : mFloats ) { setUniform( u.first.c_str(), u.second, handle ); }
    for ( auto& u : mInts ) { setUniform( u.first.c_str(), u.second, handle ); }
    for ( auto& u : mV2fs ) { setUniform( u.first.c_str(), u.second, handle ); }
    for ( auto& u : mV3fs ) { setUniform( u.first.c_str(), u.second, handle ); }
    for ( auto& u : mV4fs ) { setUniform( u.first.c_str(), u.second, handle ); }
    for ( auto& u : mM3fs ) { setUniform( u.first.c_str(), u.second, handle ); }
    for ( auto& u : mM4fs ) { setUniform( u.first.c_str(), u.second, handle ); }
    for ( auto& u : mV3fvs ) { setUniform( u.first.c_str(), u.second, handle ); }
}

std::shared_ptr<ProgramUniformSet> ProgramUniformSet::clone() {
    std::shared_ptr<ProgramUniformSet> ret = std::make_shared<ProgramUniformSet>();

    ret->mTextures = mTextures;
    ret->mFloats = mFloats;
    ret->mInts = mInts;
    ret->mV2fs = mV2fs;
    ret->mV3fs = mV3fs;
    ret->mV4fs = mV4fs;
    ret->mM3fs = mM3fs;
    ret->mM4fs = mM4fs;
    ret->mV3fvs = mV3fvs;
    ret->mNumUniforms = mNumUniforms;

    return ret;
}
