//
//  RunLoop.cpp
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

#include <string>
#include <thread>
#include <atomic>
#include <unistd.h>

#include "runloop_core.h"

#include <core/di.hpp>
#include <core/profiler.h>
#include <core/command.hpp>
#include <core/game_time.h>
#include <core/http/webclient.h>

void RunLoop::updateTime() {
	// Calculate time step, make sure it's never too big
	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - GameTime::getStartTimeStamp();
	GameTime::setCurrTimeStamp( static_cast<float>( elapsed_seconds.count()) - GameTime::getPausedTimeStep() );
	GameTime::setCurrTimeStep( GameTime::getCurrTimeStamp() - GameTime::getLastTimeStamp() );

	GameTime::setLastTimeStamp(GameTime::getCurrTimeStamp());
	GameTime::setPausedTimeStep(0.0f);
}

std::atomic<bool> deamonBreaker = false;

void daemonLoop( int _sleepSeconds, bool& _awake, std::function<void()> _elaborateFunc ) {

//	auto st = std::chrono::system_clock::now();

	while ( !deamonBreaker ) {
		sleep( _sleepSeconds );
		if ( _awake ) {
		    _elaborateFunc();
		}
//		auto tn = std::chrono::system_clock::now();
//		std::chrono::duration<double> elapsed = tn - st;
//		if ( elapsed.count() > 60*60*24*6.9 ) { // // refresh every week
//			LOGR("Refresh token");
//			Http::refreshToken();
//			st = tn;
//		}
	}
}
