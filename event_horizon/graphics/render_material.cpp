//
//  material.cpp
//  6thViewImporter
//
//  Created by Dado on 13/10/2015.
//
//

#include "render_material.hpp"

#include <core/descriptors/uniform_names.h>
#include <graphics/renderer.h>

void RenderMaterial::removeAllTextures() {
//    Uniforms()->clearTextures();
//    calcHash();
}

RenderMaterial::RenderMaterial( std::shared_ptr<Program> _program,
                                std::shared_ptr<HeterogeneousMap> _map, Renderer& _rr ) : rr(_rr) {

    BoundProgram( std::move( _program ));

    Uniforms( std::make_shared<ProgramUniformSet>(_map, rr) );
    globalUniforms = std::make_shared<ProgramUniformSet>();

    calcHash();
}

void RenderMaterial::calcHash() {
    mHash = Uniforms()->Values()->Hash();

    auto ot = Uniforms()->Values()->get<std::string>( UniformNames::opacityTexture);
    auto tt = Uniforms()->Values()->get<std::string>( UniformNames::translucencyTexture);
    float ottFactor = 1.0f;
    ottFactor *= ((*ot).empty() || (*ot) == S::WHITE ) ? 1.0f : 0.9f;
    ottFactor *= ((*tt).empty() || (*tt) == S::BLACK ) ? 1.0f : 0.9f;

    TransparencyValue( Uniforms()->Values()->getDef( UniformNames::alpha, 1.0f ) *
                       Uniforms()->Values()->getDef( UniformNames::opacity, 1.0f ) *
                       ottFactor );
}

void RenderMaterial::submitBufferUniforms() {
    for ( auto& b : bufferUniforms )
        b.second->submitUBOData();
}

void RenderMaterial::BoundProgram( std::shared_ptr<Program> val ) {
    boundProgram = val;
    //Type( val->getId() );
}
