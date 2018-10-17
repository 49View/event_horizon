#include "../../http/webclient.h"
#include "../../util.h"

#include <restbed>
#include <iomanip>

namespace Http {

    std::shared_ptr<restbed::Request> makeRequest( const Url& url ) {
        auto request = std::make_shared<restbed::Request>( restbed::Uri(url.toString()));
        request->set_header( "Accept", "*/*" );
        request->set_header( "Host", url.host );
        request->set_header( "Connection", "keep-alive" );
        request->set_method( "GET" );

        return request;
    }

    std::shared_ptr<restbed::Request> makePostRequest( const Url& url, const std::string& _data ) {
        auto request = std::make_shared<restbed::Request>( restbed::Uri( url.toString()));

        request->set_header( "Accept", "*/*" );
        request->set_header( "Host", url.host );
        request->set_header( "Content-Type", "application/json; charset=utf-8" );
        request->set_header( "Content-Length", std::to_string( _data.size()));
        request->set_header( "Connection", "keep-alive" );
        request->set_method( "POST" );
        request->set_body( _data );

        return request;
    }

    std::shared_ptr<restbed::Request> makePostRequest( const Url& url, const char *buff, uint64_t length ) {
        auto request = std::make_shared<restbed::Request>( restbed::Uri( url.toString()) );

        request->set_header( "Accept", "*/*" );
        request->set_header( "Host", url.host );
        request->set_header( "Content-Type", "application/octet-stream" );
        request->set_header( "Content-Length", std::to_string( length ) );
        request->set_method( "POST" );
        request->set_header( "Connection", "keep-alive" );
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
        auto settings = std::make_shared< restbed::Settings >( );
        settings->set_connection_limit( 0 );

        std::shared_ptr< restbed::Response > res;
        try {
            restbed::Http::async(
                    request, [&](std::shared_ptr< restbed::Request > request,
                                 std::shared_ptr< restbed::Response > res) {
                        LOGR( "[HTTP-GET] Response code: %d - %s",
                              res->get_status_code(), res->get_status_message().c_str() );
                        if ( isSuccessStatusCode( res->get_status_code()) ) {
                            callback( handleResponse( res, url, rf ) );
                        }
                    }, settings );
        } catch ( const std::exception& ex ) {
            LOGR( "HTTP SYNC FAILED on %s", url.toString().c_str());
            LOGR( "execption %s %s", typeid( ex ).name(), ex.what());
        } catch ( ... ) {
            LOGR( "HTTP SYNC FAILED on %s", url.toString().c_str());
        }
    }

    void post( const Url& url, const std::string& _data ) {
        auto request = makePostRequest( url, _data );
        auto settings = std::make_shared<restbed::Settings>();
        settings->set_connection_timeout( std::chrono::seconds( 86400 ));

        LOGR( "[HTTP-POST] %s", url.toString().c_str() );
        LOGR( "[HTTP-POST-DATA] %s", _data.c_str() );
        try {
        restbed::Http::async( request,
                              [&](std::shared_ptr< restbed::Request > request,
                                  std::shared_ptr< restbed::Response > res) {
                                  auto rcode = res->get_status_code();
                                  LOGR("[HTTP-POST] Response code %d - %s ", rcode, res->get_status_message().c_str() );
                                  if ( !isSuccessStatusCode(rcode) ) {
                                      LOGR("Reason: %s", res->get_status_message().c_str());
                                  }
                              }, settings );
        } catch ( const std::exception& ex ) {
            LOGR( "HTTP SYNC FAILED on %s", url.toString().c_str());
            LOGR( "execption %s %s", typeid( ex ).name(), ex.what());
        } catch ( ... ) {
            LOGR( "HTTP SYNC FAILED on %s", url.toString().c_str());
        }
    }

    void post( const Url& url, const char *buff, uint64_t length ) {
        LOGR( "[HTTP-POST] %s", url.toString().c_str() );
        LOGR( "[HTTP-POST-DATA-LENGTH] %d", length );

        auto request = makePostRequest( url, buff, length );
        auto settings = std::make_shared<restbed::Settings>();
        settings->set_connection_timeout( std::chrono::seconds( 86400 ));

        restbed::Http::async( request,
                              [&](std::shared_ptr< restbed::Request > request,
                                  std::shared_ptr< restbed::Response > res) {
                                  LOGR("[HTTP-POST] Response code %d - %s ",  res->get_status_code(),
                                          res->get_status_message().c_str() );
                              }, settings );
    }

    void post( const Url& url, const uint8_p& buffer ) {
        post( url, reinterpret_cast<const char*>(buffer.first.get()), buffer.second );
    }

    void post( const Url& url, const std::vector<unsigned  char>& buffer ) {
        post( url, reinterpret_cast<const char*>(buffer.data()), buffer.size() );
    }

    void postFile( const std::string& _filename, const char *buff, uint64_t length, HttpUrlEncode _filenameEnc ) {
        auto fn = _filenameEnc == HttpUrlEncode::Yes ? url_encode( _filename ) : _filename;
        auto request = makePostRequest( Url( "/fs/upload/" + fn ), buff, length );
        auto settings = std::make_shared<restbed::Settings>();
        settings->set_connection_timeout( std::chrono::seconds( 86400 ));

        restbed::Http::async( request, [&](std::shared_ptr< restbed::Request > request,
                                           std::shared_ptr< restbed::Response > res) {
            LOGR( "[HTTP RES %d]: %s", res->get_status_code(), request->get_path().c_str() );
        }, settings );
    }

    void postFile( const std::string& _filename, const std::vector<unsigned char>& _data ) {
        post( Url{HttpFilePrefix::fileupload + url_encode(_filename)}, _data );
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