//
//  RunLoop.cpp
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

#include "runloop_core.h"

#include <core/game_time.h>

void RunLoop::updateTime() {
	// Calculate time step, make sure it's never too big
	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - GameTime::getStartTimeStamp();
	GameTime::setCurrTimeStamp( static_cast<float>( elapsed_seconds.count()) - GameTime::getPausedTimeStep() );
	GameTime::setCurrTimeStep( GameTime::getCurrTimeStamp() - GameTime::getLastTimeStamp() );

	GameTime::setLastTimeStamp(GameTime::getCurrTimeStamp());
	GameTime::setPausedTimeStep(0.0f);
}
