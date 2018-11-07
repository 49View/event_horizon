#include <unordered_map>
#include "../../http/webclient.h"
#include "../../file_manager.h"
#include "../../util.h"

#include <iomanip>
#include <emscripten.h>
#include <emscripten/fetch.h>
#include <emscripten/val.h>

namespace Http {

    int arcIncCallback = 0;
    std::unordered_map<int, std::string> argCallbackMap;

    void onSuccessWget( [[maybe_unused]] unsigned boh, void* arg , void* data, unsigned numBytes ) {
        int ud = reinterpret_cast<int>(arg);
        FileManager::setCallbackData( { argCallbackMap[ud],
                                        reinterpret_cast<const char*>(data),
                                        numBytes, 200 } );
    }

    void onFailWget( [[maybe_unused]] unsigned boh, void *arg, int code, const char* why ) {
        int ud = reinterpret_cast<int>(arg);
        LOGR(" [EM] : Fail to wget %s -- %s", argCallbackMap[ud].c_str(), why );
        FileManager::setCallbackData( { argCallbackMap[ud], nullptr, 0, code } );
    }

    void onProgressWget( [[maybe_unused]] unsigned boh, [[maybe_unused]] void* arg,
                         [[maybe_unused]] int bytesRead, [[maybe_unused]] int totalBytes ) {

    }

    void getInternal( const Url& uri,
                      [[maybe_unused]] const std::function<void( const Http::Result& )> callback,
                      [[maybe_unused]] ResponseFlags rf ) {

        argCallbackMap[arcIncCallback++] = uri.toString();

        emscripten_async_wget2_data(uri.toString().c_str(),
                                    "GET",
                                    nullptr,
                                    reinterpret_cast<void*>(arcIncCallback-1),
                                    false,
                                    onSuccessWget,
                                    onFailWget,
                                    onProgressWget);
    }

    void postInternal( const Url& uri, const char *buff, uint64_t length ) {

        LOGR( "[HTTP-POST] %s", uri.toString().c_str() );
        LOGR( "[HTTP-POST-DATA-LENGTH] %d", length );

        emscripten::val xhr = emscripten::val::global("XMLHttpRequest").new_();
        xhr.call<void>( "open", std::string("POST"), uri.toString() );
        xhr.call<void>( "setRequestHeader", std::string("Content-type"), std::string("application/octet-stream") );
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