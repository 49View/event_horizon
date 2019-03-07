#include <utility>


#pragma once

#include <vector>
#include <array>
#include <unordered_set>
#include "framebuffer.h"
#include "core/uuid.hpp"
#include "core/math/matrix4f.h"
#include "core/image_builder.h"
#include "vertex_processing.h"
#include "graphic_constants.h"
#include "shadowmap_manager.h"
#include "render_list.h"
#include "light_manager.h"

class FontManager;
class Renderer;
class CommandScriptRendererManager;
class CommandQueue;
class ShaderManager;
class RenderSceneGraph;
class StreamingMediator;

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
	const static std::string blur_horizontal = "blur_horizontal_b";
	const static std::string blur_vertical = "blur_vertical_b";
	const static std::string colorFinalFrameBuffer = "colorFinalFrameBuffer";
	const static std::string offScreenFinalFrameBuffer = "offScreenFinalFrameBuffer";

	static std::unordered_set<std::string> mFBNames;

	bool isPartOf( const std::string& _val );
}

using CommandBufferLimitsT = int;

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
    void setTiming();
    void setUniforms_r();
private:
    std::unique_ptr<ProgramUniformSet> mAnimUniforms;
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

struct ChangeMaterialOnTagContainer {
	uint64_t tag;
	std::shared_ptr<Material> mat;
};

class Renderer : public DependencyMaker {
public:
	Renderer( CommandQueue& cq, ShaderManager& sm, TextureManager& tm, StreamingMediator& _ssm, LightManager& _lm );
	virtual ~Renderer() = default;

	bool exists( [[maybe_unused]] const std::string& _key ) const override { return false; };
    void cmdReloadShaders( const std::vector<std::string>& _params );

	void init();
	void afterShaderSetup();
	void injectShader( const std::string& _key, const std::string& content );

	void directRenderLoop( std::vector<std::shared_ptr<RLTarget>>& _targets );

	void removeFromCL( const UUID& _uuid );

	std::shared_ptr<RenderMaterial> addMaterial( std::shared_ptr<Material> _material,
												 std::shared_ptr<Program> _program = nullptr );
	std::shared_ptr<RenderMaterial> addMaterial( const std::string& _shaderName );
	void changeMaterialOnTagsCallback( const ChangeMaterialOnTagContainer& _cmt );
    void changeMaterialColorOnTags( uint64_t _tag, const Color4f& _color );
	void changeMaterialColorOnUUID( const UUID& _tag, const Color4f& _color, Color4f& _oldColor );

	std::shared_ptr<Program> P(const std::string& _id);
	std::shared_ptr<Texture> TD( const std::string& _id, const int tSlot = -1 );
	TextureIndex TDI( const std::string& _id, unsigned int tSlot );

	void addToCommandBuffer( CommandBufferLimitsT _entry );
	void addToCommandBuffer( const std::vector<std::shared_ptr<VPList>> _map,
							 std::shared_ptr<RenderMaterial> _forcedMaterial = nullptr);

	void setRenderHook( const std::string& _key, std::weak_ptr<CommandBufferEntry>& _hook );
	void setGlobalTextures();

	std::shared_ptr<ProgramUniformSet>& CameraUBO() { return rcm.UBO(); }
    LightManager&   LM() { return lm; }
	TextureManager& TM() { return tm; }
	RenderImageDependencyMaker& RIDM() { return ridm; }
	StreamingMediator& SSM();

	std::shared_ptr<VPList> VPL( const int _bucket, std::shared_ptr<Matrix4f> m = nullptr, float alpha = 1.0f);

	bool hasTag( uint64_t _tag ) const;

	RenderStats& Stats() { return mStats; }
	void Stats( RenderStats val ) { mStats = val; }

	inline CommandBufferList& CB_U() { return *mCommandBuffers.get(); }
	inline std::map<int, CommandBufferListVector>& CL() { return mCommandLists; }
    inline const std::map<int, CommandBufferListVector>& CL() const { return mCommandLists; }

	void MaterialCache( uint64_t, std::shared_ptr<RenderMaterial> _mat );
	void MaterialMap( std::shared_ptr<RenderMaterial> _mat );
	void resetDefaultFB( const Vector2i& forceSize = Vector2i{-1});

	std::shared_ptr<Framebuffer> getDefaultFB() {
		return mDefaultFB;
	}

	int UpdateCounter() const { return mUpdateCounter; }
	void invalidateOnAdd();

protected:
	void changeMaterialOnTags( ChangeMaterialOnTagContainer& _cmt );

	void clearCommandList();

	void renderCBList();

	void renderCommands( int eye );

protected:
    CommandQueue&   cq;
	ShaderManager&  sm;
	TextureManager& tm;
	RenderImageDependencyMaker ridm;
	StreamingMediator& ssm;
	LightManager&   lm;
	RenderAnimationManager am;
	RenderCameraManager rcm;

	std::shared_ptr<Framebuffer> mDefaultFB;

	std::unordered_map<uint64_t, std::shared_ptr<RenderMaterial>> materialCache;
	std::unordered_map<std::string, std::shared_ptr<RenderMaterial>> materialMap;

	std::shared_ptr<CommandScriptRendererManager> hcs;

	int mUpdateCounter = 0;
	bool bInvalidated = false;

	std::shared_ptr<CommandBufferList> mCommandBuffers;
	std::map<int, CommandBufferListVector> mCommandLists;

	std::vector<ChangeMaterialOnTagContainer> mChangeMaterialCallbacks;

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

	friend class RenderSceneGraph;
	friend struct HierGeomRenderObserver;
};
