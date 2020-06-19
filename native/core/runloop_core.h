//
//  runloop.h
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

#pragma once

#include <cstdint>
#include <functional>

enum class RunLoopThreading {
    Sync,
    ASynch
};

class RunLoop {
public:
    RunLoop() = default;
	virtual ~RunLoop() = default;

    void coreLoop();
	void coreFunctions();
	virtual void consolePrompt();
	virtual void runConsolePrompt();
    virtual void runSingleThread() {}

protected:
	void updateTime();

protected:
	bool mbExitTriggered = false;
};

void updateTime();
void daemonLoop( int _sleepSeconds, bool& _awake, std::function<void()> _elaborateFunc );