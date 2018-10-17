//
//  RunLoop.cpp
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

#include "../../runloop_graphics.h"
#include "render_scene_graph/di_modules.h"
#include "runloop_graphics_em.h"
#include <emscripten/bind.h>

RunLoopGraphics rl = di::make_injector(APP_GINJECTOR).create<RunLoopGraphics>();

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

void main_loop_em() {
	rl.singleThreadLoop();
}
