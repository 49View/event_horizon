//
// Created by Dado on 2019-05-12.
//

#pragma once

#ifdef USE_GLFW

struct ResizeData {
    static Vector2i callbackResizeFrameBuffer;
    static Vector2i callbackResizeFrameBufferOld;
};

static inline Vector2i callbackResizeFrameBuffer = Vector2i(-2, -2);
static inline Vector2i callbackResizeFrameBufferOld = Vector2i(-2, -2);

static inline void GSetKeyCallback([[maybe_unused]]GLFWwindow* window, [[maybe_unused]]int key, [[maybe_unused]]int scancode, [[maybe_unused]]int action, [[maybe_unused]] int mods) {
}

static inline void GDropCallback( [[maybe_unused]] GLFWwindow *window, int count, const char **paths ) {
    ASSERT( count > 0 );

    RenderOrchestrator::callbackPaths.clear();
    for ( auto i = 0; i < count; i++ ) {
        RenderOrchestrator::callbackPaths.emplace_back( std::string(paths[i]) );
    }
}

static inline void GResizeFramebufferCallback( [[maybe_unused]] GLFWwindow *, int w, int h ) {
    ResizeData::callbackResizeFrameBuffer = Vector2i{w, h};
}
#endif

inline void RenderOrchestrator::initWHCallbacks() {
#ifdef USE_GLFW
    WH::setKeyCallback( GSetKeyCallback );
    WH::setDropCallback( GDropCallback );
    WH::setResizeFramebufferCallback( GResizeFramebufferCallback );
#endif
}

inline void RenderOrchestrator::resizeCallbacks() {
#ifdef USE_GLFW
//    LOGR("Resizing window: [%d,%d]", ResizeData::callbackResizeFrameBuffer.x(), ResizeData::callbackResizeFrameBuffer.y() );
    if (!(ResizeData::callbackResizeFrameBuffer == getScreenSizei) &&
            (ResizeData::callbackResizeFrameBuffer != ResizeData::callbackResizeFrameBufferOld) ) {
        LOGR("Resizing window: [%d,%d]", ResizeData::callbackResizeFrameBuffer.x(), ResizeData::callbackResizeFrameBuffer.y() );
        WH::resizeWindow( ResizeData::callbackResizeFrameBuffer );
        WH::gatherMainScreenInfo();
        rr.resetDefaultFB(ResizeData::callbackResizeFrameBuffer);
        resizeCallback( ResizeData::callbackResizeFrameBuffer );
        AppGlobals::getInstance().setScreenSizei( ResizeData::callbackResizeFrameBuffer );
        ResizeData::callbackResizeFrameBufferOld = ResizeData::callbackResizeFrameBuffer;
    }
#endif
}
