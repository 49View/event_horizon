//
// Created by Dado on 29/04/2018.
//

#include "emscripten.h"
#include <emscripten/val.h>
#include <emscripten/bind.h>

#include <iostream>
//#include "sio_client.h"
#include "../../http/webclient.h"
#include "core/util.h"

namespace Socket {

    // NDDado: I decided to disable websocket communications in wasm until further investigation and deliberation.
    // WASM code is client side, so it's the javascript from React/WhateverFramework that has a websocket clinet too.
    // By having 2 websocket "users" (wasm, react client) for a single logical session doesn't make sense and it's
    // wrong I believe. To be discusses ( 5th Feb 2020 )

    void onSocketMessageCallback( const char* _json ) {
        std::string jsons{_json};
//        onMessage( unescape( trim( jsons, '"' ) ) );
    }

    void emitImpl( const std::string& _message ) {
//        emscripten_ws_send( _message.c_str() );
    }

    void startClient( const std::string& _host ) {
//        emscripten_ws_init( _host.c_str(), onSocketMessageCallback );
    }

    void close() {}
}
