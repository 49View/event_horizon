//
// Created by Dado on 01/01/2018.
//

#include "runloop_graphics_desktop.h"
#include "render_scene_graph/scene.hpp"

void mainLoop( std::shared_ptr<Scene> p ) {
    RunLoopGraphics rl = di::make_injector(APP_GINJECTOR).template create<RunLoopGraphics>();
    rl.initWindow( p );
    rl.runSingleThread();
}
