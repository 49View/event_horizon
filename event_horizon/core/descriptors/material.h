//
// Created by Dado on 10/02/2018.
//

#pragma once

#include <iostream>
#include <stb/stb_image_resize.h>
#include <stb/stb_image_write.h>

#include <core/http/basen.hpp>
#include <core/http/webclient.h>
#include <core/names.hpp>
#include <core/publisher.hpp>
#include <core/tar_util.h>
#include <core/zlib_util.h>
#include <core/heterogeneous_map.hpp>
#include <core/descriptors/uniform_names.h>
#include <core/image_util.h>
#include <core/util.h>
#include <core/math/vector4f.h>
#include <core/serializebin.hpp>
#include <core/serialization.hpp>

const static uint32_t dependecyTagTexture = 1;
const static uint32_t dependecyTagMaterial = 2;
using TextureDependencyBuilderPair = std::pair<std::string, uint32_t>;
using MaterialImageBuffers = std::unordered_map<std::string, uint8_p>;
using KnownBufferMap = std::unordered_map<std::string, std::string>;
using MaterialImageCallback = std::function<void( const std::string&, ucchar_p )>;

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

JSONDATA_R( MaterialProperties, pixelTexelRatio, cost, isStreaming )
    float   pixelTexelRatio = 0.04f;
    float   cost = 1.0f;
    bool    isStreaming = false;

    bool operator==( const MaterialProperties& rhs ) const {
        return pixelTexelRatio == rhs.pixelTexelRatio && cost == rhs.cost && isStreaming == rhs.isStreaming;
    }

    void serialize( std::shared_ptr<SerializeBin> writer ) const {
        writer->write(pixelTexelRatio);
        writer->write(cost);
        writer->write(isStreaming);
    }

    void deserialize( std::shared_ptr<DeserializeBin> reader ) {
        reader->read(pixelTexelRatio);
        reader->read(cost);
        reader->read(isStreaming);
    }
};

class Material : public HeterogeneousMap, public Publisher<Material, EmptyBox> {
public:
    Material() = default;
    explicit Material( const std::string& _name, const std::string& _sn );
    ~Material() override = default;

    Material& t( const std::string& _tn );
    Material& c( const Color4f& _col );

    const std::string& getShaderName() const;
    void resolveDynamicConstants();

    const std::vector<std::string> textureDependencies() const;
    static const std::vector<TextureDependencyBuilderPair> textureDependencies( const std::string& _key );

    void setShaderName( const std::string& _value );
    std::string PBRName( const std::string& _type ) const;
    const std::string getBaseColor() const;
    const std::string getNormal() const;
    const std::string getAmbientOcclusion() const;
    const std::string getRoughness() const;
    const std::string getMetallic() const;
    const std::string getHeight() const;
    float getMetallicValue() const;
    void setMetallicValue( float _metallicValue );
    float getRoughnessValue() const;
    void setRoughnessValue( float _roughnessValue );
    float getAoValue() const;
    void setAoValue( float _aoValue );
    float getOpacity() const;
    void setOpacity( float _opacityValue );

    const MaterialProperties& getProperties() const;
    void setProperties( const MaterialProperties& properties );

    void clone( const Material& _source );

    Material& buffer( const std::string& _bname, uint8_p&& _data, const std::string& _uniformName );
    Material& buffer( const std::string& _bname, const ucchar_p& _data, const std::string& _uniformName );
    const MaterialImageBuffers& Buffers() const;
    void tarBuffers( const SerializableContainer& _bufferTarFiles );
    void Buffers( MaterialImageCallback imageCallback );

    bool isStreammable() const;

protected:
    std::string calcHashImpl() override;
    void serializeImpl( std::shared_ptr<SerializeBin> writer ) const override;
    void deserializeImpl(std::shared_ptr<DeserializeBin> reader) override;

    KnownBufferMap knownBuffers() const;
    std::string generateThumbnail() const override;

protected:
    MaterialImageBuffers buffers;
    MaterialProperties properties;
    std::string shaderName;

public:
    inline constexpr static uint64_t Version() { return 1000; }
    inline const static std::string EntityGroup() { return EntityGroup::Material; }

    friend class EntityFactory;
};

class MaterialBuildable {
public:
    explicit MaterialBuildable( const std::string& _shader, const std::string& _matName = "" );
    void materialSet( std::shared_ptr<Material> _value );
    void materialSet( const std::string& _shader, const std::string& _matName = "" );

    template <typename T>
    void materialConstant( const std::string& _name, T _value );
    void materialColor( const Color4f & _color );
    void materialColor( const std::string& _hexcolor );

protected:
    std::shared_ptr<Material> material;
};

namespace MaterialDependency {
    inline const std::vector<TextureDependencyBuilderPair> textureDependencies( const std::string& _key ) {
        return Material::textureDependencies( _key );
    }
}
