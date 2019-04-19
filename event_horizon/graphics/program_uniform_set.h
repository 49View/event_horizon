#pragma once

#include <string>
#include <cstring>
#include <map>
#include <core/math/vector4f.h>
#include <core/math/matrix4f.h>
#include <core/math/matrix2f.h>
#include <core/heterogeneous_map.hpp>
#include "program.h"

class Texture;
class Program;
class Renderer;
class ShaderManager;

class GPUUniformVisitor {
public:
	explicit GPUUniformVisitor( unsigned int handle ) : handle( handle ) {}

	void visit( const char* name, const TextureUniformDesc& value ) const;
    void visit( const char* name, const std::string& _value ) const {};
	void visit( const char* name, int value ) const;
	void visit( const char* name, float value ) const;
	void visit( const char* name, float x, float y ) const;
	void visit( const char* name, float x, float y, float z ) const;
	void visit( const char* name, float x, float y, float z, float w ) const;
	void visit( const char* name, const Vector2f& v ) const;
	void visit( const char* name, const Vector3f& v ) const;
	void visit( const char* name, const std::vector<Vector3f>& v ) const;
	void visit( const char* name, const Vector4f& v ) const;
	void visit( const char* name, const Matrix2f& matrix ) const;
	void visit( const char* name, const Matrix3f& matrix ) const;
	void visit( const char* name, const Matrix4f& matrix ) const;

protected:
	int hasUniform( const char* name ) const;
	unsigned int handle;
};

class GPUHeterogeneousMap {
public:
    explicit GPUHeterogeneousMap();
    explicit GPUHeterogeneousMap( std::shared_ptr<HeterogeneousMap> _values, Renderer& _rr );

    template <typename T>
    void visit( const T& _visitor ) {
        for ( auto& u : mTextureMappings )  { _visitor.visit( u.first.c_str(), u.second ); }
        values->visit( _visitor );
    }

    std::shared_ptr<HeterogeneousMap> Values();
    void textureAssign( const std::string& _key, const TextureUniformDesc& _value );

protected:
    std::shared_ptr<HeterogeneousMap> values;
    std::unordered_map<std::string, TextureUniformDesc> mTextureMappings;
};

class ProgramUniformSet {
public:
    ProgramUniformSet();
	explicit ProgramUniformSet( std::shared_ptr<HeterogeneousMap> _map, Renderer& _rr );

	void generateUBO( std::shared_ptr<ShaderManager> sm, const std::string& uniformName );
	void submitUBOData();
	void submitUBOData( void* data );

	void setUBOStructure( const std::string& uniformName, int off );

	void setUBOData( const std::string& uniformName, int value );
	void setUBOData( const std::string& uniformName, float value );
	void setUBOData( const std::string& uniformName, float x, float y );
	void setUBOData( const std::string& uniformName, float x, float y, float z );
	void setUBOData( const std::string& uniformName, float x, float y, float z, float w );
	void setUBOData( const std::string& uniformName, const Vector2f& v );
	void setUBOData( const std::string& uniformName, const Vector3f& v );
	template< typename T>
	void setUBODatav( const std::string& uniformName, const std::vector<T>& v ) {
		int accOff = 0;
		for ( const auto& vi : v ) {
			std::memcpy( mUBOBuffer.get() + mUBOOffsetMap[uniformName] + accOff, &vi, sizeof( T ) );
			accOff += 16;
		}
	}
	void setUBOData( const std::string& uniformName, const Vector4f& v );
	void setUBOData( const std::string& uniformName, const Matrix2f& matrix );
	void setUBOData( const std::string& uniformName, const Matrix3f& matrix );
	void setUBOData( const std::string& uniformName, const Matrix4f& matrix );

	int getUBOSize() const { return mUBOSize; }

	void setOn( unsigned int handle );

    std::shared_ptr<HeterogeneousMap> Values() {
        return values->Values();
    }
    std::shared_ptr<GPUHeterogeneousMap> GValues() {
        return values;
    }

private:
	int  getUBOPoint( const std::string& ubo_name );

	// Uniform Buffer Objects
	unsigned int mUBOHandle = 0;
	unsigned int mUBOPoint = 0;
	int mUBOSize = 0;
	std::unique_ptr<char[]> mUBOBuffer;

	static std::map<std::string, int> mUBOOffsetMap;
	static std::map<std::string, int> mUBONames;
	static std::map<std::string, unsigned int> mUBOHandles;

public:
	template <typename T>
	static void mapUBOData( std::shared_ptr<ProgramUniformSet> UBOSchema,
							const std::string& uniformName, const T& value, char* to_buffer ) {
		std::memcpy( to_buffer + UBOSchema->mUBOOffsetMap[uniformName], &value, sizeof(T) );
	}

private:
    std::shared_ptr<GPUHeterogeneousMap> values;
};

using UBO = ProgramUniformSet;

#define PUS ProgramUniformSet
