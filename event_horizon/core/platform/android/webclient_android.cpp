#include "../../http/webclient.h"
#include "../../util.h"
#include <core/file_manager.h>

namespace Http {

    void getInternal( const Url& url,
                      ResponseCallbackFunc callback,
                      ResponseCallbackFunc callbackFailed,
                      [[maybe_unused]] ResponseFlags rf,
                      HttpDeferredResouceCallbackFunction ccf ) {

        Result lRes;
        std::string fileHash = url_encode( url.uri );
        if ( FM::useFileSystemCachePolicy() ) {
            lRes.buffer = FM::readLocalFile( cacheFolder() + fileHash, lRes.length );
            if ( lRes.length ) {
                lRes.uri = url.uri;
                lRes.statusCode = 200;
            }
        }
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
                       HttpDeferredResouceCallbackFunction ccf ) {

        LOGR( "[HTTP-POST] %s", uri.toString().c_str() );
        LOGR( "[HTTP-POST-DATA-LENGTH] %d", length );

//        std::string contenType = qt==HttpQuery::Binary ? "application/octet-stream" : "application/json; charset=utf-8";
    }

    bool Result::isSuccessStatusCode() const {
        return ::isSuccessStatusCode( statusCode );
    }

}