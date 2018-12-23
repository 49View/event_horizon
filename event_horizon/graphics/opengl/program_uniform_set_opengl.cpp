#include "../program_uniform_set.h"
#include "../uniform_names.h"
#include "../shader_manager.h"
#include "gl_util.h"

// Uniform Buffer Objects

void ProgramUniformSet::generateUBO( const ShaderManager& sm, const std::string& uniformName ) {

	mUBOBuffer = std::make_unique<char[]>( mUBOSize );
	glGenBuffers( 1, &mUBOHandle );
	glBindBuffer( GL_UNIFORM_BUFFER, mUBOHandle );
	glBufferData( GL_UNIFORM_BUFFER, mUBOSize, NULL, GL_DYNAMIC_DRAW );
	glBindBuffer( GL_UNIFORM_BUFFER, 0 );

	mUBOHandles[uniformName] = mUBOHandle;

	mUBOPoint = getUBOPoint( uniformName );
	for ( const auto& i : sm.ProgramsHandles() ) {
		GLuint lights_index = glGetUniformBlockIndex( i, uniformName.c_str() );
		if ( lights_index != GL_INVALID_INDEX ) {
			glUniformBlockBinding( i, lights_index, mUBOPoint );
		}
	}

	glBindBufferBase( GL_UNIFORM_BUFFER, mUBOPoint, mUBOHandle );
}

void ProgramUniformSet::submitUBOData() {
	GLCALL( glBindBuffer( GL_UNIFORM_BUFFER, mUBOHandle ) );
	GLCALL( glBufferData( GL_UNIFORM_BUFFER, mUBOSize, reinterpret_cast<void*>( mUBOBuffer.get() ), GL_STATIC_DRAW ) );
}

void ProgramUniformSet::submitUBOData( void* data ) {
	GLCALL( glBindBuffer( GL_UNIFORM_BUFFER, mUBOHandle ) );
	GLCALL( glBufferData( GL_UNIFORM_BUFFER, mUBOSize, data, GL_STATIC_DRAW ) );
}

////// Naked Uniforms

GLint ProgramUniformSet::hasUniform( GLuint handle, const char* name ) const {
	return glGetUniformLocation( handle, name );
}

void ProgramUniformSet::setUniform( const char* name, int value, GLuint handle ) const {
	GLint location = hasUniform( handle, name );
	if ( location == -1 )  return;
	GLCALL( glUniform1i( location, value ) );
}

void ProgramUniformSet::setUniform( const char* name, float value, GLuint handle ) const {
	GLint location = hasUniform( handle, name );
	if ( location == -1 )  return;
	GLCALL( glUniform1f( location, value ) );
}

void ProgramUniformSet::setUniform( const char* name, float x, float y, GLuint handle ) const {
	GLint location = hasUniform( handle, name );
	if ( location == -1 )  return;
	GLCALL( glUniform2f( location, x, y ) );
}

void ProgramUniformSet::setUniform( const char* name, float x, float y, float z, GLuint handle ) const {
	GLint location = hasUniform( handle, name );
	if ( location == -1 )  return;
	GLCALL( glUniform3f( location, x, y, z ) );
}

void ProgramUniformSet::setUniform( const char* name, float x, float y, float z, float w, GLuint handle ) const {
	GLint location = hasUniform( handle, name );
	if ( location == -1 )  return;
	GLCALL( glUniform4f( location, x, y, z, w ) );
}

void ProgramUniformSet::setUniform( const char* name, const Vector2f& v, GLuint handle ) const {
	GLint location = hasUniform( handle, name );
	if ( location == -1 )  return;
	glUniform2f( location, v.x(), v.y() );
}

void ProgramUniformSet::setUniform( const char* name, const Vector3f& v, GLuint handle ) const {
	GLint location = hasUniform( handle, name );
	if ( location == -1 )  return;
	glUniform3f( location, v.x(), v.y(), v.z() );
}

void ProgramUniformSet::setUniform( const char* name, const std::vector<Vector3f>& v, GLuint handle ) const {
	GLint location = hasUniform( handle, name );
	if ( location == -1 )  return;
	unsigned int numv = static_cast<unsigned int>( v.size() );
	glUniform3fv( location, numv, reinterpret_cast<const float*>( v.data() ) );
}

void ProgramUniformSet::setUniform( const char* name, const Vector4f& v, GLuint handle ) const {
	GLint location = hasUniform( handle, name );
	if ( location == -1 )  return;
	glUniform4f( location, v.x(), v.y(), v.z(), v.w() );
}

void ProgramUniformSet::setUniform( const char* name, const Matrix2f& matrix, GLuint handle ) const {
	GLint location = hasUniform( handle, name );
	if ( location == -1 )  return;
	GLCALL( glUniformMatrix2fv( location, 1, false, reinterpret_cast<const float*>( &matrix ) ) );
}

void ProgramUniformSet::setUniform( const char* name, const Matrix3f& matrix, GLuint handle ) const {
	GLint location = hasUniform( handle, name );
	if ( location == -1 )  return;
	GLCALL( glUniformMatrix3fv( location, 1, false, reinterpret_cast<const float*>( &matrix ) ) );
}

void ProgramUniformSet::setUniform( const char* name, const Matrix4f& matrix, GLuint handle ) const {
	GLint location = hasUniform( handle, name );
	if ( location == -1 )  return;
	GLCALL( glUniformMatrix4fv( location, 1, false, reinterpret_cast<const float*>( &matrix ) ) );
}