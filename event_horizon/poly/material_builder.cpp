#include "material_builder.h"
#include "core/zlib_util.h"
#include "core/tar_util.h"
#include "core/image_builder.h"
#include "core/math/poly_utils.hpp"
#include <core/image_util.h>
#include <core/entity_factory.hpp>
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

std::shared_ptr<Material> MaterialBuilder::makeDirect( DependencyMaker& _md ) {
    auto& sg = dynamic_cast<MaterialManager&>(_md);
    auto mat = std::make_shared<Material>( Name(), shaderName );
    mat->tarBuffers( bufferTarFiles );
    sg.add( *this, mat );

    return mat;
}

bool MaterialBuilder::makeImpl( DependencyMaker& _md, uint8_p&& _data, const DependencyStatus _status ) {

    auto& sg = dynamic_cast<MaterialManager&>(_md);

    if ( _status == DependencyStatus::LoadedSuccessfully ) {
        auto mat = EF::create<Material>( std::move( _data ) );
        sg.add( *this, mat );
    } else {
        makeDefault( _md );
    }

    return true;
}

MaterialBuilder::MaterialBuilder( const std::string& _name, const SerializableContainer& _data ) {
    Name(_name);
    bufferTarFiles = _data;
}

bool MaterialManager::add( const MaterialBuilder& _pb, std::shared_ptr<Material> _material ) {
    return add( _pb.Name(), _material );
}

bool MaterialManager::add( std::shared_ptr<Material> _material ) {
    return add( _material->Name(), _material );
}

bool MaterialManager::add( const std::string& _key, std::shared_ptr<Material> _material ) {
    _material->Buffers([&]( const std::string& _finame, ucchar_p _dataPtr ) {
        ImageBuilder{ _finame }.makeDirect( *TL(), _dataPtr );
    });
    materialList[_key] = _material;
    return true;
}

std::shared_ptr<Material> MaterialManager::get( const std::string& _key, const std::string& _subkey,
                                                const MaterialProperties& _mp ) {
    if ( _subkey.empty() ) {
        auto lMat = materialList[_key];

        if ( lMat->getProperties() == _mp ) {
            return lMat;
        } else {
//            return lMat->cloneWithNewProperties( _mp );
        }

    }
    auto m = materialList.find( _key );

//    if ( m->second->getShaderName() != _subkey ) {
//        auto secondKey = _key + _subkey;
//        if ( auto sc = materialList.find( secondKey ); sc != materialList.end()) {
//            return sc->second;
//        }
//        auto mc = m->second->cloneWithNewShader( _subkey );
//        materialList[secondKey] = mc;
//        return mc;
//    }
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

    auto& sg = dynamic_cast<ColorManager&>(_md);
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
