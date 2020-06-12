//
// Created by Dado on 2019-03-09.
//

#pragma once

#include <core/dirtable.hpp>
#include <graphics/render_list.h>

class Framebuffer;
class CommandBuffer;
class CameraRig;
class ShaderManager;
class Renderer;
class CommandBufferList;
class ShadowMapManager;
class Skybox;
class CubeEnvironmentMap;
class PrefilterSpecularMap;
class PrefilterBRDF;
class RLTarget;
class Camera;
class SunBuilder;
struct FrameBufferTextureValues;

enum class RenderTargetType {
    Plain,
    PBR,
    Cubemap,
    Probe
};

enum class CheckEnableBucket {
    False,
    True
};

enum class RLClearFlag {
    DontIncludeCore,
    All
};

struct RenderBucketRange {
    RenderBucketRange() = default;
    explicit RenderBucketRange( int _r1, int _r2, bool enabled = true ) : enabled( enabled ) {
        range = { _r1, _r2 };
    }

    std::pair<int, int> range;
    bool enabled = true;
};

class Composite {
public:
    explicit Composite( Renderer& rr ) : rr( rr ) {}
    virtual ~Composite() = default;

    virtual void blit(CommandBufferList& cbl) = 0;
    virtual void setup( const Rect2f& _destViewport ) = 0;

    const std::shared_ptr<Framebuffer>& getColorFB() const {
        return mColorFB;
    }

    const std::shared_ptr<Framebuffer>& getOffScreenFB() const {
        return mOffScreenBlitFB;
    }

    void setColorFB( const std::shared_ptr<Framebuffer>& _ColorFB ) {
        mColorFB = _ColorFB;
    }

    bool blitOnScreen() const {
        return mCompositeFinalDest == BlitType::OnScreen;
    }

protected:
    Renderer& rr;
    std::shared_ptr<Framebuffer> mColorFB;
    std::shared_ptr<Framebuffer> mUIFB;
    std::shared_ptr<Framebuffer> mUIBlitFB;
    std::shared_ptr<Framebuffer> mOffScreenBlitFB;
    BlitType mCompositeFinalDest = BlitType::OnScreen;
};

class CompositePlain : public Composite {
public:
    ~CompositePlain() override = default;
    CompositePlain( Renderer& rr, const std::string& _name, const Rect2f& _destViewport,
                    BlitType _bt = BlitType::OnScreen );
    void blit(CommandBufferList& cbl) override;
    void setup( const Rect2f& _destViewport ) override;
};

class CompositePBR : public Composite {
public:
    CompositePBR( Renderer& rr, const std::string& _name, const Rect2f& _destViewport, BlitType _bt );
    ~CompositePBR() override = default;

    void setup( const Rect2f& _destViewport ) override;

    const std::shared_ptr<Framebuffer>& getColorFinalFB() const {
        return mColorFinalFB;
    }

    void setColorFinalFB( const std::shared_ptr<Framebuffer>& _ColorFinalFB ) {
        mColorFinalFB = _ColorFinalFB;
    }

    const std::shared_ptr<Framebuffer>& getBlurHorizontalFB() const {
        return mBlurHorizontalFB;
    }

    void setBlurHorizontalFB( const std::shared_ptr<Framebuffer>& _BlurHorizontalFB ) {
        mBlurHorizontalFB = _BlurHorizontalFB;
    }

    const std::shared_ptr<Framebuffer>& getBlurVerticalFB() const {
        return mBlurVerticalFB;
    }

    std::shared_ptr<Framebuffer> getNormalMapFB() { return mNormalFB; }
    std::shared_ptr<Framebuffer> getSSAOMapFB() { return mSSAOFB; }
    std::shared_ptr<Framebuffer> getUIMapFB() { return mUIFB; }
    std::shared_ptr<Framebuffer> getUIMapResolveFB() { return mUIBlitFB; }

    void setBlurVerticalFB( const std::shared_ptr<Framebuffer>& _BlurVerticalFB ) {
        mBlurVerticalFB = _BlurVerticalFB;
    }

    bool isUsingBloom() const {
        return mbUseBloom;
    }

    void useBloom( bool _mbUseBloom ) {
        mbUseBloom = _mbUseBloom;
    }

    std::shared_ptr<VPList> getVPFinalCombine() const;

    void bloom();
    void blit( CommandBufferList& cbl ) override;

private:
    bool mbUseBloom = false;
    std::shared_ptr<Framebuffer> mColorFinalFB;
    std::shared_ptr<Framebuffer> mBlurHorizontalFB;
    std::shared_ptr<Framebuffer> mBlurVerticalFB;
    std::shared_ptr<Framebuffer> mNormalFB;
    std::shared_ptr<Framebuffer> mSSAOFB;
};

class RLTarget : public Dirtable, public std::enable_shared_from_this<RLTarget> {
public:
    explicit RLTarget( Renderer& _rr ) : rr( _rr ) {}
    RLTarget( std::shared_ptr<CameraRig> cameraRig, const Rect2f& screenViewport, BlitType _bt, Renderer& _rr ) :
            cameraRig( cameraRig ), screenViewport( screenViewport ), finalDestBlit(_bt), rr( _rr ) {}
    virtual ~RLTarget() = default;

    std::shared_ptr<Camera> getCamera();
    std::shared_ptr<CameraRig> getRig();
    void addToCBCore( CommandBufferList& cb );
    void updateStreams();
    virtual void invalidateOnAdd() {}
    virtual void changeTime( const std::string& time ) {}
    virtual void addToCB( CommandBufferList& cb ) = 0;
    virtual std::shared_ptr<Framebuffer> getFrameBuffer( CommandBufferFrameBufferType fbt ) { return framebuffer; }
    virtual void blit( CommandBufferList& cbl ) = 0;
    virtual void resize( const Rect2f& _r ) = 0;

    void clearCB();

    bool isKeyInRange( int _key, CheckEnableBucket _checkBucketVisibility = CheckEnableBucket::False,
                       RLClearFlag _clearFlags = RLClearFlag::All ) const;

    bool isTakingScreenShot() {
        return mbTakeScreenShot && ++mTakeScreenShotDelay > 2;
    }

    void takeScreenShot( bool _value = true ) {
        mbTakeScreenShot = _value;
        disable();
    }

    void takeScreenShot( ScreenShotContainerPtr _outdata ) {
        mbTakeScreenShot = true;
        screenShotContainer = _outdata;
        mTakeScreenShotDelay = 0;
        enable();
    }

    BlitType FinalDestBlit() const {
        return finalDestBlit;
    }

    ScreenShotContainerPtr& ScreenShotData() { return screenShotContainer; }

    bool enabled() const {
        return bEnabled;
    }

    void enable( bool _b ) {
        bEnabled = _b;
    }

    void enable() { bEnabled = true; }
    void disable() { bEnabled = false; }
    void setVisibleCB( int _index, bool _value );

    void enableBucket( bool _flag,  size_t _bucketInndex = 0 ) {
        bucketRanges[_bucketInndex].enabled = _flag;
    }

public:
    std::shared_ptr<CameraRig> cameraRig = nullptr;
    std::shared_ptr<Framebuffer> framebuffer = nullptr;
    Rect2f screenViewport = Rect2f::INVALID;
    bool bEnabled = true;
    BlitType finalDestBlit = BlitType::OnScreen;
    ScreenShotContainerPtr screenShotContainer;
    std::vector<RenderBucketRange> bucketRanges;
    std::unordered_set<int> hiddenSet;

protected:
    void updateStreamPacket( const std::string& _streamName );
    [[nodiscard]] bool hiddenCB( int _cbIndex );

protected:
    std::unordered_map<std::string, std::shared_ptr<CameraRig>> mAncillaryCameraRigs;
    bool mbTakeScreenShot = false;
    int  mTakeScreenShotDelay = 0;
    Renderer& rr;
};

class RLTargetPlain : public RLTarget {
public:
    RLTargetPlain( std::shared_ptr<CameraRig> cameraRig, const Rect2f& screenViewport, BlitType _bt, Renderer& rr );
    ~RLTargetPlain() override = default;

    void addToCB( CommandBufferList& cb ) override;
    virtual void blit(CommandBufferList& cbl) override;
    std::shared_ptr<Framebuffer> getFrameBuffer( CommandBufferFrameBufferType fbt ) override;
    void resize( const Rect2f& _r ) override;

protected:
    std::shared_ptr<CompositePlain> mComposite;
};

class RLTargetFB : public RLTarget {
public:
    RLTargetFB( std::shared_ptr<Framebuffer> _fbt, [[maybe_unused]] Renderer& _rr );
    ~RLTargetFB() override = default;
    void addToCB( [[maybe_unused]] CommandBufferList& cb ) override {}
    void blit( [[maybe_unused]] CommandBufferList& cbl) override {};
    std::shared_ptr<Framebuffer> getFrameBuffer( CommandBufferFrameBufferType fbt ) override;
    void resize( const Rect2f& _r ) override;
};

class RLTargetCubeMap : public RLTarget {
public:
    RLTargetCubeMap( const CubeMapRigContainer& _rig, std::shared_ptr<Framebuffer> _fb, Renderer& _rr );
    ~RLTargetCubeMap() override = default;
    void addToCB( [[maybe_unused]] CommandBufferList& cb ) override {}
    virtual void blit( [[maybe_unused]] CommandBufferList& cbl) override {};
    void render( std::shared_ptr<Texture> _renderToTexture, int cmsize, int mip, CubeMapRenderFunction rcb );
    void resize( [[maybe_unused]] const Rect2f& _r ) override {}

protected:
    CubeMapRigContainer cameraRig;
};

class RLTargetProbe : public RLTarget {
public:
    RLTargetProbe( std::shared_ptr<CameraRig> _crig, Renderer& _rr );
    ~RLTargetProbe() override = default;
    void addToCB( [[maybe_unused]] CommandBufferList& cb ) override {}
    virtual void blit( [[maybe_unused]] CommandBufferList& cbl) override {};
    virtual std::shared_ptr<Framebuffer> getFrameBuffer( CommandBufferFrameBufferType fbt ) override;
    void resize( [[maybe_unused]] const Rect2f& _r ) override {}

protected:
    std::string  cameraName;
};

class RLTargetPBR : public RLTarget {
public:
    RLTargetPBR() = delete;
    RLTargetPBR( std::shared_ptr<CameraRig> cameraRig, const Rect2f& screenViewport, BlitType _bt, Renderer& rr );
    ~RLTargetPBR() override = default;

    void addToCB( CommandBufferList& cb ) override;
    void blit(CommandBufferList& cbl) override;
    std::shared_ptr<Framebuffer> getFrameBuffer( CommandBufferFrameBufferType fbt ) override;
    void resize( const Rect2f& _r ) override;

    [[nodiscard]] bool useSSAO() const;
    [[nodiscard]] bool useDOF() const;
    [[nodiscard]] bool useMotionBlur() const;
    void useSSAO( bool _flag );
    void useDOF( bool _flag );
    void useMotionBlur( bool _flag );
    void enableSunLighting( bool _flag );
    void enableSkybox( bool _value );
    void createSkybox( const SkyBoxInitParams& _skyboxParams );
    bool UseInfiniteHorizonForShadows() const { return mbUseInfiniteHorizonForShadows; }
    void UseInfiniteHorizonForShadows( bool val ) { mbUseInfiniteHorizonForShadows = val; }

    void changeTime( const std::string& time ) override;
    void invalidateOnAdd() override;
    floata& skyBoxDeltaInterpolation();

    const V3f& getProbePosition() const;
    void setProbePosition( const V3f& _probePosition );

    std::shared_ptr<CompositePBR> Composite();
protected:
    void renderDepthMap();
    void renderNormalMap();
    void renderSSAO();
    void addProbes();
    void addProbeToCB( const std::string& _probeCameraName, const Vector3f& _at );
    std::shared_ptr<CameraRig> getProbeRig( int t, const std::string& _probeName, int mipmap );
    void addShadowMaps();
    void calcShadowMapsBBox();
    void cacheShadowMapSunPosition( const Vector3f& _smsp );
    void invalidateShadowMaps();
    void setShadowMapPosition( const Vector3f& _sp );
    [[nodiscard]] V4f mainDirectionLightValue() const;
protected:
    std::shared_ptr<CompositePBR> mComposite;

    bool mbUseInfiniteHorizonForShadows = true;
    Vector3f mCachedSunPosition = Vector3f::ZERO;
    V3f mProbePosition = Vector3f{2.0f, 1.6f, 6.0f};
    SkyBoxInitParams mSkyBoxParams;
    bool bEnableSkyBoxRendering = false;
    bool mbEnableSkybox = false;
    bool mbEnableSunLighting = false;
    bool mbUseSSAO = false;
    bool mbUseDOF = false;
    bool mbUseMotionBlur = false;

    std::shared_ptr<SunBuilder> mSunBuilder;
    std::shared_ptr<Skybox> mSkybox;
    std::shared_ptr<ShadowMapManager> smm;
    std::shared_ptr<ConvolutionEnvironmentMap> mConvolution;
    std::shared_ptr<PrefilterSpecularMap> mIBLPrefilterSpecular;
};


class RenderTargetFactory {
public:
    static std::shared_ptr<RLTarget> make( RenderTargetType rtt,
                                           std::shared_ptr<CameraRig> cameraRig,
                                           const Rect2f& screenViewport,
                                           BlitType _bt,
                                           Renderer& _rr );
};

