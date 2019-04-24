#pragma once

#include <vector>
#include <utility>
#include <array>
#include <unordered_set>
#include <core/uuid.hpp>
#include <core/names.hpp>
#include <core/math/matrix4f.h>
#include <core/heterogeneous_map.hpp>
#include <graphics/framebuffer.h>
#include <graphics/graphic_constants.h>
#include <graphics/shadowmap_manager.h>
#include <core/resources/resource_utils.hpp>

class Renderer;
class RenderMaterial;
class CommandQueue;
class ShaderManager;
class LightManager;
class RenderSceneGraph;
class StreamingMediator;
class RLTarget;
class ProgramUniformSet;
class Program;
class CommandBufferEntry;
class CommandBufferList;
struct CommandBufferListVector;
class ShaderMaterial;
class RenderMaterialManager;
struct cpuVBIB;
class GPUVData;
class GPUVDataManager;

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

	const static std::string shadowmap                  = S::shadowmap                ;
	const static std::string lightmap                   = S::lightmap                 ;
	const static std::string sceneprobe                 = S::sceneprobe               ;
	const static std::string blur_horizontal            = S::blur_horizontal          ;
	const static std::string blur_vertical              = S::blur_vertical            ;
	const static std::string colorFinalFrameBuffer      = S::colorFinalFrameBuffer    ;
	const static std::string offScreenFinalFrameBuffer  = S::offScreenFinalFrameBuffer;

	static std::unordered_set<std::string> mFBNames;

	bool isPartOf( const std::string& _val );
}

using CommandBufferLimitsT = int;

class RenderAnimationManager {
public:
    void init();
	void generateUBO( std::shared_ptr<ShaderManager> sm );
    void setTiming();
    void setUniforms_r();
private:
    std::unique_ptr<ProgramUniformSet> mAnimUniforms;
};

class RenderCameraManager {
public:
	void init();
	void generateUBO( std::shared_ptr<ShaderManager> sm );
	void setUniforms_r();
	std::shared_ptr<ProgramUniformSet>& UBO();
private:
	std::shared_ptr<ProgramUniformSet> mCameraUBO;
};

struct ChangeMaterialOnTagContainer {
	uint64_t tag;
	std::string matHash;
};

class Renderer {
public:
    explicit Renderer( StreamingMediator& _ssm );
	virtual ~Renderer() = default;

    void cmdReloadShaders( const std::vector<std::string>& _params );

	void init();
	void afterShaderSetup();
	void injectShader( const std::string& _key, const std::string& content );

	void directRenderLoop();

	void removeFromCL( const UUID& _uuid );

    std::shared_ptr<Texture> addTextureResource( const ResourceTransfer<RawImage>& _val );
    std::shared_ptr<RenderMaterial> addMaterialResource( const ResourceTransfer<Material>& _val );
    std::shared_ptr<RenderMaterial> addMaterialResource( const ShaderMaterial& _val, const std::string& _name );
    std::shared_ptr<GPUVData> addVDataResource( const ResourceTransfer<VData>& _val );
    std::shared_ptr<GPUVData> addVDataResource( const cpuVBIB& _val, const std::string& _name );

	std::shared_ptr<RenderMaterial> getMaterial( const std::string& _key );
    std::shared_ptr<GPUVData>       getGPUVData( const std::string& _key );

	void changeMaterialOnTagsCallback( const ChangeMaterialOnTagContainer& _cmt );
    void changeMaterialColorOnTags( uint64_t _tag, const Color4f& _color );
	void changeMaterialColorOnUUID( const UUID& _tag, const Color4f& _color, Color4f& _oldColor );

	std::shared_ptr<Program> P(const std::string& _id);
	std::shared_ptr<Texture> TD( const std::string& _id, int tSlot = -1 );
    TextureUniformDesc TDI( const std::string& _id, unsigned int tSlot );

	void addToCommandBuffer( CommandBufferLimitsT _entry );
	void addToCommandBuffer( std::vector<std::shared_ptr<VPList>> _map,
							 std::shared_ptr<RenderMaterial> _forcedMaterial = nullptr);

	void setRenderHook( const std::string& _key, std::weak_ptr<CommandBufferEntry>& _hook );
	void setGlobalTextures();

	std::shared_ptr<ProgramUniformSet>& CameraUBO() { return rcm.UBO(); }
    std::shared_ptr<LightManager>   LM() { return lm; }
	std::shared_ptr<TextureManager> TM() { return tm; }
	std::shared_ptr<ShaderManager>  SM() { return sm; }
	StreamingMediator& SSM();

    void VPL( int _bucket, std::shared_ptr<VPList> nvp, float alpha = 1.0f );

	bool hasTag( uint64_t _tag ) const;

	inline CommandBufferList& CB_U() { return *mCommandBuffers.get(); }
	inline std::map<int, CommandBufferListVector>& CL() { return mCommandLists; }
    inline const std::map<int, CommandBufferListVector>& CL() const { return mCommandLists; }

	void resetDefaultFB( const Vector2i& forceSize = Vector2i{-1});

	void addTarget( std::shared_ptr<RLTarget> _target );
	std::shared_ptr<RLTarget> getTarget( const std::string& _name );
	void clearTargets();

	std::shared_ptr<Framebuffer> getDefaultFB() {
		return mDefaultFB;
	}

	int UpdateCounter() const { return mUpdateCounter; }
	void invalidateOnAdd();

	void changeTime( const V3f& _solarTime );

protected:
	void changeMaterialOnTags( ChangeMaterialOnTagContainer& _cmt );
	void clearCommandList();
	void renderCBList();
	void renderCommands( int eye );

protected:
	std::shared_ptr<ShaderManager>          sm;
	std::shared_ptr<TextureManager>         tm;
	std::shared_ptr<RenderMaterialManager>  rmm;
    std::shared_ptr<LightManager>           lm;
    std::shared_ptr<GPUVDataManager>        gm;
    StreamingMediator& ssm;

	RenderAnimationManager am;
	RenderCameraManager rcm;

	std::shared_ptr<Framebuffer> mDefaultFB;

	int mUpdateCounter = 0;
	bool bInvalidated = false;

	std::vector<std::shared_ptr<RLTarget>> mTargets;
	std::shared_ptr<CommandBufferList> mCommandBuffers;
	std::map<int, CommandBufferListVector> mCommandLists;

	std::vector<ChangeMaterialOnTagContainer> mChangeMaterialCallbacks;

	template <typename V> friend class VPBuilder;

public:

	void drawIncGridLines( int numGridLines, float deltaInc, float gridLinesWidth,
						   const Vector3f& constAxis0, const Vector3f& constAxis1, const Color4f& smallAxisColor,
						   const float zoffset, const std::string& _name = "" );
	void drawArcFilled( int bucketIndex, const Vector3f& center, float radius, float fromAngle, float toAngle,
						const Vector4f& color, float width, int32_t subdivs, const std::string& _name = "" );

	void createGrid( float unit, const Color4f& mainAxisColor, const Color4f& smallAxisColor,
					 const Vector2f& limits, const float axisSize, const float zoffset = 0.0f, bool _monochrome = false,
					 const std::string& _name = "" );
	void
	drawArrow( int bucketIndex, const Vector2f& p1, const Vector2f& p2, const Vector4f& color, float width,
			   float angle, float arrowlength, float _z, float percToBeDrawn, const std::string& _name1,
			   const std::string& _name2 );

	void drawLine( int bucketIndex, const Vector3f& p1, const Vector3f& p2, const Vector4f& color, float width,
				   bool scaleEnds = true, float rotAngle = 0.0f, float percToBeDrawn = 1.0f,
				   const std::string& _name = "" );
	void
	drawLine( int bucketIndex, const std::vector<Vector2f>& verts, float z, const Vector4f& color, float width,
			  bool scaleEnds = true, float rotAngle = 0.0f, float percToBeDrawn = 1.0f, const std::string& _name = "" );
	void drawLine( int bucketIndex, const std::vector<Vector3f>& verts, const Vector4f& color, float width,
				   bool scaleEnds = true, float rotAngle = 0.0f, float percToBeDrawn = 1.0f,
				   const std::string& _name = "" );
	void drawTriangle( int bucketIndex, const std::vector<Vector2f>& verts, float _z, const Vector4f& color,
					   const std::string& _name = "" );
	void drawTriangle( int bucketIndex, const std::vector<Vector3f>& verts, const Vector4f& color,
					   const std::string& _name = "" );
	void drawTriangles(int bucketIndex, const std::vector<Vector3f>& verts, const Vector4f& color,
					   const std::string& _name="");
	void drawTriangles(int bucketIndex, const std::vector<Vector3f>& verts, const std::vector<int32_t>& indices,
					   const Vector4f& color, const std::string& _name="");
	void
	draw3dVector( int bucketIndex, const Vector3f& pos, const Vector3f& dir, const Vector4f& color, float size,
				  const std::string& _name = "" );
	void drawCircle( int bucketIndex, const Vector3f& center, float radius, const Color4f& color,
					 int32_t subdivs = 10, const std::string& _name = "" );
	void drawCircle( int bucketIndex, const Vector3f& center, const Vector3f& normal, float radius,
					 const Color4f& color, int32_t subdivs, const std::string& _name = "" );
	void drawCircle2d( int bucketIndex, const Vector2f& center, float radius, const Color4f& color,
					   int32_t subdivs = 10, const std::string& _name = "" );
	void drawArc( int bucketIndex, const Vector3f& center, float radius, float fromAngle, float toAngle,
				  const Vector4f& color, float width, int32_t subdivs = 10, float percToBeDrawn = 1.0f,
				  const std::string& _name = "" );
	void drawArc( int bucketIndex, const Vector3f& center, const Vector3f& p1, const Vector3f& p2,
				  const Vector4f& color, float width, int32_t subdivs = 10, float percToBeDrawn = 1.0f,
				  const std::string& _name = "" );
	void
	drawCylinder( int bucketIndex, const Vector3f& pos, const Vector3f& dir, const Vector4f& color, float size,
				  const std::string& _name = "" );

	void drawCone( int bucketIndex, const Vector3f& posBase, const Vector3f& posTop, const Vector4f& color,
				   float size, const std::string& _name = "" );

	friend class RenderSceneGraph;
	friend struct HierGeomRenderObserver;
};
