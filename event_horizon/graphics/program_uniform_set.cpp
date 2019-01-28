#include "program_uniform_set.h"
#include "texture_manager.h"
#include "renderer.h"
#include "core/serializebin.hpp"

std::map<std::string, int> ProgramUniformSet::mUBONames;
std::map<std::string, int> ProgramUniformSet::mUBOOffsetMap;
std::map<std::string, unsigned int> ProgramUniformSet::mUBOHandles;

int ProgramUniformSet::getUBOPoint( const std::string& ubo_name ) {
    auto n = mUBONames.find( ubo_name );
    if ( n == mUBONames.end()) {
        auto s = static_cast<int>( mUBONames.size());
        mUBONames[ubo_name] = s;
        return s;
    }
    return n->second;
}

void ProgramUniformSet::calcTotalNumUniforms() {
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

void ProgramUniformSet::calcHash() {
    mHash = mNumUniforms;

    for ( auto& i : mTextures ) mHash += std::hash<std::string>{}(i.first);
    for ( auto& i : mInts ) mHash += i.second;
    for ( auto& i : mFloats ) mHash += static_cast<int64_t>( i.second * 1000.0f );
    for ( auto& i : mV2fs ) mHash += i.second.hash();
    for ( auto& i : mV3fs ) mHash += i.second.hash();
    for ( auto& i : mV4fs ) mHash += i.second.hash();
}

void ProgramUniformSet::setUBOStructure( const std::string& uniformName, int off ) {
    mUBOOffsetMap[uniformName] = mUBOSize;
    mUBOSize+=off;
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, int value ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &value, 4 );
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, float value ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &value, 4 );
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, float x, float y ) {
    Vector2f v{ x, y };
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &v, sizeof( Vector2f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, float x, float y, float z ) {
    Vector3f v{ x, y, z };
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &v, sizeof( Vector3f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, float x, float y, float z, float w ) {
    Vector4f v{ x, y, z, w };
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &v, sizeof( Vector4f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, const Vector2f& v ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &v, sizeof( Vector2f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, const Vector3f& v ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &v, sizeof( Vector3f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, const Vector4f& v ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &v, sizeof( Vector4f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, const Matrix2f& matrix ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &matrix, sizeof( Matrix2f ));
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, const Matrix3f& matrix ) {
    int accOff = 0;
    for ( int t = 0; t < 3; t++ ) {
        std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName] + accOff, &matrix.getRow( t ), sizeof( Vector3f ));
        accOff += 16;
    }
}

void ProgramUniformSet::setUBOData( const std::string& uniformName, const Matrix4f& matrix ) {
    std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName], &matrix, sizeof( Matrix4f ));
}

void ProgramUniformSet::assign( const ProgramUniformSet *source ) {
    for ( auto& pu : source->mTextures ) assign( pu.first, pu.second );
    for ( auto& pu : source->mInts ) assign( pu.first, pu.second );
    for ( auto& pu : source->mFloats ) assign( pu.first, pu.second );
    for ( auto& pu : source->mV2fs ) assign( pu.first, pu.second );
    for ( auto& pu : source->mV3fs ) assign( pu.first, pu.second );
    for ( auto& pu : source->mV3fvs ) assign( pu.first, pu.second );
    for ( auto& pu : source->mV4fs ) assign( pu.first, pu.second );
    for ( auto& pu : source->mM3fs ) assign( pu.first, pu.second );
    for ( auto& pu : source->mM4fs ) assign( pu.first, pu.second );
}

void ProgramUniformSet::assign( const std::string& uniformName, int data ) {
    mInts[uniformName] = data;
    calcTotalNumUniforms();
}

void ProgramUniformSet::assign( const std::string& uniformName, const TextureIndex& data ) {
    mTextures[uniformName] = data;
    calcTotalNumUniforms();
}

void ProgramUniformSet::assign( const std::string& uniformName, float data ) {
    mFloats[uniformName] = data;
    calcTotalNumUniforms();
}

void ProgramUniformSet::assign( const std::string& uniformName, double data ) {
    mFloats[uniformName] = static_cast<float>( data );
    calcTotalNumUniforms();
}

void ProgramUniformSet::assign( const std::string& uniformName, const Vector2f& data ) {
    mV2fs[uniformName] = data;
    calcTotalNumUniforms();
}

void ProgramUniformSet::assign( const std::string& uniformName, const Vector3f& data ) {
    mV3fs[uniformName] = data;
    calcTotalNumUniforms();
}

void ProgramUniformSet::assign( const std::string& uniformName, const std::vector<Vector3f>& data ) {
    mV3fvs[uniformName] = data;
    calcTotalNumUniforms();
}

void ProgramUniformSet::assign( const std::string& uniformName, const Vector4f& data ) {
    mV4fs[uniformName] = data;
    calcTotalNumUniforms();
}

void ProgramUniformSet::assign( const std::string& uniformName, const Matrix4f& data ) {
    mM4fs[uniformName] = data;
    calcTotalNumUniforms();
}

void ProgramUniformSet::assign( const std::string& uniformName, const Matrix3f& data ) {
    mM3fs[uniformName] = data;
    calcTotalNumUniforms();
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, int data ) {
    mInts[uniformName] = data;
    mNumUniforms = 1;
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, TextureIndex data ) {
    mTextures[uniformName] = data;
    mNumUniforms = 1;
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, float data ) {
    mFloats[uniformName] = data;
    mNumUniforms = 1;
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, double data ) {
    mFloats[uniformName] = static_cast<float>( data );
    mNumUniforms = 1;
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, const Vector2f& data ) {
    mV2fs[uniformName] = data;
    mNumUniforms = 1;
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, const Vector3f& data ) {
    mV3fs[uniformName] = data;
    mNumUniforms = 1;
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, const std::vector<Vector3f>& data ) {
    mV3fvs[uniformName] = data;
    mNumUniforms = 1;
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, const Vector4f& data ) {
    mV4fs[uniformName] = data;
    mNumUniforms = 1;
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, const Matrix4f& data ) {
    mM4fs[uniformName] = data;
    mNumUniforms = 1;
}

void ProgramUniformSet::assignGlobal( const std::string& uniformName, const Matrix3f& data ) {
    mM3fs[uniformName] = data;
    mNumUniforms = 1;
}

void ProgramUniformSet::clearTextures() {
    mTextures.clear();
    calcTotalNumUniforms();
}

TextureIndex ProgramUniformSet::getTexture( const std::string& uniformName ) const {
    ASSERT( mTextures.find( uniformName ) != mTextures.end());
    return mTextures.at( uniformName );
}

float ProgramUniformSet::getInt( const std::string& uniformName ) const {
    ASSERT( mInts.find( uniformName ) != mInts.end());
    return static_cast<float>( mInts.at( uniformName ));
}

float ProgramUniformSet::getFloatWithDefault( const std::string& uniformName, const float def ) const {
    if ( auto it = mFloats.find( uniformName ); it != mFloats.end() ) {
        return it->second;
    }
    return def;
}

float ProgramUniformSet::getFloat( const std::string& uniformName ) const {
    ASSERT( mFloats.find( uniformName ) != mFloats.end());
    return mFloats.at( uniformName );
}

Vector2f ProgramUniformSet::getVector2f( const std::string& uniformName ) const {
    ASSERT( mV2fs.find( uniformName ) != mV2fs.end());
    return mV2fs.at( uniformName );
}

Vector3f ProgramUniformSet::getVector3f( const std::string& uniformName ) const {
    ASSERT( mV3fs.find( uniformName ) != mV3fs.end());
    return mV3fs.at( uniformName );
}

Vector4f ProgramUniformSet::getVector4f( const std::string& uniformName ) const {
    ASSERT( mV4fs.find( uniformName ) != mV4fs.end());
    return mV4fs.at( uniformName );
}

Matrix4f ProgramUniformSet::getMatrix4f( const std::string& uniformName ) const {
    ASSERT( mM4fs.find( uniformName ) != mM4fs.end());
    return mM4fs.at( uniformName );
}

Matrix3f ProgramUniformSet::getMatrix3f( const std::string& uniformName ) const {
    ASSERT( mM3fs.find( uniformName ) != mM3fs.end());
    return mM3fs.at( uniformName );
}

bool ProgramUniformSet::hasTexture( const std::string& uniformName ) const {
    return ( mTextures.find( uniformName ) != mTextures.end());
}

bool ProgramUniformSet::hasInt( const std::string& uniformName ) const {
    return ( mInts.find( uniformName ) != mInts.end());
}

bool ProgramUniformSet::hasFloat( const std::string& uniformName ) const {
    return ( mFloats.find( uniformName ) != mFloats.end());
}

bool ProgramUniformSet::hasVector2f( const std::string& uniformName ) const {
    return ( mV2fs.find( uniformName ) != mV2fs.end());
}

bool ProgramUniformSet::hasVector3f( const std::string& uniformName ) const {
    return ( mV3fs.find( uniformName ) != mV3fs.end());
}

bool ProgramUniformSet::hasVector4f( const std::string& uniformName ) const {
    return ( mV4fs.find( uniformName ) != mV4fs.end());
}

bool ProgramUniformSet::hasMatrix4f( const std::string& uniformName ) const {
    return ( mM4fs.find( uniformName ) != mM4fs.end());
}

bool ProgramUniformSet::hasMatrix3f( const std::string& uniformName ) const {
    return ( mM3fs.find( uniformName ) != mM3fs.end());
}

void ProgramUniformSet::get( const std::string& uniformName, TextureIndex ret ) const {
    ASSERT( mTextures.find( uniformName ) != mTextures.end());
    ret = mTextures.at( uniformName );
}

void ProgramUniformSet::get( const std::string& uniformName, int& ret ) const {
    ASSERT( mInts.find( uniformName ) != mInts.end());
    ret = mInts.at( uniformName );
}

void ProgramUniformSet::get( const std::string& uniformName, float& ret ) const {
    ASSERT( mFloats.find( uniformName ) != mFloats.end());
    ret = mFloats.at( uniformName );
}

void ProgramUniformSet::get( const std::string& uniformName, Vector2f& ret ) const {
    ASSERT( mV2fs.find( uniformName ) != mV2fs.end());
    ret = mV2fs.at( uniformName );
}

void ProgramUniformSet::get( const std::string& uniformName, Vector3f& ret ) const {
    ASSERT( mV3fs.find( uniformName ) != mV3fs.end());
    ret = mV3fs.at( uniformName );
}

void ProgramUniformSet::get( const std::string& uniformName, Vector4f& ret ) const {
    ASSERT( mV4fs.find( uniformName ) != mV4fs.end());
    ret = mV4fs.at( uniformName );
}

void ProgramUniformSet::get( const std::string& uniformName, Matrix4f& ret ) const {
    ASSERT( mM4fs.find( uniformName ) != mM4fs.end());
    ret = mM4fs.at( uniformName );
}

void ProgramUniformSet::get( const std::string& uniformName, Matrix3f& ret ) const {
    ASSERT( mM3fs.find( uniformName ) != mM3fs.end());
    ret = mM3fs.at( uniformName );
}

void ProgramUniformSet::setOn( unsigned int handle ) {
    if ( mNumUniforms == 0 ) return;

    // Textures
    for ( auto& u : mTextures ) { setUniform( u.first.c_str(), u.second, handle ); }
    for ( auto& u : mFloats )   { setUniform( u.first.c_str(), u.second, handle ); }
    for ( auto& u : mInts )     { setUniform( u.first.c_str(), u.second, handle ); }
    for ( auto& u : mV2fs )     { setUniform( u.first.c_str(), u.second, handle ); }
    for ( auto& u : mV3fs )     { setUniform( u.first.c_str(), u.second, handle ); }
    for ( auto& u : mV4fs )     { setUniform( u.first.c_str(), u.second, handle ); }
    for ( auto& u : mM3fs )     { setUniform( u.first.c_str(), u.second, handle ); }
    for ( auto& u : mM4fs )     { setUniform( u.first.c_str(), u.second, handle ); }
    for ( auto& u : mV3fvs )    { setUniform( u.first.c_str(), u.second, handle ); }
}

std::shared_ptr<ProgramUniformSet> ProgramUniformSet::clone() {
    std::shared_ptr<ProgramUniformSet> ret = std::make_shared<ProgramUniformSet>();

    ret->mTextures = mTextures;
    ret->mFloats = mFloats;
    ret->mInts = mInts;
    ret->mV2fs = mV2fs;
    ret->mV3fs = mV3fs;
    ret->mV4fs = mV4fs;
    ret->mM3fs = mM3fs;
    ret->mM4fs = mM4fs;
    ret->mV3fvs = mV3fvs;
    ret->mNumUniforms = mNumUniforms;

    return ret;
}
