//
// Created by Dado on 2018-10-16.
//

#pragma once

#include <memory>
#include <graphics/render_light_manager.h>
#include <core/di.hpp>
#include <graphics/shader_manager.h>
#include <graphics/render_light_manager.h>
#include <graphics/program_uniform_set.h>
#include <graphics/mouse_input.hpp>
#include <graphics/text_input.hpp>
#include <graphics/render_list.h>
#include <render_scene_graph/runloop_graphics.h>
#include <render_scene_graph/scene_bridge.h>
#include <core/resources/resource_manager.hpp>

#ifdef __EMSCRIPTEN__
#include <graphics/window_handling.hpp>
#include <render_scene_graph/platform/em/runloop_graphics_em.h>
#else
#ifdef ANDROID
#include <render_scene_graph/platform/android/runloop_graphics_android.h>
#else
#include <render_scene_graph/platform/desktop/runloop_graphics_desktop.h>
#endif
#endif

namespace di = boost::di;

template<typename BE>
class EventHorizon {
public:
    explicit EventHorizon( int argc, char *argv[] ) {
        CLIParamMap params{ argc, argv };
        auto hostname = params.getParam("hostname");
        if ( hostname ) {
            Http::cloudHost( *hostname );
        }
#if !defined(__USE_OFFLINE__) && !defined(__EMSCRIPTEN__)
        auto username = params.getParam("username");
        auto password = params.getParam("password");
        auto project  = params.getParam("project");

        if ( username && password && project ) {
            Http::init( LoginFields{*username, *password, *project} );
        } else {
            if (!Http::useClientCertificate(true,
                                            "EH_DEAMON_CERT_KEY_PATH", "EH_DEAMON_CERT_CRT_PATH")) {
                LOGRS("Daemon certificate and key environment variables needs to be present as"
                      "\n$EH_DEAMON_CERT_KEY_PATH\n$EH_DEAMON_CERT_CRT_PATH");
            }
            Socket::createConnection();
        }
#endif

#ifdef __EMSCRIPTEN__
        // We do not do any  Http::init(); or login as that's done client side by react/whatever framework javascript is using
        // on the browser.

        // The ony thing we do is to grab the aft from the session
        auto aft = params.getParam("eh_aft");
        if ( aft ) {
            Http::aft( *aft );
        }
#endif
        auto backEnd = di::make_injector().create<std::unique_ptr<BE>>();
        backEnd->setCLIParams(params);
        mainLoop( params, std::move(backEnd) );
    }
};
