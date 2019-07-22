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
#include <render_scene_graph/scene_bridge.h>
#include <core/resources/resource_manager.hpp>

#ifdef __EMSCRIPTEN__
#include <render_scene_graph/platform/em/runloop_graphics_em.h>
#else
#ifdef ANDROID
#include <render_scene_graph/platform/android/runloop_graphics_android.h>
#else
#include <render_scene_graph/platform/desktop/runloop_graphics_desktop.h>
#endif
#endif

namespace di = boost::di;

//#define __USE_OFFLINE__

template<typename BE>
class EventHorizon {
public:
    explicit EventHorizon( int argc, char *argv[] ) {
#ifdef USE_LOCALHOST
        Http::useLocalHost(true);
#endif
#if !defined(__USE_OFFLINE__) && !defined(__EMSCRIPTEN__)
        if constexpr ( BE::hasLF() ) {
            Http::init( BE::loginCert() );
        } else {
            Http::init();
        }
#endif
#ifdef __EMSCRIPTEN__
        if ( argc >= 3 ) {
            LOGR("Setting user token and sessionID ok");
            Http::userToken(argv[1]);
            Http::sessionId(argv[2]);
            Socket::createConnection();
        }
#endif
        auto backEnd = di::make_injector().create<std::unique_ptr<BE>>();
        mainLoop(checkLayoutArgvs( argc, argv ), std::move(backEnd) );
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
