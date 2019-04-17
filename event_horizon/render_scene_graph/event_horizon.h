//
// Created by Dado on 2018-10-16.
//

#pragma once

#include <memory>
#include <core/di.hpp>
#include <graphics/shader_manager.h>
#include <graphics/light_manager.h>
#include <graphics/program_uniform_set.h>
#include <graphics/mouse_input.hpp>
#include <graphics/text_input.hpp>
#include <graphics/render_list.h>
#include <render_scene_graph/runloop_graphics.h>
#include <render_scene_graph/scene_orchestrator.hpp>
#include <render_scene_graph/scene_state_machine.h>
#include <core/resources/resource_manager.hpp>

namespace di = boost::di;

template<typename T>
class EventHorizon {
public:
    explicit EventHorizon( int argc, char *argv[] ) {
        Http::init();
        presenter = di::make_injector().create<std::shared_ptr<SceneOrchestrator>>();
        presenter->StateMachine( std::make_shared<T>(presenter.get()) );
        checkLayoutArgvs( argc, argv );
        mainLoop( presenter );
    }

private:
    std::optional<InitializeWindowFlagsT> checkLayoutParam( const std::string& _param ) const {
        if ( toLower(_param) == toLower("Normal"       ) ) return InitializeWindowFlags::Normal;
        if ( toLower(_param) == toLower("FullScreen"   ) ) return InitializeWindowFlags::FullScreen;
        if ( toLower(_param) == toLower("Minimize"     ) ) return InitializeWindowFlags::Minimize;
        if ( toLower(_param) == toLower("Maximize"     ) ) return InitializeWindowFlags::Maximize;
        if ( toLower(_param) == toLower("HalfSize"     ) ) return InitializeWindowFlags::HalfSize;
        if ( toLower(_param) == toLower("ThreeQuarter" ) ) return InitializeWindowFlags::ThreeQuarter;

        return std::nullopt;
    }

    void checkLayoutArgvs(int argc, char *argv[]) {
        for ( auto t = 1; t < argc; t++ ) {
            std::string arg{argv[t]};
            if  ( auto param = checkLayoutParam( arg ); param ) {
                presenter->setLayoutInitFlags( *param);
            }
        }
    }

private:
    std::shared_ptr<SceneOrchestrator> presenter;
};
