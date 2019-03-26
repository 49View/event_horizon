#include "skybox.h"

#include <core/math/poly_shapes.hpp>
#include <core/camera_utils.hpp>
#include <core/descriptors/material.h>
#include <graphics/vertex_processing.h>
#include <graphics/framebuffer.h>
#include <graphics/renderer.h>
#include <graphics/vp_builder.hpp>
#include <graphics/render_targets.hpp>

void Skybox::equirectangularTextureInit( const std::vector<std::string>& params ) {

    PolyStruct sp = createGeomForCube( Vector3f::ZERO, Vector3f{1.0f} );
    std::unique_ptr<VFPos3d[]> vpos3d = Pos3dStrip::vtoVF( sp.verts, sp.numVerts );
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>( sp.numVerts, PRIMITIVE_TRIANGLES,
                                                                           sp.numIndices, vpos3d, sp.indices );

    auto mat = std::make_shared<HeterogeneousMap>();
    mat->assign( UniformNames::colorTexture, params[0] );
    VPBuilder<Pos3dStrip>{rr,mVPList, ShaderMaterial{S::EQUIRECTANGULAR, mat} }.p(colorStrip).n("skybox").build();
}

void Skybox::init( const SkyBoxMode _sbm, const std::string& _textureName ) {
    mVPList = std::make_shared<VPList>();
    mode = _sbm;
    invalidate();
    PolyStruct sp;
    bool bBuildVP = true;
    static bool skyBoxEquilateral = false;

    mCubeMapRender = std::make_unique<CubeEnvironmentMap>(rr, CubeEnvironmentMap::InifinititeSkyBox::True);

    if ( skyBoxEquilateral ) {
        equirectangularTextureInit( {_textureName} );
        return;
    }

    switch ( mode ) {
        case SkyBoxMode::SphereProcedural:
            sp = createGeomForSphere( Vector3f::ZERO, 1.0f, 3 );
            break;
        case SkyBoxMode::CubeProcedural:
            sp = createGeomForCube( Vector3f::ZERO, Vector3f{1.0f} );
            break;
        case SkyBoxMode::EquirectangularTexture:
//            ImageBuilder{_textureName}
//                    .cc(std::bind(&Skybox::equirectangularTextureInit, this, std::placeholders::_1), {_textureName})
//                    .build(rr.RIDM());
            bBuildVP = false;
            skyBoxEquilateral = true;
    }

    if ( bBuildVP ) {
        std::unique_ptr<VFPos3d[]> vpos3d = Pos3dStrip::vtoVF( sp.verts, sp.numVerts );
        std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>( sp.numVerts, PRIMITIVE_TRIANGLES,
                                                                               sp.numIndices, vpos3d, sp.indices );
        VPBuilder<Pos3dStrip>{rr,mVPList, ShaderMaterial{S::SKYBOX}}.p(colorStrip).n("skybox").build();
    }
}

bool Skybox::precalc( float _sunHDRMult ) {
    if ( needsRefresh() ) {

        mSkyboxTexture = rr.TM().addCubemapTexture( TextureRenderData{ "skybox" }
                                                                .setSize( 512 ).format( PIXEL_FORMAT_HDR_RGBA_16 )
                                                                .setGenerateMipMaps( false )
                                                                .setIsFramebufferTarget( true )
                                                                .wm( WRAP_MODE_CLAMP_TO_EDGE ) );

        auto cbfb = FrameBufferBuilder{rr, "ProbeFB"}.size(512).buildSimple();
        auto cubeMapRig = addCubeMapRig( "cubemapRig", Vector3f::ZERO, Rect2f(V2f{512}) );
        auto probe = std::make_shared<RLTargetCubeMap>( cubeMapRig, cbfb, rr );
        probe->render( mSkyboxTexture, 512, 0, [&]() {
            rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestLEqual } );
            rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestFalse } );
            rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeFront } );
            mVPList->setMaterialConstant( UniformNames::sunHRDMult, _sunHDRMult );
            mVPList->addToCommandBuffer( rr );
            rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestLess } );
            rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestTrue } );
            rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeBack } );
        });

        validated();
        return true;
    }
    return false;
}

void Skybox::render() {
    mCubeMapRender->render( mSkyboxTexture );
}

Skybox::Skybox( Renderer& rr, const SkyBoxInitParams& _params ) : RenderModule( rr ) {
    init( _params.mode, _params.assetString + ".hdr" );
}

void CubeEnvironmentMap::init() {
    mVPList = std::make_shared<VPList>();

    auto sp = createGeomForCube( Vector3f::ZERO, Vector3f{1.0f} );

    std::unique_ptr<VFPos3d[]> vpos3d = Pos3dStrip::vtoVF( sp.verts, sp.numVerts );
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>( sp.numVerts, PRIMITIVE_TRIANGLES,
                                                                           sp.numIndices, vpos3d, sp.indices );

    auto shaderName = InfiniteSkyboxMode() ? S::SKYBOX_CUBEMAP : S::PLAIN_CUBEMAP;
    VPBuilder<Pos3dStrip>{rr,mVPList,ShaderMaterial{shaderName}}.p(colorStrip).n("cubeEnvMap-"+shaderName).build();
}

CubeEnvironmentMap::CubeEnvironmentMap( Renderer& rr, CubeEnvironmentMap::InifinititeSkyBox mbInfiniteSkyboxMode ) :
        RenderModule( rr ), mbInfiniteSkyboxMode( mbInfiniteSkyboxMode ) {
    init();
}

void CubeEnvironmentMap::render( std::shared_ptr<Texture> cmt ) {
    if ( InfiniteSkyboxMode() ) {
        rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestLEqual } );
        rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeFront } );
    }
    mVPList->setMaterialConstant( UniformNames::cubeMapTexture, cmt->TDI(0) );
    mVPList->addToCommandBuffer( rr );
    if ( InfiniteSkyboxMode() ) {
        rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestLess } );
        rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeBack } );
    }
}

void ConvolutionEnvironmentMap::init() {
    mVPList = std::make_shared<VPList>();

    auto sp = createGeomForCube( Vector3f::ZERO, Vector3f{1.0f} );

    std::unique_ptr<VFPos3d[]> vpos3d = Pos3dStrip::vtoVF( sp.verts, sp.numVerts );
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>( sp.numVerts, PRIMITIVE_TRIANGLES,
                                                                           sp.numIndices, vpos3d, sp.indices );

    VPBuilder<Pos3dStrip>{rr,mVPList,ShaderMaterial{S::CONVOLUTION}}.p(colorStrip).n("cubeEnvMap").build();
}

void ConvolutionEnvironmentMap::render( std::shared_ptr<Texture> cmt ) {
    rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeFront } );
    mVPList->setMaterialConstant( UniformNames::cubeMapTexture, cmt->TDI(0) );
    mVPList->addToCommandBuffer( rr );
}

ConvolutionEnvironmentMap::ConvolutionEnvironmentMap( Renderer& rr ) : RenderModule( rr ) {
    init();
}

void PrefilterSpecularMap::init() {
    mVPList = std::make_shared<VPList>();

    auto sp = createGeomForCube( Vector3f::ZERO, Vector3f{1.0f} );

    std::unique_ptr<VFPos3d[]> vpos3d = Pos3dStrip::vtoVF( sp.verts, sp.numVerts );
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>( sp.numVerts, PRIMITIVE_TRIANGLES,
                                                                           sp.numIndices, vpos3d, sp.indices );

    VPBuilder<Pos3dStrip>{rr,mVPList,ShaderMaterial{S::IBL_SPECULAR}}.p(colorStrip).n("iblSpecularEnvMap").build();
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
