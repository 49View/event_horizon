//
//  runloop.h
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

#pragma once

#include <cstdint>

struct CommandConsole {};
class CommandQueue;

enum class RunLoopThreading {
    Sync,
    ASynch
};

class RunLoop {
public:
    RunLoop( CommandQueue& _cq ) : cq( _cq ) {}
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

template< typename T > void mainLoop( uint64_t _flags = 0, RunLoopThreading rt = RunLoopThreading::Sync );

template <> void mainLoop<CommandConsole>( uint64_t _flags, RunLoopThreading rt );

void updateTime();
void daemonLoop( int _sleepSeconds );