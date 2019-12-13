#include "../../http/webclient.h"
#include "../../util.h"

#include <restbed>
#include <iomanip>
#include <core/serialization.hpp>
#include <core/file_manager.h>

namespace Http {

    std::shared_ptr<restbed::Request> makeRequestBase( const Url& url ) {
        auto request = std::make_shared<restbed::Request>( restbed::Uri(url.toString()));
        request->set_header( "User-Agent", "Restbed-native" );
        request->set_header( "Accept", "*/*" );
        request->set_header( "Host", url.hostOnly() );
        request->set_header( "Connection", "keep-alive" );
        request->set_header( "Authorization", Http::userBearerToken() );
//        request->set_header( "x-eventhorizon-guest", Http::project() );
//        request->set_header( "x-eventhorizon-guest-write", Http::project() );
        return request;
    }

    auto makeSettingsBase() {
        auto settings = std::make_shared<restbed::Settings>();
        auto ssl_settings = std::make_shared< restbed::SSLSettings >( );
        ssl_settings->set_client_authentication_enabled( useClientCertificate() );
        ssl_settings->set_server_authentication_enabled( useServerCertificate() );
        if ( useClientCertificate() ) {
            ssl_settings->set_private_key( restbed::Uri( "file://" + clientCertificateKey() ) );
            ssl_settings->set_certificate( restbed::Uri( "file://" + clientCertificateCrt() ) );
        }
        settings->set_ssl_settings( ssl_settings );
        return settings;
    }
    
    std::shared_ptr<restbed::Request> makeGetRequest( const Url& url, const char *buff = nullptr, uint64_t length = 0 ) {
        auto request = makeRequestBase(url);
        request->set_method( "GET" );
//        if ( length > 0 ) {
//            request->set_header( "Content-Type", "application/json; charset=utf-8" );
//            request->set_header( "Content-Length", std::to_string( length ) );
//            const restbed::Bytes bodybuffer(buff, buff + length);
//            request->set_body( bodybuffer );
//        }

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
                request->set_header( "Content-Type", "application/json" );
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
                    res.setBuffer( reinterpret_cast<const char*>(response->get_body().data()), res.length );
                }
            }
        }
        return res;
    }

    void getInternal( const Url& url,
                      const std::string& _data,
                      ResponseCallbackFunc callback,
                      ResponseCallbackFunc callbackFailed,
                      ResponseFlags rf,
                      HttpResouceCB mainThreadCallback ) {

        try {
            std::string fileHash = url_encode( url.uri + _data );
            Result lRes = tryFileInCache( fileHash, url, rf );
            if ( !lRes.isSuccessStatusCode() ) {
                auto request = makeGetRequest( url, _data.data(), _data.size() );
                auto settings = makeSettingsBase();
                std::shared_ptr< restbed::Response > res;
                res = restbed::Http::sync( request, settings );
                LOGR( "[HTTP-GET] Response code: %d - %s", res->get_status_code(), res->get_status_message().c_str() );
                lRes = handleResponse( res, url, rf );
                if ( FM::useFileSystemCachePolicy() && lRes.isSuccessStatusCode() ) {
                    if ( lRes.buffer ) {
                        FM::writeLocalFile( cacheFolder() + fileHash,
                                            reinterpret_cast<const char *>( lRes.buffer.get() ),
                                            lRes.length );
                    } else {
                        FM::writeLocalFile( cacheFolder() + fileHash,
                                            lRes.bufferString );
                    }
                }
            }

            if ( lRes.isSuccessStatusCode() ) {
                lRes.ccf = mainThreadCallback;
                if ( callback ) callback( lRes );
            } else {
                if ( callbackFailed ) callbackFailed( lRes );
            }

//            restbed::Http::async(
//                request, [&]( [[maybe_unused]] std::shared_ptr< restbed::Request > request,
//                              std::shared_ptr< restbed::Response > res) {
//                LOGR( "[HTTP-GET] Response code: %d - %s", res->get_status_code(), res->get_status_message().c_str() );
//                auto lRes = handleResponse( res, url, rf );
//                if ( lRes.isSuccessStatusCode() ) {
//                    lRes.ccf = mainThreadCallback;
//                    if ( callback ) callback( lRes );
//                } else {
//                    if ( callbackFailed ) callbackFailed( lRes );
//                }
//            }, settings );
        } catch ( const std::exception& ex ) {
            LOGR( "[HTTP-GET-RESPONSE][ERROR] on %s", url.toString().c_str());
            LOGR( "execption %s %s", typeid( ex ).name(), ex.what());
        } catch ( ... ) {
            LOGR( "[HTTP-GET-RESPONSE][ERROR] on %s", url.toString().c_str());
        }
    }

    void postInternal( const Url& url, const char *buff, uint64_t length, HttpQuery qt,
                       ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed,
                       HttpResouceCB mainThreadCallback ) {
        LOGR( "[HTTP-POST] %s", url.toString().c_str() );
        LOGR( "[HTTP-POST-DATA-LENGTH] %d", length );

        auto request = makePostRequest( url, buff, length, qt );
        auto settings = makeSettingsBase();

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
                                              lRes.ccf = mainThreadCallback;
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
        auto request = makeGetRequest( Url( "/fs/remove/" + url_encode( _filename )) );
        restbed::Http::async( request, {} );
    }

    void listFiles( const std::string& _filename ) {
        auto request = makeGetRequest( Url( "/fs/list/" + url_encode( _filename )) );
        restbed::Http::async( request, {} );
    }

    bool Result::isSuccessStatusCode() const {
        return ::isSuccessStatusCode( statusCode );
    }
}
