//
//  material.cpp
//  6thViewImporter
//
//  Created by Dado on 13/10/2015.
//
//

#include "render_material.hpp"
#include "renderer.h"

void RenderMaterial::removeAllTextures() {
//    Uniforms()->clearTextures();
//    calcHash();
}

RenderMaterial::RenderMaterial( std::shared_ptr<Program> _program,
                                std::shared_ptr<Material> _material, Renderer& _rr ) : rr(_rr) {

    sourceMaterial = _material;
    BoundProgram( _program );
    Uniforms( std::make_shared<ProgramUniformSet>(_material, rr) );

    globalUniforms = std::make_shared<ProgramUniformSet>("g", "g");

    calcHash();
}

void RenderMaterial::calcHash() {
    mHash = Uniforms()->Hash();

    TransparencyValue( Uniforms()->getFloatWithDefault( UniformNames::alpha, 1.0f ) *
                       Uniforms()->getFloatWithDefault( UniformNames::opacity, 1.0f ));
}

void RenderMaterial::submitBufferUniforms() {
    for ( auto& b : bufferUniforms )
        b.second->submitUBOData();
}

void RenderMaterial::BoundProgram( std::shared_ptr<Program> val ) {
    boundProgram = val;
    //Type( val->getId() );
}
