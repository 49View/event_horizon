#include "material_builder.h"
#include "core/zlib_util.h"
#include "core/tar_util.h"
#include "core/image_builder.h"
#include "core/math/poly_utils.hpp"
#include <core/image_util.h>
#include <stb/stb_image_write.h>

MaterialBuilder::MaterialBuilder( const std::string& _name, const MaterialType _mt, const std::string& _sn ) : ResourceBuilder(
        _name) {
    materialType = _mt;
    shaderName = _sn;
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
    MaterialManager& sg = static_cast<MaterialManager&>(_md);
    auto mat = std::make_shared<PBRMaterial>("white");
    createDefaultPBRTextures( mat, sg );

    mat->setShaderName( shaderName );
    sg.add( *this, mat );
}

void MaterialBuilder::makeDirect( DependencyMaker& _md ) {
    auto& sg = dynamic_cast<MaterialManager&>(_md);
    auto mat = std::make_shared<PBRMaterial>(Name());

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

void resizeCallback(void* ctx, void*data, int size) {
    auto* rawThumbl = reinterpret_cast<std::string*>(ctx);
    std::string img{ reinterpret_cast<const char*>(data), static_cast<size_t>(size) };
    auto p = bn::encode_b64( img );
    *rawThumbl = { p.begin(), p.end() };
}

std::string MaterialBuilder::toMetaData() {

    //    std::ofstream tagStream(tarname, std::ios::binary);
    std::stringstream tagStream;
    tarUtil::TarWrite tar{ tagStream };

    for ( const auto& [k,v] : buffers ) {
        tar.put( (k + imageExt).c_str(), reinterpret_cast<const char*>(v.first.get()), v.second );
    }
    tar.finish();

    auto f = zlibUtil::deflateMemory( tagStream.str() );
    std::vector<unsigned char> rawm = bn::encode_b64( f );
    std::unique_ptr<std::string> rawThumb = std::make_unique<std::string>();

    std::unique_ptr<uint8_t[]> thumb;
    int thumbSize = 64;
    int oc = 3;
    int obpp = 8;
    if ( const auto& it = buffers.find(pbrPrefix() + MPBRTextures::basecolorString ); it != buffers.end() ) {
        thumb = imageUtil::resize( it->second.first.get(), it->second.second, thumbSize, thumbSize, oc, obpp );
    } else {
        thumb = imageUtil::zeroImage3( Vector4f{baseSolidColor}.RGBATOI(), 1, 1 );
    }
    stbi_write_png_to_func( resizeCallback, reinterpret_cast<void*>(rawThumb.get()),
                            thumbSize, thumbSize, oc, thumb.get(), thumbSize*oc*(obpp/8) );
    std::ostringstream streamHalf1;
    std::ostringstream streamHalf2;
    std::ostringstream streamRaw;
    streamHalf1 << "{\"name\":\"" << Name() << "\",";
    streamHalf2 <<  "\"color\":" << baseSolidColor.toStringJSONArray() << ",";
    streamHalf2 <<  "\"metallicValue\":" << metallicValue << ",";
    streamHalf2 <<  "\"roughnessValue\":" << roughnessValue << ",";
    streamHalf2 <<  "\"aoValue\":" << aoValue << ",";
    streamHalf2 <<  "\"thumb\":\"" << *rawThumb << "\"}";
    std::string str = streamHalf1.str() + streamHalf2.str();
    streamRaw <<   "\"raw\":\"" << std::string{ rawm.begin(), rawm.end() } << "\",";
    return streamHalf1.str() + streamRaw.str() + streamHalf2.str();
}

bool MaterialBuilder::makeImpl( DependencyMaker& _md, uint8_p&& _data, const DependencyStatus _status ) {

    MaterialManager& sg = static_cast<MaterialManager&>(_md);

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

    auto mat = std::make_shared<PBRMaterial>(downloadedMatName);
    mat->setShaderName( shaderName );
    sg.add( *this, mat );

    return true;
}

void MaterialBuilder::handleUninitializedDefaults( DependencyMaker& _md, const std::string& _keyTextureName ) {
    MaterialManager& sg = static_cast<MaterialManager&>(_md);

    if ( materialType == MaterialType::PBR ) {
        for ( const auto& td : PBRMaterial::textureDependencies( _keyTextureName ))
            if ( !sg.TL()->exists( td.first )) {
                ImageBuilder{ td.first }.setSize( 4 ).backup( td.second ).makeDefault( *sg.TL());
            }
    }
}

bool MaterialManager::add( const MaterialBuilder& _pb, std::shared_ptr<Material> _material ) {
    auto lKey = _pb.Name();
    materialList[lKey] = _material;
    return true;
}

std::shared_ptr<Material> MaterialManager::get( const std::string& _key, const std::string& _subkey ) {
    if ( _subkey.empty()) {
        return materialList[_key];
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

bool ColorBuilder::makeImpl( DependencyMaker& _md, uint8_p&& _data, const DependencyStatus _status ) {

    ColorManager& sg = static_cast<ColorManager&>(_md);
    MaterialColor col{ _data };
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
