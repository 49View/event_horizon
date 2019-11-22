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

class CommandQueue;

enum class RunLoopThreading {
    Sync,
    ASynch
};

class RunLoop {
public:
    explicit RunLoop( CommandQueue& _cq ) : cq( _cq ) {}
	virtual ~RunLoop() = default;

    void coreLoop();
	void coreFunctions();
	virtual void consolePrompt();
	virtual void runConsolePrompt();
    virtual void runSingleThread() {}

    CommandQueue& CQ();

protected:
	void updateTime();

protected:
	bool mbExitTriggered = false;
	CommandQueue& cq;
};

void updateTime();
void daemonLoop( int _sleepSeconds, bool& _awake, std::function<void()> _elaborateFunc );