//
//  RunLoop.cpp
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

#include <string>
#include <thread>
#include <unistd.h>

#include "runloop_core.h"

#include "profiler.h"
#include "command.hpp"
#include "game_time.h"

void updateTime( GameTime& gt ) {
	// Calculate time step, make sure it's never too big
	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - gt.mStartTimeStamp;
	gt.mCurrTimeStamp = static_cast<float>( elapsed_seconds.count()) - gt.mPausedTimeStep;
	gt.mCurrTimeStep = ( gt.mCurrTimeStamp - gt.mLastTimeStamp );
	//	if ( gt.mCurrTimeStep > 0.05 ) gt.mCurrTimeStep = 0.0333f;

	gt.mLastTimeStamp = gt.mCurrTimeStamp;
	gt.mPausedTimeStep = 0.0f;
}

template<>
void mainLoop<CommandConsole>( [[maybe_unused]] uint64_t _flags, [[maybe_unused]] RunLoopThreading rt ) {
    RunLoop rl = boost::di::make_injector(boost::di::bind<CommandQueue>().in(boost::di::singleton)).create<RunLoop>();
    rl.runConsolePrompt();
}

void daemonLoop( int _sleepSeconds ) {
	while ( 1 ) {
		sleep( _sleepSeconds );
	}
}
