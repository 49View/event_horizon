//
// Created by Dado on 12/12/2017.
//

#include "framebuffer.h"
#include "core/service_factory.h"
#include "renderer.h"

std::shared_ptr<Framebuffer> FrameBufferBuilder::build() {
    auto ret = ServiceFactory::create<Framebuffer>();

    if ( mName == "default" ) {
        GLint dfb;
        GLCALL ( glGetIntegerv( GL_FRAMEBUFFER_BINDING, &dfb ));
        ret->mFramebufferHandle = static_cast<GLuint >(dfb);
        LOGI( "Default framebuffer: %d", ret->mFramebufferHandle );
        JMATH::Rect2f r = Framebuffer::getCurrentViewport();
        ret->mWidth = static_cast<int>( r.width());
        ret->mHeight = static_cast<int>( r.height());
        return ret;
    }

    ret->mName = mName;
    ret->mFormat = mFormat;
    ret->mUseMipMaps = mUseMipMaps;
    ret->mMultisample = mIsMultisampled;// && canUseMultiSample();
    ret->mHDR = mIsHDR;
    ret->mCubeMap = mIsCubemap;
    ret->mWidth = mWidth;
    ret->mHeight = mHeight;
    ret->mTextureGPUSlot = mTextureGPUSlot;

    if ( mDepthOnly ) {
        ret->initDepth(rr.TM());
        return ret;
    }

    ret->init( rr.TM() );

    if ( mAttachDepth ) ret->attachDepthBuffer();
    for ( const auto& [cbname, index] : mColorBufferAttachments ) {
        ret->attachColorBuffer( rr.TM(), cbname, index );
    }

    if ( !mIMShaderName.empty()) {
        ret->mVPListIM = ServiceFactory::create<VPList>();
        std::string vn = mName + std::to_string(mDestViewport.size().x()) + std::to_string(mDestViewport.size().y());
        VPBuilder<PosTex2dStrip>{rr}.vl(ret->mVPListIM).
                      p(std::make_shared<PosTex2dStrip>( mDestViewport.ss(),
                        QuadVertices2::QUAD_TEX_STRIP_INV_Y_COORDS )).s(mIMShaderName).n(vn).build();
    }

    return ret;
}

FrameBufferBuilder& FrameBufferBuilder::dv( const Rect2f& _destViewport, BlitType _bt ) {
    Vector2f oneOverSize = _bt == BlitType::OnScreen ? (1.0f/getScreenSizef) : (1.0f/_destViewport.size());
    mDestViewport = _destViewport * oneOverSize;
    return *this;
}

