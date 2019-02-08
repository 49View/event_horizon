//
// Created by Dado on 01/01/2018.
//

#include "runloop_graphics_desktop.h"
#include "render_scene_graph/scene.hpp"
#include <graphics/di_modules.h>

void mainLoop( std::shared_ptr<SceneOrchestrator> p ) {
    auto rl = di::make_injector(APP_GINJECTOR).template create<RunLoopGraphics>();
    rl.initWindow( p );
    rl.runSingleThread();
}
