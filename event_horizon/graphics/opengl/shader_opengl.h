#pragma once

#include <string>

#include "gl_util.h"

// This represents either a vertex or a fragment shader
class Shader {
public:
	enum Type {
		TYPE_VERTEX_SHADER = 1,
		TYPE_TESSELATION_CONTROL_SHADER,
		TYPE_TESSELATION_EVALUATION_SHADER,
		TYPE_GEOMETRY_SHADER,
		TYPE_FRAGMENT_SHADER,
		TYPE_COMPUTE_SHADER
	};

	Shader( Type type, const std::string& id, const char* source ) {
		mType = type;
		mId = id;
		mSource = source;
		mHandle = 0;
	}

	// This gets called when the OpenGL gets initialised, we need to clear the old handle
	void clear() {
		mHandle = 0;
	}

	const char* getId() const {
		return mId.c_str();
	}

	GLuint getHandle() const {
		return mHandle;
	}

	bool isCompiled() const {
		return mHandle != 0;
	}

	void setSource( const char* source ) {
		mSource = source;
		mHandle = 0;
	}

	bool compile();

	GLenum getShaderType() const;

private:
	Type mType; // TYpe of shader, vertex or fragment
	GLuint mHandle; // OpenGL handle for the program
	std::string mId; //Human readable id
	std::string mSource; // Source code for the shader
};
