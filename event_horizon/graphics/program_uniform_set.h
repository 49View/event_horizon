#pragma once

#include <string>
#include <cstring>
#include <map>
#include <core/math/vector4f.h>
#include <core/math/matrix4f.h>
#include <core/math/matrix2f.h>
#include <core/descriptors/material.h>
#include "program.h"

class Texture;
class Program;
class Renderer;
class ShaderManager;

class GPUUniformVisitor {
public:
	explicit GPUUniformVisitor( unsigned int handle ) : handle( handle ) {}

	void visit( const char* name, const TextureIndex& value ) const;
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

class ProgramUniformSet : public Material {
public:
	using Material::Material;
	explicit ProgramUniformSet( std::shared_ptr<Material> _map, Renderer& _rr );

	void generateUBO( const ShaderManager& sm, const std::string& uniformName );
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

	template <typename T>
	void setOnGPU( const std::shared_ptr<Program>& _program, const std::string _uniformName, const T& _value ) {
		if ( _program->hasUniform(_uniformName) ) assign( _uniformName, _value );
	}

	void setOn( unsigned int handle );

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

};

using UBO = ProgramUniformSet;

#define PUS ProgramUniformSet
