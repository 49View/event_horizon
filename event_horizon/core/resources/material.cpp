#include "material.h"
#include <core/descriptors/uniform_names.h>

Material::Material( const std::string& _type ) {
    makeValues(_type);
}

Material::Material( const std::string& _type, const std::string& _key ) {
    Key(_key);
    makeValues(_type);
}

Material::Material( std::shared_ptr<HeterogeneousMap> _values ) {
    values = std::move( _values );
}

Material::Material( const Color4f& _color, const std::string& _type ) {
    makeValues(_type);
    Values()->assign( UniformNames::diffuseColor, _color );
}

void Material::makeValues( const std::string& _type ) {
    values = std::make_shared<HeterogeneousMap>(_type);
}

//static inline bool isShaderStreammable( const std::string& _sn ) {
//    return ( _sn == S::YUV_GREENSCREEN || _sn == S::YUV );
//}

float Material::getMetallicValue() const {
    auto ret = values->get<float>( UniformNames::metallic );
    return ret ? *ret : 1.0f;
}

void Material::setMetallicValue( float _metallicValue ) {
    values->assign( UniformNames::metallic, _metallicValue );
}

float Material::getRoughnessValue() const {
    auto ret = values->get<float>( UniformNames::roughness );
    return ret ? *ret : 1.0f;
}

void Material::setRoughnessValue( float _roughnessValue ) {
    values->assign( UniformNames::roughness, _roughnessValue );
}

float Material::getAoValue() const {
    auto ret = values->get<float>( UniformNames::ao );
    return ret ? *ret : 1.0f;
}

void Material::setAoValue( float _aoValue ) {
    values->assign( UniformNames::ao, _aoValue );
}

float Material::getOpacity() const {
    auto ret = values->get<float>( UniformNames::opacity );
    return ret ? *ret : 1.0f;
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

V3f Material::getDiffuseColor() const {
    auto ret = values->get<V3f>( UniformNames::diffuseColor );
    return ret ? *ret : V3f::ONE;
}

void Material::setDiffuseColor( const V3f& _value ) {
    values->assign( UniformNames::diffuseColor, _value );
}

