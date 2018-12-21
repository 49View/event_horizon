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
#include <string>

class GameTime {
public:
    static const std::chrono::time_point<std::chrono::system_clock>& getStartTimeStamp() {
        return startTimeStamp;
    }

    static float getLastTimeStamp() {
        return lastTimeStamp;
    }

    static float getCurrTimeStamp() {
        return currTimeStamp;
    }

    static std::string getCurrTimeStampString() {
        return std::to_string(currTimeStamp);
    }

    static float getCurrTimeStep() {
        return currTimeStep;
    }

    static float getPausedTimeStep() {
        return pausedTimeStep;
    }

private:
    static void setLastTimeStamp( float lastTimeStamp ) {
        GameTime::lastTimeStamp = lastTimeStamp;
    }

    static void setCurrTimeStamp( float currTimeStamp ) {
        GameTime::currTimeStamp = currTimeStamp;
    }

    static void setCurrTimeStep( float currTimeStep ) {
        GameTime::currTimeStep = currTimeStep;
    }

    static void setPausedTimeStep( float pausedTimeStep ) {
        GameTime::pausedTimeStep = pausedTimeStep;
    }

private:
    static std::chrono::time_point<std::chrono::system_clock> startTimeStamp;
	static float lastTimeStamp;
	static float currTimeStamp;
    static float currTimeStep;
	static float pausedTimeStep;

	friend class RunLoop;
};

