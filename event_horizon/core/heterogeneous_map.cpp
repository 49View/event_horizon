//
// Created by Dado on 2019-01-28.
//

#include "heterogeneous_map.hpp"
#include <core/serializebin.hpp>

void HeterogeneousMap::calcTotalNumUniforms() {
    size_t lNumUniforms = 0;
    lNumUniforms += mTextures.size();
    lNumUniforms += mInts.size();
    lNumUniforms += mFloats.size();
    lNumUniforms += mV2fs.size();
    lNumUniforms += mV3fs.size();
    lNumUniforms += mV3fvs.size();
    lNumUniforms += mV4fs.size();
    lNumUniforms += mM3fs.size();
    lNumUniforms += mM4fs.size();

    mNumUniforms = static_cast<int>( lNumUniforms );

    calcHash();
}

void HeterogeneousMap::calcHash( int64_t _base ) {
    mHash = mNumUniforms;

    mHash += std::hash<std::string>()(Name());
    for ( auto& i : mTextures ) mHash += std::hash<std::string>{}(i.first);
    for ( auto& i : mInts ) mHash += i.second;
    for ( auto& i : mFloats ) mHash += static_cast<int64_t>( i.second * 1000.0f );
    for ( auto& i : mV2fs ) mHash += i.second.hash();
    for ( auto& i : mV3fs ) mHash += i.second.hash();
    for ( auto& i : mV4fs ) mHash += i.second.hash();
    mHash += _base;
}

void HeterogeneousMap::inject( const HeterogeneousMap& source ) {
    for ( auto& pu : source.mTextures ) assign( pu.first, pu.second );
    for ( auto& pu : source.mInts ) assign( pu.first, pu.second );
    for ( auto& pu : source.mFloats ) assign( pu.first, pu.second );
    for ( auto& pu : source.mV2fs ) assign( pu.first, pu.second );
    for ( auto& pu : source.mV3fs ) assign( pu.first, pu.second );
    for ( auto& pu : source.mV3fvs ) assign( pu.first, pu.second );
    for ( auto& pu : source.mV4fs ) assign( pu.first, pu.second );
    for ( auto& pu : source.mM3fs ) assign( pu.first, pu.second );
    for ( auto& pu : source.mM4fs ) assign( pu.first, pu.second );

    calcTotalNumUniforms();
}

void HeterogeneousMap::assign( const std::string& uniformName, int data ) {
    mInts[uniformName] = data;
    calcTotalNumUniforms();
}

void HeterogeneousMap::assign( const std::string& uniformName, const TextureUniformDesc& data ) {
    mTextures[uniformName] = data;
}

void HeterogeneousMap::assign( const std::string& uniformName, const std::string& data ) {
    mTextures[uniformName] = { data, 0, 0 };
}

void HeterogeneousMap::assign( const std::string& uniformName, float data ) {
    mFloats[uniformName] = data;
    calcTotalNumUniforms();
}

void HeterogeneousMap::assign( const std::string& uniformName, double data ) {
    mFloats[uniformName] = static_cast<float>( data );
    calcTotalNumUniforms();
}

void HeterogeneousMap::assign( const std::string& uniformName, const Vector2f& data ) {
    mV2fs[uniformName] = data;
    calcTotalNumUniforms();
}

void HeterogeneousMap::assign( const std::string& uniformName, const Vector3f& data ) {
    mV3fs[uniformName] = data;
    calcTotalNumUniforms();
}

void HeterogeneousMap::assign( const std::string& uniformName, const std::vector<Vector3f>& data ) {
    mV3fvs[uniformName] = data;
    calcTotalNumUniforms();
}

void HeterogeneousMap::assign( const std::string& uniformName, const Vector4f& data ) {
    mV4fs[uniformName] = data;
    calcTotalNumUniforms();
}

void HeterogeneousMap::assign( const std::string& uniformName, const Matrix4f& data ) {
    mM4fs[uniformName] = data;
    calcTotalNumUniforms();
}

void HeterogeneousMap::assign( const std::string& uniformName, const Matrix3f& data ) {
    mM3fs[uniformName] = data;
    calcTotalNumUniforms();
}

std::string HeterogeneousMap::getTexture( const std::string& uniformName ) const {
    ASSERT( mTextures.find( uniformName ) != mTextures.end());
    return mTextures.at( uniformName ).name;
}

float HeterogeneousMap::getInt( const std::string& uniformName ) const {
    ASSERT( mInts.find( uniformName ) != mInts.end());
    return static_cast<float>( mInts.at( uniformName ));
}

float HeterogeneousMap::getFloatWithDefault( const std::string& uniformName, const float def ) const {
    if ( auto it = mFloats.find( uniformName ); it != mFloats.end() ) {
        return it->second;
    }
    return def;
}

float HeterogeneousMap::getFloat( const std::string& uniformName ) const {
    ASSERT( mFloats.find( uniformName ) != mFloats.end());
    return mFloats.at( uniformName );
}

Vector2f HeterogeneousMap::getVector2f( const std::string& uniformName ) const {
    ASSERT( mV2fs.find( uniformName ) != mV2fs.end());
    return mV2fs.at( uniformName );
}

Vector3f HeterogeneousMap::getVector3f( const std::string& uniformName ) const {
    ASSERT( mV3fs.find( uniformName ) != mV3fs.end());
    return mV3fs.at( uniformName );
}

Vector4f HeterogeneousMap::getVector4f( const std::string& uniformName ) const {
    ASSERT( mV4fs.find( uniformName ) != mV4fs.end());
    return mV4fs.at( uniformName );
}

Matrix4f HeterogeneousMap::getMatrix4f( const std::string& uniformName ) const {
    ASSERT( mM4fs.find( uniformName ) != mM4fs.end());
    return mM4fs.at( uniformName );
}

Matrix3f HeterogeneousMap::getMatrix3f( const std::string& uniformName ) const {
    ASSERT( mM3fs.find( uniformName ) != mM3fs.end());
    return mM3fs.at( uniformName );
}

bool HeterogeneousMap::hasTexture( const std::string& uniformName ) const {
    return ( mTextures.find( uniformName ) != mTextures.end());
}

bool HeterogeneousMap::hasInt( const std::string& uniformName ) const {
    return ( mInts.find( uniformName ) != mInts.end());
}

bool HeterogeneousMap::hasFloat( const std::string& uniformName ) const {
    return ( mFloats.find( uniformName ) != mFloats.end());
}

bool HeterogeneousMap::hasVector2f( const std::string& uniformName ) const {
    return ( mV2fs.find( uniformName ) != mV2fs.end());
}

bool HeterogeneousMap::hasVector3f( const std::string& uniformName ) const {
    return ( mV3fs.find( uniformName ) != mV3fs.end());
}

bool HeterogeneousMap::hasVector4f( const std::string& uniformName ) const {
    return ( mV4fs.find( uniformName ) != mV4fs.end());
}

bool HeterogeneousMap::hasMatrix4f( const std::string& uniformName ) const {
    return ( mM4fs.find( uniformName ) != mM4fs.end());
}

bool HeterogeneousMap::hasMatrix3f( const std::string& uniformName ) const {
    return ( mM3fs.find( uniformName ) != mM3fs.end());
}

void HeterogeneousMap::get( const std::string& uniformName, std::string& ret ) const {
    ASSERT( mTextures.find( uniformName ) != mTextures.end());
    ret = mTextures.at( uniformName ).name;
}

void HeterogeneousMap::get( const std::string& uniformName, int& ret ) const {
    ASSERT( mInts.find( uniformName ) != mInts.end());
    ret = mInts.at( uniformName );
}

bool HeterogeneousMap::get( const std::string& uniformName, float& ret ) const {
    if ( const auto& it = mFloats.find( uniformName ); it != mFloats.end()) {
        ret = it->second;
        return true;
    }
    ret = 1.0f;
    return false;
}

void HeterogeneousMap::get( const std::string& uniformName, Vector2f& ret ) const {
    ASSERT( mV2fs.find( uniformName ) != mV2fs.end());
    ret = mV2fs.at( uniformName );
}

void HeterogeneousMap::get( const std::string& uniformName, Vector3f& ret ) const {
    ASSERT( mV3fs.find( uniformName ) != mV3fs.end());
    ret = mV3fs.at( uniformName );
}

void HeterogeneousMap::get( const std::string& uniformName, Vector4f& ret ) const {
    ASSERT( mV4fs.find( uniformName ) != mV4fs.end());
    ret = mV4fs.at( uniformName );
}

void HeterogeneousMap::get( const std::string& uniformName, Matrix4f& ret ) const {
    ASSERT( mM4fs.find( uniformName ) != mM4fs.end());
    ret = mM4fs.at( uniformName );
}

void HeterogeneousMap::get( const std::string& uniformName, Matrix3f& ret ) const {
    ASSERT( mM3fs.find( uniformName ) != mM3fs.end());
    ret = mM3fs.at( uniformName );
}

std::shared_ptr<HeterogeneousMap> HeterogeneousMap::clone() {
    std::shared_ptr<HeterogeneousMap> ret = std::make_shared<HeterogeneousMap>();

    ret->clone( *this );

    return ret;
}

void HeterogeneousMap::clone( const HeterogeneousMap& _source ) {

    NamePolicy::Name( _source.Name() );
    mTextures    = _source.mTextures;
    mFloats      = _source.mFloats;
    mInts        = _source.mInts;
    mV2fs        = _source.mV2fs;
    mV3fs        = _source.mV3fs;
    mV4fs        = _source.mV4fs;
    mM3fs        = _source.mM3fs;
    mM4fs        = _source.mM4fs;
    mV3fvs       = _source.mV3fvs;

    calcTotalNumUniforms();
}

std::vector<std::string> HeterogeneousMap::getTextureNames() const {
    std::vector<std::string> ret;

    for ( const auto& [k, v] : mTextures ) {
        ret.emplace_back( v.name );
    }
    return ret;
}

std::unordered_map<std::string, std::string> HeterogeneousMap::getTextureNameMap() const {
    std::unordered_map<std::string, std::string> ret;

    for ( const auto& [k, v] : mTextures ) {
        ret.emplace( k, v.name );
    }
    return ret;
}

void HeterogeneousMap::serialize( std::shared_ptr<SerializeBin> writer ) const {
    writer->write( Name() );
    writer->write( mNumUniforms );
    writer->write( mHash );
    writer->write( getTextureNameMap() ); //mTextures
    writer->write( mInts );
    writer->write( mFloats );
    writer->write( mV2fs );
    writer->write( mV3fs );
    writer->write( mV3fvs );
    writer->write( mV4fs );
    writer->write( mM3fs );
    writer->write( mM4fs );
}

void HeterogeneousMap::deserialize( std::shared_ptr<DeserializeBin> reader ) {
    reader->read( NameRef() );
    reader->read( mNumUniforms );
    reader->read( mHash );
    std::unordered_map<std::string, std::string> tns;
    reader->read( tns );
    for ( const auto& [k,v] : tns ) mTextures.emplace( k, TextureIndex{v, 0, 0, 0} );
    reader->read( mInts );
    reader->read( mFloats );
    reader->read( mV2fs );
    reader->read( mV3fs );
    reader->read( mV3fvs );
    reader->read( mV4fs );
    reader->read( mM3fs );
    reader->read( mM4fs );
}
