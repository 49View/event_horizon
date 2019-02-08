//
//  RunLoop.cpp
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

#include "../../runloop_graphics.h"
#include "runloop_graphics_em.h"
#include <emscripten/bind.h>
#include <graphics/di_modules.h>
#include <render_scene_graph/scene.hpp>

RunLoopGraphics rl = di::make_injector(APP_GINJECTOR).template create<RunLoopGraphics>();

std::string addScriptLine( std::string _str ) {
	rl.addScriptLine( _str );
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
	SceneOrchestrator::callbackResizeFrameBuffer = Vector2i{ uiEvent->documentBodyClientWidth, uiEvent->documentBodyClientHeight };
//	LOGR("documentBodyClient size %d, %d: ", uiEvent->documentBodyClientWidth, uiEvent->documentBodyClientHeight );
//	LOGR("windowInner size %d, %d: ", uiEvent->windowInnerWidth, uiEvent->windowInnerHeight );
//	LOGR("windowOuter size %d, %d: ", uiEvent->windowOuterWidth, uiEvent->windowOuterHeight );
    return true;
}

void main_loop_em() {
	rl.singleThreadLoop();
}

void mainLoop( std::shared_ptr<SceneOrchestrator> p ) {

    emscripten_set_resize_callback(nullptr, nullptr, true, em_resize_callback );
	rl.initWindow( p );

//	auto canvas = emscripten::val::global(“window”);
//	canbas.set(“resize”, js::bind(…));

	emscripten_set_main_loop( main_loop_em, 0, 0 );
}