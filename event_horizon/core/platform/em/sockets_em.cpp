//
// Created by Dado on 29/04/2018.
//

#include "emscripten.h"
#include <emscripten/val.h>
#include <emscripten/bind.h>

#include <iostream>
#include "sio_client.h"
#include "../../http/webclient.h"
#include "core/util.h"

namespace Socket {

    std::unique_ptr<emscripten::val> socket;

    void onSocketEventCallback( std::string _event, std::string _json ) {
        LOGR( "[WEB-SOCKET] Event: %s", _event.c_str() );
        LOGR( "[WEB-SOCKET] Message: %s", _json.c_str() );

        if ( _event == "message" ) {
            auto jt = unescape( trim( _json, '"' ) );
            onMessage( jt );
        }

        if ( _event == "connect" ) {
            LOGR( "[WEB-SOCKET] Client connected to Socket server" );
        }
    }

    EMSCRIPTEN_BINDINGS(my_module) {
        emscripten::function("onSocketEventCallback", &onSocketEventCallback);
    }

    void startClient( const std::string& _host ) {

        LOGR("[WEB-SOCKET] Connecting to host: %s", _host.c_str() );
        socket = std::make_unique<emscripten::val>(emscripten::val::global("SocketClientHandler"));

        if (socket->as<bool>()) {
            LOGR("[WEB-SOCKET] OK - Init");
            socket->call<void>( "connect", _host );
            LOGR("[WEB-SOCKET] Host: %s", _host.c_str());
        } else {
            LOGR("[WEB-SOCKET] Cannot Initialise");
            return;
        }
    }

    void removeClient() {
    }
}
