#pragma once

#include <memory>
#include <string>
#include <vector>
#include <core/util.h>

class VPList;
class Texture;
class Framebuffer;
class Renderer;
class RenderImageDependencyMaker;
class RLTarget;

class RenderModule {
public:
	RenderModule( Renderer& rr ) : rr( rr ) {}
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

class Skybox : public RenderModule, public FrameInvalidator {
public:
	using RenderModule::RenderModule;

	Skybox( Renderer& rr, const SkyBoxInitParams& mode );

	void render(float _sunHDRMult);
private:
	void init( const SkyBoxMode _sbm, const std::string& _textureName = "" );
    void equirectangularTextureInit( const std::vector<std::string>& params );
private:
	SkyBoxMode mode = SkyBoxMode::CubeProcedural;
	bool isReadyToRender = false;
};

class CubeEnvironmentMap : public RenderModule {
public:
	using RenderModule::RenderModule;
	void init();
	void render( std::shared_ptr<Texture> cmt );
};

class PrefilterSpecularMap : public RenderModule {
public:
	using RenderModule::RenderModule;
    void init();
    void render( std::shared_ptr<Texture> cmt, const float roughness );
};

class PrefilterBRDF : public RenderModule {
public:
	using RenderModule::RenderModule;
	void init();
	void render();
private:
	std::shared_ptr<Framebuffer> mBRDF;
};
