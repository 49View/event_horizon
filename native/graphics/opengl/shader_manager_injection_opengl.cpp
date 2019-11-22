//
// Created by Dado on 28/12/2017.
//

#include "shader_manager_opengl.h"

void ShaderManager::inject( const std::string& key, const std::string& text ) {
    shaderSourcesMap[key] = text;
}

bool ShaderManager::checkShaderChanged( const std::string& key, const std::string& text ) const {
    if ( auto it = shaderSourcesMap.find(key); it != shaderSourcesMap.end() ) {
        return it->second != text;
    }

    return false;
}

bool ShaderManager::injectIfChanged( const std::string& key, const std::string& text ) {
    if ( checkShaderChanged( key, text) ) {
        inject( key, text );
        return true;
    }
    return false;
}
