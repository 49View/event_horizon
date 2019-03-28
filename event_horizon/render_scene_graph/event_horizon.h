//
// Created by Dado on 2018-10-16.
//

#pragma once

#include <memory>
#include <core/di.hpp>
#include <poly/resources/image_builder.h>
#include <poly/resources/font_builder.h>
#include <poly/resources/profile_builder.h>
#include <poly/camera_manager.h>
#include <event_horizon/poly/resources/material_builder.h>
#include <graphics/shader_manager.h>
#include <graphics/light_manager.h>
#include <graphics/program_uniform_set.h>
#include <graphics/mouse_input.hpp>
#include <graphics/text_input.hpp>
#include <graphics/render_list.h>
#include <render_scene_graph/runloop_graphics.h>
#include <render_scene_graph/scene_orchestrator.hpp>
#include <render_scene_graph/scene_state_machine.h>

namespace di = boost::di;

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
