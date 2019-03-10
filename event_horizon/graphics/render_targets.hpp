//
// Created by Dado on 2019-03-09.
//

#pragma once

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
struct FrameBufferTextureValues;

enum class RenderTargetType {
    Plain,
    PBR,
    Cubemap,
    Probe
};

enum class RLClearFlag {
    DontIncludeCore,
    All
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

    void blit( CommandBufferList& cbl ) override;

private:
    void bloom();

private:
    bool mbUseBloom = false;
    std::shared_ptr<Framebuffer> mColorFinalFB;
    std::shared_ptr<Framebuffer> mBlurHorizontalFB;
    std::shared_ptr<Framebuffer> mBlurVerticalFB;
};

class RLTarget : public std::enable_shared_from_this<RLTarget> {
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
    virtual void changeTime( const V3f& solarTime ) {}
    virtual void addToCB( CommandBufferList& cb ) = 0;
    virtual void startCL( CommandBufferList& fbt ) = 0;
    virtual void endCL( CommandBufferList& fbt ) = 0;
    virtual std::shared_ptr<Framebuffer> getFrameBuffer( CommandBufferFrameBufferType fbt ) { return framebuffer; }
    virtual void blit( CommandBufferList& cbl ) = 0;
    virtual void resize( const Rect2f& _r ) = 0;

    void clearCB();

    bool isKeyInRange( const int _key, RLClearFlag _clearFlags = RLClearFlag::All ) const;

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

public:
    std::shared_ptr<CameraRig> cameraRig = nullptr;
    std::shared_ptr<Framebuffer> framebuffer = nullptr;
    Rect2f screenViewport = Rect2f::INVALID;
    bool bEnabled = true;
    BlitType finalDestBlit = BlitType::OnScreen;
    ScreenShotContainerPtr screenShotContainer;
    std::vector<std::pair<int, int>> bucketRanges;

protected:
    void updateStreamPacket( const std::string& _streamName );
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
    void startCL( CommandBufferList& fbt ) override;
    void endCL( CommandBufferList& fbt ) override;
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
    void startCL( [[maybe_unused]] CommandBufferList& fbt ) override {};
    void endCL( [[maybe_unused]] CommandBufferList& fbt ) override {}
    void resize( const Rect2f& _r ) override;
};

class RLTargetCubeMap : public RLTarget {
public:
    RLTargetCubeMap( const CubeMapRigContainer& _rig, std::shared_ptr<Framebuffer> _fb, Renderer& _rr );
    ~RLTargetCubeMap() override = default;
    void addToCB( [[maybe_unused]] CommandBufferList& cb ) override {}
    virtual void blit( [[maybe_unused]] CommandBufferList& cbl) override {};
    void startCL( CommandBufferList& fbt ) override;
    void render( std::shared_ptr<Texture> _renderToTexture, int cmsize, int mip, CubeMapRenderFunction rcb );
    void endCL( [[maybe_unused]] CommandBufferList& fbt ) override {}
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
    void startCL( CommandBufferList& fbt ) override;;
    void endCL( [[maybe_unused]] CommandBufferList& fbt ) override {}
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
    void startCL( CommandBufferList& fbt ) override;
    void endCL( CommandBufferList& fbt ) override;
    void resize( const Rect2f& _r ) override;

    bool UseInfiniteHorizonForShadows() const { return mbUseInfiniteHorizonForShadows; }
    void UseInfiniteHorizonForShadows( bool val ) { mbUseInfiniteHorizonForShadows = val; }

    std::shared_ptr<Framebuffer> getShadowMapFB();

    void changeTime( const V3f& _solarTime ) override;
    void invalidateOnAdd() override;
protected:
    std::shared_ptr<Skybox> createSkybox();
    void addProbes();
    void addProbeToCB( const std::string& _probeCameraName, const Vector3f& _at );
    std::shared_ptr<CameraRig> getProbeRig( int t, const std::string& _probeName, int mipmap );
    void addShadowMaps();
    void cacheShadowMapSunPosition( const Vector3f& _smsp );
    void invalidateShadowMaps();
    void setShadowMapPosition( const Vector3f& _sp );

protected:
    std::shared_ptr<CompositePBR> mComposite;

    std::shared_ptr<Framebuffer> mShadowMapFB;
    bool mbUseInfiniteHorizonForShadows = true;
    Vector3f mCachedSunPosition = Vector3f::ZERO;
    SkyBoxInitParams mSkyBoxParams;

    std::shared_ptr<Skybox> mSkybox;
    std::shared_ptr<ShadowMapManager> smm;
    std::shared_ptr<ConvolutionEnvironmentMap> mConvolution;
    std::shared_ptr<PrefilterSpecularMap> mIBLPrefilterSpecular;
    std::shared_ptr<PrefilterBRDF> mIBLPrefilterBRDF;
};


class RenderTargetFactory {
public:
    static std::shared_ptr<RLTarget> make( RenderTargetType rtt,
                                           std::shared_ptr<CameraRig> cameraRig,
                                           const Rect2f& screenViewport,
                                           BlitType _bt,
                                           Renderer& _rr );
};
