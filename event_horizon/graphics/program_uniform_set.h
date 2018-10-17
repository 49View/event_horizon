#pragma once

#include <string>
#include <cstring>
#include <map>
#include "core/math/vector4f.h"
#include "core/math/matrix4f.h"
#include "core/math/matrix2f.h"
#include "program.h"
#include "uniform_names.h"

enum TextureSetFlags : int {
	TSF_Color = 1,
	TSF_Normal = 1 << 1,
	TSF_Specular = 1 << 2
};

class Texture;
class SerializeBin;
class DeserializeBin;
class Program;
class Renderer;
class ShaderManager;

enum class PUSType {
	Empty,
	Color,
	Alpha,
	Texture,
	Color_alpha,
	Color_texture,
	Color_alpha_texture,
	UI,
	UI_3d,
	Full_3d,
	Full_3d_NoTexture,
	Font,
	Global,
};

enum class ColorMatType {
	Color,
	Diffuse
};

struct ProgramUniformTextureBuilder {
	ProgramUniformTextureBuilder( Renderer& rr ) : rr( rr ) {}

	ProgramUniformTextureBuilder& p( std::shared_ptr<Program> _p ) {
		program = _p;
		return *this;
	}
	ProgramUniformTextureBuilder& u( std::string _uniformName ) {
		uniformName = _uniformName;
		return *this;
	}
	ProgramUniformTextureBuilder& t( std::string _textureName ) {
		textureName = _textureName;
		return *this;
	}
	ProgramUniformTextureBuilder& tb( std::string _textureBaseName ) {
		textureBaseName = _textureBaseName;
		return *this;
	}
	ProgramUniformTextureBuilder& te( std::string _textureExt ) {
		textureExt = _textureExt;
		return *this;
	}
	ProgramUniformTextureBuilder& s( TextureSlots _tslot ) {
		tslot = _tslot;
		return *this;
	}
	ProgramUniformTextureBuilder& b( std::string _backupTextureName ) {
		backupTextureName = _backupTextureName;
		return *this;
	}
	ProgramUniformTextureBuilder& p( bool _direct ) {
		direct = _direct;
		return *this;
	}

	Renderer& rr;
	std::shared_ptr<Program> program;
	std::string uniformName;
	std::string textureName;
	std::string textureBaseName;
	std::string textureExt;
	TextureSlots tslot = TSLOT_COLOR;
	std::string backupTextureName = "white";
	bool direct = false;
};

using PUTB = ProgramUniformTextureBuilder;

class ProgramUniformSet {
public:
	void serialize( std::shared_ptr<SerializeBin> writer );
	void deserialize( std::shared_ptr<DeserializeBin> reader );

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
	void assign( const ProgramUniformSet* source );

	template <typename T>
	void assign( std::shared_ptr<Program> _program, const std::string _uniformName, const T& _value ) {
		if ( _program->hasUniform(_uniformName) ) assign( _uniformName, _value );
	}

	void assign( ProgramUniformTextureBuilder& pub );

	void assign( const std::string& uniformName, int data );
	void assign( const std::string& uniformName, float data );
	void assign( const std::string& uniformName, double data );
	void assign( const std::string& uniformName, std::shared_ptr<Texture> data );
	void assign( const std::string& uniformName, const Vector2f& data );
	void assign( const std::string& uniformName, const Vector3f& data );
	void assign( const std::string& uniformName, const Vector4f& data );
	void assign( const std::string& uniformName, const Matrix4f& data );
	void assign( const std::string& uniformName, const Matrix3f& data );
	void assign( const std::string& uniformName, const std::vector<Vector3f>& data );

	void assignGlobal( const std::string& uniformName, int data );
	void assignGlobal( const std::string& uniformName, float data );
	void assignGlobal( const std::string& uniformName, double data );
	void assignGlobal( const std::string& uniformName, std::shared_ptr<Texture> data );
	void assignGlobal( const std::string& uniformName, const Vector2f& data );
	void assignGlobal( const std::string& uniformName, const Vector3f& data );
	void assignGlobal( const std::string& uniformName, const Vector4f& data );
	void assignGlobal( const std::string& uniformName, const Matrix4f& data );
	void assignGlobal( const std::string& uniformName, const Matrix3f& data );
	void assignGlobal( const std::string& uniformName, const std::vector<Vector3f>& data );

//	void setStatic( const std::string& uniformName, int data );
//	void setStatic( const std::string& uniformName, float data );
//	void setStatic( const std::string& uniformName, double data );
//	void setStatic( const std::string& uniformName, std::shared_ptr<Texture> data, const Program* _p = nullptr );
//	void setStatic( const std::string& uniformName, const Vector2f& data );
//	void setStatic( const std::string& uniformName, const Vector3f& data );
//	void setStatic( const std::string& uniformName, const Vector4f& data );
//	void setStatic( const std::string& uniformName, const Matrix4f& data );
//	void setStatic( const std::string& uniformName, const Matrix3f& data );
//	void setStatic( const std::string& uniformName, const std::vector<Vector3f>& data );

	void clearTextures();
	std::shared_ptr<Texture> getTexture( const std::string& uniformName ) const;
	float getInt( const std::string & uniformName ) const;
	float getFloatWithDefault( const std::string& uniformName, const float def ) const;
	float getFloat( const std::string & uniformName ) const;
	Vector2f getVector2f( const std::string & uniformName ) const;
	Vector3f getVector3f( const std::string & uniformName ) const;
	Vector4f getVector4f( const std::string & uniformName ) const;
	Matrix4f getMatrix4f( const std::string & uniformName ) const;
	Matrix3f getMatrix3f( const std::string & uniformName ) const;
	void get( const std::string& uniformName, std::shared_ptr<Texture> ret ) const;
	void get( const std::string& uniformName, int& ret ) const;
	void get( const std::string& uniformName, float& ret ) const;
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

	void setOn( unsigned int handle );

	int NumUniforms() const { return mNumUniforms; }

	std::shared_ptr<ProgramUniformSet> clone();

	int64_t Hash() const { return mHash; }
private:
	int hasUniform( unsigned int handle, const char* name ) const;
	void setUniform( const char* name, int value, unsigned int handle ) const;
	void setUniform( const char* name, float value, unsigned int handle ) const;
	void setUniform( const char* name, float x, float y, unsigned int handle ) const;
	void setUniform( const char* name, float x, float y, float z, unsigned int handle ) const;

	void setUniform( const char* name, float x, float y, float z, float w, unsigned int handle ) const;
	void setUniform( const char* name, const Vector2f& v, unsigned int handle ) const;
	void setUniform( const char* name, const Vector3f& v, unsigned int handle ) const;
	void setUniform( const char* name, const std::vector<Vector3f>& v, unsigned int handle ) const;
	void setUniform( const char* name, const Vector4f& v, unsigned int handle ) const;
	void setUniform( const char* name, const Matrix2f& matrix, unsigned int handle ) const;
	void setUniform( const char* name, const Matrix3f& matrix, unsigned int handle ) const;
	void setUniform( const char* name, const Matrix4f& matrix, unsigned int handle ) const;

	int  getUBOPoint( const std::string& ubo_name );

	void calcTotalNumUniforms();
	void calcHash();

private:
	std::map<std::string, std::shared_ptr<Texture>> mTextures;
	std::map<std::string, int> mInts;
	std::map<std::string, float> mFloats;
	std::map<std::string, Vector2f> mV2fs;
	std::map<std::string, Vector3f> mV3fs;
	std::map<std::string, std::vector<Vector3f>> mV3fvs;
	std::map<std::string, Vector4f> mV4fs;
	std::map<std::string, Matrix3f> mM3fs;
	std::map<std::string, Matrix4f> mM4fs;
	int mNumUniforms = 0;

	// Uniform Buffer Objects
	unsigned int mUBOHandle = 0;
	unsigned int mUBOPoint = 0;
	int mUBOSize = 0;
	std::unique_ptr<char[]> mUBOBuffer;

	static std::map<std::string, int> mUBOOffsetMap;
	static std::map<std::string, int> mUBONames;
	static std::map<std::string, unsigned int> mUBOHandles;

	int64_t mHash = 0;
public:
	template <typename T>
	static void mapUBOData( std::shared_ptr<ProgramUniformSet> UBOSchema,
							const std::string& uniformName, const T& value, char* to_buffer ) {
		std::memcpy( to_buffer + UBOSchema->mUBOOffsetMap[uniformName], &value, sizeof(T) );
	}

//	static ProgramUniformSet& globalStaticUniform();
};

using UBO = ProgramUniformSet;

#define PUS ProgramUniformSet
