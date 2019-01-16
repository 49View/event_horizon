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
    explicit EventHorizon( InitializeWindowFlagsT initFlags, const LoginFields& _lf = LoginFields::Computer() ) {
        auto l = SceneLayout::makeDefault();
        l->setInitFlags( initFlags );
        construct( l, _lf );
    }

    explicit EventHorizon( const LoginFields& _lf = LoginFields::Computer() ) {
        construct( SceneLayout::makeDefault(), _lf  );
    }

    explicit EventHorizon( std::shared_ptr<SceneLayout> _layout, const LoginFields& _lf = LoginFields::Computer(), InitializeWindowFlagsT initFlags = InitializeWindowFlags::Normal ) {
        _layout->setInitFlags( initFlags );
        construct( _layout, _lf );
    }

private:
    void construct( std::shared_ptr<SceneLayout> _layout, const LoginFields& _lf ) {
        Http::xProjectHeader( _lf );
        presenter = di::make_injector(APP_RSGINJECTOR).template create<std::shared_ptr<Scene>>();
        presenter->Layout( _layout );
        mainLoop( presenter );
    }

private:
    std::shared_ptr<Scene> presenter;
};
