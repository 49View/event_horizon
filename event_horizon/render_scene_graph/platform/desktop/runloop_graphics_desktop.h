//
// Created by Dado on 01/01/2018.
//

#pragma once

#include "../../di_modules.h"
#include <core/runloop_core.h>
#include <render_scene_graph/runloop_graphics.h>

//template <typename T> void mainLoop( uint64_t _flags, RunLoopThreading rt ) {
//    RunLoopGraphics rl = di::make_injector(APP_GINJECTOR).template create<RunLoopGraphics>();
//    rl.initWindow( di::make_injector(APP_RSGINJECTOR).template create<std::shared_ptr<T>>(), _flags );
//
//    if ( rt == RunLoopThreading::Sync ) {
//        rl.runConsolePrompt();
//        rl.runSingleThread();
//    } else {
//        rl.run();
//    }
//}

void mainLoop( std::shared_ptr<UiPresenter> p );
