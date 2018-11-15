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

    void on( const std::string& eventName, std::function<void(const rapidjson::Document&)> f ) {
        static bool connected_first_time = false;
        if ( !connected_first_time ) {
            const std::string host = Url::Host( Url::HttpsProtocol, Http::CLOUD_SERVER(), Http::CLOUD_PORT_SSL() );
            startClient(host);
            connected_first_time = true;
        }

        callbacksMap[eventName] = f;
    }

    void onMessage( const std::string& _message ) {

        LOGR( "[WEB-SOCKET-MESSAGE] %s", _message.c_str());

        rapidjson::Document document;
        document.Parse(_message.c_str());

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
            fi->second(document);
        }
    }
}
