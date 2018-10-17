//
//  GameTime.h
//  SixthView
//
//  Created by Dado on 12/01/2015.
//  Copyright (c) 2015 JFDP Labs. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <time.h>
#include <chrono>

class GameTime {
public:
	GameTime();

	std::chrono::time_point<std::chrono::system_clock> mStartTimeStamp;
	std::chrono::time_point<std::chrono::system_clock> mPausedTimeStamp;
	float mLastTimeStamp;
	float mCurrTimeStamp;
	float mCurrTimeStep;
	float mLastTimeStampTouchDown;
	float mDemoTimeStamp;
	float mPausedTimeStep;
};

