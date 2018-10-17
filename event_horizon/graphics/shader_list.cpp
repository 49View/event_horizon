//
//  shader_list.cpp
//  SixthView
//
//  Created byDado on 11/01/2013.
//  Copyright (c) 2013Dado. All rights reserved.
//

#include "shader_list.h"

std::vector<ShaderSource>  ShaderList::SHADER_LIST;

void ShaderList::addShader( const ShaderSource& ss ) {
	if ( ss.getId() != "" ) {
		SHADER_LIST.push_back( ss );
	}
}

uint64_t ShaderList::getShaderCount() {
	return SHADER_LIST.size();
}

ShaderSource* ShaderList::getShader( int index ) {
	return &SHADER_LIST[index];
}