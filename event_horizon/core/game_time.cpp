//
//  GameTime.cpp
//  SixthView
//
//  Created by Dado on 12/01/2015.
//  Copyright (c) 2015 JFDP Labs. All rights reserved.
//

#include "game_time.h"

GameTime::GameTime() {
	mCurrTimeStep = 0.0f;
	mLastTimeStamp = 0.0f;
	mCurrTimeStamp = 0.0f;
	mCurrTimeStep = 0.0f;
	mLastTimeStampTouchDown = 0.0f;
	mPausedTimeStep = 0.0f;
	mStartTimeStamp = std::chrono::system_clock::now();
}
