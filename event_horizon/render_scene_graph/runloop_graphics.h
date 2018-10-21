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

class UiPresenter;

class RunLoopGraphics : public RunLoop {
public:
	RunLoopGraphics( CommandQueue& _cq, UiControlManager& _uicm, Renderer& rr, TextInput& ti, MouseInput& mi )
                   : RunLoop( _cq ), rr( rr ), ti( ti), mi( mi) {}

	void run();
	void runSingleThread();
	void singleThreadLoop();

	virtual ~RunLoopGraphics() {}

	void addScriptLine( const std::string& _cmd );
    Renderer& RR() { return rr; }
    TextInput& TI() { return ti; }
	MouseInput& MI() { return mi; }

	void initWindow( std::shared_ptr<UiPresenter> _presenter );

protected:
	void elaborateAllSignals();
	void update();

	void updateLoop();
	void renderLoop();

protected:
    Renderer& rr;
    TextInput& ti;
    MouseInput& mi;

	int nUpdates = 0;
	int nRenders = 0;
	GameTime update_gt;
	UpdateSignals mUpdateSignals;
	std::shared_ptr<UiPresenter> pm;
};

void mainLoop( std::shared_ptr<UiPresenter> p );
