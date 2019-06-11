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
#include <graphics/ghtypes.hpp>
#include <graphics/shadowmap_manager.h>
#include <core/resources/resource_utils.hpp>

namespace CommandBufferLimits {
	const static int CoreStart = 0;
	const static int CoreGrid = 1;
	const static int CoreEnd = 999;
    const static int UnsortedStart = 1000;
    const static int UnsortedEnd = 1999;
	const static int UIStart = 2000;
	const static int UIEnd = 9999;
	const static int PBRStart = 10000;
	const static int PBREnd = 99999999;
    const static int UI2dStart = 100000000;
    const static int UI2dEnd = 100090000;
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

struct scene_t;

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
    void clearBucket( int _bucket );

    std::shared_ptr<Texture> addTextureResource( const ResourceTransfer<RawImage>& _val );
    std::shared_ptr<RenderMaterial> addMaterialResource( const ResourceTransfer<Material>& _val );
    std::shared_ptr<RenderMaterial> addMaterialResource( const ShaderMaterial& _val, const std::string& _name );
    std::shared_ptr<GPUVData> addVDataResource( const ResourceTransfer<VData>& _val );
    std::shared_ptr<GPUVData> addVDataResource( cpuVBIB&& _val, const std::string& _name );

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
							 std::shared_ptr<RenderMaterial> _forcedMaterial = nullptr,
                             Program* _forceProgram = nullptr,
                             float _alphaDrawThreshold = 0.0f );

	void setRenderHook( const std::string& _key, std::weak_ptr<CommandBufferEntry>& _hook );
	void setGlobalTextures();

	std::shared_ptr<ProgramUniformSet>& CameraUBO() { return rcm.UBO(); }
    std::shared_ptr<LightManager>   LM() { return lm; }
	std::shared_ptr<TextureManager> TM() { return tm; }
	std::shared_ptr<ShaderManager>  SM() { return sm; }
    std::shared_ptr<Framebuffer> BRDFTarget() { return mBRDF; };
	StreamingMediator& SSM();

    void VPL( int _bucket, std::shared_ptr<VPList> nvp, float alpha = 1.0f );

	bool hasTag( uint64_t _tag ) const;

	inline CommandBufferList& CB_U() { return *mCommandBuffers; }
	inline std::map<int, CommandBufferListVector>& CL() { return mCommandLists; }
    inline const std::map<int, CommandBufferListVector>& CL() const { return mCommandLists; }

	void resetDefaultFB( const Vector2i& forceSize = Vector2i{-1});

	void addTarget( std::shared_ptr<RLTarget> _target );
	std::shared_ptr<RLTarget> getTarget( const std::string& _name );
	void clearTargets();

	std::shared_ptr<Framebuffer> getDefaultFB() { return mDefaultFB; }
    std::shared_ptr<Framebuffer> getShadowMapFB() { return mShadowMapFB; }
    std::shared_ptr<Framebuffer> getProbing(int _index);

	int UpdateCounter() const { return mUpdateCounter; }
	void invalidateOnAdd();

    void changeMaterialOnTags( const ChangeMaterialOnTagContainer& _cmt );
    void remapLightmapUVs( const scene_t& scene );

protected:
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
    std::shared_ptr<Framebuffer> mBRDF;
    std::shared_ptr<Framebuffer> mShadowMapFB;
    std::unordered_map<int, std::shared_ptr<Framebuffer>> mProbingsFB;

	int mUpdateCounter = 0;
	bool bInvalidated = false;

	std::vector<std::shared_ptr<RLTarget>> mTargets;
	std::shared_ptr<CommandBufferList> mCommandBuffers;
	std::map<int, CommandBufferListVector> mCommandLists;
	std::unordered_map<UUID, VPListSP> mVPLMap;

	std::vector<ChangeMaterialOnTagContainer> mChangeMaterialCallbacks;

	template <typename V> friend class VPBuilder;

public:
    auto addVertexStrips( int bucketIndex, const V3fVector& v1, const V4f& color,
                          const Matrix4f& mat = Matrix4f::IDENTITY, const std::string& _name = "");
	void drawIncGridLines( int bucketIndex, int numGridLines, float deltaInc, float gridLinesWidth,
						   const Vector3f& constAxis0, const Vector3f& constAxis1, const Color4f& smallAxisColor,
						   float zoffset, const std::string& _name = "" );
    VPListSP drawArcFilled( int bucketIndex, const Vector3f& center, float radius, float fromAngle, float toAngle,
						const Vector4f& color, float width, int32_t subdivs, const std::string& _name = "" );

	void createGrid( int bucketIndex, float unit, const Color4f& mainAxisColor, const Color4f& smallAxisColor,
					 const Vector2f& limits, float axisSize, const std::string& _name = "" );
    std::vector<VPListSP> createGridV2( int bucketIndex, float unit, const Color4f& mainAxisColor, const Color4f& smallAxisColor,
                     const Vector2f& limits, float axisSize, const std::string& _name = "" );
    VPListSP
	drawArrow( int bucketIndex, const Vector3f& p1, const Vector3f& p2, const C4f& color, float width,
			   float angle, float arrowlength, const std::string& _name = "" );
    VPListSP
    drawDoubleArrow( int bucketIndex, const Vector3f& p1, const Vector3f& p2, const C4f& color, float width,
               float angle, float arrowlength, const std::string& _name = "" );

    VPListSP drawLineFinal( int bucketIndex, const std::vector<Vector3f>& verts, const Vector4f& color, float width,
                            const Matrix4f& mat, bool wrapiIt, const std::string& _name );
    VPListSP drawLines( int bucketIndex, const V2fVectorOfVector& verts, const Vector4f& color, float width,
                        const Matrix4f& mat = Matrix4f::IDENTITY, const std::string& _name = {} );
    VPListSP drawTriangle( int bucketIndex, const std::vector<Vector2f>& verts, float _z, const Vector4f& color,
					   const std::string& _name = "" );
    VPListSP drawTriangle( int bucketIndex, const std::vector<Vector3f>& verts, const Vector4f& color,
					   const std::string& _name = "" );
    VPListSP drawTriangles(int bucketIndex, const std::vector<Vector3f>& verts, const Vector4f& color,
					   const std::string& _name="");
    VPListSP drawTriangles(int bucketIndex, const std::vector<Vector3f>& verts, const std::vector<int32_t>& indices,
					   const Vector4f& color, const std::string& _name="");
    VPListSP
	draw3dVector( int bucketIndex, const Vector3f& pos, const Vector3f& dir, const Vector4f& color, float size,
				  const std::string& _name = "" );
    VPListSP drawDot( int bucketIndex, const Vector3f& center, float radius, const Color4f& color, const std::string& _name = "" );
    VPListSP drawCircle( int bucketIndex, const Vector3f& center, float radius, const Color4f& color,
					 int32_t subdivs = 12, const std::string& _name = "" );
    VPListSP drawCircle( int bucketIndex, const Vector3f& center, const Vector3f& normal, float radius,
					 const Color4f& color, int32_t subdivs, const std::string& _name = "" );
    VPListSP drawCircle2d( int bucketIndex, const Vector2f& center, float radius, const Color4f& color,
					   int32_t subdivs = 12, const std::string& _name = "" );
    VPListSP drawArc( int bucketIndex, const Vector3f& center, float radius, float fromAngle, float toAngle,
				  const Vector4f& color, float width, int32_t subdivs = 10, float percToBeDrawn = 1.0f,
				  const std::string& _name = "" );
    VPListSP drawArc( int bucketIndex, const Vector3f& center, const Vector3f& p1, const Vector3f& p2,
				  const Vector4f& color, float width, int32_t subdivs = 10, float percToBeDrawn = 1.0f,
				  const std::string& _name = "" );
    VPListSP
	drawCylinder( int bucketIndex, const Vector3f& pos, const Vector3f& dir, const Vector4f& color, float size,
				  const std::string& _name = "" );

    VPListSP drawCone( int bucketIndex, const Vector3f& posBase, const Vector3f& posTop, const Vector4f& color,
				   float size, const std::string& _name = "" );

    void drawText( int bucketIndex, const std::string& text, const V3f& pos, float scale,
                   const Font* font, const Color4f& color, float angle = 0.0f );

    VPListSP drawRect( int bi, const Rect2f& r, const Color4f& color, const std::string& _name = {} );

    VPListSP drawRect( int bucketIndex, const Vector2f& p1, const Vector2f& p2, CResourceRef _texture, float ratio = 1.0f,
                   const Color4f& color = C4f::WHITE, RectFillMode fm = RectFillMode::Scale, const std::string& _name = {} );
    VPListSP drawRect( int bucketIndex, const Vector2f& p1, const Vector2f& p2, const Color4f& color,
                   const std::string& _name = {} );
    VPListSP drawRect2d( int bucketIndex, const Vector2f& p1, const Vector2f& p2, CResourceRef _texture, float ratio = 1.0f,
                   const Color4f& color = C4f::WHITE, RectFillMode fm = RectFillMode::Scale, const std::string& _name = {} );
    VPListSP drawRect2d( int bucketIndex, const Rect2f& r1, CResourceRef _texture, float ratio = 1.0f,
                     const Color4f& color = C4f::WHITE, RectFillMode fm = RectFillMode::Scale, const std::string& _name = {} );
    VPListSP drawRect2d( int bucketIndex, const Vector2f& p1, const Vector2f& p2, const Color4f& color,
                   const std::string& _name = {} );
    VPListSP drawRect2d( int bucketIndex, const Rect2f& r1, const Color4f& color, const std::string& _name = {} );

    VPListSP drawMeasurementArrow1( int bucketIndex, const Vector3f& p1, const Vector3f& p2,
                                    const V4f& color, float width, float angle, float arrowlength,
                                    float offsetGap, const Font* font, float fontHeight, const C4f& fontColor,
                                    const C4f& fontBackGroundColor, const std::string& _name = {} );
    VPListSP drawMeasurementArrow2( int bucketIndex, const Vector3f& p1, const Vector3f& p2,
                                    const V2f& p12n, const Vector3f& op1, const Vector3f& op2,
                                    const V4f& color, float width, float angle, float arrowlength,
                                    float offsetGap, const Font* font, float fontHeight, const C4f& fontColor,
                                    const C4f& fontBackGroundColor, const std::string& _name = {} );

    struct RendererDrawingSet {
        int bucketIndex = -1;
        bool wrapIt = false;
        std::vector<V3f> verts;
        C4f color = C4f::WHITE;
        float width = 0.1f;
        std::string name{};
        Matrix4f matrix{Matrix4f::IDENTITY};
    };

    template<typename M>
    void addRendererDrawingSetParam( RendererDrawingSet& rds, const M& _param ) {
        if constexpr ( std::is_same_v<M, int> ) {
            rds.bucketIndex = _param;
            return;
        }
        if constexpr ( std::is_same_v<M, bool> ) {
            rds.wrapIt = _param;
            return;
        }
        if constexpr ( std::is_same_v<M, std::vector<V3f>> ) {
            rds.verts = _param;
            return;
        }
        if constexpr ( std::is_same_v<M, C4f> ) {
            rds.color = _param;
            return;
        }
        if constexpr ( std::is_same_v<M, float> ) {
            rds.width = _param;
            return;
        }
        if constexpr ( std::is_same_v<M, std::string> ) {
            rds.name = _param;
            return;
        }
        if constexpr ( std::is_same_v<M, Matrix4f> ) {
            rds.matrix = _param;
            return;
        }
        if constexpr ( std::is_same_v<M, V3f> ) {
            rds.verts.emplace_back( _param );
            return;
        }
    }

    template <typename ...Args>
    VPListSP drawLine( Args&& ... args ) {
        RendererDrawingSet rds{};

        (addRendererDrawingSetParam( rds, std::forward<Args>( args )), ...);

        return drawLineFinal( rds.bucketIndex, rds.verts, rds.color, rds.width, rds.matrix, rds.wrapIt, rds.name );
    }
};
