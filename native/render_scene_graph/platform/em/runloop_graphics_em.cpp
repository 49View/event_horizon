//
//  RunLoop.cpp
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

//#include "runloop_graphics_em.h"
#include <emscripten/bind.h>
#include <graphics/window_handling.hpp>
#include <render_scene_graph/runloop_graphics.h>
#include <render_scene_graph/render_orchestrator.h>
#include <render_scene_graph/render_orchestrator_callbacks.hpp>

RunLoopGraphics rl = di::make_injector().create<RunLoopGraphics>();

void updateLuaScript( std::string _str ) {
	rl.updateLuaScript( _str );
}

void RunLoop::consolePrompt() {

}

void RunLoop::runConsolePrompt() {

}

void pauseMainLoop() {
	LOGR("Pausing main loop");
	emscripten_pause_main_loop();
}

void resumeMainLoop() {
	LOGR("Resuming main loop");
	emscripten_resume_main_loop();
}

EMSCRIPTEN_BINDINGS(my_module) {
	emscripten::function("addScriptLine", &updateLuaScript);
	emscripten::function("pauseMainLoop", &pauseMainLoop);
	emscripten::function("resumeMainLoop", &resumeMainLoop);
}

int em_resize_callback(int eventType, const EmscriptenUiEvent *uiEvent, void *userData) {
	callbackResizeFrameBuffer = Vector2i{ uiEvent->documentBodyClientWidth, uiEvent->documentBodyClientHeight };
	LOGR("documentBodyClient size %d, %d: ", uiEvent->documentBodyClientWidth, uiEvent->documentBodyClientHeight );
	LOGR("windowInner size %d, %d: ", uiEvent->windowInnerWidth, uiEvent->windowInnerHeight );
	LOGR("windowOuter size %d, %d: ", uiEvent->windowOuterWidth, uiEvent->windowOuterHeight );

    double width{ 1280.0 };
    double height{ 720.0 };
    emscripten_get_element_css_size( nullptr, &width, &height );
    auto pixelRatio = emscripten_get_device_pixel_ratio();
    GResizeFramebufferCallback( nullptr, static_cast<int>(width*pixelRatio), static_cast<int>(height*pixelRatio) );

    return true;
}

int em_focus_callback(int eventType, const EmscriptenFocusEvent  *uiEvent, void *userData) {

    double width{ 1280.0 };
    double height{ 720.0 };
    emscripten_get_element_css_size( nullptr, &width, &height );
    auto pixelRatio = emscripten_get_device_pixel_ratio();
    GResizeFramebufferCallback( nullptr, static_cast<int>(width*pixelRatio), static_cast<int>(height*pixelRatio) );

//    LOGRS("Canvas sizes " << callbackResizeFrameBuffer.x() << "," << callbackResizeFrameBuffer.y() );
//    LOGRS("Focus Callback, type: " << eventType << " on element ID: " << uiEvent->id << " and nodeName: " << uiEvent->nodeName);

    return true;
}

void main_loop_em() {
	rl.singleThreadLoop();
}

void mainLoop( InitializeWindowFlagsT initFlags, std::unique_ptr<RunLoopBackEndBase>&& _be ) {
    emscripten_set_resize_callback(NULL, nullptr, true, em_resize_callback );
    emscripten_set_focus_callback(NULL, nullptr, true, em_focus_callback );
    rl.setBackEnd(std::move(_be));
    rl.init( initFlags );
    emscripten_set_main_loop( main_loop_em, 0, 0 );
}
