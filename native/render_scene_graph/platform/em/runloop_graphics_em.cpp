//
//  RunLoop.cpp
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

#include <emscripten/bind.h>
#include <graphics/window_handling.hpp>
#include <render_scene_graph/runloop_graphics.h>
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

    auto ws = WindowHandling::captureWindowSize();
	LOGR("documentBodyClient size %d, %d: ", uiEvent->documentBodyClientWidth, uiEvent->documentBodyClientHeight );
	LOGR("windowInner size %d, %d: ", uiEvent->windowInnerWidth, uiEvent->windowInnerHeight );
	LOGR("windowOuter size %d, %d: ", uiEvent->windowOuterWidth, uiEvent->windowOuterHeight );
    LOGRS("canvas size with retina" << ws );

    GResizeFramebufferCallback( nullptr, ws.x(), ws.y() );

    return true;
}

int em_focus_callback(int eventType, const EmscriptenFocusEvent  *uiEvent, void *userData) {

    auto ws = WindowHandling::captureWindowSize();
    GResizeFramebufferCallback( nullptr, ws.x(), ws.y() );

//    LOGRS("Focus Callback, type: " << eventType << " on element ID: " << uiEvent->id << " and nodeName: " << uiEvent->nodeName);

    return true;
}

void main_loop_em() {
	rl.singleThreadLoop();
}

void mainLoop( const CLIParamMap& params, std::unique_ptr<RunLoopBackEndBase>&& _be ) {
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, em_resize_callback );
    emscripten_set_focus_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, true, em_focus_callback );
    rl.setBackEnd(std::move(_be));
    rl.init( params );
    emscripten_set_main_loop( main_loop_em, 0, 0 );
}
