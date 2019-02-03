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

namespace S {

    const std::string COLOR_2D = "PN_2D_COLOR";
    const std::string TEXTURE_2D = "PN_2D_TEXTURE";

    const std::string COLOR_3D = "PN_3D_COLOR";
    const std::string TEXTURE_3D = "PN_3D_TEXTURE";

    const std::string YUV = "PN_YUV";
    const std::string YUV_GREENSCREEN = "PN_YUV_GREENSCREEN";

    const std::string PLAIN_CUBEMAP = "PN_PLAIN_CUBEMAP";
    const std::string EQUIRECTANGULAR = "PN_EQUIRECTANGULAR";
    const std::string CONVOLUTION = "PN_PLAIN_CONVOLUTION";
    const std::string IBL_SPECULAR = "IBL_SPECULAR";
    const std::string IBL_BRDF = "IBL_BRDF";
    const std::string SKYBOX = "skybox";
    const std::string WIREFRAME = "wireframe";
    const std::string FONT_2D = "PN_2D_FONT";
    const std::string FONT = "PN_FONT";

    const std::string SH = "PN_SH";
    const std::string SH_NOTEXTURE = "PN_SH_NOTEXTURE";

    const std::string SHOWNORMAL = "PN_SHOWNORMAL";
    const std::string BLUR_HORIZONTAL = "PN_BLUR_HORIZONTAL";
    const std::string BLUR_VERTICAL = "PN_BLUR_VERTICAL";
    const std::string FINAL_COMBINE = "PN_FINAL_COMBINE";
    const std::string SHADOW_MAP = "shadowmap";

    const std::string WHITE = "white";
};


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

    const static std::string convolution = "convolution";
    const static std::string specular_prefilter = "specular_prefilter";
    const static std::string ibl_brdf = "ibl_brdf";

    static const std::string basecolorString = "basecolor";
    static const std::string heightString = "height";
    static const std::string metallicString = "metallic";
    static const std::string roughnessString = "roughness";
    static const std::string normalString = "normal";
    static const std::string ambientOcclusionString = "ambient_occlusion";

    const static std::vector<std::string> g_pbrNames{ "_basecolor","_normal","_ambient_occlusion","_roughness",
                                                      "_metallic","_height" };

    static inline const std::vector<std::string>& Names() {
        return g_pbrNames;
    }

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

JSONDATA_R( MaterialProperties, pixelTexelRatio, cost )
    float   pixelTexelRatio = 0.04f;
    float   cost = 1.0f;

    bool operator==( const MaterialProperties& rhs ) const {
        return pixelTexelRatio == rhs.pixelTexelRatio && cost == rhs.cost;
    }
};

class Material : public HeterogeneousMap {
public:
    explicit Material( const Material& _mat ) {
        clone(_mat);
    }
    explicit Material( std::shared_ptr<Material> _mat ) {
        clone(*_mat.get());
    }
    explicit Material(  const std::string& _name, const std::string& _sn ) : shaderName(_sn) {
        Name(_name);
    }
    virtual ~Material() = default;

    std::shared_ptr<Material> cloneWithNewShader( const std::string& _subkey ) {
//        return std::make_shared<Material>( Name(), _subkey, textureName, color, opacity );
        return std::make_shared<Material>(*this);
    }

    std::shared_ptr<Material> cloneWithNewProperties( const MaterialProperties& _mp ) {
//        return std::make_shared<Material>( Name(), shaderName, textureName, _mp.pigment, opacity );
        return std::make_shared<Material>(*this);
    }

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

    void resolveDynamicConstants() {
        visitTextures( [&]( TextureUniformDesc& u, unsigned int counter ) {
            if ( u.name == UniformNames::diffuseTexture) {
                u.name = Name() + MPBRTextures::g_pbrNames[0];
            } else if ( u.name == UniformNames::normalTexture) {
                u.name = Name() + MPBRTextures::g_pbrNames[1];
            } else if ( u.name == UniformNames::aoTexture) {
                u.name = Name() + MPBRTextures::g_pbrNames[2];
            } else if ( u.name == UniformNames::roughness) {
                u.name = Name() + MPBRTextures::g_pbrNames[3];
            } else if ( u.name == UniformNames::metallic) {
                u.name = Name() + MPBRTextures::g_pbrNames[4];
            } else if ( u.name == UniformNames::heightTexture) {
                u.name = Name() + MPBRTextures::g_pbrNames[5];
            } else if ( u.name == UniformNames::ibl_irradianceMap) {
                u.name = MPBRTextures::convolution;
            }else if ( u.name == UniformNames::ibl_specularMap) {
                u.name = MPBRTextures::specular_prefilter;
            }else if ( u.name == UniformNames::ibl_brdfLUTMap) {
                u.name = MPBRTextures::ibl_brdf;
            }else if ( u.name == UniformNames::yTexture) {
                u.name = Name() + "_y";
            } else if ( u.name == UniformNames::uTexture) {
                u.name = Name() + "_u";
            } else if ( u.name == UniformNames::vTexture) {
                u.name = Name() + "_v";
            }
        });
    }

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

    float getOpacity() const {
        float ret;
        get( UniformNames::opacity, ret );
        return ret;
    }

    void setOpacity( float _opacityValue ) {
        assign( UniformNames::opacity, _opacityValue );
    }

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

    void clone( const Material& _source ) {
        HeterogeneousMap::clone( _source );
        properties = _source.properties;
        shaderName = _source.shaderName;
    }

protected:
    MaterialProperties properties;
    std::string shaderName;

public:
    inline constexpr static uint64_t Version() { return 1000; }
};

class MaterialBuildable {
public:
    explicit MaterialBuildable( const std::string& _shader, const std::string& _matName = "" ) {
        material = std::make_shared<Material>( _matName, _shader );
    }

    void materialSet( const std::string& _shader, const std::string& _matName = "" ) {
        material->Name(_matName);
        material->setShaderName(_shader);
    }

    template <typename T>
    void materialConstant( const std::string& _name, T _value ) {
        material->assign( _name, _value);
    }

    void materialColor( const Color4f & _color ) {
        material->c( _color );
    }

    void materialColor( const std::string& _hexcolor ) {
        material->c( Vector4f::XTORGBA( _hexcolor ) );
    }

protected:
    std::shared_ptr<Material> material;
};

namespace MaterialDependency {
    inline const std::vector<TextureDependencyBuilderPair> textureDependencies( const std::string& _key ) {
        return Material::textureDependencies( _key );
    }
}
