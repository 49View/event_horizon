//
//  render_list.h
//  SixthView
//
//  Created byDado on 11/01/1613.
//  Copyright (c) 2013Dado. All rights reserved.
//

#pragma once

#include <map>
#include <ostream>
#include <atomic>
#include <mutex>
#include "graphic_constants.h"
#include "vertex_processing.h"
#include "skybox.h"

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

using MVList = std::map<std::string, std::shared_ptr<VPList>>;

class RenderStats {
public:
    int NumTriangles() const { return mNumTriangles; }
    int& NumTriangles() { return mNumTriangles; }
    void NumTriangles( int val ) { mNumTriangles = val; }
    int NumVerts() const { return mNumVerts; }
    void NumVerts( int val ) { mNumVerts = val; }
    int& NumVerts() { return mNumVerts; }
    int NumIndices() const { return mNumIndices; }
    void NumIndices( int val ) { mNumIndices = val; }
    int& NumIndices() { return mNumIndices; }
    int NumNormals() const { return mNumNormals; }
    void NumNormals( int val ) { mNumNormals = val; }
    int& NumNormals() { return mNumNormals; }

    //void addMaterial( std::shared_ptr<RenderMaterial> _mat ) { mMaterialSet.insert( _mat ); }
    //int NumMaterials() const { return mNumMaterials; }
    //void NumMaterials( int val ) { mNumMaterials = val; }
    //int& NumMaterials() { return mNumMaterials; }

    int NumGeoms() const { return mNumGeoms; }
    void NumGeoms( int val ) { mNumGeoms = val; }
    int& NumGeoms() { return mNumGeoms; }

    void clear() {
        mNumTriangles = 0;
        mNumVerts = 0;
        mNumIndices = 0;
        mNumNormals = 0;
        mNumGeoms = 0;
    }

private:
    int mNumTriangles;
    int mNumVerts;
    int mNumIndices;
    int mNumNormals;
    //	int mNumMaterials;
    int mNumGeoms;
    //	std::set < std::shared_ptr<RenderMaterial> > mMaterialSet;
};

enum CommandBufferFlags {
    CBF_None = 0,
    CBF_DoNotSort = 1,
};

enum class CommandBufferCommandName {
    nop,
    depthWriteTrue,
    depthWriteFalse,
    depthTestFalse,
    depthTestTrue,
    depthTestLEqual,
    depthTestLess,

    cullModeNone,
    cullModeFront,
    cullModeBack,

    alphaBlendingTrue,
    alphaBlendingFalse,

    wireFrameModeTrue,
    wireFrameModeFalse,

    colorBufferBind,
    colorBufferBindAndClear,
    colorBufferClear,

    clearDefaultFramebuffer,

    setCameraUniforms,
    setGlobalTextures,

    shadowMapBufferBind,
    shadowMapClearDepthBufferZero,
    shadowMapClearDepthBufferOne,

    blitToScreen,
    blitPRB,

    takeScreenShot,

    targetVP,
};

class CommandBufferCommand {
public:
    CommandBufferCommand( CommandBufferCommandName name = CommandBufferCommandName::nop ) : name( name ) {}

    void issue( Renderer& rr, CommandBuffer *sourceCB ) const;

    CommandBufferCommandName name;
};

class CommandBufferEntry {
public:
    int64_t mHash = -1;
    VertexProcessing mVPList;
    std::shared_ptr<RenderMaterial> mMaterial;
    std::shared_ptr<Matrix4f> mModelMatrix;
};

enum class CommandBufferFrameBufferType {
    sourceColor,
    shadowMap,
    finalResolve,
    finalBlit,
    blurVertical,
    blurHorizontal
};

enum class CommandBufferEntryCommandType {
    Comamnd,
    VP
};

class CommandBufferEntryCommand {
public:
    CommandBufferEntryCommand( CommandBufferCommand _command );
    CommandBufferEntryCommand( CommandBufferEntry _vp );

    void run( Renderer& rr, CommandBuffer* cb ) const;

    CommandBufferEntryCommandType Type() const;
    void Type( CommandBufferEntryCommandType type );

    std::string entryName() const;

    std::shared_ptr<CommandBufferEntry>& entry();

private:
    CommandBufferEntryCommandType mType;
    CommandBufferCommand mCommand;
    std::shared_ptr<CommandBufferEntry> mVP;
};

class CommandBuffer {
public:
    CommandBuffer() = default;
    CommandBuffer( CommandBufferFlags flags ) : flags( flags ) {}
    void push( const CommandBufferEntry& entry );
    void push( const CommandBufferCommand& entry );
    void clear();
    void render( Renderer& rr );
    void sort();
    bool findEntry( const std::string& _key, std::weak_ptr<CommandBufferEntry>& _wp );
    void                         postBlit();

    std::shared_ptr<Framebuffer> fb( CommandBufferFrameBufferType fbt );
    Rect2f                       destViewport();
    Rect2f                       sourceViewport();
    std::string                  renderIndex();
    int                          mipMapIndex();

    std::shared_ptr<RLTarget>&   Target() {
        return mTarget;
    }
private:
    std::vector<CommandBufferEntryCommand> mCommandList;

public:
    // Stack
    std::shared_ptr<RLTarget> mTarget;
    std::unique_ptr<char[]> UBOCameraBuffer;
    CommandBufferFlags flags = CommandBufferFlags::CBF_None;
};

class CommandBufferNewEntry {
public:
    std::weak_ptr<VPList> mVPList;
    std::shared_ptr<cpuVBIB> mVBIB;
//    std::string mName;
};

class CommandBufferList {
public:
    CommandBufferList( Renderer& rr ) : rr( rr ) {}

    void start();
    void end();

    void startTarget( std::shared_ptr<Framebuffer> _fbt, Renderer& _rr );

    void pushVP( std::shared_ptr<VertexProcessing> _vp,
                 std::shared_ptr<RenderMaterial> _mat,
                 std::shared_ptr<Matrix4f> _modelMatrix );
    void pushCommand( const CommandBufferCommand& _cmd );

    void startList( std::shared_ptr<RLTarget> _target, CommandBufferFlags flags = CommandBufferFlags::CBF_None );
    void render( int eye );
    void setCameraUniforms( std::shared_ptr<Camera> c0 );
    void getCommandBufferEntry( const std::string& _key, std::weak_ptr<CommandBufferEntry>& wp );

private:
    Renderer& rr;
    CommandBuffer* mCurrent = nullptr;
    std::vector<CommandBuffer> mCommandBuffers;
};

struct CommandBufferListVector {
    CommandBufferFlags flags = CommandBufferFlags::CBF_None;
    std::vector<std::shared_ptr<VPList>> mVList;
    std::vector<std::shared_ptr<VPList>> mVListTransparent;
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
    void addToCBCore( CommandBufferList& cb );
    virtual void invalidateOnAdd() {}
    virtual void changeTime( const V3f& solarTime ) {}
    virtual void addToCB( CommandBufferList& cb ) = 0;
    virtual void startCL( CommandBufferList& fbt ) = 0;
    virtual void endCL( CommandBufferList& fbt ) = 0;
    virtual std::shared_ptr<Framebuffer> getFrameBuffer( CommandBufferFrameBufferType fbt ) = 0;
    virtual void blit( CommandBufferList& cbl ) = 0;
    virtual void resize( const Rect2f& _r ) = 0;

    void clearCB();

    bool isKeyInRange( const int _key, RLClearFlag _clearFlags = RLClearFlag::All ) const;

    bool isTakingScreenShot() {
        if ( mbTakeScreenShot && ++mTakeScreenShotDelay > 2 ) return true;
        return false;
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
    std::shared_ptr<CameraRig> cameraRig;
    Rect2f screenViewport = Rect2f::INVALID;
    std::string renderIndex;
    int mipMapIndex = 0;
    bool bEnabled = true;
    BlitType finalDestBlit = BlitType::OnScreen;
    ScreenShotContainerPtr screenShotContainer;
    std::vector<std::pair<int, int>> bucketRanges;

protected:
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

protected:
    std::shared_ptr<Framebuffer> framebuffer;
};

class RLTargetProbe : public RLTarget {
public:
    RLTargetProbe( const std::string& _cameraRig, const int _faceIndex, Renderer& _rr, int _mipmapIndex = 0 );
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
    void addShadowMaps();
    void renderSkybox();
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
    std::shared_ptr<CubeEnvironmentMap> mConvolution;
    std::shared_ptr<PrefilterSpecularMap> mIBLPrefilterSpecular;
    std::shared_ptr<PrefilterBRDF> mIBLPrefilterBRDF;
};

struct VSGUIDData {
    Color4f color2d;
};

using GUIDT = std::tuple<std::string, VSGUIDData>;

template<typename T>
struct GUIDMapInput {
    uint64_t Hash() const { return as->hash; }

    std::shared_ptr<T> as;
    Color4f color2d;
};

class VPSGUID {
public:
    static VPSGUID& getInstance() {
        static VPSGUID instance; // Guaranteed to be destroyed.
        return instance;// Instantiated on first use.
    }

private:
    VPSGUID() {}
    VPSGUID( VPSGUID const& ) = delete;
    void operator=( VPSGUID const& ) = delete;
public:

    template<typename T>
    std::string add( const GUIDMapInput<T>& _data ) {

        mHashMapCount[_data.Hash()].push_back( { _data.color2d } );

        return GUID( _data.Hash(), mHashMapCount[_data.Hash()].size() );
    }

    template<typename T>
    std::vector<GUIDT> getNames( std::shared_ptr<T> _data ) {
        std::vector<GUIDT> ret;
        size_t size = mHashMapCount[_data->hash].size();

        for ( uint32_t q = 0; q < size; q++ ) {
            uint64_t k1 = _data->hash;
            uint64_t k2 = q+1;
            ret.push_back( { GUID( k1, k2 ), mHashMapCount[_data->hash][q] } );
        }

        return ret;
    }

private:
    std::string GUID( uint64_t k1, uint32_t k2 ) {
        uint64_t z = ((k1+k2)*(k1+k2+1)/2) + k2;
//        uint32_t ci = (( static_cast<uint32_t >(_col.x()*255.0f) & 0xff) << 16) +
//        (( static_cast<uint32_t >(_col.y()*255.0f) & 0xff) << 8) +
//        ( static_cast<uint32_t >(_col.z()*255.0f) & 0xff);
        return prefix + std::to_string( z );
    }

private:
    std::unordered_map<uint64_t, std::vector<VSGUIDData>> mHashMapCount;
    const std::string prefix = "VPG2D";
};

#define VPG VPSGUID::getInstance()
#define VPGADD( X ) VPG.add<X>( { Data(), color2d } )

