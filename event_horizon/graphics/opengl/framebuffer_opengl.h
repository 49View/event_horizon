#pragma once

#include <string>
#include <unordered_map>

#include "gl_util.h"
#include "core/math/vector2i.h"
#include "core/math/vector4f.h"
#include "core/math/rect2f.h"
#include "core/app_globals.h"

class Texture;
class TextureManager;
class Framebuffer;
class VPList;

//========================================================================
// Framebuffer
// This represents a framebuffer as in OpenGL
// We follow the OpenGL naming for framebuffers and renderbuffers
//========================================================================

class Framebuffer {
public:
	Framebuffer() {}

	void release();

	int getWidth() const {
		return mWidth;
	}

	int getHeight() const {
		return mHeight;
	}

	GLuint Handle() const {
		return mFramebufferHandle;
	}

	std::shared_ptr<VPList> VP() { return mVPListIM; };
	// Init the framebuffer with a texture
	void init( TextureManager& tm );
	void initDepth( TextureManager& tm );
	void attachDepthBuffer();
	void attachColorBuffer( unsigned int index );

	void bind( const std::string& renderTargetIndex = "", const int _mipMapIndex = 0 );
	void bindAndClear( const std::string& renderTargetIndex = "", const int _mipMapIndex = 0  );
	void bindAndClearWithColor( const Color4f& clearColor, const std::string& renderTargetIndex = "", const int _mipMapIndex = 0  );

	Vector4f ClearColor() const { return mClearColor; }
	void ClearColor( const Vector4f& val ) { mClearColor = val; }

	static void clearColorBuffer();
	static void clearColorBufferWithColor( const Vector4f& color );
	static void clearDepthBuffer( const float _clearDepthValue = 1.0f );
	static void enableDepthTest( bool enabled );
	static void blit( std::shared_ptr<Framebuffer> source, std::shared_ptr<Framebuffer> dest,
					  GLenum atthSource, GLenum atthDest );
	static void blitWithRect( std::shared_ptr<Framebuffer> source, std::shared_ptr<Framebuffer> dest,
					  		  GLenum atthSource, GLenum atthDest, Rect2f _sourceRect, Rect2f _destRect  );
	static void setViewport( int x, int y, int width, int height );
	static void setViewport( int width, int height );
	static void setViewport( const Vector2i& size );
	static void setViewport( const Rect2f& rect );
	static JMATH::Rect2f getCurrentViewport();
	static void setSRGB( bool value );

	std::shared_ptr<Texture> RenderToTexture() { return mRenderToTexture; }
	std::shared_ptr<Texture> ColorAttachment1Texture() const { return mColorAttachment1Texture; }

	GLenum framebufferTextureTarget( bool _multisampled );

private:
	GLuint mFramebufferHandle = 0;
	GLuint mRenderbufferHandle = 0; // This is only used if we don't have a texture
    GLuint depthTexture = 0;

	std::string mName;
	int mWidth;
	int mHeight;
	bool mMultisample = false;
	bool mHDR = false;
	bool mCubeMap = false;
	bool mUseMipMaps = false;
	TextureSlots mTextureGPUSlot = TextureSlots::TSLOT_NORMAL;
	Vector4f mClearColor = Vector4f::BLACK;
	std::shared_ptr<Texture> mRenderToTexture;
	std::shared_ptr<Texture> mColorAttachment1Texture;
	std::shared_ptr<VPList>  mVPListIM; // immediate render
	PixelFormat mFormat;

private:
	friend class FrameBufferBuilder;
};

void checkFrameBufferStatus();