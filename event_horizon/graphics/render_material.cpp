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
    Uniforms()->clearTextures();
    calcHash();
}

RenderMaterial::RenderMaterial( std::shared_ptr<Program> _program, std::shared_ptr<ProgramUniformSet> _uniforms ) {
    BoundProgram( _program );
    Uniforms( _uniforms );

    globalUniforms = std::make_shared<ProgramUniformSet>();

    calcHash();
}

void RenderMaterial::calcHash() {
    mHash = static_cast<int64_t>(mType);
    mHash += Uniforms()->Hash();

    TransparencyValue( Uniforms()->getFloatWithDefault( UniformNames::alpha, 1.0f ) *
                       Uniforms()->getFloatWithDefault( UniformNames::opacity, 1.0f ));
}

std::string RenderMaterial::typeAsString() {
    return std::to_string( static_cast<int64_t >(mType));
}

void RenderMaterial::submitBufferUniforms() {
    for ( auto& b : bufferUniforms )
        b.second->submitUBOData();
}

void RenderMaterial::BoundProgram( std::shared_ptr<Program> val ) {
    boundProgram = val;
    //Type( val->getId() );
}

MaterialType RenderMaterial::Type() const
{
    return mType;
}

std::shared_ptr<RenderMaterial> RenderMaterialBuilder::build() {
    std::shared_ptr<Program> program;

    auto pus = std::make_shared<ProgramUniformSet>();
//    PUTB tub{ rr };

    switch ( material->getType()) {
        case MaterialType::Generic:
            program = rr.P( shaderName );
            MaterialUniformRenderSetup{ std::dynamic_pointer_cast<GenericMaterial>( material ) }( program, pus, rr );
            break;
        case MaterialType::PBR:
            program = rr.P( shaderName );
            MaterialPBRUniformRenderSetup{ std::dynamic_pointer_cast<PBRMaterial>( material ) }
                    ( program, pus, rr);
            break;
    }

    auto m = std::make_shared<RenderMaterial>( program, pus );

    //LOGI( "New RenderMaterial created %s", m->typeAsString().c_str());
    rr.MaterialMap( m );

    return m;
}

void MaterialUniformRenderSetup::operator()( std::shared_ptr<Program> program,
                                             std::shared_ptr<ProgramUniformSet>& pus,
                                             Renderer& rr ) const {

    pus->assign( program, UniformNames::opacity, material->getOpacity() );
    pus->assign( program, UniformNames::alpha, material->getColor().w());
    pus->assign( program, UniformNames::diffuseColor, material->getColor().xyz());

    pus->assign( program, UniformNames::colorTexture, rr.TDI( material->getTextureName(), TSLOT_COLOR ) );
//    pus->assign( tub.u( UniformNames::yTexture ).t("http://192.168.1.123:8080/video_y").s( TSLOT_COLOR ));
}

void
MaterialPBRUniformRenderSetup::operator()( std::shared_ptr<Program> program, std::shared_ptr<ProgramUniformSet>& pus,
                                           Renderer& rr ) const {

    std::shared_ptr<PBRMaterial> pmat = std::dynamic_pointer_cast<PBRMaterial>( material );

    pus->assign( program, UniformNames::opacity, material->getOpacity() );
    pus->assign( program, UniformNames::alpha, pmat->getColor().w());
    pus->assign( program, UniformNames::diffuseColor, pmat->getColor().xyz());

    pus->assign( program, UniformNames::diffuseTexture, rr.TDI( pmat->getBaseColor(), TSLOT_COLOR ));
    pus->assign( program, UniformNames::normalTexture, rr.TDI( pmat->getNormal(), TSLOT_NORMAL ));
    pus->assign( program, UniformNames::aoTexture, rr.TDI( pmat->getAmbientOcclusion(), TSLOT_AO ));
    pus->assign( program, UniformNames::roughnessTexture, rr.TDI( pmat->getRoughness(), TSLOT_ROUGHNESS ));
    pus->assign( program, UniformNames::metallicTexture, rr.TDI( pmat->getMetallic(), TSLOT_METALLIC ));
    pus->assign( program, UniformNames::heightTexture, rr.TDI( pmat->getHeight(), TSLOT_HEIGHT ));

    pus->assign( program, UniformNames::ibl_irradianceMap, rr.TDI( FBNames::convolution , TSLOT_IBL_IRRADIANCE ));
    pus->assign( program, UniformNames::ibl_specularMap, rr.TDI( FBNames::specular_prefilter , TSLOT_IBL_PREFILTER ));
    pus->assign( program, UniformNames::ibl_brdfLUTMap, rr.TDI( FBNames::ibl_brdf , TSLOT_IBL_BRDFLUT ));

    pus->assign( program, UniformNames::metallic, pmat->getMetallicValue());
    pus->assign( program, UniformNames::roughness, pmat->getRoughnessValue());
    pus->assign( program, UniformNames::ao, pmat->getAoValue());
}
