//
//  RunLoop.cpp
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

//#include "runloop_graphics_em.h"
#include <render_scene_graph/runloop_graphics.h>
#include <render_scene_graph/render_orchestrator.h>
#include <render_scene_graph/render_orchestrator_callbacks.hpp>

RunLoopGraphics rl = di::make_injector().create<RunLoopGraphics>();
void RunLoop::consolePrompt() {}
void RunLoop::runConsolePrompt() {}

#ifdef USE_GLFM

#include <graphics/opengl/mobile/glfm.h>

GLFMDisplay *glfmdisplay = nullptr;

static void onFrame(GLFMDisplay *display, double frameTime);
static void onSurfaceCreated(GLFMDisplay *display, int width, int height);
static void onSurfaceDestroyed(GLFMDisplay *display);
static bool onTouch(GLFMDisplay *display, int touch, GLFMTouchPhase phase, double x, double y);
static bool onKey(GLFMDisplay *display, GLFMKey keyCode, GLFMKeyAction action, int modifiers);

void mainLoop( InitializeWindowFlagsT initFlags, std::unique_ptr<RunLoopBackEndBase>&& _be ) {
    rl.setBackEnd(std::move(_be));

    // We need to initialize GLFM stuff here to handle all their crappy runloop life cycles events
    ASSERT( glfmdisplay );
    glfmSetDisplayConfig(glfmdisplay,
                         GLFMRenderingAPIOpenGLES3,
                         GLFMColorFormatRGBA8888,
                         GLFMDepthFormatNone,
                         GLFMStencilFormatNone,
                         GLFMMultisampleNone);

//    glfmSetUserData(glfmdisplay, app);
    glfmSetMultitouchEnabled(glfmdisplay, true);
    glfmSetSurfaceCreatedFunc(glfmdisplay, onSurfaceCreated);
    glfmSetSurfaceResizedFunc(glfmdisplay, onSurfaceCreated);
    glfmSetSurfaceDestroyedFunc(glfmdisplay, onSurfaceDestroyed);
    glfmSetMainLoopFunc(glfmdisplay, onFrame);
    glfmSetTouchFunc(glfmdisplay, onTouch);
    glfmSetKeyFunc(glfmdisplay, onKey);
}

static bool onTouch(GLFMDisplay *display, int touch, GLFMTouchPhase phase, double x, double y) {
//    LOGRS( "Touch# " << touch << " Phase: " << phase << " [" << x << "," << y << "]" );
    rl.MI().GMouseButtonData.button[touch].action = (phase == 1 || phase == 2) ? MB_PRESS : MB_RELEASE;
    rl.MI().GMouseButtonData.button[touch].xpos = x;
    rl.MI().GMouseButtonData.button[touch].ypos = y;

    return true;
}

static bool onKey(GLFMDisplay *display, GLFMKey keyCode, GLFMKeyAction action, int modifiers) {
    bool handled = false;
//    if (action == GLFMKeyActionPressed) {
//        ExampleApp *app = (ExampleApp*)glfmGetUserData(display);
//        switch (keyCode) {
//            case GLFMKeyLeft:
//                app->offsetX -= 0.1f;
//                handled = true;
//                break;
//            case GLFMKeyRight:
//                app->offsetX += 0.1f;
//                handled = true;
//                break;
//            case GLFMKeyUp:
//                app->offsetY += 0.1f;
//                handled = true;
//                break;
//            case GLFMKeyDown:
//                app->offsetY -= 0.1f;
//                handled = true;
//                break;
//            default:
//                break;
//        }
//    }
    return handled;
}

static bool bSurfaceCreatedCallback = false;

static void onSurfaceCreated(GLFMDisplay *display, int width, int height) {
    GLFMRenderingAPI api = glfmGetRenderingAPI(display);
    V2i sizei{width,height};
    AppGlobals::getInstance().setScreenSizef( Vector2f{width,height});
    AppGlobals::getInstance().setScreenSizei( sizei );

    printf("Hello from GLFM! Using OpenGL %s\n",
           api == GLFMRenderingAPIOpenGLES32 ? "ES 3.2" :
           api == GLFMRenderingAPIOpenGLES31 ? "ES 3.1" :
           api == GLFMRenderingAPIOpenGLES3 ? "ES 3.0" : "ES 2.0");

    bSurfaceCreatedCallback = true;
}

static void onSurfaceDestroyed(GLFMDisplay *display) {
    // When the surface is destroyed, all existing GL resources are no longer valid.
}

static void onFrame(GLFMDisplay *display, double frameTime) {
    if ( bSurfaceCreatedCallback ) {
        rl.init(InitializeWindowFlags::FullScreen);
        bSurfaceCreatedCallback = false;
    }
    rl.singleThreadLoop();
}

#elif USE_GLFVR

#include <graphics/opengl/mobile_vr/glfvr.h>

void mainLoopFunc() {
    rl.singleThreadLoop();
}

void mainLoop( InitializeWindowFlagsT initFlags, std::unique_ptr<RunLoopBackEndBase>&& _be ) {
    rl.setBackEnd(std::move(_be));
}

#endif