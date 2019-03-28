//
// Created by Dado on 2019-01-28.
//

#include "heterogeneous_map.hpp"

void HeterogeneousMap::calcTotalNumUniforms() {
    size_t lNumUniforms = 0;
    lNumUniforms += mStrings.size();
    lNumUniforms += mInts.size();
    lNumUniforms += mFloats.size();
    lNumUniforms += mV2fs.size();
    lNumUniforms += mV3fs.size();
    lNumUniforms += mV3fvs.size();
    lNumUniforms += mV4fs.size();
    lNumUniforms += mM3fs.size();
    lNumUniforms += mM4fs.size();

    mNumUniforms = static_cast<int>( lNumUniforms );

//    HeterogeneousMap::calcHash();
}

//std::string HeterogeneousMap::calcHashImpl() {
//    std::stringstream hashInput;
//    hashInput << mNumUniforms;
//
//    for ( auto& i : mStrings ) hashInput << i.first;
//    for ( auto& i : mInts ) hashInput << i.second;
//    for ( auto& i : mFloats ) hashInput << i.second;
//    for ( auto& i : mV2fs ) hashInput << i.second.hash();
//    for ( auto& i : mV3fs ) hashInput << i.second.hash();
//    for ( auto& i : mV4fs ) hashInput << i.second.hash();
//
//    return hashInput.str();
//}

void HeterogeneousMap::inject( const HeterogeneousMap& source ) {
    for ( auto& pu : source.mStrings ) assign( pu.first, pu.second );
    for ( auto& pu : source.mInts )    assign( pu.first, pu.second );
    for ( auto& pu : source.mFloats )  assign( pu.first, pu.second );
    for ( auto& pu : source.mV2fs )    assign( pu.first, pu.second );
    for ( auto& pu : source.mV3fs )    assign( pu.first, pu.second );
    for ( auto& pu : source.mV3fvs )   assign( pu.first, pu.second );
    for ( auto& pu : source.mV4fs )    assign( pu.first, pu.second );
    for ( auto& pu : source.mM3fs )    assign( pu.first, pu.second );
    for ( auto& pu : source.mM4fs )    assign( pu.first, pu.second );

    calcTotalNumUniforms();
}

void HeterogeneousMap::injectIfNotPresent( const HeterogeneousMap& source ) {
    for ( auto& pu : source.mStrings  ) assignIfNotPresent( pu.first, pu.second );
    for ( auto& pu : source.mInts     ) assignIfNotPresent( pu.first, pu.second );
    for ( auto& pu : source.mFloats   ) assignIfNotPresent( pu.first, pu.second );
    for ( auto& pu : source.mV2fs     ) assignIfNotPresent( pu.first, pu.second );
    for ( auto& pu : source.mV3fs     ) assignIfNotPresent( pu.first, pu.second );
    for ( auto& pu : source.mV3fvs    ) assignIfNotPresent( pu.first, pu.second );
    for ( auto& pu : source.mV4fs     ) assignIfNotPresent( pu.first, pu.second );
    for ( auto& pu : source.mM3fs     ) assignIfNotPresent( pu.first, pu.second );
    for ( auto& pu : source.mM4fs     ) assignIfNotPresent( pu.first, pu.second );

    calcTotalNumUniforms();
}

void HeterogeneousMap::assign( const std::string& uniformName, int data ) {
    mInts[uniformName] = data;
    calcTotalNumUniforms();
}

void HeterogeneousMap::assign( const std::string& uniformName, const std::string& _data ) {
    mStrings[uniformName] = _data;
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

void HeterogeneousMap::assignIfNotPresent( const std::string& uniformName, int data ) {
    if ( !hasInt(uniformName) ) {
        assign( uniformName, data );
    }
}

void HeterogeneousMap::assignIfNotPresent( const std::string& uniformName, const std::string& data ) {
    if ( !hasTexture(uniformName) ) {
        assign( uniformName, data );
    }
}

void HeterogeneousMap::assignIfNotPresent( const std::string& uniformName, float data ) {
    if ( !hasFloat(uniformName) ) {
        assign( uniformName, data );
    }
}

void HeterogeneousMap::assignIfNotPresent( const std::string& uniformName, double data ) {
    if ( !hasFloat(uniformName) ) {
        assign( uniformName, data );
    }
}

void HeterogeneousMap::assignIfNotPresent( const std::string& uniformName, const Vector2f& data ) {
    if ( !hasVector2f(uniformName) ) {
        assign( uniformName, data );
    }
}

void HeterogeneousMap::assignIfNotPresent( const std::string& uniformName, const Vector3f& data ) {
    if ( !hasVector3f(uniformName) ) {
        assign( uniformName, data );
    }
}

void HeterogeneousMap::assignIfNotPresent( const std::string& uniformName, const std::vector<Vector3f>& data ) {
    if ( !hasVectorOfVector3f(uniformName) ) {
        assign( uniformName, data );
    }
}

void HeterogeneousMap::assignIfNotPresent( const std::string& uniformName, const Vector4f& data ) {
    if ( !hasVector4f(uniformName) ) {
        assign( uniformName, data );
    }
}

void HeterogeneousMap::assignIfNotPresent( const std::string& uniformName, const Matrix4f& data ) {
    if ( !hasMatrix4f(uniformName) ) {
        assign( uniformName, data );
    }
}

void HeterogeneousMap::assignIfNotPresent( const std::string& uniformName, const Matrix3f& data ) {
    if ( !hasMatrix3f(uniformName) ) {
        assign( uniformName, data );
    }
}

std::string HeterogeneousMap::getTexture( const std::string& uniformName ) const {
    ASSERT( mStrings.find( uniformName ) != mStrings.end());
    return mStrings.at( uniformName );
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
    return ( mStrings.find( uniformName ) != mStrings.end());
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

bool HeterogeneousMap::hasVectorOfVector3f( const std::string& uniformName ) const {
    return ( mV3fvs.find( uniformName ) != mV3fvs.end());
}

void HeterogeneousMap::get( const std::string& uniformName, std::string& ret ) const {
    ASSERT( mStrings.find( uniformName ) != mStrings.end());
    ret = mStrings.at( uniformName );
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

    mStrings    = _source.mStrings;
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

    for ( const auto& [k, v] : mStrings ) {
        ret.emplace_back( v );
    }
    return ret;
}

std::unordered_map<std::string, std::string> HeterogeneousMap::getTextureNameMap() const {
    std::unordered_map<std::string, std::string> ret;

    for ( const auto& [k, v] : mStrings ) {
        ret.emplace( k, v );
    }
    return ret;
}

HeterogeneousMap::HeterogeneousMap( const HeterogeneousMap& _source ) {
    inject( _source );
}

HeterogeneousMap::HeterogeneousMap( std::shared_ptr<HeterogeneousMap> _source ) {
    inject( *_source.get() );
}
