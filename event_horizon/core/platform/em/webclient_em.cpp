#include <unordered_map>
#include "../../http/webclient.h"
#include "../../util.h"

#include <iomanip>
#include <emscripten.h>
#include <emscripten/fetch.h>
#include <emscripten/val.h>

namespace Http {

    std::unordered_map<std::string, std::function<void( const Http::Result& )>> argCallbackMap;

    bool login() {
        return true;
    }

    void onSuccessWget( unsigned boh, void* arg, int code, void* data, unsigned numBytes ) {
        auto ckey = reinterpret_cast<char*>(arg);
        auto skey = std::string( ckey );
        LOGR( "[HTTP-RESPONSE] code: %d, handle %d, numBytes: %d", code, boh, numBytes );
        if ( argCallbackMap[skey] ) {
            argCallbackMap[skey]( { skey,
                                    reinterpret_cast<const char*>(data),
                                    numBytes, code } );
        }
        delete [] ckey;
    }

    void onFailWget( [[maybe_unused]] unsigned boh, void *arg, int code, const char* why ) {
        auto ckey = reinterpret_cast<char*>(arg);
        LOGR("[HTTP-RESPONSE][ERROR] handle: %d code: %d URI: %s -- %s", boh, code, ckey, why );
        delete [] ckey;
    }

    void onProgressWget( [[maybe_unused]] unsigned boh, [[maybe_unused]] void* arg,
                         [[maybe_unused]] int bytesRead, [[maybe_unused]] int totalBytes ) {

    }

    std::string makeHeaders() {
        std::stringstream ss;

//        ss << "Accept" << " " << "*/*" << " ";
//        ss << "Connection" << " " << "keep-alive" << " ";
//        ss << "Authorization" << " " <<  Http::userBearerToken() );
        ss << "x-eventhorizon-guest" << " " << Http::project();

        return ss.str();
    }

    void getInternal( const Url& uri,
                      const std::function<void( const Http::Result& )> callback,
                      [[maybe_unused]] ResponseFlags rf ) {

        auto key = uri.toString();
        argCallbackMap[key] = callback;
        char* keyToCharCPassing = new char[key.size()];
        strcpy( keyToCharCPassing, key.c_str() );

        emscripten_async_http_request(uri.toString().c_str(),
                                    "GET",
                                    makeHeaders().c_str(),
                                    nullptr,
                                    0,
                                    nullptr,
                                    reinterpret_cast<void*>(keyToCharCPassing),
                                    false,
                                    onSuccessWget,
                                    onFailWget,
                                    onProgressWget,
                                    true );
    }

    void postInternal( const Url& uri, const char *buff, uint64_t length, HttpQuery qt, ResponseCallbackFunc callback ) {

        LOGR( "[HTTP-POST] %s", uri.toString().c_str() );
        LOGR( "[HTTP-POST-DATA-LENGTH] %d", length );

        auto key = uri.toString();
        argCallbackMap[key] = callback;
        char* keyToCharCPassing = new char[key.size()];
        strcpy( keyToCharCPassing, key.c_str() );

        std::string contenType = qt==HttpQuery::Binary ? "application/octet-stream" : "application/json; charset=utf-8";

        emscripten_async_http_request( uri.toString().c_str(),
                                       "POST",
                                       makeHeaders().c_str(),
                                       buff,
                                       static_cast<int>(length),
                                       contenType.c_str(),
                                       reinterpret_cast<void*>(keyToCharCPassing),
                                       false,
                                       onSuccessWget,
                                       onFailWget,
                                       onProgressWget,
                                       true );
    }

    bool Result::isSuccessStatusCode() const {
        return ::isSuccessStatusCode( statusCode );
    }

}