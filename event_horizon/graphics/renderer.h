
#pragma once

#include <vector>
#include <array>
#include <unordered_set>
#include "framebuffer.h"
#include "core/math/matrix4f.h"
#include "core/image_builder.h"
#include "core/game_time.h"
#include "core/callback_dependency.h"
#include "vertex_processing.h"
#include "skybox.h"
#include "graphic_constants.h"
#include "shader_manager.h"
#include "camera_manager.h"
#include "shadowmap_manager.h"
#include "render_list.h"
#include "light_manager.h"
#include "di_modules.h"

class FontManager;
class CameraManager;
class Renderer;

namespace CommandBufferLimits {
	const static int CoreStart = 0;
	const static int CoreGrid = 1;
	const static int CoreEnd = 999;
	const static int UIStart = 1000;
	const static int UIEnd = 9999;
	const static int PBRStart = 10000;
	const static int PBREnd = 99999999;
}

namespace FBNames {

	const static std::string shadowmap = "shadowMap_d";
	const static std::string lightmap = "lightMap_t";
	const static std::string sceneprobe = "sceneprobe";
	const static std::string convolution = "convolution";
	const static std::string specular_prefilter = "specular_prefilter";
	const static std::string ibl_brdf = "ibl_brdf";
	const static std::string blur_horizontal = "blur_horizontal_b";
	const static std::string blur_vertical = "blur_vertical_b";
	const static std::string colorFinalFrameBuffer = "colorFinalFrameBuffer";
	const static std::string offScreenFinalFrameBuffer = "offScreenFinalFrameBuffer";

	static std::unordered_set<std::string> mFBNames;

	bool isPartOf( const std::string& _val );
}

using CommandBufferLimitsT = int;

class CommandScriptRendererManager : public CommandScript {
public:
	CommandScriptRendererManager( Renderer& hm );
	virtual ~CommandScriptRendererManager() {}
};

struct ShaderAssetBuilder;

struct RBUILDER( ShaderAssetBuilder, shaders, shd, Binary, BuilderQueryType::Exact )
};

class RenderImageDependencyMaker : public ImageDepencencyMaker {
public:
	RenderImageDependencyMaker( TextureManager& tm ) : tm( tm ) {}
	bool addImpl( ImageBuilder& tbd, std::unique_ptr<uint8_t []>& _data ) override;
	TextureManager& tm;
};

class RenderAnimationManager {
public:
    void init();
	void generateUBO( const ShaderManager& sm );
    void setTiming( const GameTime& _gt );
    void setUniforms_r();
private:
    std::unique_ptr<ProgramUniformSet> mAnimUniforms;
    GameTime gt;
};

class RenderCameraManager {
public:
	void init();
	void generateUBO( const ShaderManager& sm );
	void setUniforms_r();
	std::shared_ptr<ProgramUniformSet>& UBO();
private:
	std::shared_ptr<ProgramUniformSet> mCameraUBO;
};


class Renderer : public DependencyMaker {
public:
	Renderer( CommandQueue& cq, ShaderManager& sm, FontManager& fm, TextureManager& tm, CameraManager& _cm );
	virtual ~Renderer() = default;

	bool exists( [[maybe_unused]] const std::string& _key ) const override { return false; };
    void cmdReloadShaders( const std::vector<std::string>& _params );

	void init();
	void afterShaderSetup();
	bool isInitialized() const;

	template<typename T>
	std::shared_ptr<CameraRig> addTarget( const std::string& _name, const Rect2f& _viewport,
										  BlitType _bt, CameraManager& _cm ) {
		auto rig = _cm.getRig( _name );
		if ( !rig ) {
			rig = _cm.addRig( _name, _viewport );
			mTargets.push_back( std::make_shared<T>(T{ rig, _viewport, _bt, *this }) );
		}
		return rig;
	}

	std::shared_ptr<RLTarget> getTarget( const std::string& _name );

	void clearTargets();
	void directRenderLoop( const GameTime& gt );

    void changeMaterialOnTags( uint64_t _tag, std::shared_ptr<PBRMaterial> _mat );
    void changeMaterialColorOnTags( uint64_t _tag, const Color4f& _color );

	std::shared_ptr<Program> P(const std::string& _id);
	std::shared_ptr<Texture> TD( const std::string& _id, const int tSlot = -1 );

	void addToCommandBuffer( const CommandBufferLimitsT _entry );
	void addToCommandBuffer( const std::vector<std::shared_ptr<VPList>> _map,
							 std::shared_ptr<RenderMaterial> _forcedMaterial = nullptr);

	void setRenderHook( const std::string& _key, std::weak_ptr<CommandBufferEntry>& _hook );
	void setGlobalTextures();

	std::shared_ptr<ProgramUniformSet>& CameraUBO() { return rcm.UBO(); }
    LightManager&   LM() { return lm; }
    FontManager&    FM() { return fm; }
	TextureManager& TM() { return tm; }
	CameraManager&  CM() { return cm; }
	RenderImageDependencyMaker& RIDM() { return ridm; }

	std::shared_ptr<VPList> VPL( const int _bucket, const std::string& _key = "",
								 std::shared_ptr<Matrix4f> m = nullptr, float alpha = 1.0f);

	bool hasTag( uint64_t _tag ) const;

	RenderStats& Stats() { return mStats; }
	void Stats( RenderStats val ) { mStats = val; }
	std::mutex& CBVPAddMutex() { return mCBVPAddMutex; }
	std::mutex& CBTextureAddMutex() { return mCBTextureAddMutex; }
	const std::vector<CommandBufferNewEntry>& VPToAddList() { return mVPToAdd; }

	void clearVPAddList();

	inline CommandBufferList& CB_U() { return *mCommandBuffers.get(); }
	inline CommandBufferList& CB_R() { return *mCommandBuffers.get(); }
	inline std::map<int, CommandBufferListVector>& CL() { return mCommandLists; }
    inline const std::map<int, CommandBufferListVector>& CL() const { return mCommandLists; }

	bool isUseMultiThreadRendering() const {
		return useMultiThreadRendering;
	}

	void setUseMultiThreadRendering( bool _useMultiThreadRendering ) {
		useMultiThreadRendering = _useMultiThreadRendering;
	}

	void MaterialCache( const MaterialType& mt, std::shared_ptr<RenderMaterial> _mat );
	void MaterialMap( std::shared_ptr<RenderMaterial> _mat );
	void changeTime( const V3f& _solarTime );
	void resetDefaultFB();

	std::shared_ptr<Framebuffer> getDefaultFB() {
		return mDefaultFB;
	}

	int UpdateCounter() const { return mUpdateCounter; }
	void invalidateOnAdd();

protected:
	void postInit();

	void clearCommandList();

	void renderCBList();

	void renderCommands( int eye );

protected:
	LightManager    lm;
    CommandQueue&   cq;
	ShaderManager&  sm;
    FontManager&    fm;
	TextureManager& tm;
	CameraManager&  cm;
	RenderImageDependencyMaker ridm;
	RenderAnimationManager am;
	RenderCameraManager rcm;

	std::shared_ptr<Framebuffer> mDefaultFB;

	std::unordered_map<MaterialType, std::shared_ptr<RenderMaterial>> materialCache;
	std::unordered_map<int64_t, std::shared_ptr<RenderMaterial>> materialMap;

	std::shared_ptr<CommandScriptRendererManager> hcs;

	int mUpdateCounter = 0;

	constexpr static int sBufferMax = 2;
	std::mutex mCommandBuffersMutex[sBufferMax];
	std::shared_ptr<CommandBufferList> mCommandBuffers;
	std::vector<CommandBufferNewEntry> mVPToAdd;
	std::mutex mCBVPAddMutex;
	std::mutex mCBTextureAddMutex;
	bool useMultiThreadRendering;

	std::vector<std::shared_ptr<RLTarget>> mTargets;
	std::map<int, CommandBufferListVector> mCommandLists;

	bool mbIsInitialized = false;

	RenderStats mStats;

	template <typename V> friend class VPBuilder;

public:

	void drawIncGridLines( int numGridLines, float deltaInc, float gridLinesWidth,
						   const Vector3f& constAxis0, const Vector3f& constAxis1, const Color4f& smallAxisColor,
						   const float zoffset, const std::string& _name = "" );
	void drawArcFilled( std::shared_ptr<VPList> _vpl, const Vector3f& center, float radius, float fromAngle, float toAngle,
						const Vector4f& color, float width, int32_t subdivs, const std::string& _name = "" );

	void createGrid( float unit, const Color4f& mainAxisColor, const Color4f& smallAxisColor,
					 const Vector2f& limits, const float axisSize, const float zoffset = 0.0f, bool _monochrome = false,
					 const std::string& _name = "" );
	void
	drawArrow( std::shared_ptr<VPList> _vpl, const Vector2f& p1, const Vector2f& p2, const Vector4f& color, float width,
			   float angle, float arrowlength, float _z, float percToBeDrawn, const std::string& _name1,
			   const std::string& _name2 );

	void drawLine( std::shared_ptr<VPList> _vpl, const Vector3f& p1, const Vector3f& p2, const Vector4f& color, float width,
				   bool scaleEnds = true, float rotAngle = 0.0f, float percToBeDrawn = 1.0f,
				   const std::string& _name = "" );
	void
	drawLine( std::shared_ptr<VPList> _vpl, const std::vector<Vector2f>& verts, float z, const Vector4f& color, float width,
			  bool scaleEnds = true, float rotAngle = 0.0f, float percToBeDrawn = 1.0f, const std::string& _name = "" );
	void drawLine( std::shared_ptr<VPList> _vpl, const std::vector<Vector3f>& verts, const Vector4f& color, float width,
				   bool scaleEnds = true, float rotAngle = 0.0f, float percToBeDrawn = 1.0f,
				   const std::string& _name = "" );
	void drawTriangle( std::shared_ptr<VPList> _vpl, const std::vector<Vector2f>& verts, float _z, const Vector4f& color,
					   const std::string& _name = "" );
	void drawTriangle( std::shared_ptr<VPList> _vpl, const std::vector<Vector3f>& verts, const Vector4f& color,
					   const std::string& _name = "" );
	void drawTriangles(std::shared_ptr<VPList> _vpl, const std::vector<Vector3f>& verts, const Vector4f& color,
					   const std::string& _name="");
	void drawTriangles(std::shared_ptr<VPList> _vpl, const std::vector<Vector3f>& verts, const std::vector<int32_t>& indices,
					   const Vector4f& color, const std::string& _name="");
	void
	draw3dVector( std::shared_ptr<VPList> _vpl, const Vector3f& pos, const Vector3f& dir, const Vector4f& color, float size,
				  const std::string& _name = "" );
	void drawCircle( std::shared_ptr<VPList> _vpl, const Vector3f& center, float radius, const Color4f& color,
					 int32_t subdivs = 10, const std::string& _name = "" );
	void drawCircle( std::shared_ptr<VPList> _vpl, const Vector3f& center, const Vector3f& normal, float radius,
					 const Color4f& color, int32_t subdivs, const std::string& _name = "" );
	void drawCircle2d( std::shared_ptr<VPList> _vpl, const Vector2f& center, float radius, const Color4f& color,
					   int32_t subdivs = 10, const std::string& _name = "" );
	void drawArc( std::shared_ptr<VPList> _vpl, const Vector3f& center, float radius, float fromAngle, float toAngle,
				  const Vector4f& color, float width, int32_t subdivs = 10, float percToBeDrawn = 1.0f,
				  const std::string& _name = "" );
	void drawArc( std::shared_ptr<VPList> _vpl, const Vector3f& center, const Vector3f& p1, const Vector3f& p2,
				  const Vector4f& color, float width, int32_t subdivs = 10, float percToBeDrawn = 1.0f,
				  const std::string& _name = "" );
	void
	drawCylinder( std::shared_ptr<VPList> _vpl, const Vector3f& pos, const Vector3f& dir, const Vector4f& color, float size,
				  const std::string& _name = "" );

	void drawCone( std::shared_ptr<VPList> _vpl, const Vector3f& posBase, const Vector3f& posTop, const Vector4f& color,
				   float size, const std::string& _name = "" );

	void drawRect2d( CommandListType clt, const std::string& tname, const JMATH::Rect2f& rect, int zLevel = 0,
					 const std::string& _vname = staticvpss, const std::string& _name = "" );

	friend struct ShaderAssetBuilder;
	friend class RenderSceneGraph;
	friend struct HierGeomRenderObserver;
};

template <typename V>
class VPBuilder {
public:
	VPBuilder(Renderer& _rr) : rr(_rr) {}

	VPBuilder& vl( std::shared_ptr<VPList> _vpl ) { vpl = _vpl; return *this; }
	VPBuilder& c( const Color4f& _matColor ) { matColor = _matColor; return *this; }
	VPBuilder& a( float _matAlpha ) { matAlpha = _matAlpha; return *this; }
	VPBuilder& p( std::shared_ptr<V> _ps ) { ps = _ps; return *this; }
	VPBuilder& n( const std::string& _name ) { name = _name; return *this; }
	VPBuilder& s( const std::string& _shader ) { matShader = _shader; return *this; }
	VPBuilder& m( const std::string& _material ) { matName = _material; return *this; }
	VPBuilder& m( std::shared_ptr<Material> _material ) { material = _material; return *this; }
    VPBuilder& t( const std::string& _tex ) { matTexture = _tex; return *this; }
	VPBuilder& g( const uint64_t _tag) { tag = _tag; return *this; }

	void build();

private:
	Renderer& rr;
	std::shared_ptr<VPList> vpl;
	Color4f matColor = Color4f::WHITE;
	float   matAlpha = -1.0f;
	uint64_t tag = GT_Generic;
	std::shared_ptr<V> ps;
	std::shared_ptr<Material> material;
	std::string matName;
    std::string matTexture;
	std::string matShader = S::SH;
	std::string name;
};

template<typename V>
void VPBuilder<V>::build() {
	rr.invalidateOnAdd();
	auto rmb = RenderMaterialBuilder{rr};
	if ( material ) {
		rmb.m(material);
	} else {
		rmb.p(matShader).c(matColor).m(matName).t(matTexture);
	}
	auto rmaterial = rmb.build();
	auto sid = name.empty() ? VertexProcessing::totalCountS() : name;
	std::shared_ptr<cpuVBIB> vbib = VertexProcessing::create_cpuVBIB( ps, rmaterial, sid );
	vpl->create( vbib, tag );
}
