//
// Created by Dado on 10/02/2018.
//

#pragma once

#include <iostream>
#include <unordered_set>

#include <stb/stb_image_resize.h>
#include <stb/stb_image_write.h>

#include <core/http/basen.hpp>
#include <core/http/webclient.h>
#include <core/names.hpp>
#include <core/name_policy.hpp>
#include <core/tar_util.h>
#include <core/zlib_util.h>
#include <core/heterogeneous_map.hpp>
#include <core/descriptors/uniform_names.h>
#include <core/image_util.h>
#include <core/util.h>
#include <core/math/vector4f.h>

const static uint32_t dependecyTagTexture = 1;
const static uint32_t dependecyTagMaterial = 2;
using TextureDependencyBuilderPair = std::pair<std::string, uint32_t>;
using MaterialImageBuffers = std::unordered_map<std::string, uint8_p>;
using KnownBufferMap = std::unordered_map<std::string, std::string>;
using MaterialImageCallback = std::function<void( const std::string&, ucchar_p )>;

class MaterialColor {
public:
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
};

//JSONDATA_R( MaterialProperties, pixelTexelRatio, cost, isStreaming )
//    float   pixelTexelRatio = 0.04f;
//    float   cost = 1.0f;
//    bool    isStreaming = false;
//
//    bool operator==( const MaterialProperties& rhs ) const {
//        return pixelTexelRatio == rhs.pixelTexelRatio && cost == rhs.cost && isStreaming == rhs.isStreaming;
//    }
//
////    void serialize( std::shared_ptr<SerializeBin> writer ) const {
////        writer->write(pixelTexelRatio);
////        writer->write(cost);
////        writer->write(isStreaming);
////    }
////
////    void deserialize( std::shared_ptr<DeserializeBin> reader ) {
////        reader->read(pixelTexelRatio);
////        reader->read(cost);
////        reader->read(isStreaming);
////    }
//};

class Material : public NamePolicy<> {
public:
    RESOURCE_CTORS(Material);
    virtual ~Material() = default;
    Material(const Material& _val);
    explicit Material( const std::string& _name, const std::string& _sn );
    void bufferDecode( const unsigned char* _buffer, size_t _length ) {}

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

//    const MaterialProperties& getProperties() const;
//    void setProperties( const MaterialProperties& properties );

    void clone( const Material& _source );

    Material& buffer( const std::string& _bname, uint8_p&& _data, const std::string& _uniformName );
    Material& buffer( const std::string& _bname, const ucchar_p& _data, const std::string& _uniformName );
    const MaterialImageBuffers& Buffers() const;
//    void tarBuffers( const SerializableContainer& _bufferTarFiles );
    void Buffers( MaterialImageCallback imageCallback );

//    bool isStreammable() const;
    float translucency() const;

    KnownBufferMap knownBuffers() const;

    const std::shared_ptr<HeterogeneousMap> Values() const {
        return values;
    }

    std::shared_ptr<HeterogeneousMap> Values() {
        return values;
    }

    void Values( std::shared_ptr<HeterogeneousMap> _values ) {
        Material::values = _values;
    }

protected:
    std::shared_ptr<HeterogeneousMap>     values;
    MaterialImageBuffers                  buffers;
    std::string                           shaderName;

public:
    static Material WHITE_PBR();
};

class MaterialBuildable {
public:
    explicit MaterialBuildable( const std::string& _shader, const std::string& _matName = "" );
    void materialSet( std::shared_ptr<Material> _value );
    void materialSet( const std::string& _shader, const std::string& _matName = "" );

    void materialColor( const Color4f & _color );
    void materialColor( const std::string& _hexcolor );

protected:
    std::shared_ptr<Material> material;
};
