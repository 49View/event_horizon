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

void RenderMaterial::serialize( std::shared_ptr<SerializeBin> writer ) {
    writer->write( mType );
    uniforms->serialize( writer );
}

void RenderMaterial::deserialize( std::shared_ptr<DeserializeBin> reader ) {
    // This is asymmetric wrt serialize because we need to read the material type apriori in order to allocate the right type
    uniforms->deserialize( reader );
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
    PUTB tub{ rr };

    switch ( material->getType()) {
        case MaterialType::Generic:
            program = rr.P( shaderName );
            MaterialUniformRenderSetup{ std::dynamic_pointer_cast<GenericMaterial>( material ) }( program, pus, tub.p
                    ( program ));
            break;
        case MaterialType::PBR:
            program = rr.P( shaderName );
            MaterialPBRUniformRenderSetup{ std::dynamic_pointer_cast<PBRMaterial>( material ) }
                    ( program, pus, tub.p( program ));
            break;
    }

    auto m = std::make_shared<RenderMaterial>( program, pus );

    //LOGI( "New RenderMaterial created %s", m->typeAsString().c_str());
    rr.MaterialMap( m );

    return m;
}

void MaterialUniformRenderSetup::operator()( std::shared_ptr<Program> program,
                                             std::shared_ptr<ProgramUniformSet>& pus,
                                             PUTB& tub ) const {

    pus->assign( program, UniformNames::opacity, material->getOpacity() );
    pus->assign( program, UniformNames::alpha, material->getColor().w());
    pus->assign( program, UniformNames::diffuseColor, material->getColor().xyz());

    pus->assign( tub.u( UniformNames::colorTexture ).t( material->getTextureName()).s( TSLOT_COLOR ));
//    pus->assign( tub.u( UniformNames::yTexture ).t("http://192.168.1.123:8080/video_y").s( TSLOT_COLOR ));
}

void
MaterialPBRUniformRenderSetup::operator()( std::shared_ptr<Program> program, std::shared_ptr<ProgramUniformSet>& pus,
                                           PUTB& tub ) const {
//    MaterialUniformRenderSetup::operator()( program, pus, tub );

    std::shared_ptr<PBRMaterial> pmat = std::dynamic_pointer_cast<PBRMaterial>( material );

    pus->assign( program, UniformNames::opacity, material->getOpacity() );
    pus->assign( program, UniformNames::alpha, pmat->getColor().w());
    pus->assign( program, UniformNames::diffuseColor, pmat->getColor().xyz());

    pus->assign( tub.u( UniformNames::diffuseTexture ).t( pmat->getBaseColor()).s( TSLOT_COLOR ));
    pus->assign( tub.u( UniformNames::normalTexture ).t( pmat->getNormal()).s( TSLOT_NORMAL ));
    pus->assign( tub.u( UniformNames::aoTexture ).t( pmat->getAmbientOcclusion()).s( TSLOT_AO ));
    pus->assign( tub.u( UniformNames::roughnessTexture ).t( pmat->getRoughness()).s( TSLOT_ROUGHNESS ));
    pus->assign( tub.u( UniformNames::metallicTexture ).t( pmat->getMetallic()).s( TSLOT_METALLIC ));
    pus->assign( tub.u( UniformNames::heightTexture ).t( pmat->getHeight()).s( TSLOT_HEIGHT ));

//    pus->assign( tub.u( UniformNames::shadowMapTexture ).t( FBNames::shadowmap ).s( TSLOT_SHADOWMAP ));
//    pus->assign( tub.u( UniformNames::lightmapTexture ).t( FBNames::lightmap ).s( TSLOT_LIGHTMAP ));
    pus->assign( tub.u( UniformNames::ibl_irradianceMap ).t( FBNames::convolution ).s( TSLOT_IBL_IRRADIANCE ));
    pus->assign( tub.u( UniformNames::ibl_specularMap ).t( FBNames::specular_prefilter ).s( TSLOT_IBL_PREFILTER ));
    pus->assign( tub.u( UniformNames::ibl_brdfLUTMap ).t( FBNames::ibl_brdf ).s( TSLOT_IBL_BRDFLUT ));

    pus->assign( program, UniformNames::metallic, pmat->getMetallicValue());
    pus->assign( program, UniformNames::roughness, pmat->getRoughnessValue());
    pus->assign( program, UniformNames::ao, pmat->getAoValue());
}
