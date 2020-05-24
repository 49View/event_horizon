//
// Created by Dado on 01/01/2018.
//

#pragma once

#include <render_scene_graph/runloop_graphics.h>

inline void mainLoop( const CLIParamMap& params, std::unique_ptr<RunLoopBackEndBase>&& _be ) {
    auto rl = di::make_injector().create<RunLoopGraphics>();
    // We split setBackEnd and init to allow shitty platforms (IE android) to have their own mainloops etc, poor. 
    rl.setBackEnd(std::move(_be));
    rl.init( params );
    rl.runSingleThread();
}
