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

//std::shared_ptr<RenderMaterial> RenderMaterialBuilder::build() {
//    std::shared_ptr<Program> program;
//
//    program = rr.P( shaderName );
//
//    auto pus = std::make_shared<ProgramUniformSet>(material);
//
//    switch ( material->getType()) {
//        case MaterialType::Generic:
////            MaterialUniformRenderSetup{ std::dynamic_pointer_cast<GenericMaterial>( material ) }( program, pus, rr );
//            break;
//        case MaterialType::PBR:
//            MaterialPBRUniformRenderSetup{ std::dynamic_pointer_cast<PBRMaterial>( material ) }( program, pus, rr);
//            break;
//    }
//
//    auto m = std::make_shared<RenderMaterial>( program, pus );
//    rr.MaterialMap( m );
//
//    return m;
//}

//void
//MaterialPBRUniformRenderSetup::operator()( std::shared_ptr<Program> program, std::shared_ptr<ProgramUniformSet>& pus,
//                                           Renderer& rr ) const {
//
//    std::shared_ptr<PBRMaterial> pmat = std::dynamic_pointer_cast<PBRMaterial>( material );
//
//    pus->setOnGPU( program, UniformNames::opacity, material->getOpacity() );
//    pus->setOnGPU( program, UniformNames::alpha, pmat->getColor().w());
//    pus->setOnGPU( program, UniformNames::diffuseColor, pmat->getColor().xyz());
//
//    pus->setOnGPU( program, UniformNames::diffuseTexture, rr.TDI( pmat->getBaseColor(), TSLOT_COLOR ));
//    pus->setOnGPU( program, UniformNames::normalTexture, rr.TDI( pmat->getNormal(), TSLOT_NORMAL ));
//    pus->setOnGPU( program, UniformNames::aoTexture, rr.TDI( pmat->getAmbientOcclusion(), TSLOT_AO ));
//    pus->setOnGPU( program, UniformNames::roughnessTexture, rr.TDI( pmat->getRoughness(), TSLOT_ROUGHNESS ));
//    pus->setOnGPU( program, UniformNames::metallicTexture, rr.TDI( pmat->getMetallic(), TSLOT_METALLIC ));
//    pus->setOnGPU( program, UniformNames::heightTexture, rr.TDI( pmat->getHeight(), TSLOT_HEIGHT ));
//
//    pus->setOnGPU( program, UniformNames::ibl_irradianceMap, rr.TDI( FBNames::convolution , TSLOT_IBL_IRRADIANCE ));
//    pus->setOnGPU( program, UniformNames::ibl_specularMap, rr.TDI( FBNames::specular_prefilter , TSLOT_IBL_PREFILTER ));
//    pus->setOnGPU( program, UniformNames::ibl_brdfLUTMap, rr.TDI( FBNames::ibl_brdf , TSLOT_IBL_BRDFLUT ));
//
//    pus->setOnGPU( program, UniformNames::metallic, pmat->getMetallicValue());
//    pus->setOnGPU( program, UniformNames::roughness, pmat->getRoughnessValue());
//    pus->setOnGPU( program, UniformNames::ao, pmat->getAoValue());
//}

