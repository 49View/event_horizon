//
// Created by Dado on 2018-10-16.
//

#pragma once

#include <memory>
#include "render_scene_graph/runloop_graphics.h"
#include "render_scene_graph/scene_orchestrator.hpp"
#include "render_scene_graph/scene_state_machine.h"

template<typename T>
class EventHorizon {
public:
    explicit EventHorizon() {
        Http::init();
//        presenter = di::make_injector(APP_RSGINJECTOR).template create<std::shared_ptr<SceneOrchestrator>>();
        presenter = di::make_injector().create<std::shared_ptr<SceneOrchestrator>>();
        presenter->StateMachine( std::make_shared<T>(presenter.get()) );
        mainLoop( presenter );
    }
private:
    std::shared_ptr<SceneOrchestrator> presenter;
};
