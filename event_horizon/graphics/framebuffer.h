#pragma once

#include "core/math/vector2i.h"
#include "core/math/vector4f.h"
#include "core/math/rect2f.h"
#include "core/app_globals.h"
#include "graphic_constants.h"

class Framebuffer;
class Renderer;

struct FrameBufferTextureValues {
    FrameBufferTextureTarget2d  targetType = FBT_TEXTURE_2D;
    uint32_t  targetHandle = 0;
    uint32_t  targetMipmap = 0;
    int       width  = 0;
    int       height = 0;
};

class FrameBufferBuilder {
    std::string mName;
    int  mWidth = 0;
    int  mHeight = 0;
    PixelFormat mFormat = PIXEL_FORMAT_RGB;
    TextureSlots mTextureGPUSlot = TextureSlots::TSLOT_COLOR;
    bool mIsMultisampled = false;
    bool mIsHDR = false;
    bool mIsCubemap = false;
    bool mUseMipMaps = false;
    bool mAttachDepth = true;
    bool mDepthOnly = false;
    Rect2f mDestViewport = Rect2f::IDENTITY;
    std::vector<std::pair<std::string, int>> mColorBufferAttachments;
    std::string mIMShaderName;

public:
    FrameBufferBuilder( Renderer& _rr, const std::string& name ) : mName( name ), rr(_rr) {}

    const std::string& getName() const {
        return mName;
    }

    FrameBufferBuilder& name( const std::string& name ) {
        mName = name;
        return *this;
    }

    int getWidth() const {
        return mWidth;
    }

    FrameBufferBuilder& width( int width ) {
        mWidth = width;
        return *this;
    }

    int getHeight() const {
        return mHeight;
    }

    FrameBufferBuilder& height( int height ) {
        mHeight = height;
        return *this;
    }

    template < typename  T >
    FrameBufferBuilder& size( T width ) {
        mWidth = static_cast<int>( width );
        mHeight = static_cast<int>( width );
        return *this;
    }

    FrameBufferBuilder& size( int width, int height ) {
        mWidth = width;
        mHeight = height;
        return *this;
    }

    FrameBufferBuilder& size( const Vector2f& size ) {
        mWidth = static_cast<int>( size.x() );
        mHeight = static_cast<int>( size.y() );
        return *this;
    }

    FrameBufferBuilder& size( const Vector2i& size ) {
        if ( size.x() > 0 && size.y() > 0 ) {
            mWidth = size.x();
            mHeight = size.y();
        }
        return *this;
    }

    bool isMultisampled() const {
        return mIsMultisampled;
    }

    FrameBufferBuilder& multisampled() {
        mIsMultisampled = true;
        return *this;
    }

    bool isHDR() const {
        return mIsHDR;
    }

    FrameBufferBuilder& HDR() {
        mIsHDR = true;
        return *this;
    }

    bool isCubemap() const {
        return mIsCubemap;
    }

    FrameBufferBuilder& cubemap() {
        mIsCubemap = true;
        return *this;
    }

    bool isUseMipMaps() const {
        return mUseMipMaps;
    }

    FrameBufferBuilder& mipMaps() {
        mUseMipMaps = true;
        return *this;
    }

    FrameBufferBuilder& mipMaps(  const bool _value ) {
        mUseMipMaps = _value;
        return *this;
    }

    FrameBufferBuilder& format( PixelFormat _format ) {
        mFormat = _format;
        return *this;
    }

    FrameBufferBuilder& fullScreen() {
        mWidth = getScreenSizei.x();
        mHeight = getScreenSizei.y();
        return *this;
    }

    FrameBufferBuilder& fullScreenScaled( int nTimes) {
        mWidth = getScreenSizei.x() / nTimes;
        mHeight = getScreenSizei.y() / nTimes;
        return *this;
    }

    FrameBufferBuilder& noDepth() {
        mAttachDepth = false;
        return *this;
    }

    FrameBufferBuilder& dv( const Rect2f& _destViewport, BlitType _bt );


    FrameBufferBuilder& depthOnly() {
        mDepthOnly = true;
        return *this;
    }

    FrameBufferBuilder& addColorBufferAttachments( const std::pair<std::string, int>& cba ) {
        mColorBufferAttachments.push_back( cba );
        return *this;
    }

    FrameBufferBuilder& IM( const std::string& _val ) {
        mIMShaderName = _val;
        return *this;
    }

    FrameBufferBuilder& GPUSlot( TextureSlots _val ) {
        mTextureGPUSlot = _val;
        return *this;
    }

    std::shared_ptr<Framebuffer> build();
    std::shared_ptr<Framebuffer> buildSimple();
    cubeMapFrameBuffers buildCube();
private:
    Renderer& rr;
};

bool canUseMultiSample();

#ifdef _OPENGL

#include "opengl/framebuffer_opengl.h"

#endif // VertexProcessing
