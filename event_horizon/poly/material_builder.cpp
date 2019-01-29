#include "material_builder.h"
#include "core/zlib_util.h"
#include "core/tar_util.h"
#include "core/image_builder.h"
#include "core/math/poly_utils.hpp"
#include <core/image_util.h>
#include <stb/stb_image_write.h>

const static std::vector<std::string> g_pbrNames{ "_basecolor","_normal","_ambient_occlusion","_roughness",
                                                  "_metallic","_height" };

const std::vector<std::string>& pbrNames() {
    return g_pbrNames;
}

//MaterialBuilder::MaterialBuilder( const std::string& _name, const std::string& _sn ) : ResourceBuilder(
//        _name) {
//    shaderName = _sn;
//    defPrePosfixes();
//}

MaterialBuilder::MaterialBuilder( const std::string& _name,
                                  const std::string& _sn, const MaterialProperties& _p ) : ResourceBuilder( _name) {
    shaderName = _sn;
    properties = _p;
    defPrePosfixes();
}

void MaterialBuilder::createDefaultPBRTextures( std::shared_ptr<PBRMaterial> mat, DependencyMaker& _md ) {
    auto& sg = dynamic_cast<MaterialManager&>(_md);
    ImageBuilder{ mat->getBaseColor()        }.backup(0xffffffff).makeDefault( *sg.TL() );
    ImageBuilder{ mat->getNormal()           }.backup(0x00007f7f).makeDefault( *sg.TL() );
    ImageBuilder{ mat->getAmbientOcclusion() }.backup(0xffffffff).makeDefault( *sg.TL() );
    ImageBuilder{ mat->getRoughness()        }.backup(0xffffffff).makeDefault( *sg.TL() );
    ImageBuilder{ mat->getMetallic()         }.backup(0x00000000).makeDefault( *sg.TL() );
    ImageBuilder{ mat->getHeight()           }.backup(0x00000000).makeDefault( *sg.TL() );
}

void MaterialBuilder::makeDefault( DependencyMaker& _md ) {
    auto& sg = dynamic_cast<MaterialManager&>(_md);
    auto mat = std::make_shared<PBRMaterial>();
    createDefaultPBRTextures( mat, sg );

    mat->setShaderName( shaderName );
    sg.add( *this, mat );
}

void MaterialBuilder::makeDirect( DependencyMaker& _md ) {
    auto& sg = dynamic_cast<MaterialManager&>(_md);
    auto mat = std::make_shared<PBRMaterial>();

    if ( const auto& it = buffers.find(mat->getBaseColor()); it != buffers.end() ) {
        ImageBuilder{ mat->getBaseColor()        }.backup(0xffffffff).makeDirect( *sg.TL(), it->second );
    }
    if ( const auto& it = buffers.find(mat->getNormal()); it != buffers.end() ) {
        ImageBuilder{ mat->getNormal()        }.backup(0xffffffff).makeDirect( *sg.TL(), it->second );
    }
    if ( const auto& it = buffers.find(mat->getAmbientOcclusion()); it != buffers.end() ) {
        ImageBuilder{ mat->getAmbientOcclusion()        }.backup(0xffffffff).makeDirect( *sg.TL(), it->second );
    }
    if ( const auto& it = buffers.find(mat->getRoughness()); it != buffers.end() ) {
        ImageBuilder{ mat->getRoughness()        }.backup(0xffffffff).makeDirect( *sg.TL(), it->second );
    }
    if ( const auto& it = buffers.find(mat->getMetallic()); it != buffers.end() ) {
        ImageBuilder{ mat->getMetallic()        }.backup(0xffffffff).makeDirect( *sg.TL(), it->second );
    }
    if ( const auto& it = buffers.find(mat->getHeight()); it != buffers.end() ) {
        ImageBuilder{ mat->getHeight()        }.backup(0xffffffff).makeDirect( *sg.TL(), it->second );
    }
    createDefaultPBRTextures( mat, sg );

    mat->setShaderName( shaderName );
    sg.add( *this, mat );
}

void MaterialBuilder::publish() const {
    Http::post( Url{ HttpFilePrefix::entities }, toMetaData() );
}

void resizeCallback(void* ctx, void*data, int size) {
    auto* rawThumbl = reinterpret_cast<std::string*>(ctx);
    std::string img{ reinterpret_cast<const char*>(data), static_cast<size_t>(size) };
    auto p = bn::encode_b64( img );
    *rawThumbl = { p.begin(), p.end() };
}

std::string MaterialBuilder::generateThumbnail() const {
    auto thumb = std::make_unique<std::string>();

    std::unique_ptr<uint8_t[]> lthumb;
    int thumbSize = 64;
    int oc = 3;
    int obpp = 8;
    if ( const auto& it = buffers.find(pbrPrefix() + MPBRTextures::basecolorString ); it != buffers.end() ) {
        lthumb = imageUtil::resize( it->second.first.get(), it->second.second, thumbSize, thumbSize, oc, obpp );
    } else {
        lthumb = imageUtil::zeroImage3( Vector4f{baseSolidColor}.RGBATOI(), 1, 1 );
    }
    stbi_write_png_to_func( resizeCallback, reinterpret_cast<void*>(thumb.get()),
                            thumbSize, thumbSize, oc, lthumb.get(), thumbSize*oc*(obpp/8) );

    return std::string{ thumb->data(), thumb->size() };
}

std::string MaterialBuilder::generateRawData() const {
    std::stringstream tagStream;
    tarUtil::TarWrite tar{ tagStream };

    for ( const auto& [k,v] : buffers ) {
        tar.put( (k + imageExt).c_str(), reinterpret_cast<const char*>(v.first.get()), v.second );
    }
    tar.finish();

    auto f = zlibUtil::deflateMemory( tagStream.str() );
    auto rawm = bn::encode_b64( f );
    return std::string{ rawm.begin(), rawm.end() };
}

std::string MaterialBuilder::toMetaData() const {

    MegaWriter writer;

    writer.StartObject();
    writer.serialize( CoreMetaData{Name(), EntityGroup::Material, Material::Version(),
                                   generateThumbnail(), generateRawData(), generateTags()} );
    writer.serialize( "color", baseSolidColor );
    writer.serialize( "metallicValue", metallicValue );
    writer.serialize( "roughnessValue", roughnessValue );
    writer.serialize( "aoValue", aoValue );
    writer.EndObject();

    return writer.getString();
}

bool MaterialBuilder::makeImpl( DependencyMaker& _md, uint8_p&& _data, const DependencyStatus _status ) {

    auto& sg = dynamic_cast<MaterialManager&>(_md);

    std::string downloadedMatName = Name();

    if ( _status == DependencyStatus::LoadedSuccessfully ) {
        auto inflatedData = zlibUtil::inflateFromMemory( std::move( _data ));

        auto files = tarUtil::untar( inflatedData );
        for ( const auto& fi  : files ) {
            auto fn = getFileNameNoExt( fi.name );
            downloadedMatName = string_trim_upto( fn, pbrNames() );
            auto ext = getFileNameExt( fi.name );
            if ( isFileExtAnImage( ext ) ) {
                ImageBuilder{ fn }.makeDirect( *sg.TL(), fi.dataPtr );
            }
        }
    }
    handleUninitializedDefaults( _md, downloadedMatName );

//    auto mat = std::make_shared<PBRMaterial>(downloadedMatName);
    auto mat = std::make_shared<PBRMaterial>();
    mat->setShaderName( shaderName );
    sg.add( *this, mat );

    return true;
}

void MaterialBuilder::handleUninitializedDefaults( DependencyMaker& _md, const std::string& _keyTextureName ) {
    auto& sg = dynamic_cast<MaterialManager&>(_md);

    for ( const auto& td : PBRMaterial::textureDependencies( _keyTextureName ))
        if ( !sg.TL()->exists( td.first )) {
            ImageBuilder{ td.first }.setSize( 4 ).backup( td.second ).makeDefault( *sg.TL());
        }
}

bool MaterialManager::add( const MaterialBuilder& _pb, std::shared_ptr<Material> _material ) {
    auto lKey = _pb.Name();
    materialList[lKey] = _material;
    return true;
}

std::shared_ptr<Material> MaterialManager::get( const std::string& _key, const std::string& _subkey,
                                                const MaterialProperties& _mp ) {
    if ( _subkey.empty() ) {
        auto lMat = materialList[_key];

        if ( lMat->getProperties() == _mp ) {
            return lMat;
        } else {
            return lMat->cloneWithNewProperties( _mp );
        }

    }
    auto m = materialList.find( _key );

    if ( m->second->getShaderName() != _subkey ) {
        auto secondKey = _key + _subkey;
        if ( auto sc = materialList.find( secondKey ); sc != materialList.end()) {
            return sc->second;
        }
        auto mc = m->second->cloneWithNewShader( _subkey );
        materialList[secondKey] = mc;
        return mc;
    }
    return m->second;
}

std::shared_ptr<Material> MaterialManager::get( const std::string& _key ) {
    return materialList[_key];
}

void MaterialManager::dump() const  {
    for ( const auto& [k, v] : materialList ) {
        LOGR("Material: %s", k.c_str());
    }
}

std::vector<std::shared_ptr<Material>> MaterialManager::list() const {
    std::vector<std::shared_ptr<Material>> ret;
    for ( const auto& [k, v] : materialList ) {
        ret.push_back(v);
    }
    return ret;
}

bool ColorBuilder::makeImpl( DependencyMaker& _md, uint8_p&& _data, [[maybe_unused]] const DependencyStatus _status ) {

    auto& sg = static_cast<ColorManager&>(_md);
    MaterialColor col{ std::move(_data) };
    sg.add( *this, col );

    if ( ccf ) {
        ccf(params);
    }
    return true;
}

bool ColorManager::add( const ColorBuilder& pb, const MaterialColor& _color ) {
    colorList[pb.Name()] = _color;
    return true;
}

Color4f ColorManager::getColor( const std::string& _key ) {
    if ( const auto& it = colorList.find(_key); it != colorList.end() ) {
        return it->second.color;
    }
    return Color4f::WHITE;
}
