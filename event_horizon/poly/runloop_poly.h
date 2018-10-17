//
//  runloop_poly.h
//  SixthView
//
//  Created byDado on 27/11/2012.
//  Copyright (c) 2012Dado. All rights reserved.
//

#pragma once

#include "core/runloop_core.h"
#include "di_modules.h"
#include "scene_graph.h"

class RunLoopPoly : public Daemon {
public:
	RunLoopPoly( CommandQueue& _cq, PolySceneGraph& _sg )
             : Daemon( _cq ), sg( _sg ) {}
	virtual ~RunLoopPoly() {}

	virtual void run() = 0;

protected:
	SceneGraph& sg;
};

class DaemonPoly : public Daemon {
public:
	DaemonPoly( CommandQueue& _cq, PolySceneGraph& _sg ) : Daemon( _cq ), sg( _sg ) {}
	virtual ~DaemonPoly() {}

protected:
	SceneGraph& sg;
};

template< typename RLP>
void daemonPoly() {
	RLP rl = boost::di::make_injector(APP_PINJECTOR).template create<RLP>();
	rl.runCoreLoop();
}
