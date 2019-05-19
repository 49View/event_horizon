//
//  RunLoop.cpp
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

//#include "runloop_graphics_em.h"
#include <emscripten/bind.h>
#include <render_scene_graph/runloop_graphics.h>
#include <render_scene_graph/render_orchestrator.h>
#include <render_scene_graph/render_orchestrator_callbacks.hpp>

RunLoopGraphics rl = di::make_injector().create<RunLoopGraphics>();

std::string addScriptLine( std::string _str ) {
//	rl.addScriptLine( _str );
	return "Enqueued command: " + _str;
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
	emscripten::function("addScriptLine", &addScriptLine);
	emscripten::function("pauseMainLoop", &pauseMainLoop);
	emscripten::function("resumeMainLoop", &resumeMainLoop);
}

int em_resize_callback(int eventType, const EmscriptenUiEvent *uiEvent, void *userData) {
	callbackResizeFrameBuffer = Vector2i{ uiEvent->documentBodyClientWidth, uiEvent->documentBodyClientHeight };
//	LOGR("documentBodyClient size %d, %d: ", uiEvent->documentBodyClientWidth, uiEvent->documentBodyClientHeight );
//	LOGR("windowInner size %d, %d: ", uiEvent->windowInnerWidth, uiEvent->windowInnerHeight );
//	LOGR("windowOuter size %d, %d: ", uiEvent->windowOuterWidth, uiEvent->windowOuterHeight );
    return true;
}

void main_loop_em() {
	rl.singleThreadLoop();
}

void mainLoop( InitializeWindowFlagsT initFlags, std::unique_ptr<RunLoopBackEndBase>&& _be ) {
    emscripten_set_resize_callback(nullptr, nullptr, true, em_resize_callback );
    rl.setBackEnd(std::move(_be));
    rl.init( initFlags );
    emscripten_set_main_loop( main_loop_em, 0, 0 );
}
