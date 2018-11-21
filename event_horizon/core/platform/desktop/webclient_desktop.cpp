#include "../../http/webclient.h"
#include "../../util.h"

#include <restbed>
#include <iomanip>

namespace Http {

    std::shared_ptr<restbed::Request> makeRequest( const Url& url ) {
        auto request = std::make_shared<restbed::Request>( restbed::Uri(url.toString()));
        request->set_header( "Accept", "*/*" );
        request->set_header( "Host", url.hostOnly() );
        request->set_header( "Connection", "keep-alive" );
        request->set_header( "Authorization", "Bearer eyJhbGciOiJIUzM4NCIsInR5cCI6IkpXVCJ9.eyJ1Ijp7ImkiOiI1YmU3MWFjN2UzZThhNDM1OTQxZTUwNDMiLCJwIjoiNDlWaWV3In0sImlhdCI6MTU0Mjc5NDY0NCwiZXhwIjoxNzA2MDA0MzQyMDI0NCwiYXVkIjoiZXZlbnRob3Jpem9uLnB3IiwiaXNzIjoiZXZlbnRob3Jpem9uLnB3In0._M5lJz168CsQV8QnMdzXGjffW-a-jkTZ7Ms2eSN18lBv4cSDaT9WTMhtequUIgpZ" );
        request->set_method( "GET" );

        return request;
    }

    std::shared_ptr<restbed::Request> makePostRequest( const Url& url, const char *buff, uint64_t length, HttpQuery qt) {
        auto request = std::make_shared<restbed::Request>( restbed::Uri( url.toString()) );

        request->set_header( "Accept", "*/*" );
        request->set_header( "Host", url.hostOnly() );
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
        request->set_header( "Connection", "keep-alive" );
        request->set_header( "Authorization", "Bearer eyJhbGciOiJIUzM4NCIsInR5cCI6IkpXVCJ9.eyJ1Ijp7ImkiOiI1YmU3MWFjN2UzZThhNDM1OTQxZTUwNDMiLCJwIjoiNDlWaWV3In0sImlhdCI6MTU0Mjc5NDY0NCwiZXhwIjoxNzA2MDA0MzQyMDI0NCwiYXVkIjoiZXZlbnRob3Jpem9uLnB3IiwiaXNzIjoiZXZlbnRob3Jpem9uLnB3In0._M5lJz168CsQV8QnMdzXGjffW-a-jkTZ7Ms2eSN18lBv4cSDaT9WTMhtequUIgpZ" );
        const restbed::Bytes bodybuffer(buff, buff + length);
        request->set_body( bodybuffer );

        return request;
    }

    Result handleResponse( const std::shared_ptr<restbed::Response>& response,
                           const Url& url, ResponseFlags rf ) {
        Result res;

        res.uri = url.toString();
        res.statusCode = response->get_status_code();
        res.length = response->get_header( "Content-Length", 0 );
        res.contentType = response->get_header( "Content-Type", "application/json" );
        res.ETag = response->get_header( "ETag", "" );
        res.lastModified = response->get_header( "Content-Last-Modified", "" );
        res.flags = rf;
        if ( isSuccessStatusCode( res.statusCode ) ) {
            if ( !checkBitWiseFlag(rf, ResponseFlags::HeaderOnly) ) {
                if ( res.length > 0 ) {
                    restbed::Http::fetch( res.length, response );
                    if ( res.contentType == "application/json" ||
                         res.contentType == "application/text" ) {
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

    void getInternal( const Url& url, std::function<void(const Http::Result&)> callback, ResponseFlags rf ) {
        auto request = makeRequest( url );
//        auto ssl_settings = std::make_shared< restbed::SSLSettings >( );

//        settings->set_client_authentication_enabled( true );
//        ssl_settings->set_private_key( restbed::Uri( "file:///Users/Dado/Documents/49View/event_horizon/configurations/certificates/client1-key.pem" ));
//        ssl_settings->set_certificate( restbed::Uri( "file:///Users/Dado/Documents/49View/event_horizon/configurations/certificates/client1-crt.pem" ) );
//        ssl_settings->set_certificate_authority_pool( restbed::Uri( "file:///Users/Dado/Documents/49View/event_horizon/configurations/certificates/ca-crt.pem" ) );

//        auto settings = make_shared< Settings >( );
//        settings->set_ssl_settings( ssl_settings );
//

        auto settings = std::make_shared< restbed::Settings >( );
        settings->set_connection_limit( 0 );
//        settings->set_ssl_settings( ssl_settings );

//        try {
//            auto lres = restbed::Http::sync( request, settings );
//            LOGR( "%d", lres->get_status_code() );
//        } catch ( const std::exception& ex ) {
//            LOGR( "HTTP SYNC FAILED on %s", url.toString().c_str());
//            LOGR( "execption %s %s", typeid( ex ).name(), ex.what());
//        } catch ( ... ) {
//            LOGR( "HTTP SYNC FAILED on %s", url.toString().c_str());
//        }

        std::shared_ptr< restbed::Response > res;
        try {
            restbed::Http::async(
                    request, [&]( [[maybe_unused]] std::shared_ptr< restbed::Request > request,
                                 std::shared_ptr< restbed::Response > res) {
                        LOGR( "[HTTP-GET] Response code: %d - %s",
                              res->get_status_code(), res->get_status_message().c_str() );
                        if ( isSuccessStatusCode( res->get_status_code()) ) {
                            callback( handleResponse( res, url, rf ) );
                        }
                    }, settings );
        } catch ( const std::exception& ex ) {
            LOGR( "[HTTP-GET-RESPONSE][ERROR] on %s", url.toString().c_str());
            LOGR( "execption %s %s", typeid( ex ).name(), ex.what());
        } catch ( ... ) {
            LOGR( "[HTTP-GET-RESPONSE][ERROR] on %s", url.toString().c_str());
        }
    }

    void postInternal( const Url& url, const char *buff, uint64_t length, HttpQuery qt ) {
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
                                      if ( !isSuccessStatusCode(rcode) ) {
                                          LOGR("Reason: %s", res->get_status_message().c_str());
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

    bool Ping() {
        auto request = std::make_shared<restbed::Request>( restbed::Uri( "http://192.168.99.100/api/ping/" ));
        request->set_header( "Accept", "application/json" );
        request->set_header( "Host", "192.168.99.100" );

        auto response = restbed::Http::sync( request );

        auto statusCode = response->get_status_code();

        return statusCode == 200;
    }

    bool Result::isSuccessStatusCode() const {
        return ::isSuccessStatusCode( statusCode );
    }

}