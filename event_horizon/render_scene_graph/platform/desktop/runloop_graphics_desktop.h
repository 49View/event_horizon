//
// Created by Dado on 01/01/2018.
//

#pragma once

#include <render_scene_graph/runloop_graphics.h>

inline void mainLoop( InitializeWindowFlagsT initFlags, std::unique_ptr<RunLoopBackEndBase>&& _be ) {
    auto rl = di::make_injector().create<RunLoopGraphics>();
    rl.init( initFlags, std::move(_be) );
    rl.runSingleThread();
}
