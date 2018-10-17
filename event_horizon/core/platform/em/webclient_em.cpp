#include <unordered_map>
#include "../../http/webclient.h"
#include "../../file_manager.h"
#include "../../util.h"

#include <iomanip>
#include <emscripten.h>
#include <emscripten/fetch.h>

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

    void post( const Url& url, const std::string& _data ) {

        std::string script = std::string( R"( var http = new XMLHttpRequest();
                var url = ")" );
        script += url.toString();
        script += std::string( R"("; var params = JSON.stringify()" );
        script += _data;
        script += std::string( R"();
                http.open("POST", url, true);
                http.setRequestHeader("Content-type", "application/json");
                http.onreadystatechange = function() {
                    if(http.readyState == 4 && http.status == 200) {
                        //alert(http.responseText);
                    }
                }
                http.send(params);
        )" );

//        LOGR( "HTTP Post script: %s", script.c_str() );

        emscripten_run_script( script.c_str());
    }

    void postFile( const std::string& _filename, const char *buff, uint64_t length ) {
        LOGE( "Unimplemented postFile" );
    }

    void postFile( const std::string& _filename, const std::vector<unsigned char>& _data ) {
        LOGE( "Unimplemented postFile" );
    }

    void getFile( const std::string& _filename, const bool addTrailingZero ) {
        LOGE( "Unimplemented getFile" );

//        return getRaw( Url( "/fs/get/" + url_encode( _filename )), addTrailingZero );
    }

    void getFileHeader( const std::string& _filename ) {
        LOGE( "Unimplemented getFileHeader" );

    }

    void removeFile( const std::string& _filename ) {
        LOGE( "Unimplemented removeFile" );

//        auto request = makeRequest( Url( "/fs/remove/" + url_encode( _filename )) );
//        auto result = restbed::Http::sync( request );
//        return responseHeader( result );
    }

    void listFiles( const std::string& _filename ) {
        LOGE( "Unimplemented listFiles" );
//        auto request = makeRequest( Url( "/fs/list/" + url_encode( _filename )) );
//        auto result = restbed::Http::sync( request );
//        if ( isSuccessStatusCode( result->get_status_code()) ) {
//            return resultToString( result );
//        }
//        return "";
    }

    void getRaw( const Url& url, const bool addTrailingZero ) {
        LOGE( "Unimplemented getRaw" );

    }

    bool Result::isSuccessStatusCode() const {
        return ::isSuccessStatusCode( statusCode );
    }
}