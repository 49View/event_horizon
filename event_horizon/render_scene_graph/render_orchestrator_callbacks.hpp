//
// Created by Dado on 2019-05-12.
//

#pragma once

#ifdef USE_GLFW
static inline Vector2i callbackResizeWindow = Vector2i(-1, -1);
static inline Vector2i callbackResizeFrameBuffer = Vector2i(-1, -1);

static inline void GDropCallback( [[maybe_unused]] GLFWwindow *window, int count, const char **paths ) {
    ASSERT( count > 0 );

    RenderOrchestrator::callbackPaths.clear();
    for ( auto i = 0; i < count; i++ ) {
        RenderOrchestrator::callbackPaths.emplace_back( std::string(paths[i]) );
    }
}

static inline void GResizeWindowCallback( [[maybe_unused]] GLFWwindow *, int w, int h ) {
    callbackResizeWindow = Vector2i{w, h};
}

static inline void GResizeFramebufferCallback( [[maybe_unused]] GLFWwindow *, int w, int h ) {
    callbackResizeFrameBuffer = Vector2i{w, h};
}
#endif

inline void RenderOrchestrator::initWHCallbacks() {
#ifdef USE_GLFW
    WH::setDropCallback( GDropCallback );
    WH::setResizeWindowCallback( GResizeWindowCallback );
    WH::setResizeFramebufferCallback( GResizeFramebufferCallback );
#endif
}

inline void RenderOrchestrator::resizeCallbacks() {
#ifdef USE_GLFW
    if ( callbackResizeWindow.x() > 0 && callbackResizeWindow.y() > 0 ) {
// For now we do everything in the callbackResizeFrameBuffer so this is redundant for now, just a nop
// to be re-enabled in the future if we need it
//		LOGR("Resized window: [%d, %d]", callbackResizeWindow.x(), callbackResizeWindow.y() );
        callbackResizeWindow = Vector2i{-1, -1};
    }

    if ( callbackResizeFrameBuffer.x() > 0 && callbackResizeFrameBuffer.y() > 0 ) {
        WH::resizeWindow( callbackResizeFrameBuffer );
        WH::gatherMainScreenInfo();
        rr.resetDefaultFB(callbackResizeFrameBuffer);
        resizeCallback( callbackResizeFrameBuffer );
        callbackResizeFrameBuffer = Vector2i{-1, -1};
    }
#endif
}
