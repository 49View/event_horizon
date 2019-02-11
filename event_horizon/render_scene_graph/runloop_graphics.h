//
//  runloop.h
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

#pragma once

#include "core/runloop_core.h"
#include "di_modules.h"

class SceneOrchestrator;

class RunLoopGraphics : public RunLoop {
public:
	RunLoopGraphics( CommandQueue& _cq, Renderer& rr, TextInput& ti, MouseInput& mi )
                   : RunLoop( _cq ), rr( rr ), ti( ti), mi( mi) {}
    virtual ~RunLoopGraphics() = default;

	void run();
	void runSingleThread() override;
	void singleThreadLoop();

	void addScriptLine( const std::string& _cmd );
    Renderer& RR() { return rr; }
    TextInput& TI() { return ti; }
	MouseInput& MI() { return mi; }

	void initWindow( std::shared_ptr<SceneOrchestrator> _presenter );

protected:
	void update();
	void render();

protected:
    Renderer& rr;
    TextInput& ti;
    MouseInput& mi;

	int nUpdates = 0;
	int nRenders = 0;
	int nTicks = 0;
	UpdateSignals mUpdateSignals;
	std::shared_ptr<SceneOrchestrator> pm;
};

void mainLoop( std::shared_ptr<SceneOrchestrator> p );
