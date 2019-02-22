//
// Created by Dado on 2019-01-28.
//

#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <core/htypes_shared.hpp>
#include <core/name_policy.hpp>
#include <core/math/vector4f.h>
#include <core/math/matrix4f.h>
#include <core/serializable.hpp>
#include <core/hashable.hpp>

using TextureIndex = TextureUniformDesc;
class SerializeBin;
class DeserializeBin;

class HeterogeneousMap : public virtual Serializable<HeterogeneousMap>,
                         public virtual Hashable,
                         public std::enable_shared_from_this<HeterogeneousMap> {
public:
    virtual ~HeterogeneousMap() = default;
    void inject( const HeterogeneousMap& source );
    void assign( const std::string& uniformName, int data );
    void assign( const std::string& uniformName, float data );
    void assign( const std::string& uniformName, double data );
    void assign( const std::string& uniformName, const std::string& data );
    void assign( const std::string& uniformName, const TextureUniformDesc& data );
    void assign( const std::string& uniformName, const Vector2f& data );
    void assign( const std::string& uniformName, const Vector3f& data );
    void assign( const std::string& uniformName, const Vector4f& data );
    void assign( const std::string& uniformName, const Matrix4f& data );
    void assign( const std::string& uniformName, const Matrix3f& data );
    void assign( const std::string& uniformName, const std::vector<Vector3f>& data );

    int NumUniforms() const { return mNumUniforms; }

    std::vector<std::string> getTextureNames() const;
    std::unordered_map<std::string, std::string> getTextureNameMap() const;
    std::string getTexture( const std::string& uniformName ) const;
    float getInt( const std::string & uniformName ) const;
    float getFloatWithDefault( const std::string& uniformName, const float def ) const;
    float getFloat( const std::string & uniformName ) const;
    Vector2f getVector2f( const std::string & uniformName ) const;
    Vector3f getVector3f( const std::string & uniformName ) const;
    Vector4f getVector4f( const std::string & uniformName ) const;
    Matrix4f getMatrix4f( const std::string & uniformName ) const;
    Matrix3f getMatrix3f( const std::string & uniformName ) const;
    void get( const std::string& uniformName, std::string& ret ) const;
    void get( const std::string& uniformName, int& ret ) const;
    bool get( const std::string& uniformName, float& ret ) const;
    void get( const std::string& uniformName, Vector2f& ret ) const;
    void get( const std::string& uniformName, Vector3f& ret ) const;
    void get( const std::string& uniformName, Vector4f& ret ) const;
    void get( const std::string& uniformName, Matrix3f& ret ) const;
    void get( const std::string& uniformName, Matrix4f& ret ) const;

    bool hasTexture( const std::string& uniformName ) const;
    bool hasInt( const std::string& uniformName ) const;
    bool hasFloat( const std::string& uniformName ) const;
    bool hasVector2f( const std::string& uniformName ) const;
    bool hasVector3f( const std::string& uniformName ) const;
    bool hasVector4f( const std::string& uniformName ) const;
    bool hasMatrix4f( const std::string& uniformName ) const;
    bool hasMatrix3f( const std::string& uniformName ) const;

    std::shared_ptr<HeterogeneousMap> clone();
    void clone( const HeterogeneousMap& _map );

    template <typename T>
    void visit( const T& _visitor ) {
        if ( mNumUniforms == 0 ) return;

        for ( auto& u : mTextures ) { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mFloats )   { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mInts )     { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mV2fs )     { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mV3fs )     { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mV4fs )     { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mM3fs )     { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mM4fs )     { _visitor.visit( u.first.c_str(), u.second ); }
        for ( auto& u : mV3fvs )    { _visitor.visit( u.first.c_str(), u.second ); }
    }

    template <typename T>
    void visitTextures( T _func ) {
        unsigned int i = 0;
        for ( auto& [k,u] : mTextures ) {
            _func( u, i );
            ++i;
        }
    }

    template <typename T>
    void visitTexturesWithKey( T _func ) {
        for ( auto& [k,u] : mTextures ) {
            _func( u, k );
        }
    }

    void serialize( std::shared_ptr<SerializeBin> writer ) const override;
    void deserialize( std::shared_ptr<DeserializeBin> reader ) override;

protected:
    std::string calcHash() override;

private:
    void calcTotalNumUniforms();

private:
    std::unordered_map<std::string, TextureIndex> mTextures;
    std::unordered_map<std::string, int> mInts;
    std::unordered_map<std::string, float> mFloats;
    std::unordered_map<std::string, Vector2f> mV2fs;
    std::unordered_map<std::string, Vector3f> mV3fs;
    std::unordered_map<std::string, std::vector<Vector3f>> mV3fvs;
    std::unordered_map<std::string, Vector4f> mV4fs;
    std::unordered_map<std::string, Matrix3f> mM3fs;
    std::unordered_map<std::string, Matrix4f> mM4fs;
    int mNumUniforms = 0;
};
