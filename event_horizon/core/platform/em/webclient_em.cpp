#include <unordered_map>
#include "../../http/webclient.h"
#include "../../util.h"

#include <iomanip>
#include <emscripten.h>
#include <emscripten/fetch.h>
#include <emscripten/val.h>

namespace Http {

    std::unordered_map<std::string, std::function<void( const Http::Result& )>> argCallbackMap;

    void onSuccessWget( unsigned boh, void* arg , void* data, unsigned numBytes ) {
        auto ckey = reinterpret_cast<char*>(arg);
        auto skey = std::string( ckey );
        LOGR( "[HTTP-GET] Response code: 200, handle %d", boh );
        argCallbackMap[skey]( { skey,
                                reinterpret_cast<const char*>(data),
                                numBytes, 200 } );
        delete [] ckey;
    }

    void onFailWget( [[maybe_unused]] unsigned boh, void *arg, int code, const char* why ) {
        auto ckey = reinterpret_cast<char*>(arg);
        LOGR("[HTTP-GET-RESPONSE][ERROR] handle: %d code: %d URI: %s -- %s", boh, code, ckey, why );
        delete [] ckey;
    }

    void onProgressWget( [[maybe_unused]] unsigned boh, [[maybe_unused]] void* arg,
                         [[maybe_unused]] int bytesRead, [[maybe_unused]] int totalBytes ) {

    }

    void getInternal( const Url& uri,
                      const std::function<void( const Http::Result& )> callback,
                      [[maybe_unused]] ResponseFlags rf ) {

        auto key = uri.toString();
        argCallbackMap[key] = callback;
        char* keyToCharCPassing = new char[key.size()];
        strcpy( keyToCharCPassing, key.c_str() );

        emscripten_async_wget2_data(uri.toString().c_str(),
                                    "GET",
                                    nullptr,
                                    reinterpret_cast<void*>(keyToCharCPassing),
                                    false,
                                    onSuccessWget,
                                    onFailWget,
                                    onProgressWget);
    }

    void postInternal( const Url& uri, const char *buff, uint64_t length, HttpQuery qt ) {

        LOGR( "[HTTP-POST] %s", uri.toString().c_str() );
        LOGR( "[HTTP-POST-DATA-LENGTH] %d", length );

        emscripten::val xhr = emscripten::val::global("XMLHttpRequest").new_();
        xhr.call<void>( "open", std::string("POST"), uri.toString() );
        switch ( qt ) {
            case HttpQuery::Binary:
                xhr.call<void>( "setRequestHeader", std::string("Content-type"), std::string("application/octet-stream") );
                break;
            case HttpQuery::JSON:
            case HttpQuery::Text:
                xhr.call<void>( "setRequestHeader", std::string("Content-type"), std::string("application/json; charset=utf-8") );
                break;
        }
        // NDDado: due to security reasons *Content-length* header has been blocked on modern browsers, DO NOT USE IT
//        xhr.call<void>( "setRequestHeader", std::string("Content-length"), (int)length );

        emscripten::val buffArray = emscripten::val::global("ArrayBuffer").new_((int)length);
        emscripten::val buffI8Array = emscripten::val::global("Int8Array").new_(buffArray);

        for (uint64_t i=0; i< length; i++) {
            buffI8Array.call<void>( "fill", buff[i], (int)i, (int)(i+1) );
        }
        xhr.call<void>( "send", buffArray );
    }

    bool Result::isSuccessStatusCode() const {
        return ::isSuccessStatusCode( statusCode );
    }
}