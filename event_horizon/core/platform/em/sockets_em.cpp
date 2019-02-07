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

    void onSocketMessageCallback( const char* _json ) {
        std::string jsons{_json};
        onMessage( unescape( trim( jsons, '"' ) ) );
    }

    void emitImpl( const std::string& _message ) {
        emscripten_ws_send( _message.c_str() );
    }

    void startClient( const std::string& _host ) {
        emscripten_ws_init( _host.c_str(), onSocketMessageCallback );
    }

    void removeClient() {}
}
