#include "skybox.h"

#include <core/math/poly_shapes.hpp>
#include <core/camera_utils.hpp>
#include <graphics/vertex_processing.h>
#include <graphics/framebuffer.h>
#include <graphics/renderer.h>
#include <graphics/vp_builder.hpp>
#include <graphics/render_targets.hpp>

void Skybox::equirectangularTextureInit( const std::vector<std::string>& params ) {

    PolyStruct sp = createGeomForCube(Vector3f::ZERO, Vector3f{ 1.0f });
    std::unique_ptr<VFPos3d[]> vpos3d = Pos3dStrip::vtoVF(sp.verts, sp.numVerts);
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>(sp.numVerts, PRIMITIVE_TRIANGLES,
                                                                          sp.numIndices, vpos3d, std::move(sp.indices));

}

void Skybox::init( const SkyBoxMode _sbm, const std::string& _textureName ) {
    mode = _sbm;
    invalidate();
    PolyStruct sp;

    mCubeMapRender = std::make_unique<CubeEnvironmentMap>(rr, CubeEnvironmentMap::InifinititeSkyBox::True);

    auto mat = std::make_shared<HeterogeneousMap>();
    switch ( mode ) {
        case SkyBoxMode::SphereProcedural:
            sp = createGeomForSphere(Vector3f::ZERO, 1.0f, 3);
            break;
        case SkyBoxMode::CubeProcedural:
            sp = createGeomForCube(Vector3f::ZERO, Vector3f{ 1.0f });
            break;
        case SkyBoxMode::EquirectangularTexture:
            sp = createGeomForCube(Vector3f::ZERO, Vector3f{ 1.0f });
            mat->assign(UniformNames::colorTexture, _textureName);
    }

    std::unique_ptr<VFPos3d[]> vpos3d = Pos3dStrip::vtoVF(sp.verts, sp.numVerts);
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>(sp.numVerts, PRIMITIVE_TRIANGLES,
                                                                          sp.numIndices, vpos3d, std::move(sp.indices));


    ShaderMaterial sm = mode == SkyBoxMode::EquirectangularTexture ?
                        ShaderMaterial{ S::EQUIRECTANGULAR, mat } :
                        ShaderMaterial{ S::SKYBOX };

    std::string skyBoxName = "skybox";
    mVPList = VPBuilder<Pos3dStrip>{ rr, sm, skyBoxName }.p(colorStrip).n(skyBoxName).build();

    auto trd = ImageParams{}.setSize(512).format(PIXEL_FORMAT_HDR_RGBA_16).setWrapMode(WRAP_MODE_CLAMP_TO_EDGE);
    mSkyboxTexture = rr.TM()->addCubemapTexture(TextureRenderData{ skyBoxName, trd }
                                                        .setGenerateMipMaps(false)
                                                        .setIsFramebufferTarget(true));
    cubeMapRig = addCubeMapRig("cubemapRig", V3f::ZERO, Rect2f(V2f{ 512 }));

}

bool Skybox::precalc( float _sunHDRMult ) {
    if ( invalidated() ) {
        auto probe = std::make_shared<RLTargetCubeMap>(cubeMapRig, rr.getProbing(512), rr);
        probe->render(mSkyboxTexture, 512, 0, [&]() {
            mVPList->setMaterialConstant(UniformNames::sunHRDMult, _sunHDRMult);
            rr.CB_U().pushCommand({ CommandBufferCommandName::depthTestLEqual });
            rr.CB_U().pushCommand({ CommandBufferCommandName::cullModeFront });
            rr.CB_U().pushVP(mVPList);
            rr.CB_U().pushCommand({ CommandBufferCommandName::cullModeBack });
            rr.CB_U().pushCommand({ CommandBufferCommandName::depthTestLess });
        });
        validated();
        return true;
    }
    return false;
}

void Skybox::render() {
    mCubeMapRender->render(mSkyboxTexture);
}

Skybox::Skybox( Renderer& rr, const SkyBoxInitParams& _params ) : RenderModule(rr) {
    init(_params.mode, _params.assetString);
}

void CubeEnvironmentMap::init() {
    auto sp = createGeomForCube(Vector3f::ZERO, Vector3f{ 1.0f });
    mDeltaInterpolation = std::make_shared<AnimType<float>>(1.0f, "deltaInterpolation");

    std::unique_ptr<VFPos3d[]> vpos3d = Pos3dStrip::vtoVF(sp.verts, sp.numVerts);
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>(sp.numVerts, PRIMITIVE_TRIANGLES,
                                                                          sp.numIndices, vpos3d, std::move(sp.indices));

    auto shaderName = InfiniteSkyboxMode() ? S::SKYBOX_CUBEMAP : S::PLAIN_CUBEMAP;
    std::string cubeEnvName = "cubeEnvMap-" + shaderName;
    mVPList = VPBuilder<Pos3dStrip>{ rr, ShaderMaterial{ shaderName }, cubeEnvName }.p(colorStrip).n(
            cubeEnvName).build();
}

CubeEnvironmentMap::CubeEnvironmentMap( Renderer& rr, CubeEnvironmentMap::InifinititeSkyBox mbInfiniteSkyboxMode ) :
        RenderModule(rr), mbInfiniteSkyboxMode(mbInfiniteSkyboxMode) {
    init();
}

void CubeEnvironmentMap::render( std::shared_ptr<Texture> cmt ) {
    if ( InfiniteSkyboxMode() ) {
//        rr.CB_U().pushCommand( { CommandBufferCommandName::depthWriteFalse } );
//        rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestLEqual } );
        rr.CB_U().pushCommand({ CommandBufferCommandName::depthWriteTrue });
        rr.CB_U().pushCommand({ CommandBufferCommandName::depthTestFalse });
        rr.CB_U().pushCommand({ CommandBufferCommandName::cullModeFront });
    }
    mVPList->setMaterialConstant(UniformNames::cubeMapTexture, cmt->TDI(0));
    mVPList->setMaterialConstant("delta", mDeltaInterpolation->value);
    rr.CB_U().pushVP(mVPList);
    if ( InfiniteSkyboxMode() ) {
//        rr.CB_U().pushCommand( { CommandBufferCommandName::depthWriteTrue } );
//        rr.CB_U().pushCommand( { CommandBufferCommandName::depthTestLess } );
        rr.CB_U().pushCommand({ CommandBufferCommandName::depthTestTrue });
        rr.CB_U().pushCommand({ CommandBufferCommandName::cullModeBack });
    }
}

void ConvolutionEnvironmentMap::init() {
    auto sp = createGeomForCube(Vector3f::ZERO, Vector3f{ 1.0f });

    std::unique_ptr<VFPos3d[]> vpos3d = Pos3dStrip::vtoVF(sp.verts, sp.numVerts);
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>(sp.numVerts, PRIMITIVE_TRIANGLES,
                                                                          sp.numIndices, vpos3d, std::move(sp.indices));

    std::string cubeName = "cubeEnvMap";
    mVPList = VPBuilder<Pos3dStrip>{ rr, ShaderMaterial{ S::CONVOLUTION }, cubeName }.p(colorStrip).n(cubeName).build();
}

void ConvolutionEnvironmentMap::render( std::shared_ptr<Texture> cmt ) {
    rr.CB_U().pushCommand({ CommandBufferCommandName::cullModeNone });
    rr.CB_U().pushCommand({ CommandBufferCommandName::depthTestFalse });
    rr.CB_U().pushCommand({ CommandBufferCommandName::depthWriteFalse });
    mVPList->setMaterialConstant(UniformNames::cubeMapTexture, cmt->TDI(0));
    rr.CB_U().pushVP(mVPList);
}

ConvolutionEnvironmentMap::ConvolutionEnvironmentMap( Renderer& rr ) : RenderModule(rr) {
    init();
}

void PrefilterSpecularMap::init() {
    auto sp = createGeomForCube(Vector3f::ZERO, Vector3f{ 1.0f });

    std::unique_ptr<VFPos3d[]> vpos3d = Pos3dStrip::vtoVF(sp.verts, sp.numVerts);
    std::shared_ptr<Pos3dStrip> colorStrip = std::make_shared<Pos3dStrip>(sp.numVerts, PRIMITIVE_TRIANGLES,
                                                                          sp.numIndices, vpos3d, std::move(sp.indices));

    std::string IBLName = "iblSpecularEnvMap";
    mVPList = VPBuilder<Pos3dStrip>{ rr, ShaderMaterial{ S::IBL_SPECULAR }, IBLName }.p(colorStrip).n(IBLName).build();
}

void PrefilterSpecularMap::render( std::shared_ptr<Texture> cmt, const float roughness ) {
//    rr.CB_U().startList( nullptr, CommandBufferFlags::CBF_DoNotSort );
//    if ( roughness > 1 ) return;
//    rr.CB_U().pushCommand( { CommandBufferCommandName::cullModeFront } );
    mVPList->setMaterialConstant(UniformNames::cubeMapTexture, cmt->TDI(0));
    mVPList->setMaterialConstant(UniformNames::roughness, roughness);
    rr.CB_U().pushVP(mVPList);
}

PrefilterSpecularMap::PrefilterSpecularMap( Renderer& rr ) : RenderModule(rr) {
    init();
}

void PrefilterBRDF::init() {
}

void PrefilterBRDF::render() {
    rr.CB_U().startTarget(rr.BRDFTarget(), rr);
    rr.CB_U().pushVP(rr.BRDFTarget()->VP());
}

PrefilterBRDF::PrefilterBRDF( Renderer& rr ) : RenderModule(rr) {
    init();
}
