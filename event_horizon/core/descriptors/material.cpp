//
// Created by Dado on 2019-02-16.
//

#include "material.h"

static inline bool isShaderStreammable( const std::string& _sn ) {
    return ( _sn == S::YUV_GREENSCREEN || _sn == S::YUV );
}

Material::Material( const Material& _mat ) {
    clone(_mat);
}

Material::Material( std::shared_ptr<Material> _mat ) {
    clone(*_mat.get());
}

Material::Material( const std::vector<char>& _data ) {
    deserialize( std::make_shared<DeserializeBin>(_data, Version() ) );
}

Material::Material( const std::string& _name, const std::string& _sn ) {
    Name(_name);
    setShaderName(_sn);
    Material::calcHash();
}

void Material::calcHash( int64_t _base ) {
    HeterogeneousMap::calcHash( std::hash<std::string>()(getShaderName()) );
}

std::shared_ptr<Material> Material::cloneWithNewShader( const std::string& _subkey ) {
//        return std::make_shared<Material>( Name(), _subkey, textureName, color, opacity );
    return std::make_shared<Material>(*this);
}

std::shared_ptr<Material> Material::cloneWithNewProperties( const MaterialProperties& _mp ) {
//        return std::make_shared<Material>( Name(), shaderName, textureName, _mp.pigment, opacity );
    return std::make_shared<Material>(*this);
}

Material& Material::t( const std::string& _tn ) {
    assign( UniformNames::colorTexture, _tn );
    return *this;
}

Material& Material::c( const Color4f& _col ) {
    assign( UniformNames::opacity, _col.w() );
    assign( UniformNames::diffuseColor, _col.xyz() );
    return *this;
}

const std::string& Material::getShaderName() const {
    return shaderName;
}

void Material::resolveDynamicConstants() {
    visitTexturesWithKey( [&]( TextureUniformDesc& u, const std::string& _key ) {
        if ( _key == UniformNames::yTexture) {
            u.name = Name() + "_y";
        } else if ( _key == UniformNames::uTexture) {
            u.name = Name() + "_u";
        } else if ( _key == UniformNames::vTexture) {
            u.name = Name() + "_v";
        }
    });
}

const std::vector<std::string> Material::textureDependencies() const {
    return getTextureNames();
}

const std::vector<TextureDependencyBuilderPair> Material::textureDependencies( const std::string& _key ) {
    return {{ _key, 0xffffffff }};
}

void Material::setShaderName( const std::string& _value ) {
    shaderName = _value;
    properties.isStreaming = isShaderStreammable( shaderName );
}

std::string Material::PBRName( const std::string& _type ) const {
    return Name() + "_" + _type;
}

const std::string Material::getBaseColor() const {
    return PBRName(MPBRTextures::basecolorString);
}

const std::string Material::getNormal() const {
    return PBRName(MPBRTextures::normalString);
}

const std::string Material::getAmbientOcclusion() const {
    return PBRName(MPBRTextures::ambientOcclusionString);
}

const std::string Material::getRoughness() const {
    return PBRName(MPBRTextures::roughnessString);
}

const std::string Material::getMetallic() const {
    return PBRName(MPBRTextures::metallicString);
}

const std::string Material::getHeight() const {
    return PBRName(MPBRTextures::heightString);
}

float Material::getMetallicValue() const {
    float ret;
    get( UniformNames::metallic, ret );
    return ret;
}

void Material::setMetallicValue( float _metallicValue ) {
    assign( UniformNames::metallic, _metallicValue );
}

float Material::getRoughnessValue() const {
    float ret;
    get( UniformNames::roughness, ret );
    return ret;
}

void Material::setRoughnessValue( float _roughnessValue ) {
    assign( UniformNames::roughness, _roughnessValue );
}

float Material::getAoValue() const {
    float ret;
    get( UniformNames::ao, ret );
    return ret;
}

void Material::setAoValue( float _aoValue ) {
    assign( UniformNames::ao, _aoValue );
}

float Material::getOpacity() const {
    float ret;
    get( UniformNames::opacity, ret );
    return ret;
}

void Material::setOpacity( float _opacityValue ) {
    assign( UniformNames::opacity, _opacityValue );
}

const MaterialProperties& Material::getProperties() const {
    return properties;
}

void Material::setProperties( const MaterialProperties& properties ) {
    Material::properties = properties;
}

void Material::serializeDependencies( std::shared_ptr<SerializeBin> writer ) {
//        writer->write( 1 );
//        writer->write( dependecyTagMaterial );
//        writer->write( textureName );
}

void Material::serialize( std::shared_ptr<SerializeBin> writer ) const {
    HeterogeneousMap::serialize(writer);
    writer->write( buffers );
    properties.serialize(writer);
    writer->write(shaderName);
}

void Material::deserialize( std::shared_ptr<DeserializeBin> reader ) {
    HeterogeneousMap::deserialize(reader);
    reader->read( buffers );
    properties.deserialize(reader);
    reader->read(shaderName);
}

void Material::clone( const Material& _source ) {
    HeterogeneousMap::clone( _source );
    properties = _source.properties;
    shaderName = _source.shaderName;
}

Material& Material::buffer( const std::string& _bname, uint8_p&& _data, const std::string& _uniformName ) {
    if ( _data.second > 0 ) {
        buffers.emplace( std::make_pair(_bname, std::move(_data)) );
        assign( _uniformName, { _bname, 0,0,0 } );
    }
    return *this;
}

Material& Material::buffer( const std::string& _bname, const ucchar_p& _data, const std::string& _uniformName ) {
    return buffer( _bname, ucchar_pTouint8_p(_data), _uniformName );
}

KnownBufferMap Material::knownBuffers() const {
    KnownBufferMap ret;

    ret.emplace( getBaseColor(), UniformNames::diffuseTexture );
    ret.emplace( getNormal(), UniformNames::normalTexture );
    ret.emplace( getAmbientOcclusion(), UniformNames::aoTexture );
    ret.emplace( getRoughness(), UniformNames::roughnessTexture );
    ret.emplace( getMetallic(), UniformNames::metallicTexture );
    ret.emplace( getHeight(), UniformNames::heightTexture );

    return ret;
}

void Material::tarBuffers( const std::vector<char>& _bufferTarFiles, MaterialImageCallback imageCallback ) {
    if ( !_bufferTarFiles.empty() ) {
        auto kbs = knownBuffers();
        auto files = tarUtil::untar( _bufferTarFiles );
        for ( const auto& fi  : files ) {
            auto finame = getFileNameOnly(fi.name);
            auto lUniformName = finame;
            if ( auto unk = kbs.find( finame ); unk != kbs.end() ) {
                lUniformName = unk->second;
            }
            buffer( finame, fi.dataPtr, lUniformName );
            assign(lUniformName, finame );
            imageCallback( finame, fi.dataPtr );
        }
    }
}

void Material::Buffers( MaterialImageCallback imageCallback ) {
    if ( !buffers.empty() ) {
        auto kbs = knownBuffers();
        for ( const auto& [k,v] : buffers ) {
            imageCallback( k, { v.first.get(), v.second } );
        }
    }
}

const MaterialImageBuffers& Material::Buffers() const {
    return buffers;
}

bool Material::isStreammable() const {
    return properties.isStreaming;
}

std::string Material::generateThumbnail() const {
    auto thumb = std::make_unique<std::string>();

    std::unique_ptr<uint8_t[]> lthumb;
    int thumbSize = 64;
    int oc = 3;
    int obpp = 8;
    if ( const auto& it = buffers.find(getBaseColor()); it != buffers.end() ) {
        lthumb = imageUtil::resize( it->second.first.get(), it->second.second, thumbSize, thumbSize, oc, obpp );
    } else {
        V3f defColor = Color3f::ONE;
        if ( hasVector3f(UniformNames::diffuseColor) ) {
            get(UniformNames::diffuseColor, defColor);
        }
        lthumb = imageUtil::zeroImage3( Vector4f{defColor}.RGBATOI(), 1, 1 );
    }
    stbi_write_png_to_func( imageUtil::resizeCallbackb64, reinterpret_cast<void*>(thumb.get()),
                            thumbSize, thumbSize, oc, lthumb.get(), thumbSize*oc*(obpp/8) );

    return std::string{ thumb->data(), thumb->size() };
}

std::set<std::string> Material::generateTags() const {
    std::set<std::string> ret{};
    nameSplit( ret );
    ret.emplace(std::to_string(Hash()));
    return ret;
}

std::string Material::generateRawData() const {
//    std::stringstream tagStream;
//    tarUtil::TarWrite tar{ tagStream };

    auto writer = std::make_shared<SerializeBin>(Version());
    serialize( writer );
    auto matFile = writer->buffer();

//    tar.put( Name().c_str(), std::string{ matFile.begin(), matFile.end() } );
//    for ( const auto& [k,v] : buffers ) {
//        tar.put( k.c_str(), reinterpret_cast<const char*>(v.first.get()), v.second );
//    }
//    tar.finish();

    auto f = zlibUtil::deflateMemory( std::string{ matFile.begin(), matFile.end() } );
    auto rawm = bn::encode_b64( f );
    return std::string{ rawm.begin(), rawm.end() };
}

// *********************************************************************************************************
// Material Buildable
// *********************************************************************************************************

MaterialBuildable::MaterialBuildable( const std::string& _shader, const std::string& _matName ) {
    material = std::make_shared<Material>( _matName, _shader );
}

void MaterialBuildable::materialSet( std::shared_ptr<Material> _value ) {
    material = _value;
}

void MaterialBuildable::materialSet( const std::string& _shader, const std::string& _matName ) {
    material->Name(_matName);
    material->setShaderName(_shader);
}

template<typename T>
void MaterialBuildable::materialConstant( const std::string& _name, T _value ) {
    material->assign( _name, _value);
}

void MaterialBuildable::materialColor( const Color4f& _color ) {
    material->c( _color );
}

void MaterialBuildable::materialColor( const std::string& _hexcolor ) {
    material->c( Vector4f::XTORGBA( _hexcolor ) );
}
