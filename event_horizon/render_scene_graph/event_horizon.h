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
#include <render_scene_graph/scene_bridge.h>
#include <core/resources/resource_manager.hpp>

namespace di = boost::di;

template<typename FE>
class EventHorizon {
public:
    explicit EventHorizon( int argc, char *argv[] ) {
        Http::init();
//        presenter->StateMachine( std::make_shared<T>(presenter.get()) );
//        backEnd = std::make_unique<msm::back::state_machine<BE>>();
//        app->start();
        mainLoop<FE>(checkLayoutArgvs( argc, argv ));
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

    InitializeWindowFlagsT checkLayoutArgvs(int argc, char *argv[]) {
        InitializeWindowFlagsT initFlags =  InitializeWindowFlags::Normal;
        for ( auto t = 1; t < argc; t++ ) {
            std::string arg{argv[t]};
            if  ( auto param = checkLayoutParam( arg ); param ) {
                orBitWiseFlag( initFlags, *param );
            }
        }
        return initFlags;
    }
};
