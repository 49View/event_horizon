//
// Created by Dado on 10/02/2018.
//

#pragma once

#include <iostream>
#include <core/http/basen.hpp>
#include <core/http/webclient.h>
#include <core/heterogeneous_map.hpp>
#include <core/descriptors/uniform_names.h>
#include "../util.h"
#include "../math/vector4f.h"
#include "../serializebin.hpp"
#include "../serialization.hpp"

const static uint32_t dependecyTagTexture = 1;
const static uint32_t dependecyTagMaterial = 2;
using TextureDependencyBuilderPair = std::pair<std::string, uint32_t>;

namespace MQSettings {
    const static std::string Low = "_lowqDD256";
    const static std::string Medium = "";
    const static std::string Hi = "_hiqDD2048";
    const static std::string UltraHi = "_ultrahiDD4k";
};

namespace MPBRTextures {
    static const std::string basecolorString = "basecolor";
    static const std::string heightString = "height";
    static const std::string metallicString = "metallic";
    static const std::string roughnessString = "roughness";
    static const std::string normalString = "normal";
    static const std::string ambientOcclusionString = "ambient_occlusion";

    static inline const std::string findTextureInString( const std::string& _value ) {
        if ( _value.find( basecolorString ) != std::string::npos ) return basecolorString;
        if ( _value.find( heightString  ) != std::string::npos ) return heightString;
        if ( _value.find( metallicString  ) != std::string::npos ) return metallicString;
        if ( _value.find( roughnessString  ) != std::string::npos ) return roughnessString;
        if ( _value.find( normalString ) != std::string::npos ) return normalString;
        if ( _value.find( ambientOcclusionString ) != std::string::npos ) return ambientOcclusionString;
        return "";
    }
}

JSONDATA_R( MaterialColor, name, color, category, brand, code, application )
    Vector4f color = Vector4f::WHITE;
    std::string name = "perfect white";
    std::string category = "white";
    std::string brand = "natural";
    std::string code = "255";
    std::string application = "matte";

    std::string toString() {
        return name + "\n" + color.toString() + "\n" + category + "\n" + brand + "\n" +
        code + "\n" + application + "\n";
    }

    std::string toURLPathEncoded() const {
        return ::toLower(application + "_" + brand + "_" + category + "_" + code + "_" + url_encode(name)) + ".col";
    }

    std::string toMetaData() const {
        std::ostringstream streamHalf1;
        std::ostringstream streamHalf2;
        std::ostringstream streamRaw;
        streamHalf1 << "{\"name\":\"" << name << "\",";
        streamHalf2 <<   "\"color\":" << color.toStringJSONArray() << "," <<
                         "\"category\":\"" << category << "\"," <<
                         "\"brand\":\"" << brand << "\"," <<
                         "\"code\":\"" << code << "\"," <<
                         "\"application\":\"" << application <<
                         "\"} ";
        std::string str = streamHalf1.str() + streamHalf2.str();
        std::string raw = bn::encode_b64( str );
        streamRaw <<   "\"raw\":\"" << raw << "\",";
        return streamHalf1.str() + streamRaw.str() + streamHalf2.str();
    }

    static uint64_t Version() { return 1000; }
};

JSONDATA_R( MaterialProperties, pixelTexelRatio, pigment )
    float   pixelTexelRatio = 0.04f;
    float   cost = 1.0f;
    Color4f pigment = Color4f::WHITE;

    bool operator==( const MaterialProperties& rhs ) const {
        return rhs.pigment == pigment && pixelTexelRatio == rhs.pixelTexelRatio && cost == rhs.cost;
    }
};

class Material : public HeterogeneousMap {
public:
    Material() {
        // We assign here some "global" constants might move it on in few minutes or leave here forever
        assign( UniformNames::opacity, 1.0f );
        assign( UniformNames::alpha, 1.0f );
    }
    virtual ~Material() = default;

//    Material( const std::string& _name, const std::string& shaderName,
//              const std::string& textureName, const Color4f& color, float opacity ) :
//              shaderName(shaderName ), textureName( textureName ), color( color ), opacity( opacity ) {
//        Name( _name );
//    }

    std::shared_ptr<Material> cloneWithNewShader( const std::string& _subkey ) {
//        return std::make_shared<Material>( Name(), _subkey, textureName, color, opacity );
        return std::make_shared<Material>();
    }

    std::shared_ptr<Material> cloneWithNewProperties( const MaterialProperties& _mp ) {
//        return std::make_shared<Material>( Name(), shaderName, textureName, _mp.pigment, opacity );
        return std::make_shared<Material>();
    }
//    std::shared_ptr<Material> cloneWithNewShader( const std::string& _subkey ) override {
//        auto ret = std::make_shared<PBRMaterial>();
//        ret->setShaderName( _subkey );
//        return ret;
//    }
//
//    std::shared_ptr<Material> cloneWithNewProperties( const MaterialProperties& _mp ) override {
//        auto ret = std::make_shared<PBRMaterial>();
//        ret->setColor( _mp.pigment );
//        return ret;
//    }

    Material& t( const std::string& _tn ) {
        assign( UniformNames::colorTexture, _tn );
        return *this;
    }

    Material& c( const Color4f& _col ) {
        assign( UniformNames::opacity, _col.w() );
        assign( UniformNames::diffuseColor, _col.xyz() );
        return *this;
    }

    const std::string& getShaderName() const {
        return shaderName;
    }

//    const std::string& getTextureName() const {
//        return textureName;
//    }
//
//    const Color4f& getColor() const {
//        return color;
//    }

    const std::vector<std::string> textureDependencies() const {
        return getTextureNames();
    }

    static const std::vector<TextureDependencyBuilderPair> textureDependencies( const std::string& _key ) {
        return {{ _key, 0xffffffff }};
    }

    void setShaderName( const std::string& _value ) {
        shaderName = _value;
    }

    std::string PBRName( const std::string& _type ) const {
        return Name() + "_" + _type;
    }

    const std::string getBaseColor() const {
        return PBRName(MPBRTextures::basecolorString);
    }

    void setBaseColor( const std::string& _baseColor ) {
//        baseColor = _baseColor;
    }

    const std::string getNormal() const {
        return PBRName(MPBRTextures::normalString);
    }

    void setNormal( const std::string& _normal ) {
//        normal = _normal;
    }

    const std::string getAmbientOcclusion() const {
        return PBRName(MPBRTextures::ambientOcclusionString);
    }

    void setAmbientOcclusion( const std::string& _ambientOcclusion ) {
//        ambientOcclusion = _ambientOcclusion;
    }

    const std::string getRoughness() const {
        return PBRName(MPBRTextures::roughnessString);
    }

    void setRoughness( const std::string& _roughness ) {
//        roughness = _roughness;
    }

    const std::string getMetallic() const {
        return PBRName(MPBRTextures::metallicString);
    }

    void setMetallic( const std::string& _metallic ) {
//        metallic = _metallic;
    }

    const std::string getHeight() const {
        return PBRName(MPBRTextures::heightString);
    }

    void setHeight( const std::string& _height ) {
//        height = _height;
    }

    float getMetallicValue() const {
        float ret;
        get( UniformNames::metallic, ret );
        return ret;
    }

    void setMetallicValue( float _metallicValue ) {
        assign( UniformNames::metallic, _metallicValue );
    }

    float getRoughnessValue() const {
        float ret;
        get( UniformNames::roughness, ret );
        return ret;
    }

    void setRoughnessValue( float _roughnessValue ) {
        assign( UniformNames::roughness, _roughnessValue );
    }

    float getAoValue() const {
        float ret;
        get( UniformNames::ao, ret );
        return ret;
    }

    void setAoValue( float _aoValue ) {
        assign( UniformNames::ao, _aoValue );
    }

//    const Vector3f& getBaseSolidColor() const {
//        return baseSolidColor;
//    }
//
//    void setBaseSolidColor( const Vector3f& baseSolidColor ) {
//        PBRMaterial::baseSolidColor = baseSolidColor;
//    }

//    void setTextureName( const std::string& _textureName ) {
//        textureName = _textureName;
//    }
//
//    void setColor( const Color4f& _color ) {
//        color = _color;
//    }

//    float getOpacity() const {
//        return opacity;
//    }
//
//    void setOpacity( float _opacityValue ) {
//        opacity = _opacityValue;
//    }

    const MaterialProperties& getProperties() const {
        return properties;
    }

    void setProperties( const MaterialProperties& properties ) {
        Material::properties = properties;
    }

    void serializeDependencies( std::shared_ptr<SerializeBin> writer ) {
//        writer->write( 1 );
//        writer->write( dependecyTagMaterial );
//        writer->write( textureName );
    }

    void serialize( std::shared_ptr<SerializeBin> writer ) {
//        writer->write( textureName );
//        writer->write( color );
//        writer->write( opacity );
    }

    void deserialize( std::shared_ptr<DeserializeBin> reader ) {
//        reader->read( textureName );
//        reader->read( color );
//        reader->read( opacity );
    }

    void clone( std::shared_ptr<Material> _source ) {
        HeterogeneousMap::clone( *(_source.get()) );
        properties = _source->properties;
        shaderName = _source->shaderName;
    }

protected:
    MaterialProperties properties;
    std::string shaderName;

public:
    inline constexpr static uint64_t Version() { return 1000; }
};

//class PBRMaterial : public Material {
//public:
//
//    PBRMaterial() = default;
//
//    PBRMaterial( std::shared_ptr<DeserializeBin> reader ) {
//        deserialize( reader );
//    }
//
//    virtual ~PBRMaterial() = default;
//
//    PBRMaterial& t( const std::string& _plain ) {
//        std::string base = getFileNameNoExt( _plain );
//        std::string ext = getFileNameExt( _plain );
//
////        textureName = _plain;
//        baseColor = base + "_basecolor";
//        normal = base + "_normal";
//        ambientOcclusion = base + "_ambient_occlusion";
//        roughness = base + "_roughness";
//        metallic = base + "_metallic";
//        height = base + "_height";
//        return *this;
//    }
//
//    static const std::vector<TextureDependencyBuilderPair> textureDependencies( const std::string& _key ) {
//        std::vector<TextureDependencyBuilderPair> ret;
//        PBRMaterial tm;//{ _key };
//        ret.push_back( { tm.baseColor, 0xffffffff } );
//        ret.push_back( { tm.normal, 0x00ff7f7f } );
//        ret.push_back( { tm.ambientOcclusion, 0xffffffff } );
//        ret.push_back( { tm.roughness, 0xffffffff } );
//        ret.push_back( { tm.metallic, 0x00000000 } );
//        ret.push_back( { tm.height, 0x00000000 } );
//        return ret;
//    }
//
//
//private:
//    std::string baseColor;
//    std::string normal;
//    std::string ambientOcclusion;
//    std::string roughness;
//    std::string metallic;
//    std::string height;
//
//    Vector3f baseSolidColor = Vector3f::ONE;
//    float metallicValue = 1.0f;
//    float roughnessValue = 1.0f;
//    float aoValue = 1.0f;
//};

namespace MaterialDependency {
    inline const std::vector<TextureDependencyBuilderPair> textureDependencies( const std::string& _key ) {
        return Material::textureDependencies( _key );
    }
}
