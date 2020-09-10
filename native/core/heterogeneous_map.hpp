//
// Created by Dado on 2019-01-28.
//

#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <optional>
#include <core/htypes_shared.hpp>
#include <core/math/vector4f.h>
#include <core/math/matrix4f.h>
#include <core/hashable.hpp>
#include <core/serialization.hpp>

class HeterogeneousMap : public virtual Hashable<> {
public:
    HeterogeneousMap() = default;
    virtual ~HeterogeneousMap() = default;
    HeterogeneousMap( const HeterogeneousMap& _source );
    explicit HeterogeneousMap( std::shared_ptr<HeterogeneousMap> _source );
    explicit HeterogeneousMap( std::string _type ) : mType( std::move( _type )) {}
    const std::string& Type() const;
    void Type( const std::string& _type );

    JSONSERIAL( HeterogeneousMap, mType, mStrings, mInts, mFloats, mV2fs, mV3fs, mV3fvs, mV4fs, mM3fs, mM4fs );

    void inject( const HeterogeneousMap& source );
    void injectIfNotPresent( const HeterogeneousMap& source );

    template<typename T>
    void assign( const std::string& uniformName, T _data ) {
        if constexpr (std::is_same<T, std::string>::value )           mStrings[uniformName] = std::move(_data);
        if constexpr (std::is_same<T, float>::value )                 mFloats [uniformName] = std::move(_data);
        if constexpr (std::is_same<T, double>::value )                mFloats [uniformName] = std::move(_data);
        if constexpr (std::is_same<T, int>::value )                   mInts   [uniformName] = std::move(_data);
        if constexpr (std::is_same<T, V2f>::value )                   mV2fs   [uniformName] = std::move(_data);
        if constexpr (std::is_same<T, V3f>::value )                   mV3fs   [uniformName] = std::move(_data);
        if constexpr (std::is_same<T, V4f>::value )                   mV4fs   [uniformName] = std::move(_data);
        if constexpr (std::is_same<T, Matrix3f>::value )              mM3fs   [uniformName] = std::move(_data);
        if constexpr (std::is_same<T, Matrix4f>::value )              mM4fs   [uniformName] = std::move(_data);
        if constexpr (std::is_same<T, std::vector<Vector3f>>::value ) mV3fvs  [uniformName] = std::move(_data);
    }

    template<typename T>
    void assignIfNotPresent( const std::string& uniformName, T _data ) {
        if ( !get<T>(uniformName) ) {
            assign( uniformName, _data );
        }
    }

    template<typename T>
    void remove( const std::string& uniformName ) {
        if constexpr (std::is_same<T, std::string>::value )           mStrings.erase(uniformName);
        if constexpr (std::is_same<T, float>::value )                 mFloats .erase(uniformName);
        if constexpr (std::is_same<T, double>::value )                mFloats .erase(uniformName);
        if constexpr (std::is_same<T, int>::value )                   mInts   .erase(uniformName);
        if constexpr (std::is_same<T, V2f>::value )                   mV2fs   .erase(uniformName);
        if constexpr (std::is_same<T, V3f>::value )                   mV3fs   .erase(uniformName);
        if constexpr (std::is_same<T, V4f>::value )                   mV4fs   .erase(uniformName);
        if constexpr (std::is_same<T, Matrix3f>::value )              mM3fs   .erase(uniformName);
        if constexpr (std::is_same<T, Matrix4f>::value )              mM4fs   .erase(uniformName);
        if constexpr (std::is_same<T, std::vector<Vector3f>>::value ) mV3fvs  .erase(uniformName);
    }

    template<typename T>
    std::optional<T> get( const std::string& uniformName ) const {
        if constexpr ( std::is_same<T, std::string>::value )           if ( auto it = mStrings.find( uniformName ); it != mStrings.end() ) return it->second;
        if constexpr ( std::is_same<T, float>::value )                 if ( auto it = mFloats .find( uniformName ); it != mFloats .end() ) return it->second;
        if constexpr ( std::is_same<T, double>::value )                if ( auto it = mFloats .find( uniformName ); it != mFloats .end() ) return it->second;
        if constexpr ( std::is_same<T, int>::value )                   if ( auto it = mInts   .find( uniformName ); it != mInts   .end() ) return it->second;
        if constexpr ( std::is_same<T, V2f>::value )                   if ( auto it = mV2fs   .find( uniformName ); it != mV2fs   .end() ) return it->second;
        if constexpr ( std::is_same<T, V3f>::value )                   if ( auto it = mV3fs   .find( uniformName ); it != mV3fs   .end() ) return it->second;
        if constexpr ( std::is_same<T, V4f>::value )                   if ( auto it = mV4fs   .find( uniformName ); it != mV4fs   .end() ) return it->second;
        if constexpr ( std::is_same<T, Matrix3f>::value )              if ( auto it = mM3fs   .find( uniformName ); it != mM3fs   .end() ) return it->second;
        if constexpr ( std::is_same<T, Matrix4f>::value )              if ( auto it = mM4fs   .find( uniformName ); it != mM4fs   .end() ) return it->second;
        if constexpr ( std::is_same<T, std::vector<Vector3f>>::value ) if ( auto it = mV3fvs  .find( uniformName ); it != mV3fvs  .end() ) return it->second;
        return std::nullopt;
    }

    template<typename T>
    T getDef( const std::string& uniformName ) const {
        if constexpr ( std::is_same<T, std::string>::value )           { auto v = get<T>( uniformName ); return (v ? *v : T{}); }
        if constexpr ( std::is_same<T, float>::value )                 { auto v = get<T>( uniformName ); return (v ? *v : 0.0f); }
        if constexpr ( std::is_same<T, double>::value )                { auto v = get<T>( uniformName ); return (v ? *v : 0.0f); }
        if constexpr ( std::is_same<T, int>::value )                   { auto v = get<T>( uniformName ); return (v ? *v : 0); }
        if constexpr ( std::is_same<T, V2f>::value )                   { auto v = get<T>( uniformName ); return (v ? *v : V2fc::ZERO); }
        if constexpr ( std::is_same<T, V3f>::value )                   { auto v = get<T>( uniformName ); return (v ? *v : V3fc::ZERO); }
        if constexpr ( std::is_same<T, V4f>::value )                   { auto v = get<T>( uniformName ); return (v ? *v : V4fc::ZERO); }
        if constexpr ( std::is_same<T, Matrix3f>::value )              { auto v = get<T>( uniformName ); return (v ? *v : Matrix3f::ZERO); }
        if constexpr ( std::is_same<T, Matrix4f>::value )              { auto v = get<T>( uniformName ); return (v ? *v : Matrix4f::ZERO); }
        if constexpr ( std::is_same<T, std::vector<Vector3f>>::value ) { auto v = get<T>( uniformName ); return (v ? *v : T{}); }
    }

    template<typename T>
    T getDef( const std::string& uniformName, T _def ) const {
        if constexpr ( std::is_same<T, std::string>::value )           { auto v = get<T>( uniformName ); return (v ? *v : _def); }
        if constexpr ( std::is_same<T, float>::value )                 { auto v = get<T>( uniformName ); return (v ? *v : _def); }
        if constexpr ( std::is_same<T, double>::value )                { auto v = get<T>( uniformName ); return (v ? *v : _def); }
        if constexpr ( std::is_same<T, int>::value )                   { auto v = get<T>( uniformName ); return (v ? *v : _def); }
        if constexpr ( std::is_same<T, V2f>::value )                   { auto v = get<T>( uniformName ); return (v ? *v : _def); }
        if constexpr ( std::is_same<T, V3f>::value )                   { auto v = get<T>( uniformName ); return (v ? *v : _def); }
        if constexpr ( std::is_same<T, V4f>::value )                   { auto v = get<T>( uniformName ); return (v ? *v : _def); }
        if constexpr ( std::is_same<T, Matrix3f>::value )              { auto v = get<T>( uniformName ); return (v ? *v : _def); }
        if constexpr ( std::is_same<T, Matrix4f>::value )              { auto v = get<T>( uniformName ); return (v ? *v : _def); }
        if constexpr ( std::is_same<T, std::vector<Vector3f>>::value ) { auto v = get<T>( uniformName ); return (v ? *v : _def); }
    }

    template<typename T>
    std::vector<T> getKeys() const{
        std::vector<T> ret;

        if constexpr ( std::is_same<T, std::string>::value )           { for ( const auto& [k, v] : mStrings ) ret.emplace_back( v ); }
        if constexpr ( std::is_same<T, float>::value )                 { for ( const auto& [k, v] : mFloats  ) ret.emplace_back( v ); }
        if constexpr ( std::is_same<T, double>::value )                { for ( const auto& [k, v] : mFloats  ) ret.emplace_back( v ); }
        if constexpr ( std::is_same<T, int>::value )                   { for ( const auto& [k, v] : mInts    ) ret.emplace_back( v ); }
        if constexpr ( std::is_same<T, V2f>::value )                   { for ( const auto& [k, v] : mV2fs    ) ret.emplace_back( v ); }
        if constexpr ( std::is_same<T, V3f>::value )                   { for ( const auto& [k, v] : mV3fs    ) ret.emplace_back( v ); }
        if constexpr ( std::is_same<T, V4f>::value )                   { for ( const auto& [k, v] : mV4fs    ) ret.emplace_back( v ); }
        if constexpr ( std::is_same<T, Matrix3f>::value )              { for ( const auto& [k, v] : mM3fs    ) ret.emplace_back( v ); }
        if constexpr ( std::is_same<T, Matrix4f>::value )              { for ( const auto& [k, v] : mM4fs    ) ret.emplace_back( v ); }
        if constexpr ( std::is_same<T, std::vector<Vector3f>>::value ) { for ( const auto& [k, v] : mV3fvs   ) ret.emplace_back( v ); }

        return ret;
    }

    template<typename T>
    std::unordered_map<std::string, T> getMap() const {
        if constexpr ( std::is_same<T, std::string>::value )           return mStrings;
        if constexpr ( std::is_same<T, float>::value )                 return mFloats ;
        if constexpr ( std::is_same<T, double>::value )                return mFloats ;
        if constexpr ( std::is_same<T, int>::value )                   return mInts   ;
        if constexpr ( std::is_same<T, V2f>::value )                   return mV2fs   ;
        if constexpr ( std::is_same<T, V3f>::value )                   return mV3fs   ;
        if constexpr ( std::is_same<T, V4f>::value )                   return mV4fs   ;
        if constexpr ( std::is_same<T, Matrix3f>::value )              return mM3fs   ;
        if constexpr ( std::is_same<T, Matrix4f>::value )              return mM4fs   ;
        if constexpr ( std::is_same<T, std::vector<Vector3f>>::value ) return mV3fvs  ;
    };

    std::shared_ptr<HeterogeneousMap> clone();
    void clone( const HeterogeneousMap& _map );

    template <typename T>
    void visit( const T& _visitor ) {
        for ( auto& u : mStrings )  { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mFloats )   { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mInts )     { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mV2fs )     { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mV3fs )     { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mV4fs )     { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mM3fs )     { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mM4fs )     { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mV3fvs )    { _visitor.visit( u.first.c_str(), u.second ); }
    }

private:
    std::string mType;
    std::unordered_map<std::string, std::string>           mStrings;
    std::unordered_map<std::string, int>                   mInts;
    std::unordered_map<std::string, float>                 mFloats;
    std::unordered_map<std::string, Vector2f>              mV2fs;
    std::unordered_map<std::string, Vector3f>              mV3fs;
    std::unordered_map<std::string, Vector4f>              mV4fs;
    std::unordered_map<std::string, Matrix3f>              mM3fs;
    std::unordered_map<std::string, Matrix4f>              mM4fs;
    std::unordered_map<std::string, std::vector<Vector3f>> mV3fvs;
};
