#include <utility>

//
// Created by Dado on 01/01/2018.
//

#include "runloop_graphics_desktop.h"
#include <core/di.hpp>
#include <poly/resources/image_builder.h>
#include <graphics/shader_manager.h>
#include <graphics/renderer.h>
#include <graphics/text_input.hpp>
#include <graphics/mouse_input.hpp>
#include <render_scene_graph/scene_orchestrator.hpp>

namespace di = boost::di;

void mainLoop( std::shared_ptr<SceneOrchestrator> p ) {
    auto rl = di::make_injector().create<RunLoopGraphics>();
    rl.initWindow( std::move( p ));
    rl.runSingleThread();
}
