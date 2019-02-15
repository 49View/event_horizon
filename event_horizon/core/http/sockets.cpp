//
// Created by Dado on 29/04/2018.
//

#include <iostream>
#include <unordered_map>
#include <thread>
#include <condition_variable>
#include <rapidjson/document.h>
#include "webclient.h"
#include "core/util.h"

namespace Socket {

    static bool connected_first_time = false;

    void createConnectionIfNotConnected() {
        if ( !Socket::connected_first_time ) {
            if ( !Http::hasUserLoggedIn() ) {
                Http::login();
            }
            Http::UsePort up = Http::isLocalHost() ? Http::UsePort::True : Http::UsePort::False;
            std::string host = Url::Host( Url::WssProtocol, Http::CLOUD_SERVER(), Http::CLOUD_PORT_SSL(), up );
            host += "/?s=" + std::string{Http::sessionId()};
            startClient(host);
            Socket::connected_first_time = true;
        }

    }

    void on( const std::string& eventName, SocketCallbackFunc f ) {
        createConnectionIfNotConnected();
        callbacksMap[eventName] = f;
    }

    void emitImpl( const std::string& _message );
    void emit( const std::string& _message ) {
        createConnectionIfNotConnected();
        emitImpl(_message);
    }

    void onMessage( const std::string& _message ) {

        LOGR( "[WEB-SOCKET-MESSAGE] %s", _message.c_str());

        rapidjson::Document document;
        document.Parse( _message.c_str() );

        if (!document.IsObject()) {
            LOGR( "[WEB-SOCKET] [ERROR] Document not an object" );
            return;
        }
        if (!document.HasMember("msg")) {
            LOGR( "[WEB-SOCKET] [ERROR] Document msg entry missing" );
            return;
        }
        if (!document["msg"].IsString()) {
            LOGR( "[WEB-SOCKET] [ERROR] Document msg NOT a string" );
            return;
        }

        auto msg = document["msg"].GetString();

        if ( auto fi = callbacksMap.find(std::string(msg)); fi != callbacksMap.end() ) {
            fi->second( document );
        }
    }

}
