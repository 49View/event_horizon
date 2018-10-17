//
// Created by Dado on 2018-10-16.
//

#pragma once

#include <memory>
#include "render_scene_graph/runloop_graphics.h"
#include "render_scene_graph/ui_presenter.hpp"

class EventHorizon {

public:
    EventHorizon( InitializeWindowFlagsT initFlags ) {
        auto l = PresenterLayout::makeDefault();
        l->setInitFlags( initFlags );
        construct( l );
    }

    EventHorizon() {
        construct( PresenterLayout::makeDefault() );
    }

    EventHorizon( std::shared_ptr<PresenterLayout> _layout ) {
        construct( _layout );
    }

private:
    void construct( std::shared_ptr<PresenterLayout> _layout ) {
        presenter = di::make_injector(APP_RSGINJECTOR).template create<std::shared_ptr<UiPresenter>>();
        presenter->Layout( _layout );

        mainLoop( presenter );
    }

private:
    std::shared_ptr<UiPresenter> presenter;
};
