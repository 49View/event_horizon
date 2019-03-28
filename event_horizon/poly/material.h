#pragma once

#include <utility>
#include <unordered_set>

#include <stb/stb_image_resize.h>
#include <stb/stb_image_write.h>

#include <core/http/basen.hpp>
#include <core/http/webclient.h>
#include <core/names.hpp>
#include <core/name_policy.hpp>
#include <core/heterogeneous_map.hpp>
#include <core/descriptors/uniform_names.h>
#include <core/image_util.h>
#include <core/util.h>
#include <core/math/vector4f.h>
#include <poly/resources/resource_utils.hpp>

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

class Material {
public:
    Material();
    RESOURCE_CTORS(Material);
    JSONSERIAL( Material, type, images, values );
    Material( std::string type, std::vector<ResourceRef> images, std::shared_ptr<HeterogeneousMap> values );
    //    Material(const Material& _val);
    void bufferDecode( const unsigned char* _buffer, size_t _length );

//    Material& t( const std::string& _tn );
//    Material& c( const Color4f& _col );

    void resolveDynamicConstants();

//    std::string PBRName( const std::string& _type ) const;
//    const std::string getBaseColor() const;
//    const std::string getNormal() const;
//    const std::string getAmbientOcclusion() const;
//    const std::string getRoughness() const;
//    const std::string getMetallic() const;
//    const std::string getHeight() const;
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

//    Material& buffer( const std::string& _bname, uint8_p&& _data, const std::string& _uniformName );
//    Material& buffer( const std::string& _bname, const ucchar_p& _data, const std::string& _uniformName );
//    const MaterialImageBuffers& Buffers() const;
//    void tarBuffers( const SerializableContainer& _bufferTarFiles );
//    void Buffers( MaterialImageCallback imageCallback );

//    bool isStreammable() const;
    float translucency() const;

//    KnownBufferMap knownBuffers() const;

    const std::shared_ptr<HeterogeneousMap> Values() const {
        return values;
    }

    std::shared_ptr<HeterogeneousMap> Values() {
        return values;
    }

    void Values( std::shared_ptr<HeterogeneousMap> _values ) {
        Material::values = std::move( _values );
    }

protected:
    std::string                           type;
    std::vector<ResourceRef>              images;
    std::shared_ptr<HeterogeneousMap>     values;

public:
    static Material WHITE_PBR();
};

