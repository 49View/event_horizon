#include "../../http/webclient.h"
#include "../../util.h"
#include <core/file_manager.h>

namespace Http {

    void getInternal( const Url& url,
                      ResponseCallbackFunc callback,
                      ResponseCallbackFunc callbackFailed,
                      [[maybe_unused]] ResponseFlags rf,
                      HttpResouceCB ccf ) {

        std::string fileHash = url_encode( url.uri );
        Result lRes = tryFileInCache( fileHash, url, rf );
//        if ( !lRes.isSuccessStatusCode() ) {
//            res = restbed::Http::sync( request, settings );
//            LOGR( "[HTTP-GET] Response code: %d - %s", res->get_status_code(), res->get_status_message().c_str() );
//            lRes = handleResponse( res, url, rf );
//            if ( FM::useFileSystemCachePolicy() && lRes.isSuccessStatusCode() ) {
//                FM::writeLocalFile( cacheFolder() + fileHash, reinterpret_cast<const char *>( lRes.buffer.get() ), lRes.length );
//            }
//        }

        if ( lRes.isSuccessStatusCode() ) {
            lRes.ccf = ccf;
            if ( callback ) callback( lRes );
        } else {

            if ( callbackFailed ) callbackFailed( lRes );
        }
    }

    void postInternal( const Url& uri, const char *buff, uint64_t length, HttpQuery qt,
                       ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed,
                       HttpResouceCB ccf ) {

        LOGR( "[HTTP-POST] %s", uri.toString().c_str() );
        LOGR( "[HTTP-POST-DATA-LENGTH] %d", length );

//        std::string contenType = qt==HttpQuery::Binary ? "application/octet-stream" : "application/json; charset=utf-8";
    }

    bool Result::isSuccessStatusCode() const {
        return ::isSuccessStatusCode( statusCode );
    }

}