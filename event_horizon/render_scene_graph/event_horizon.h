//
// Created by Dado on 2018-10-16.
//

#pragma once

#include <memory>
#include "render_scene_graph/runloop_graphics.h"
#include "render_scene_graph/scene.hpp"
#include "render_scene_graph/scene_layout.h"

class EventHorizon {

public:
    EventHorizon( InitializeWindowFlagsT initFlags ) {
        auto l = SceneLayout::makeDefault();
        l->setInitFlags( initFlags );
        construct( l );
    }

    EventHorizon() {
        construct( SceneLayout::makeDefault() );
    }

    EventHorizon( std::shared_ptr<SceneLayout> _layout ) {
        construct( _layout );
    }

private:
    void construct( std::shared_ptr<SceneLayout> _layout ) {
        presenter = di::make_injector(APP_RSGINJECTOR).template create<std::shared_ptr<Scene>>();
        presenter->Layout( _layout );

        mainLoop( presenter );
    }

private:
    std::shared_ptr<Scene> presenter;
};
