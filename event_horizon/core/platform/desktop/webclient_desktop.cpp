#include "../../http/webclient.h"
#include "../../util.h"

#include <restbed>
#include <iomanip>
#include <core/serialization.hpp>

namespace Http {

    const std::string userBearerToken() {
        return std::string{"Bearer "} + std::string{Http::userToken()};
    }

    std::shared_ptr<restbed::Request> makeRequestBase( const Url& url ) {
        auto request = std::make_shared<restbed::Request>( restbed::Uri(url.toString()));
        request->set_header( "Accept", "*/*" );
        request->set_header( "Host", url.hostOnly() );
        request->set_header( "Connection", "keep-alive" );
        request->set_header( "Authorization", Http::userBearerToken() );
        request->set_header( "x-eventhorizon-guest", Http::project() );
        return request;
    }

    std::shared_ptr<restbed::Request> makeRequest( const Url& url ) {
        auto request = makeRequestBase(url);
        request->set_method( "GET" );

        return request;
    }

    std::shared_ptr<restbed::Request> makePostRequest( const Url& url, const char *buff, uint64_t length, HttpQuery qt) {
        auto request = makeRequestBase(url);

        switch (qt) {
            case HttpQuery::Binary:
                request->set_header( "Content-Type", "application/octet-stream" );
                break;
            case HttpQuery::JSON:
            case HttpQuery::Text:
                request->set_header( "Content-Type", "application/json; charset=utf-8" );
                break;
        }
        request->set_header( "Content-Length", std::to_string( length ) );
        request->set_method( "POST" );
        if ( length > 0 ) {
            const restbed::Bytes bodybuffer(buff, buff + length);
            request->set_body( bodybuffer );
        }

        return request;
    }

    Result handleResponse( const std::shared_ptr<restbed::Response>& response,
                           const Url& url, ResponseFlags rf ) {
        Result res;

        res.uri = url.toString();
        res.statusCode = response->get_status_code();
        res.length = static_cast<uint64_t>(response->get_header( "Content-Length", 0 ));
        res.contentType = response->get_header( "Content-Type", "application/json" );
        res.ETag = response->get_header( "ETag", "" );
        res.lastModified = response->get_header( "Content-Last-Modified", "" );
        res.flags = rf;
        if ( isSuccessStatusCode( res.statusCode ) ) {
            if ( !checkBitWiseFlag(rf, ResponseFlags::HeaderOnly) ) {
                if ( res.length > 0 ) {
                    restbed::Http::fetch( res.length, response );
                    if ( res.contentType.find("application/json") != std::string::npos ||
                         res.contentType.find("application/text") != std::string::npos ) {
                        std::vector<unsigned char> a = response->get_body();
                        std::stringstream ss;
                        for ( size_t i = 0; i < res.length; i++ ) { ss << a[i]; }
                        res.bufferString = ss.str();
                        orBitWiseFlag( res.flags, ResponseFlags::Text );
                    } else {
                        res.setBuffer( reinterpret_cast<const char*>(response->get_body().data()), res.length );
                    }
                }
            }
        }
        return res;
    }

    void getInternal( const Url& url,
                      ResponseCallbackFunc callback,
                      ResponseCallbackFunc callbackFailed,
                      ResponseFlags rf ) {
        auto request = makeRequest( url );

        auto settings = std::make_shared< restbed::Settings >( );
        settings->set_connection_limit( 5 );

        std::shared_ptr< restbed::Response > res;
        try {
            restbed::Http::async(
                request, [&]( [[maybe_unused]] std::shared_ptr< restbed::Request > request,
                              std::shared_ptr< restbed::Response > res) {
                LOGR( "[HTTP-GET] Response code: %d - %s", res->get_status_code(), res->get_status_message().c_str() );
                auto lRes = handleResponse( res, url, rf );
                if ( lRes.isSuccessStatusCode() ) {
                    if ( callback ) callback( lRes );
                } else {
                    if ( callbackFailed ) callbackFailed( lRes );
                }
            }, settings );
        } catch ( const std::exception& ex ) {
            LOGR( "[HTTP-GET-RESPONSE][ERROR] on %s", url.toString().c_str());
            LOGR( "execption %s %s", typeid( ex ).name(), ex.what());
        } catch ( ... ) {
            LOGR( "[HTTP-GET-RESPONSE][ERROR] on %s", url.toString().c_str());
        }
    }

    void postInternal( const Url& url, const char *buff, uint64_t length, HttpQuery qt,
                       ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed ) {
        LOGR( "[HTTP-POST] %s", url.toString().c_str() );
        LOGR( "[HTTP-POST-DATA-LENGTH] %d", length );

//        auto dataCut = _data.substr(0, 512);
//        if ( _data.size() > 512 ) dataCut += "...";
//        LOGR( "[HTTP-POST-DATA] %s", dataCut.c_str() );

        auto request = makePostRequest( url, buff, length, qt );
        auto settings = std::make_shared<restbed::Settings>();
        settings->set_connection_timeout( std::chrono::seconds( 86400 ));

        try {
            restbed::Http::async( request,
                                  [&]( [[maybe_unused]] std::shared_ptr< restbed::Request > request,
                                      std::shared_ptr< restbed::Response > res) {
                                      auto rcode = res->get_status_code();
                                      LOGR("[HTTP-POST] Response code %d - %s ", rcode,
                                                                                 res->get_status_message().c_str() );
                                      auto lRes = handleResponse( res, url, ResponseFlags::None );
                                      if ( lRes.isSuccessStatusCode() ) {
                                          if ( callback ) {
                                              callback( lRes );
                                          }
                                      } else {
                                          if ( callbackFailed ) callbackFailed( lRes );
                                      }
                                  }, settings );
        } catch ( const std::exception& ex ) {
            LOGR( "[HTTP-POST-RESPONSE][ERROR] on %s", url.toString().c_str());
            LOGR( "execption %s %s", typeid( ex ).name(), ex.what());
        } catch ( ... ) {
            LOGR( "[HTTP-POST-RESPONSE][ERROR] on %s", url.toString().c_str());
        }
    }

    void removeFile( const std::string& _filename ) {
        auto request = makeRequest( Url( "/fs/remove/" + url_encode( _filename )) );
        restbed::Http::async( request, {} );
    }

    void listFiles( const std::string& _filename ) {
        auto request = makeRequest( Url( "/fs/list/" + url_encode( _filename )) );
        restbed::Http::async( request, {} );
    }

    bool Result::isSuccessStatusCode() const {
        return ::isSuccessStatusCode( statusCode );
    }
}
