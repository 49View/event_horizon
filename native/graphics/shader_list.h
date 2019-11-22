//
//  shader_list.h
//  SixthView
//
//  Created byDado on 11/01/2013.
//  Copyright (c) 2013Dado. All rights reserved.
//

#ifndef __SixthView__shader_list__
#define __SixthView__shader_list__

#include <iostream>
#include "shader.h"

class ShaderSource {
public:
	ShaderSource( const std::string& id, const std::string& filename, Shader::Type type ) {
		mId = id;
		mFilename = filename;
		mType = type;
	}

	std::string getId() const {
		return mId;
	}

	std::string getFilename() const {
		return mFilename;
	}

	Shader::Type getType() const {
		return mType;
	}

private:
	std::string mId;
	std::string mFilename;
	Shader::Type mType;
};

class ShaderList {
public:
	static uint64_t getShaderCount();
	static ShaderSource* getShader( int index );
	static void addShader( const ShaderSource& ss );
private:
	static std::vector<ShaderSource> SHADER_LIST;
};

#endif /* defined(__SixthView__shader_list__) */
