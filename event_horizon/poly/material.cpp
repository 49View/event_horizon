
#include "material.h"

Material::Material( const std::string& _type ) {
    values = std::make_shared<HeterogeneousMap>(_type);
}

//static inline bool isShaderStreammable( const std::string& _sn ) {
//    return ( _sn == S::YUV_GREENSCREEN || _sn == S::YUV );
//}

void Material::resolveDynamicConstants() {
//    ### MAT Fix up this
//    values->visitTexturesWithKey( [&]( std::string& u, const std::string& _key ) {
//        if ( _key == UniformNames::yTexture) {
//            u = Name() + "_y";
//        } else if ( _key == UniformNames::uTexture) {
//            u = Name() + "_u";
//        } else if ( _key == UniformNames::vTexture) {
//            u = Name() + "_v";
//        }
//    });
}

//std::string Material::PBRName( const std::string& _type ) const {
//    return Name() + "_" + _type;
//}
//
//const std::string Material::getBaseColor() const {
//    return PBRName(MPBRTextures::basecolorString);
//}
//
//const std::string Material::getNormal() const {
//    return PBRName(MPBRTextures::normalString);
//}
//
//const std::string Material::getAmbientOcclusion() const {
//    return PBRName(MPBRTextures::ambientOcclusionString);
//}
//
//const std::string Material::getRoughness() const {
//    return PBRName(MPBRTextures::roughnessString);
//}
//
//const std::string Material::getMetallic() const {
//    return PBRName(MPBRTextures::metallicString);
//}
//
//const std::string Material::getHeight() const {
//    return PBRName(MPBRTextures::heightString);
//}

float Material::getMetallicValue() const {
    float ret;
    values->get( UniformNames::metallic, ret );
    return ret;
}

void Material::setMetallicValue( float _metallicValue ) {
    values->assign( UniformNames::metallic, _metallicValue );
}

float Material::getRoughnessValue() const {
    float ret;
    values->get( UniformNames::roughness, ret );
    return ret;
}

void Material::setRoughnessValue( float _roughnessValue ) {
    values->assign( UniformNames::roughness, _roughnessValue );
}

float Material::getAoValue() const {
    float ret;
    values->get( UniformNames::ao, ret );
    return ret;
}

void Material::setAoValue( float _aoValue ) {
    values->assign( UniformNames::ao, _aoValue );
}

float Material::getOpacity() const {
    float ret;
    values->get( UniformNames::opacity, ret );
    return ret;
}

void Material::setOpacity( float _opacityValue ) {
    values->assign( UniformNames::opacity, _opacityValue );
}

float Material::translucency() const {
//    if ( shaderName == S::YUV_GREENSCREEN ) return 0.5f;
    return getOpacity();
}

const std::shared_ptr<HeterogeneousMap> Material::Values() const {
    return values;
}

std::shared_ptr<HeterogeneousMap> Material::Values() {
    return values;
}

void Material::Values( std::shared_ptr<HeterogeneousMap> _values ) {
    Material::values = std::move( _values );
}

