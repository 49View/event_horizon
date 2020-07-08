#include "../../http/webclient.h"
#include "../../util.h"

#include <restbed>
#include <iomanip>
#include <core/serialization.hpp>
#include <core/file_manager.h>

namespace Http {

    std::shared_ptr<restbed::Request> makeRequestBase( const Url& url ) {
        auto request = std::make_shared<restbed::Request>(restbed::Uri(url.toString()));
//        request->set_header( "User-Agent", "Restbed-native" );
        request->set_header("User-Agent",
                            "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/73.0.3683.103 Safari/537.36");
        request->set_header("Accept", "*/*");
        request->set_header("Host", url.hostOnly());
        request->set_header("Connection", "keep-alive");
        request->set_header("Authorization", Http::userBearerToken());
        request->set_header("eh_aft", Http::AFT());
        return request;
    }

    auto makeSettingsBase() {
        auto settings = std::make_shared<restbed::Settings>();
        auto ssl_settings = std::make_shared<restbed::SSLSettings>();
        ssl_settings->set_client_authentication_enabled(useClientCertificate());
        ssl_settings->set_server_authentication_enabled(useServerCertificate());
        if ( useClientCertificate() ) {
            ssl_settings->set_private_key(restbed::Uri("file://" + clientCertificateKey()));
            ssl_settings->set_certificate(restbed::Uri("file://" + clientCertificateCrt()));
        }
        settings->set_ssl_settings(ssl_settings);
        return settings;
    }

    std::shared_ptr<restbed::Request> makeGetRequest( const Url& url ) {
        auto request = makeRequestBase(url);
        request->set_method("GET");
        return request;
    }

    std::shared_ptr<restbed::Request> makeGetRequestPlainUrl( const std::string& url ) {
        auto request = std::make_shared<restbed::Request>(restbed::Uri(url));
        request->set_header("Host", Http::CLOUD_HOST());
        request->set_method("GET");
        return request;
    }

    std::shared_ptr<restbed::Request>
    makePostRequest( const Url& url, const char *buff, uint64_t length, HttpQuery qt ) {
        auto request = makeRequestBase(url);

        switch ( qt ) {
            case HttpQuery::Binary:
                request->set_header("Content-Type", "application/octet-stream");
                break;
            case HttpQuery::JSON:
            case HttpQuery::Text:
                request->set_header("Content-Type", "application/json");
                break;
        }
        request->set_header("Content-Length", std::to_string(length));
        request->set_method("POST");
        if ( length > 0 ) {
            const restbed::Bytes bodybuffer(buff, buff + length);
            request->set_body(bodybuffer);
        }

        return request;
    }

    void handleResponseCallbacks( Result& lRes, ResponseCallbackFunc callback,
                                  ResponseCallbackFunc callbackFailed,
                                  HttpResouceCB mainThreadCallback) {
        if ( lRes.isSuccessStatusCode() ) {
            lRes.ccf = mainThreadCallback;
            if ( callback ) callback(lRes);
        } else {
            if ( callbackFailed ) callbackFailed(lRes);
        }
    }

    template<typename URL>
    Result handleResponse( const std::shared_ptr<restbed::Response>& response,
                           const URL& url, ResponseFlags rf ) {
        Result res;

        if constexpr ( std::is_same_v<URL, Url> ) {
            res.uri = url.toString();
        } else {
            res.uri = url;
        }
        res.statusCode = response->get_status_code();
        res.length = static_cast<uint64_t>(response->get_header("Content-Length", 0));
        res.contentType = response->get_header("Content-Type", "application/json");
        res.ETag = response->get_header("ETag", "");
        res.lastModified = response->get_header("Content-Last-Modified", "");
        res.flags = rf;
        if ( isSuccessStatusCode(res.statusCode) ) {
            if ( !checkBitWiseFlag(rf, ResponseFlags::HeaderOnly) ) {
                if ( res.length > 0 ) {
                    restbed::Http::fetch(res.length, response);
                    res.setBuffer(reinterpret_cast<const char *>(response->get_body().data()), res.length);
                }
            }
        }
        return res;
    }

    void addToCacheIfNecessary( const Result& lRes, const std::string& uri ) {
        if ( FM::useFileSystemCachePolicy() && lRes.isSuccessStatusCode() ) {
            if ( lRes.buffer ) {
                FM::writeLocalFile(cacheFolder() + url_encode(uri),
                                   reinterpret_cast<const char *>( lRes.buffer.get() ),
                                   lRes.length);
            } else {
                FM::writeLocalFile(cacheFolder() + url_encode(uri),
                                   lRes.BufferString());
            }
        }
    }

    void getInternal( const Url& url,
                      ResponseCallbackFunc callback,
                      ResponseCallbackFunc callbackFailed,
                      ResponseFlags rf,
                      HttpResouceCB mainThreadCallback ) {

        try {
            auto request = makeGetRequest(url);
            auto settings = makeSettingsBase();
            restbed::Http::async(request, [url, rf, callback, callbackFailed, mainThreadCallback](
                                         const std::shared_ptr<restbed::Request> req, const std::shared_ptr<restbed::Response> res ) {
                                     LOGR("[HTTP-GET] Response code: %d - %s - Length: %d", res->get_status_code(),
                                          res->get_status_message().c_str(), res->get_header("Content-Length", 0));
                                     auto lRes = handleResponse(res, url, rf);
                                     addToCacheIfNecessary(lRes, url.uri);
                                     handleResponseCallbacks(lRes, callback, callbackFailed, mainThreadCallback);
                                 },
                                 settings);
        } catch ( const std::exception& ex ) {
            LOGR("[HTTP-GET-RESPONSE][ERROR] on %s", url.toString().c_str());
            LOGR("execption %s %s", typeid(ex).name(), ex.what());
        } catch ( ... ) {
            LOGR("[HTTP-GET-RESPONSE][ERROR] on %s", url.toString().c_str());
        }
    }

    SerializableContainer getSync( const std::string& url ) {
        try {
            std::string fileHash = url_encode(url);
            Result lRes = tryFileInCache(fileHash, url, ResponseFlags::None);
            if ( !lRes.isSuccessStatusCode() ) {
                auto request = makeGetRequestPlainUrl(url_encode_spacesonly(url));
                auto settings = makeSettingsBase();
                std::shared_ptr<restbed::Response> res = restbed::Http::sync(request, settings);
                lRes = handleResponse(res, url, ResponseFlags::None);
                addToCacheIfNecessary(lRes, url);
            }
            return SerializableContainer{ lRes.buffer.get(), lRes.buffer.get() + lRes.length };
        } catch ( const std::exception& ex ) {
            LOGR("[HTTP-GET-RESPONSE][ERROR] on %s", url.c_str());
            LOGR("execption %s %s", typeid(ex).name(), ex.what());
        } catch ( ... ) {
            LOGR("[HTTP-GET-RESPONSE][ERROR] on %s", url.c_str());
        }
        return {};
    }

    void postInternal( const Url& url, const char *buff, uint64_t length, HttpQuery qt,
                       ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed,
                       HttpResouceCB mainThreadCallback ) {
        LOGR("[HTTP-POST] %s", url.toString().c_str());
        LOGR("[HTTP-POST-DATA-LENGTH] %d", length);

        auto request = makePostRequest(url, buff, length, qt);
        auto settings = makeSettingsBase();

        try {
            restbed::Http::async(request,
                                 [&]( [[maybe_unused]] std::shared_ptr<restbed::Request> request,
                                      std::shared_ptr<restbed::Response> res ) {
                                     auto rcode = res->get_status_code();
                                     LOGR("[HTTP-POST] Response code %d - %s ", rcode,
                                          res->get_status_message().c_str());
                                     auto lRes = handleResponse(res, url, ResponseFlags::None);
                                     if ( lRes.isSuccessStatusCode() ) {
                                         if ( callback ) {
                                             lRes.ccf = mainThreadCallback;
                                             callback(lRes);
                                         }
                                     } else {
                                         if ( callbackFailed ) callbackFailed(lRes);
                                     }
                                 }, settings);
        } catch ( const std::exception& ex ) {
            LOGR("[HTTP-POST-RESPONSE][ERROR] on %s", url.toString().c_str());
            LOGR("execption %s %s", typeid(ex).name(), ex.what());
        } catch ( ... ) {
            LOGR("[HTTP-POST-RESPONSE][ERROR] on %s", url.toString().c_str());
        }
    }

    void removeFile( const std::string& _filename ) {
        auto request = makeGetRequest(Url("/fs/remove/" + url_encode(_filename)));
        restbed::Http::async(request, {});
    }

    void listFiles( const std::string& _filename ) {
        auto request = makeGetRequest(Url("/fs/list/" + url_encode(_filename)));
        restbed::Http::async(request, {});
    }

    bool Result::isSuccessStatusCode() const {
        return ::isSuccessStatusCode(statusCode);
    }
}
