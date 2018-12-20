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

#include "runloop_graphics.h"
#include "scene.hpp"
#include "graphics/window_handling.hpp"

bool profileUpdateRenderer = false;

std::atomic_bool mIsClosingFlag;

void RunLoopGraphics::initWindow( std::shared_ptr<Scene> _presenter ) {
	WH::initializeWindow( _presenter->getLayoutInitFlags(), rr );

	rr.init();
	mi.subscribe( _presenter );
	pm = _presenter;
}

void RunLoopGraphics::run() {

	rr.setUseMultiThreadRendering(true);

	std::thread updateThread( &RunLoopGraphics::update, this );

	// main loop
	while ( !WH::shouldWindowBeClosed() ) {
//		rr.CBConsumerLock();
//		{
			renderLoop();
//		}
//		rr.CBConsumerUnlock();
		nRenders++;
	}

	mIsClosingFlag = true;

	if ( updateThread.joinable() )
		updateThread.join();
}

void RunLoopGraphics::singleThreadLoop() {

	elaborateAllSignals();

//	if ( mUpdateSignals.NeedsUpdate() ) {
	WH::imguiUpdateStart();
	pm->enableInputs(WH::isInputEnabled());
	pm->render();
	rr.directRenderLoop();
	WH::imguiUpdateEnd();
	WH::flush();
//	}

	// Timers
	AnimUpdateTimeline::update();
	updateTime();

	nRenders++;
}

void RunLoopGraphics::elaborateAllSignals() {

	mUpdateSignals.NeedsUpdate(false);
	DH::update();
	cq.execute();
	WH::pollEvents();
	WH::imguiUpdate();
	mi.update( mUpdateSignals );
	pm->update( );
}

void RunLoopGraphics::runSingleThread() {

	rr.setUseMultiThreadRendering(false);

	// main loop
	while ( !WH::shouldWindowBeClosed() ) {
		singleThreadLoop();
	}
}

void RunLoopGraphics::updateLoop() {

}

void RunLoopGraphics::update() {

	while ( !mIsClosingFlag.load() ) {
//		rr.CBProducerLock();
//		{
			updateLoop();
//		}
		nUpdates++;
//		rr.CBProducerUnlock();
	}
}

void RunLoopGraphics::renderLoop() {
}

void RunLoopGraphics::addScriptLine( const std::string& _cmd ) {
	cq.script( _cmd );
}

