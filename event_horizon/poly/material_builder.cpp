#include "material_builder.h"
#include "core/zlib_util.h"
#include "core/tar_util.h"
#include "core/image_builder.h"
#include "core/math/poly_utils.hpp"
#include <core/image_util.h>
#include <stb/stb_image_write.h>

MaterialBuilder::MaterialBuilder( const std::string& _name, const std::string& _sn ) : ResourceBuilder(
        _name) {
    shaderName = _sn;
    defPrePosfixes();
}

void MaterialBuilder::makeDefault( DependencyMaker& _md ) {
    auto& sg = dynamic_cast<MaterialManager&>(_md);
    auto mat = std::make_shared<Material>( Name(), shaderName );

    sg.add( *this, mat );
}

void MaterialBuilder::imageBuilderInjection( DependencyMaker& _md, const std::string& _finame, ucchar_p _dataPtr ) {
    ImageBuilder{ _finame }.makeDirect( _md, _dataPtr );
}

std::shared_ptr<Material> MaterialBuilder::makeDirect( DependencyMaker& _md ) {
    auto& sg = dynamic_cast<MaterialManager&>(_md);
    auto mat = std::make_shared<Material>( Name(), shaderName );
    mat->tarBuffers( bufferTarFiles, [&]( const std::string& _finame, ucchar_p _dataPtr ) {
        ImageBuilder{ _finame }.makeDirect( *sg.TL(), _dataPtr );
    } );
    sg.add( *this, mat );

    return mat;
}

bool MaterialBuilder::makeImpl( DependencyMaker& _md, uint8_p&& _data, const DependencyStatus _status ) {

    auto& sg = dynamic_cast<MaterialManager&>(_md);
    std::string downloadedMatName = Name();
    auto mat = std::make_shared<Material>(downloadedMatName, shaderName );
    if ( _status == DependencyStatus::LoadedSuccessfully ) {
        mat->tarBuffers( zlibUtil::inflateFromMemory( std::move( _data )),
                         [&]( const std::string& _finame, ucchar_p _dataPtr ) {
                            ImageBuilder{ _finame }.makeDirect( *sg.TL(), _dataPtr );
                         } );
    }

    sg.add( *this, mat );

    return true;
}

void MaterialBuilder::handleUninitializedDefaults( DependencyMaker& _md, const std::string& _keyTextureName ) {
    auto& sg = dynamic_cast<MaterialManager&>(_md);

    for ( const auto& td : Material::textureDependencies( _keyTextureName ))
        if ( !sg.TL()->exists( td.first )) {
            ImageBuilder{ td.first }.setSize( 4 ).backup( td.second ).makeDefault( *sg.TL());
        }
}

MaterialBuilder::MaterialBuilder( const std::string& _name, const std::vector<char>& _data ) {
    Name(_name);
    bufferTarFiles = _data;
}

bool MaterialManager::add( const MaterialBuilder& _pb, std::shared_ptr<Material> _material ) {
    auto lKey = _pb.Name();
    materialList[lKey] = _material;
    return true;
}

bool MaterialManager::add( std::shared_ptr<Material> _material ) {
    materialList[_material->Name()] = _material;
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
