//
//  GameTime.cpp
//  SixthView
//
//  Created by Dado on 12/01/2015.
//  Copyright (c) 2015 JFDP Labs. All rights reserved.
//

#include "game_time.h"

std::chrono::time_point<std::chrono::system_clock> GameTime::startTimeStamp = std::chrono::system_clock::now();
float GameTime::lastTimeStamp = 0.0f;
float GameTime::currTimeStamp = 0.0f;
float GameTime::currTimeStep = 0.0f;
float GameTime::pausedTimeStep = 0.0f;
