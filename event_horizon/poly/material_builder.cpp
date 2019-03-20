#include "material_builder.h"
#include "core/zlib_util.h"
#include "core/tar_util.h"
#include "poly/resources/image_builder.h"
#include "core/math/poly_utils.hpp"
#include <core/image_util.h>
#include <poly/resources/entity_factory.hpp>
#include <stb/stb_image_write.h>

MaterialBuilder::MaterialBuilder( MaterialManager& _mm,
                                  const std::string& _name,
                                  const std::string& _sn ) : ResourceBuilder( _mm ) {
    Name(_name);
    shaderName = _sn;
//    defPrePosfixes();
}

MaterialBuilder::MaterialBuilder( MaterialManager& _mm,
                                  const std::string& _name,
                                  const SerializableContainer& _data ) : ResourceBuilder( _mm ) {
    Name(_name);
    bufferTarFiles = _data;
}

void MaterialBuilder::makeDefault() {
    mm.add( std::make_shared<Material>( Name(), shaderName ) );
}

std::shared_ptr<Material> MaterialBuilder::makeDirect() {
    auto mat = std::make_shared<Material>( Name(), shaderName );
    mat->tarBuffers( bufferTarFiles );
    mm.add( mat );

    return mat;
}

//bool MaterialBuilder::makeImpl( uint8_p&& _data, const DependencyStatus _status ) {
//
//    if ( _status == DependencyStatus::LoadedSuccessfully ) {
//        auto mat = EF::create<Material>( std::move( _data ) );
//        mm.add( mat );
//    } else {
//        makeDefault();
//    }
//
//    return true;
//}

//bool MaterialManager::add( const MaterialBuilder& _pb, std::shared_ptr<Material> _material ) {
//    return add( _pb.Name(), _material );
//}
//
//bool MaterialManager::add( std::shared_ptr<Material> _material ) {
//    return add( _material->Name(), _material );
//}
//
//bool MaterialManager::add( const std::string& _key, std::shared_ptr<Material> _material ) {
//    _material->Buffers([&]( const std::string& _finame, ucchar_p _dataPtr ) {
//        ImageBuilder{ _finame }.makeDirect( *TL(), _dataPtr );
//    });
//    materialList[_key] = _material;
//    return true;
//}

//std::shared_ptr<Material> MaterialManager::get( const std::string& _key, const std::string& _subkey,
//                                                const MaterialProperties& _mp ) {
//    if ( _subkey.empty() ) {
//        auto lMat = materialList[_key];
//
//        if ( lMat->getProperties() == _mp ) {
//            return lMat;
//        } else {
////            return lMat->cloneWithNewProperties( _mp );
//        }
//
//    }
//    auto m = materialList.find( _key );
//
//    return m->second;
//}
//
//std::shared_ptr<Material> MaterialManager::get( const std::string& _key ) {
//    return materialList[_key];
//}

void MaterialManager::dump() const  {
    for ( const auto& [k, v] : Resources() ) {
        LOGR("Material: %s", k.c_str());
    }
}

std::vector<std::shared_ptr<Material>> MaterialManager::list() const {
    std::vector<std::shared_ptr<Material>> ret;
    for ( const auto& [k, v] : Resources() ) {
        ret.push_back(v);
    }
    return ret;
}

//bool ColorBuilder::makeImpl( uint8_p&& _data, const DependencyStatus _status ) {
//
////    MaterialColor col{ std::move(_data) };
////    mm.add( col );
////
////    if ( ccf ) {
////        ccf(params);
////    }
//    return true;
//}

//bool ColorManager::add( const ColorBuilder& pb, const MaterialColor& _color ) {
//    colorList[pb.Name()] = _color;
//    return true;
//}
//
//Color4f ColorManager::getColor( const std::string& _key ) {
//    if ( const auto& it = colorList.find(_key); it != colorList.end() ) {
//        return it->second.color;
//    }
//    return Color4f::WHITE;
//}
