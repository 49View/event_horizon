#pragma once

#include <vector>
#include <utility>
#include <array>
#include <unordered_set>
#include <core/uuid.hpp>
#include <core/names.hpp>
#include <core/math/matrix4f.h>
#include <core/heterogeneous_map.hpp>
#include <core/resources/resource_utils.hpp>
#include <core/resources/profile.hpp>
#include <graphics/framebuffer.h>
#include <graphics/graphic_constants.h>
#include <graphics/ghtypes.hpp>
#include <graphics/shadowmap_manager.h>
#include <core/math/anim_type.hpp>

class Camera;

struct DCircle {
};
struct DCircleFilled {
};
struct DLine {
};
struct DArrow {
};
struct DPoly {
};
struct DFlatPoly {
};
struct DRect {
};
struct DRect2d {
};
struct DRect2dRounded {
};
struct DText {
};
struct DText2d {
};

using DShaderMatrixValue = uint64_t;
static constexpr DShaderMatrixValue DShaderMatrixValue2dColor = 1u << 1;
static constexpr DShaderMatrixValue DShaderMatrixValue2dTexture = 1u << 2;
static constexpr DShaderMatrixValue DShaderMatrixValue3dColor = 1u << 3;
static constexpr DShaderMatrixValue DShaderMatrixValue3dTexture = 1u << 4;

struct DShaderMatrix {
    template<typename ...Args>
    explicit DShaderMatrix( Args&& ... args ) : data(std::forward<Args>(args)...) {}
    DShaderMatrixValue operator()() const noexcept {
        return data;
    }
    [[nodiscard]] bool has2d() const;
    [[nodiscard]] bool hasTexture() const;
    [[nodiscard]] std::string hash() const;
    DShaderMatrixValue data;
};

struct FontDrawingSet {
    std::string text{};
    const Font *font = nullptr;
    V3f pos = V3f::ZERO;
    float fontHeight = 0.01f;
    float fontAngle = 0.0f;
};

using FDS = FontDrawingSet;

enum class RDSRectAxis {
    XY,
    XZ
};

struct RendererDrawingSet {
    RendererDrawingSet() = default;
    RendererDrawingSet( int bi, Color4f c, std::string sn, std::string n ) :
            bucketIndex(bi), color(std::move(c)), name(std::move(n)), shaderName(std::move(sn)) {}
    void setupFontData();
    [[nodiscard]] bool hasTexture() const;
    void resolveShaderMatrix();

    int bucketIndex = CommandBufferLimits::UI2dStart;
    Primitive prim = PRIMITIVE_TRIANGLE_STRIP;
    VTMVectorWrap verts;
    std::vector<Triangle2d> triangles{};
    VTMVectorOfVectorWrap multiVerts;
    Rect2f rect = Rect2f::MIDENTITY();
    RDSRectAxis rectAxis = RDSRectAxis::XY;
    float roundedCorner = 0.01f;
    float arrowAngle = 0.3f;
    float arrowLength = 0.3f;
    float radius = 0.5f;
    C4f color = C4fc::WHITE;
    float width = 0.1f;
    uint32_t archSegments = 12;
    FDS fds;
    std::string name{};
    DShaderMatrix shaderMatrix;
    std::string shaderName = S::COLOR_3D;
    std::string textureRef{};
    std::shared_ptr<Matrix4f> matrix;
    Matrix4f preMultMatrix{ Matrix4f::IDENTITY };
    V3f rotationNormalAxis = V3f::ZERO;
    bool usePreMult = false;
};

struct RDSPrimitive {
    template<typename ...Args>
    explicit RDSPrimitive( Args&& ... args ) : data(std::forward<Args>(args)...) {}
    Primitive operator()() const noexcept {
        return data;
    }
    Primitive data = PRIMITIVE_TRIANGLE_STRIP;
};

struct RDSPreMult {
    template<typename ...Args>
    explicit RDSPreMult( Args&& ... args ) : data(std::forward<Args>(args)...) {}
    Matrix4f operator()() const noexcept {
        return data;
    }
    Matrix4f data;
};

struct RDSImage {
    template<typename ...Args>
    explicit RDSImage( Args&& ... args ) : data(std::forward<Args>(args)...) {}
    ResourceRef operator()() const noexcept {
        return data;
    }
    ResourceRef data;
};

struct RDSRoundedCorner {
    template<typename ...Args>
    explicit RDSRoundedCorner( Args&& ... args ) : data(std::forward<Args>(args)...) {}
    float operator()() const noexcept {
        return data;
    }
    float data;
};

struct RDSArchSegments {
    template<typename ...Args>
    explicit RDSArchSegments( Args&& ... args ) : data(std::forward<Args>(args)...) {}
    size_t operator()() const noexcept {
        return data;
    }
    size_t data;
};

struct RDSArrowAngle {
    template<typename ...Args>
    explicit RDSArrowAngle( Args&& ... args ) : data(std::forward<Args>(args)...) {}
    float operator()() const noexcept {
        return data;
    }
    float data;
};

struct RDSArrowLength {
    template<typename ...Args>
    explicit RDSArrowLength( Args&& ... args ) : data(std::forward<Args>(args)...) {}
    float operator()() const noexcept {
        return data;
    }
    float data;
};

struct RDSCircleLineWidth {
    template<typename ...Args>
    explicit RDSCircleLineWidth( Args&& ... args ) : data(std::forward<Args>(args)...) {}
    float operator()() const noexcept {
        return data;
    }
    float data;
};

struct RDSRotationNormalAxis {
    template<typename ...Args>
    explicit RDSRotationNormalAxis( Args&& ... args ) : data(std::forward<Args>(args)...) {}
    V3f operator()() const noexcept {
        return data;
    }
    V3f data;
};

using CommandBufferLimitsT = int;
using CommandBufferListVectorMap = std::map<int, CommandBufferListVector>;

class RenderAnimationManager {
public:
    void init();
    void generateUBO( std::shared_ptr<ShaderManager> sm );
    void setTiming();
    void setProgressionTiming( float _progress );
    void setUniforms_r();
private:
    std::unique_ptr<ProgramUniformSet> mAnimUniforms;
    float progress = 0.0f;
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

struct ChangeMaterialOnContainer {
    ChangeMaterialOnContainer( uint64_t tag, const std::string& matHash ) : tag(tag), matHash(matHash) {}
    ChangeMaterialOnContainer( const std::string& name, const std::string& matHash ) : name(name),
                                                                                       matHash(matHash) {}

    uint64_t tag = 0;
    std::string name{};
    std::string matHash{};
};

struct scene_t;

class RenderStats;

class Renderer {
public:
    explicit Renderer( StreamingMediator& _ssm );
    virtual ~Renderer() = default;

    void cmdReloadShaders( const std::vector<std::string>& _params );

    void init();
    static void clearColor( const C4f& _color );
    void afterShaderSetup();
    void injectShader( const std::string& _key, const std::string& content );

    void directRenderLoop( RenderStats& rs );

    void removeFromCL( const UUID& _uuid );
    void clearBucket( int _bucket );
    void showBucket( int _bucket, bool visible );

    std::shared_ptr<Texture> addTextureResource( const ResourceTransfer<RawImage>& _val );
    void updateTextureResource( const ResourceTransfer<RawImage>& _val );
    std::shared_ptr<RenderMaterial> addMaterialResource( const ResourceTransfer<Material>& _val );
    std::shared_ptr<RenderMaterial> addMaterialResource( const ShaderMaterial& _val, const std::string& _name );
    std::shared_ptr<RenderMaterial> upsertMaterialResource( const ShaderMaterial& _val, const std::string& _name );
    std::shared_ptr<GPUVData> addVDataResource( const ResourceTransfer<VData>& _val );
    std::shared_ptr<GPUVData> addVDataResource( cpuVBIB&& _val, const std::string& _name );
    std::shared_ptr<GPUVData> upsertVDataResource( cpuVBIB&& _val, const std::string& _name );

    std::shared_ptr<RenderMaterial> getMaterial( const std::string& _key );
    std::shared_ptr<GPUVData> getGPUVData( const std::string& _key );

    void changeMaterialOnTagsCallback( const ChangeMaterialOnContainer& _cmt );
    void changeMaterialColorOnTags( uint64_t _tag, const Color4f& _color );
    void changeMaterialColorOnTags( uint64_t _tag, float r, float g, float b );
    void changeMaterialAlphaOnTags( uint64_t _tag, float _alpha );
    void setVisibilityOnTags( uint64_t _tag, bool _visibility );
    [[maybe_unused]] VPListFlatContainer getVPListWithTags( uint64_t _tag );
    void changeMaterialColorOnUUID( const UUID& _tag, const Color4f& _color, Color4f& _oldColor );
    void replaceMaterial( const std::string& _oldMatRef, const std::string& _newMatRef );
    void changeMaterialProperty( const std::string& _prop, const std::string& _matKey, const std::string& _value );

    std::shared_ptr<Program> P( const std::string& _id );
    std::shared_ptr<Texture> TD( const std::string& _id, int tSlot = -1 );
    TextureUniformDesc TDI( const std::string& _id, unsigned int tSlot );

    void addToCommandBuffer( CommandBufferLimitsT _entry );
    void addToCommandBuffer( VPListContainerCRef _map,
                             const Camera *_cameraRig = nullptr,
                             std::shared_ptr<RenderMaterial> _forcedMaterial = nullptr,
                             Program *_forceProgram = nullptr,
                             float _alphaDrawThreshold = 0.0f );

    void setRenderHook( const std::string& _key, std::weak_ptr<CommandBufferEntry>& _hook );
    void setGlobalTextures();
    [[nodiscard]] std::shared_ptr<RenderMaterial> getRenderMaterialFromHash( CResourceRef _hash );

    std::shared_ptr<ProgramUniformSet>& CameraUBO() { return rcm.UBO(); }
    std::shared_ptr<RenderLightManager> LM() { return lm; }
    std::shared_ptr<TextureManager> TM() { return tm; }
    std::shared_ptr<ShaderManager> SM() { return sm; }
    std::shared_ptr<Framebuffer> BRDFTarget() { return mBRDF; };
    StreamingMediator& SSM();

    void VPL( int _bucket, std::shared_ptr<VPList> nvp, float alpha = 1.0f );

    bool hasTag( uint64_t _tag ) const;

    inline CommandBufferList& CB_U() { return *mCommandBuffers; }
    inline CommandBufferListVectorMap& CL() { return mCommandLists; }
    void CLI( uint64_t cli, std::function<void(const std::shared_ptr<VPList>&)> func );
    VPListFlatContainer CLI( uint64_t cli );
    VPListFlatContainer CLIWithPred( uint64_t cli, std::function<bool(const std::shared_ptr<VPList>&)> );
    VPListFlatContainer CLIExcludingTag( uint64_t cli, uint64_t excludingTag );
    VPListFlatContainer CLIIncludingTag( uint64_t cli, uint64_t _tag );
    inline const CommandBufferListVectorMap& CL() const { return mCommandLists; }

    void resetDefaultFB( const Vector2i& forceSize = Vector2i{ -1 } );

    void addTarget( std::shared_ptr<RLTarget> _target );
    std::shared_ptr<RLTarget> getTarget( const std::string& _name );
    void clearTargets();

    std::shared_ptr<Framebuffer> getDefaultFB() { return mDefaultFB; }
    std::shared_ptr<Framebuffer> getShadowMapFB() { return mShadowMapFB; }
    std::shared_ptr<Framebuffer> getDepthMapFB() { return mDepthFB; }
    std::shared_ptr<Framebuffer> getProbing( int _index );

    int UpdateCounter() const { return mUpdateCounter; }
    void invalidateOnAdd();

    template<typename ...Args>
    void changeMaterialOn( Args&& ... args ) {
        changeMaterialOnInternal(perfectForward<ChangeMaterialOnContainer>(std::forward<Args>(args)...));
    }

    void remapLightmapUVs( const scene_t& scene );

    [[nodiscard]] bool isLoading() const;
    void setLoadingFlag( bool _value );
    void setProgressionTiming( float _progress );

    [[nodiscard]] size_t drawCallsPerFrame() const;
    const Vector2i& getForcedFrameBufferSize() const;
    void setForcedFrameBufferSize( const Vector2i& mForcedFrameBufferSize );

    void useVignette( bool _flag );
    void useFilmGrain( bool _flag );
    void useBloom( bool _flag );
    void useDOF( bool _flag );
    void useSSAO( bool _flag );
    void useMotionBlur( bool _flag );
    void setShadowOverBurnCofficient( float _overBurn );
    void setShadowZFightCofficient( float _value );
    void setIndoorSceneCoeff( float _value );

    [[nodiscard]] float ssaoBlendFactor() const;
    [[nodiscard]] floata& ssaoBlendFactorAnim();
    void ssaoBlendFactor( float mSsaoBlendFactor );
protected:
    void clearCommandList();
    size_t renderCBList();
    size_t renderCommands( int eye );

protected:
    void changeMaterialOnInternal( const ChangeMaterialOnContainer& _cmt );

protected:
    std::shared_ptr<ShaderManager> sm;
    std::shared_ptr<TextureManager> tm;
    std::shared_ptr<RenderMaterialManager> rmm;
    std::shared_ptr<RenderLightManager> lm;
    std::shared_ptr<GPUVDataManager> gm;
    StreamingMediator& ssm;

    RenderAnimationManager am;
    RenderCameraManager rcm;

    std::shared_ptr<Framebuffer> mDefaultFB;
    std::shared_ptr<Framebuffer> mBRDF;
    std::shared_ptr<Framebuffer> mShadowMapFB;
    std::shared_ptr<Framebuffer> mDepthFB;
    std::unordered_map<int, std::shared_ptr<Framebuffer>> mProbingsFB;

    int mUpdateCounter = 0;
    size_t mDrawCallsPerFrame = 0;
    bool bInvalidated = false;
    bool bIsLoading = true;
    Vector2i mForcedFrameBufferSize{ -1, -1 };

    // Post processing controls
    floata mSsaoBlendFactor;

    std::vector<std::shared_ptr<RLTarget>> mTargets;
    std::shared_ptr<CommandBufferList> mCommandBuffers;
    std::map<int, CommandBufferListVector> mCommandLists;
    std::unordered_map<UUID, VPListSP> mVPLMap;

    std::vector<ChangeMaterialOnContainer> mChangeMaterialCallbacks;

    template<typename V> friend class VPBuilder;

public:

    void drawGrid( int bucketIndex, float unit, const Color4f& mainAxisColor, const Color4f& smallAxisColor,
                     const Vector2f& limits, float axisSize, const std::string& _name = "" );
    std::vector<VPListSP>
    drawGridV2( int bucketIndex, float unit, const Color4f& mainAxisColor, const Color4f& smallAxisColor,
                  const Vector2f& limits, float axisSize, const std::string& _name = "" );

    VPListSP drawMeasurementArrow1( int bucketIndex, const Vector3f& p1, const Vector3f& p2,
                                    const V4f& color, float width, float angle, float arrowlength,
                                    float offsetGap, const Font *font, float fontHeight, const C4f& fontColor,
                                    const C4f& fontBackGroundColor, const std::string& _name = {} );
    VPListSP drawMeasurementArrow2( int bucketIndex, const Vector3f& p1, const Vector3f& p2,
                                    const V2f& p12n, const Vector3f& op1, const Vector3f& op2,
                                    const V4f& color, float width, float angle, float arrowlength,
                                    float offsetGap, const Font *font, float fontHeight, const C4f& fontColor,
                                    const C4f& fontBackGroundColor, const std::string& _name = {} );

    std::vector<VPListSP> drawDotCircled( float dotSize, const V3f& centrePoint, const V3f& normal,
                                          const C4f& _dotColor, float finalAlphaValue, const std::string& _name );

    template<typename T, typename M>
    void addRendererDrawingSetParam( RendererDrawingSet& rds, const M& _param ) {
        if constexpr ( std::is_same_v<M, DShaderMatrix> ) {
            rds.shaderMatrix = _param;
            return;
        }
        if constexpr ( std::is_integral_v<M> && !std::is_same_v<M, bool> ) {
            rds.bucketIndex = _param;
            return;
        }
        if constexpr ( std::is_same_v<M, bool> ) {
            rds.verts.wrap = _param;
            return;
        }
        if constexpr ( std::is_same_v<M, V3fVector> ) {
            rds.verts.v = _param;
            return;
        }
        if constexpr ( std::is_same_v<M, V2fVector> ) {
            rds.verts.v = XZY::C(_param);
            return;
        }
        if constexpr ( std::is_same_v<M, V3fVectorWrap> ) {
            rds.verts = _param;
            return;
        }
        if constexpr ( std::is_same_v<M, std::array<V3f,4>> ) {
            for ( auto t = 0u; t < 4; t++) rds.verts.v.emplace_back(_param[t]);
            return;
        }
        if constexpr ( std::is_same_v<M, std::vector<Triangle2d>> ) {
            rds.triangles = _param;
            return;
        }
        if constexpr ( std::is_same_v<M, Rect2f> ) {
            if constexpr ( std::is_same_v<T, DRect2d> || std::is_same_v<T, DRect2dRounded> ||
                           std::is_same_v<T, DRect> ) {
                rds.rect = _param;
            } else {
                rds.verts.v = _param.points3dcw_xzy();
                rds.verts.wrap = true;
            }
            return;
        }
        if constexpr ( std::is_same_v<M, C4f> ) {
            rds.color = _param;
            return;
        }
        if constexpr ( std::is_same_v<M, float> ) {
            if constexpr ( std::is_same_v<T, DCircle> || std::is_same_v<T, DCircleFilled> ) {
                rds.radius = _param;
            } else {
                rds.width = _param;
            }
            return;
        }
        if constexpr ( std::is_same_v<M, std::string> ||
                       std::is_same_v<typename std::remove_all_extents<M>::type, char> ) {
            rds.name = _param;
            return;
        }
        if constexpr ( std::is_same_v<M, FDS> ) {
            rds.fds = _param;
            return;
        }
        if constexpr ( std::is_same_v<M, RDSPrimitive> ) {
            rds.prim = _param();
            return;
        }
        if constexpr ( std::is_same_v<M, RDSImage> ) {
            rds.textureRef = _param();
            return;
        }
        if constexpr ( std::is_same_v<M, std::shared_ptr<Matrix4f>> ) {
            rds.matrix = _param;
            return;
        }
        if constexpr ( std::is_same_v<M, V3f> ) {
            rds.verts.v.emplace_back(_param);
            return;
        }
        if constexpr ( std::is_same_v<M, V2f> ) {
            rds.verts.v.emplace_back(XZY::C(_param));
            return;
        }
        if constexpr ( std::is_same_v<M, RDSRectAxis> ) {
            rds.rectAxis = _param;
            return;
        }
        if constexpr ( std::is_same_v<M, RDSPreMult> ) {
            rds.usePreMult = true;
            rds.preMultMatrix = _param();
            return;
        }
        if constexpr ( std::is_same_v<M, RDSRoundedCorner> ) {
            rds.roundedCorner = _param();
            return;
        }
        if constexpr ( std::is_same_v<M, RDSArchSegments> ) {
            rds.archSegments = _param();
            return;
        }
        if constexpr ( std::is_same_v<M, RDSArrowAngle> ) {
            rds.arrowAngle = _param();
            return;
        }
        if constexpr ( std::is_same_v<M, RDSArrowLength> ) {
            rds.arrowLength = _param();
            return;
        }
        if constexpr ( std::is_same_v<M, RDSCircleLineWidth> ) {
            rds.width = _param();
            return;
        }
        if constexpr ( std::is_same_v<M, RDSRotationNormalAxis> ) {
            rds.rotationNormalAxis = _param();
            return;
        }
        if constexpr ( std::is_same_v<M, V3fVectorOfVectorWrap> ) {
            ASSERTV(false, "Please provide a VTMVectorOfVectorWrap instead of a V3fVectorOfVectorWrap");
            return;
        }
        if constexpr ( std::is_same_v<M, std::shared_ptr<Profile> > ) {
            if ( _param ) {
                rds.multiVerts = _param->Paths3dWithUV();
            }
            return;
        }
        if constexpr ( std::is_same_v<M, VTMVectorOfVectorWrap> ) {
            rds.multiVerts = _param;
            return;
        }
    }

    template<typename T, typename ...Args>
    VPListSP draw( Args&& ... args ) {
        RendererDrawingSet rds{};

        (addRendererDrawingSetParam<T>(rds, std::forward<Args>(args)), ...);

        rds.resolveShaderMatrix();

        if constexpr ( std::is_same_v<T, DLine> ) {
            return drawLineFinal(rds);
        }
        if constexpr ( std::is_same_v<T, DArrow> ) {
            return drawArrowFinal(rds);
        }
        if constexpr ( std::is_same_v<T, DPoly> ) {
            return drawPolyFinal(rds);
        }
        if constexpr ( std::is_same_v<T, DFlatPoly> ) {
            return drawFlatPolyFinal(rds);
        }
        if constexpr ( std::is_same_v<T, DCircle> ) {
            return drawCircleFinal(rds);
        }
        if constexpr ( std::is_same_v<T, DCircleFilled> ) {
            return drawCircleFilledFinal(rds);
        }
        if constexpr ( std::is_same_v<T, DRect> ) {
            rds.shaderName = S::TEXTURE_3D;
            return drawRectFinalTM(rds);
        }
        if constexpr ( std::is_same_v<T, DText2d> || std::is_same_v<T, DText> ) {
            rds.shaderName = std::is_same_v<T, DText2d> ? S::FONT_2D : S::FONT;
            rds.setupFontData();
            return drawTextFinal(rds);
        }
        if constexpr ( std::is_same_v<T, DRect2d> ) {
//            rds.shaderName = S::COLOR_2D;
            rds.shaderName = S::TEXTURE_2D;
            return drawRectFinalTM(rds);
        }
        if constexpr ( std::is_same_v<T, DRect2dRounded> ) {
            rds.shaderName = S::COLOR_2D;
            rds.prim = PRIMITIVE_TRIANGLE_FAN;
            rds.verts.v = roundedCornerFanFromRect(rds.rect, rds.roundedCorner);
            return drawRectFinal(rds);
        }
        return nullptr;
    }

private:
    VPListSP drawLineFinal( RendererDrawingSet& rds );
    VPListSP drawArrowFinal( RendererDrawingSet& rds );
    VPListSP drawPolyFinal( RendererDrawingSet& rds );
    VPListSP drawFlatPolyFinal( RendererDrawingSet& rds );
    VPListSP drawCircleFinal( RendererDrawingSet& rds );
    VPListSP drawCircleFilledFinal( RendererDrawingSet& rds );
    VPListSP drawRectFinal( RendererDrawingSet& rds );
    VPListSP drawRectFinalTM( RendererDrawingSet& rds );
    VPListSP drawTextFinal( const RendererDrawingSet& rds );
};
