#include <utility>

#include <utility>

//
// Created by Dado on 2019-03-26.
//

#include "shader_material.hpp"
#include <graphics/renderer.h>
#include <graphics/program.h>

ShaderMaterial::ShaderMaterial( std::string _sn ) : shaderName( std::move( _sn )) {
}

ShaderMaterial::ShaderMaterial( std::string shaderName, std::shared_ptr<HeterogeneousMap> values )
        : shaderName( std::move( shaderName )), values( std::move( values )) {}

void ShaderMaterial::activate( Renderer& _rr ) {
    shaderProgram = _rr.P( shaderName );
    ASSERT(shaderProgram != nullptr);
    if ( !values ) {
        values = std::make_shared<HeterogeneousMap>(shaderProgram->getDefaultUniforms());
    }
//  ### MAT Double check how we inject default unassigned values, it should never be the case anyway!!
    values->injectIfNotPresent( *shaderProgram->getDefaultUniforms() );
}

const std::string& ShaderMaterial::SN() const {
    return shaderName;
}

const std::shared_ptr<Program> ShaderMaterial::P() const {
    return shaderProgram;
}

const std::shared_ptr <HeterogeneousMap>& ShaderMaterial::Values() const {
    return values;
}

ShaderMaterial::ShaderMaterial( std::string shaderName, Renderer& _rr ) : shaderName( std::move( shaderName )) {
    activate(_rr);
}
