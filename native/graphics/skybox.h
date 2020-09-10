#pragma once

#include <memory>
#include <string>
#include <vector>
#include <core/camera_utils.hpp>
#include <core/math/anim_type.hpp>
#include <core/frame_invalidator.hpp>

class VPList;
class Texture;
class Framebuffer;
class Renderer;
class RenderImageDependencyMaker;
class RLTarget;

class RenderModule {
public:
	explicit RenderModule( Renderer& rr ) : rr( rr ) {}
protected:
	Renderer& rr;
	std::shared_ptr<VPList> mVPList;
};

enum class SkyBoxMode {
	SphereProcedural,
	CubeProcedural,
	EquirectangularTexture
};

struct SkyBoxInitParams {
	SkyBoxMode mode = SkyBoxMode::CubeProcedural;
	std::string assetString = "barcelona_rooftop_env";
};

class CubeEnvironmentMap : public RenderModule {
public:
	enum class InfiniteSkyBox {
		True,
		False
	};
	using RenderModule::RenderModule;
	explicit CubeEnvironmentMap( Renderer& rr, InfiniteSkyBox mbInfiniteSkyboxMode = InfiniteSkyBox::False );

	void render( std::shared_ptr<Texture> cmt );
	[[nodiscard]] bool InfiniteSkyboxMode() const {
		return mbInfiniteSkyboxMode == InfiniteSkyBox::True;
	}
	void InfiniteSkyboxMode( bool _infiniteSkyboxMode ) {
		mbInfiniteSkyboxMode = _infiniteSkyboxMode ? InfiniteSkyBox::True : InfiniteSkyBox::False;
	}
    floata& DeltaInterpolation() {
	    return mDeltaInterpolation;
	}

private:
	void init();
	InfiniteSkyBox mbInfiniteSkyboxMode = InfiniteSkyBox::False;
	floata mDeltaInterpolation;
};

class Skybox : public RenderModule, public FrameInvalidator {
public:
	using RenderModule::RenderModule;

	Skybox( Renderer& rr, const SkyBoxInitParams& mode );

	void render();
    bool preCalc( float _sunHDRMult, const Vector3f& _cubeMapCenter);

    std::shared_ptr<Texture> getSkyboxTexture() { return mSkyboxTexture; }

    floata& DeltaInterpolation() { return mCubeMapRender->DeltaInterpolation(); }
private:
	void init( SkyBoxMode _sbm, const std::string& _textureName = "" );
	void equirectangularTextureInit( const std::vector<std::string>& params );
private:
	SkyBoxMode mode = SkyBoxMode::CubeProcedural;
    std::shared_ptr<VPList> mVPListInfiniteIshPlane;
	std::unique_ptr<CubeEnvironmentMap> mCubeMapRender;
	std::shared_ptr<Texture> mSkyboxTexture;
    CubeMapRigContainer cubeMapRig;
    static int sSkyboxCubemapSize;
};

class ConvolutionEnvironmentMap : public RenderModule {
public:
	using RenderModule::RenderModule;
	explicit ConvolutionEnvironmentMap( Renderer& rr );
	void render( std::shared_ptr<Texture> cmt );
private:
	void init();
};

class PrefilterSpecularMap : public RenderModule {
public:
	using RenderModule::RenderModule;
	explicit PrefilterSpecularMap( Renderer& rr );
    void render( std::shared_ptr<Texture> cmt, float roughness );
private:
	void init();
};

class [[maybe_unused]] PrefilterBRDF : public RenderModule {
public:
	using RenderModule::RenderModule;
	void render();
    [[maybe_unused]] explicit PrefilterBRDF( Renderer& rr );
private:
	void init();
};
