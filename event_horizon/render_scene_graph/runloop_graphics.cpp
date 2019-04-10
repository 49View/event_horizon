#include <utility>

//
//  RunLoop.cpp
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

#include <string>
#include <thread>

#include "core/configuration/app_options.h"
#include "core/callback_dependency.h"
#include "core/math/anim.h"

#include "runloop_graphics.h"
#include "scene_orchestrator.hpp"
#include "graphics/window_handling.hpp"

bool profileUpdateRenderer = false;

std::atomic_bool mIsClosingFlag;

void RunLoopGraphics::initWindow( std::shared_ptr<SceneOrchestrator> _presenter ) {
    pm = std::move( _presenter );
	WH::initializeWindow( pm->getLayoutInitFlags(), rr );

	rr.init();
	mi.subscribe( pm );
}

void RunLoopGraphics::run() {
	// This will be use for multithreading rendering
	// to be supported properly within wasm, atm out of the scope of current understanding :D
}

void RunLoopGraphics::update() {

	updateTime();
	WH::preUpdate();
	mUpdateSignals.NeedsUpdate(false);
	Timeline::update();
	DH::update();
	cq.execute();
	pm->enableInputs(WH::isInputEnabled());
	WH::pollEvents();
	mi.update( mUpdateSignals );
	pm->update();
}

void RunLoopGraphics::render() {
	pm->render();
	WH::flush();
}

void RunLoopGraphics::singleThreadLoop() {

	update();
	render();

	nTicks++;
}

void RunLoopGraphics::runSingleThread() {

	pm->activate();
	while ( !WH::shouldWindowBeClosed() ) {
		singleThreadLoop();
	}
	Http::shutDown();

}

void RunLoopGraphics::addScriptLine( const std::string& _cmd ) {
	cq.script( _cmd );
}

