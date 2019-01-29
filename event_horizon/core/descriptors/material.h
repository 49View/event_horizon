//
// Created by Dado on 10/02/2018.
//

#pragma once

#include <iostream>
#include <core/http/basen.hpp>
#include <core/http/webclient.h>
#include <core/heterogeneous_map.hpp>
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
    Material() = default;

//    Material( const std::string& tn  ) {
//        name = tn;
//    }

    Material( const std::string& _name, const std::string& shaderName,
              const std::string& textureName, const Color4f& color, float opacity ) :
              shaderName(shaderName ), textureName( textureName ), color( color ), opacity( opacity ) {
        Name( _name );
    }

    virtual std::shared_ptr<Material> cloneWithNewShader( const std::string& _subkey ) = 0;
    virtual std::shared_ptr<Material> cloneWithNewProperties( const MaterialProperties& _mp ) = 0;

    Material& t( const std::string& tn ) {
        textureName = tn;
        return *this;
    }

    Material& c( const Color4f& col ) {
        color = col;
        return *this;
    }

    const std::string& getShaderName() const {
        return shaderName;
    }

    const std::string& getTextureName() const {
        return textureName;
    }

    const Color4f& getColor() const {
        return color;
    }

    virtual const std::vector<std::string> textureDependencies() const {
        return { textureName };
    }

    static const std::vector<TextureDependencyBuilderPair> textureDependencies( const std::string& _key ) {
        return {{ _key, 0xffffffff }};
    }

    void setShaderName( const std::string& _value ) {
        shaderName = _value;
    }

    void setTextureName( const std::string& _textureName ) {
        textureName = _textureName;
    }

    void setColor( const Color4f& _color ) {
        color = _color;
    }

    float getOpacity() const {
        return opacity;
    }

    void setOpacity( float _opacityValue ) {
        opacity = _opacityValue;
    }

    const MaterialProperties& getProperties() const {
        return properties;
    }

    void setProperties( const MaterialProperties& properties ) {
        Material::properties = properties;
    }

    void serializeDependencies( std::shared_ptr<SerializeBin> writer ) {
        writer->write( 1 );
        writer->write( dependecyTagMaterial );
        writer->write( textureName );
    }

    virtual void serialize( std::shared_ptr<SerializeBin> writer ) {
        writer->write( textureName );
        writer->write( color );
        writer->write( opacity );
    }

    virtual void deserialize( std::shared_ptr<DeserializeBin> reader ) {
        reader->read( textureName );
        reader->read( color );
        reader->read( opacity );
    }

protected:
    MaterialProperties properties;
    std::string shaderName;
    std::string textureName = "white";
    Color4f color = Color4f::WHITE;
    float opacity = 1.0f;

public:
    inline constexpr static uint64_t Version() { return 1000; }
};

class GenericMaterial : public Material {
public:
    using Material::Material;

    GenericMaterial( std::shared_ptr<DeserializeBin> reader ) {
        deserialize( reader );
    }

    virtual ~GenericMaterial() = default;

    std::shared_ptr<Material> cloneWithNewShader( const std::string& _subkey ) override {
        return std::make_shared<GenericMaterial>( Name(), _subkey, textureName, color, opacity );
    }

    std::shared_ptr<Material> cloneWithNewProperties( const MaterialProperties& _mp ) override {
        return std::make_shared<GenericMaterial>( Name(), shaderName, textureName, _mp.pigment, opacity );
    }
};

class PBRMaterial : public Material {
public:

    PBRMaterial() = default;

    PBRMaterial( std::shared_ptr<DeserializeBin> reader ) {
        deserialize( reader );
    }

    virtual ~PBRMaterial() = default;

    PBRMaterial& t( const std::string& _plain ) {
        std::string base = getFileNameNoExt( _plain );
        std::string ext = getFileNameExt( _plain );

        textureName = _plain;
        baseColor = base + "_basecolor";
        normal = base + "_normal";
        ambientOcclusion = base + "_ambient_occlusion";
        roughness = base + "_roughness";
        metallic = base + "_metallic";
        height = base + "_height";
        return *this;
    }

    std::shared_ptr<Material> cloneWithNewShader( const std::string& _subkey ) override {
        auto ret = std::make_shared<PBRMaterial>();
        ret->setShaderName( _subkey );
        return ret;
    }

    std::shared_ptr<Material> cloneWithNewProperties( const MaterialProperties& _mp ) override {
        auto ret = std::make_shared<PBRMaterial>();
        ret->setColor( _mp.pigment );
        return ret;
    }

    static const std::vector<TextureDependencyBuilderPair> textureDependencies( const std::string& _key ) {
        std::vector<TextureDependencyBuilderPair> ret;
        PBRMaterial tm;//{ _key };
        ret.push_back( { tm.baseColor, 0xffffffff } );
        ret.push_back( { tm.normal, 0x00ff7f7f } );
        ret.push_back( { tm.ambientOcclusion, 0xffffffff } );
        ret.push_back( { tm.roughness, 0xffffffff } );
        ret.push_back( { tm.metallic, 0x00000000 } );
        ret.push_back( { tm.height, 0x00000000 } );
        return ret;
    }

    virtual void serialize( std::shared_ptr<SerializeBin> writer ) override {
        Material::serialize( writer );
        writer->write( baseColor );
        writer->write( normal );
        writer->write( roughness );
        writer->write( metallic );
        writer->write( ambientOcclusion );
        writer->write( height );
        writer->write( baseSolidColor );
        writer->write( metallicValue );
        writer->write( roughnessValue );
        writer->write( aoValue );
    }

    virtual void deserialize( std::shared_ptr<DeserializeBin> reader ) override {
        Material::deserialize( reader );
        reader->read( baseColor );
        reader->read( normal );
        reader->read( roughness );
        reader->read( metallic );
        reader->read( ambientOcclusion );
        reader->read( height );
        reader->read( baseSolidColor );
        reader->read( metallicValue );
        reader->read( roughnessValue );
        reader->read( aoValue );
    }

    const std::string& getBaseColor() const {
        return baseColor;
    }

    void setBaseColor( const std::string& _baseColor ) {
        baseColor = _baseColor;
    }

    const std::string& getNormal() const {
        return normal;
    }

    void setNormal( const std::string& _normal ) {
        normal = _normal;
    }

    const std::string& getAmbientOcclusion() const {
        return ambientOcclusion;
    }

    void setAmbientOcclusion( const std::string& _ambientOcclusion ) {
        ambientOcclusion = _ambientOcclusion;
    }

    const std::string& getRoughness() const {
        return roughness;
    }

    void setRoughness( const std::string& _roughness ) {
        roughness = _roughness;
    }

    const std::string& getMetallic() const {
        return metallic;
    }

    void setMetallic( const std::string& _metallic ) {
        metallic = _metallic;
    }

    const std::string& getHeight() const {
        return height;
    }

    void setHeight( const std::string& _height ) {
        height = _height;
    }

    float getMetallicValue() const {
        return metallicValue;
    }

    void setMetallicValue( float _metallicValue ) {
        metallicValue = _metallicValue;
    }

    float getRoughnessValue() const {
        return roughnessValue;
    }

    void setRoughnessValue( float _roughnessValue ) {
        roughnessValue = _roughnessValue;
    }

    float getAoValue() const {
        return aoValue;
    }

    void setAoValue( float _aoValue ) {
        aoValue = _aoValue;
    }

    const Vector3f& getBaseSolidColor() const {
        return baseSolidColor;
    }

    void setBaseSolidColor( const Vector3f& baseSolidColor ) {
        PBRMaterial::baseSolidColor = baseSolidColor;
    }

private:
    std::string baseColor;
    std::string normal;
    std::string ambientOcclusion;
    std::string roughness;
    std::string metallic;
    std::string height;

    Vector3f baseSolidColor = Vector3f::ONE;
    float metallicValue = 1.0f;
    float roughnessValue = 1.0f;
    float aoValue = 1.0f;
};

namespace MaterialDependency {
    inline const std::vector<TextureDependencyBuilderPair> textureDependencies( const std::string& _key ) {
        return Material::textureDependencies( _key );
    }
}