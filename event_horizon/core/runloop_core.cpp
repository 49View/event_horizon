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

#include "profiler.h"
#include "command.hpp"
#include "game_time.h"
#include "http/webclient.h"

void RunLoop::updateTime() {
	// Calculate time step, make sure it's never too big
	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - GameTime::getStartTimeStamp();
	GameTime::setCurrTimeStamp( static_cast<float>( elapsed_seconds.count()) - GameTime::getPausedTimeStep() );
	GameTime::setCurrTimeStep( GameTime::getCurrTimeStamp() - GameTime::getLastTimeStamp() );

	GameTime::setLastTimeStamp(GameTime::getCurrTimeStamp());
	GameTime::setPausedTimeStep(0.0f);
}

template<>
void mainLoop<CommandConsole>( [[maybe_unused]] uint64_t _flags, [[maybe_unused]] RunLoopThreading rt ) {
	auto rl = boost::di::make_injector(boost::di::bind<CommandQueue>().in(boost::di::singleton)).create<RunLoop>();
    rl.runConsolePrompt();
}

std::atomic<bool> deamonBreaker = false;

void daemonLoop( int _sleepSeconds ) {

    Http::xProjectHeader(LoginFields::Daemon());
	Http::login(LoginFields::Daemon());

	auto st = std::chrono::system_clock::now();

	while ( !deamonBreaker ) {
		sleep( _sleepSeconds );
		auto tn = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed = tn - st;
		if ( elapsed.count() > 60*60*5.9 ) {
			Http::login(LoginFields::Daemon());
			st = tn;
		}
	}
}
