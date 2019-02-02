#include "skybox.h"
#include "core/math/poly_shapes.hpp"
#include <core/descriptors/material.h>
#include "vertex_processing.h"
#include "framebuffer.h"
#include "renderer.h"
#include <graphics/vp_builder.hpp>

void Skybox::equirectangularTextureInit( const std::vector<std::string>& params ) {

    PolyStruct sp = createGeomForCube( Vector3f::ZERO, Vector3f{1.0f} );
    std::unique_ptr<VFPos3d[]> vpos3d = Pos3dStrip::vtoVF( sp.verts, sp.numVerts );
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>( sp.numVerts, PRIMITIVE_TRIANGLES,
                                                                           sp.numIndices, vpos3d, sp.indices );

    VPBuilder<Pos3dStrip>{rr,mVPList,S::EQUIRECTANGULAR}.p(colorStrip).t(params[0]).n("skybox")
    .build();

    isReadyToRender = true;
}

void Skybox::init( const SkyBoxMode _sbm, const std::string& _textureName ) {
    mVPList = std::make_shared<VPList>();
    mode = _sbm;
    PolyStruct sp;
    bool bBuildVP = true;
    static bool skyBoxEquilateral = false;

    if ( skyBoxEquilateral ) {
        equirectangularTextureInit( {_textureName} );
        return;
    }

    switch ( mode ) {
        case SkyBoxMode::SphereProcedural:
            sp = createGeomForSphere( Vector3f::ZERO, 1.0f, 3 );
            isReadyToRender = true;
            break;
        case SkyBoxMode::CubeProcedural:
            sp = createGeomForCube( Vector3f::ZERO, Vector3f{1.0f} );
            isReadyToRender = true;
            break;
        case SkyBoxMode::EquirectangularTexture:
            ImageBuilder{_textureName}
                    .cc(std::bind(&Skybox::equirectangularTextureInit, this, std::placeholders::_1), {_textureName})
                    .build(rr.RIDM());
            bBuildVP = false;
            skyBoxEquilateral = true;
    }

    if ( bBuildVP ) {
        std::unique_ptr<VFPos3d[]> vpos3d = Pos3dStrip::vtoVF( sp.verts, sp.numVerts );
        std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>( sp.numVerts, PRIMITIVE_TRIANGLES,
                                                                               sp.numIndices, vpos3d, sp.indices );
        VPBuilder<Pos3dStrip>{rr,mVPList,S::SKYBOX}.p(colorStrip).n("skybox").build();
    }

}

void Skybox::render( float _sunHDRMult ) {
    if ( isReadyToRender ) {
        rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestLEqual } );
        rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeFront } );
        mVPList->setMaterialConstant( UniformNames::sunHRDMult, _sunHDRMult );
        mVPList->addToCommandBuffer( rr );
        rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestLess } );
    }
}

Skybox::Skybox( Renderer& rr, const SkyBoxInitParams& _params ) : RenderModule( rr ) {
    init( _params.mode, _params.assetString );
}

//bool Skybox::needsReprobing( int _renderCounter ) {
//    if ( mode == SkyBoxMode::CubeProcedural || mode == SkyBoxMode::SphereProcedural ) {
//        if ( !isReadyToRender && _renderCounter == renderIndexExtraFrameToAvoidGlitch+1 ) {
//            isReadyToRender = true;
//            return true;
//        }
//        renderIndexExtraFrameToAvoidGlitch = _renderCounter;
//    }
//    if ( mode == SkyBoxMode::EquirectangularTexture ) {
//        if ( bTriggerReprobing ) {
//            if ( _renderCounter == renderIndexExtraFrameToAvoidGlitch + 2 ) {
//                bTriggerReprobing = false;
//                isReadyToRender = true;
//                return true;
//            }
//        } else {
//            renderIndexExtraFrameToAvoidGlitch = _renderCounter;
//        }
//    }
//    return false;
//}

void CubeEnvironmentMap::init() {
    mVPList = std::make_shared<VPList>();

    auto sp = createGeomForCube( Vector3f::ZERO, Vector3f{1.0f} );

    std::unique_ptr<VFPos3d[]> vpos3d = Pos3dStrip::vtoVF( sp.verts, sp.numVerts );
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>( sp.numVerts, PRIMITIVE_TRIANGLES,
                                                                           sp.numIndices, vpos3d, sp.indices );

    VPBuilder<Pos3dStrip>{rr,mVPList,S::CONVOLUTION}.p(colorStrip).n("cubeEnvMap").build();
}

void CubeEnvironmentMap::render( std::shared_ptr<Texture> cmt ) {
    rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeFront } );
    mVPList->setMaterialConstant( UniformNames::cubeMapTexture, cmt->TDI(0) );
    mVPList->addToCommandBuffer( rr );
}

CubeEnvironmentMap::CubeEnvironmentMap( Renderer& rr ) : RenderModule( rr ) {
    init();
}

void PrefilterSpecularMap::init() {
    mVPList = std::make_shared<VPList>();

    auto sp = createGeomForCube( Vector3f::ZERO, Vector3f{1.0f} );

    std::unique_ptr<VFPos3d[]> vpos3d = Pos3dStrip::vtoVF( sp.verts, sp.numVerts );
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>( sp.numVerts, PRIMITIVE_TRIANGLES,
                                                                           sp.numIndices, vpos3d, sp.indices );

    VPBuilder<Pos3dStrip>{rr,mVPList,S::IBL_SPECULAR}.p(colorStrip).n("iblSpecularEnvMap").build();
}

void PrefilterSpecularMap::render( std::shared_ptr<Texture> cmt, const float roughness ) {
    rr.CB_U().startList( nullptr, CommandBufferFlags::CBF_DoNotSort );
    rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeFront } );
    mVPList->setMaterialConstant( UniformNames::cubeMapTexture, cmt->TDI(0) );
    mVPList->setMaterialConstant( UniformNames::roughness, roughness );
    mVPList->addToCommandBuffer( rr );
}

PrefilterSpecularMap::PrefilterSpecularMap( Renderer& rr ) : RenderModule( rr ) {
    init();
}

void PrefilterBRDF::init() {
    mBRDF = FrameBufferBuilder{ rr, MPBRTextures::ibl_brdf }.size( 512 ).GPUSlot( TSLOT_IBL_BRDFLUT ).format(
            PIXEL_FORMAT_HDR_RG_16 ). IM(S::IBL_BRDF).noDepth().build();
}

void PrefilterBRDF::render( ) {
    rr.CB_U().startTarget( mBRDF, rr );
    mBRDF->VP()->addToCommandBuffer( rr );
}

PrefilterBRDF::PrefilterBRDF( Renderer& rr ) : RenderModule( rr ) {
    init();
}
