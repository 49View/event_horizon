#include "webclient.h"

#include <iomanip>
#include <unordered_set>

#include "../util.h"
#include "../platform_util.h"
#include "core/zlib_util.h"
#include "core/string_util.h"
#include "core/file_manager.h"

static bool sUserLoggedIn = false;
static bool sUseClientCertificate = false;
static bool sUseServerCertificate = false;
static std::string sClientCertificateCrtFilename{};
static std::string sClientCertificateKeyFilename{};
static std::string sProject;
static std::string sAFT;
static std::string sUserToken;
static std::string sUserSessionId;
static std::string sCloudHost;
static LoginFields sCachedLoginFields;

const std::string Url::WsProtocol = "ws";
const std::string Url::WssProtocol = "wss";
const std::string Url::HttpProtocol = "http";
const std::string Url::HttpsProtocol = "https";

namespace zlibUtil {
    // Decompress
    SerializableContainer inflateFromMemory( const Http::Result& _fin ) {
        return inflateFromMemory( uint8_p{std::move(_fin.buffer), _fin.length} );
    }
}

Url::Url( std::string _uri ) : uri( _uri ) {
}

std::smatch Url::parseUrl( const std::string& _fullurl ) {
    std::regex regv = std::regex( "(https?):\\/{2}(\\w.*?)(:?\\d*)\\/(.*)" );
    std::smatch base_match;
    std::regex_search( _fullurl, base_match, regv );

    return base_match;
}

bool Url::parsedMatchIsUrl( const std::smatch& base_match ) {
    return ( base_match.size() == 5 );
}

void Url::fromString( const std::string& _fullurl ) {

    std::smatch base_match = parseUrl( _fullurl );
    if ( base_match.size() == 5 ) {
        auto protocol = base_match[1].str();
        host = base_match[2].str();
        uri = "/" + base_match[4].str();
    }
}

std::string Url::hostOnly() const {
    auto ret = host;
    auto p = ret.find_first_of("/");
    if ( p != std::string::npos ) {
       ret = { ret.begin(), ret.begin()+p };
    }

    return ret;
}

std::string Url::toString() const {
    return Http::CLOUD_PROTOCOL() + "://" + host + uri;
}

Url Url::privateAPI( const std::string& _params ) {
    auto luri = _params.at(0) == '/' ? _params : "/" + _params;
    return Url{ "/" + Http::project() + luri };
}

std::string Url::entityURLParams( const std::string& _key, const std::string& _name ) {
    return url_encode_spacesonly( ( _name.empty() ? _key : ( _key + "/"+_name) ) );
}

Url Url::entityMetadata( const std::string& _key, const std::string& _name ) {
    return Url( HttpFilePrefix::entities_all + entityURLParams( _key, _name) );
}

Url Url::entityContent( const std::string& _key, const std::string& _name ) {
    return Url( HttpFilePrefix::entities_onebinary + entityURLParams( _key, _name) );
}

std::string url_encode( const std::string& value ) {
    std::ostringstream escaped;
    escaped.fill( '0' );
    escaped << std::hex;

    for ( std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i ) {
        std::string::value_type c = ( *i );

        // Keep alphanumeric and other accepted characters intact
        if ( isalnumCC( c ) || c == '-' || c == '_' || c == '.' || c == '~' ) {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << std::setw( 2 ) << int((unsigned char) c );
        escaped << std::nouppercase;
    }

    return escaped.str();
}

std::string url_encode_spacesonly( const std::string& value ) {
    std::ostringstream escaped;
    escaped.fill( '0' );
    escaped << std::hex;

    for ( std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i ) {
        std::string::value_type c = ( *i );

        // Keep alphanumeric and other accepted characters intact
        if ( c == ' ' ) {
            // Any other characters are percent-encoded
            escaped << std::uppercase;
            escaped << '%' << std::setw( 2 ) << int((unsigned char) c );
            escaped << std::nouppercase;
        } else {
            escaped << c;
        }
    }

    return escaped.str();
}


std::string url_decode(const std::string& str) {
    std::string ret;
    char ch;
    int i, ii, len = str.length();

    for (i=0; i < len; i++){
        if(str[i] != '%'){
            if(str[i] == '+')
                ret += ' ';
            else
                ret += str[i];
        }else{
            sscanf(str.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret += ch;
            i = i + 2;
        }
    }
    return ret;
}

bool isSuccessStatusCode( int statusCode ) {
    return statusCode >= 200 && ( statusCode - 200 ) < 100;
} //all 200s

namespace Http {

    static std::string PUT() { return "PUT"; }
    static std::string POST() { return "POST"; }

    static std::unordered_set<std::string> requestCache;

    void clearRequestCache() {
        requestCache.clear();
    }

    Result tryFileInCache( const std::string& fileHash, const std::string& uri, ResponseFlags rf ) {
        Result lRes{};
        if ( FM::useFileSystemCachePolicy() && !checkBitWiseFlag(rf, ResponseFlags::ExcludeFromCache)) {
            lRes.buffer = FM::readLocalFile( cacheFolder() + fileHash, lRes.length );
            if ( checkBitWiseFlag( rf, ResponseFlags::JSON | ResponseFlags::Text ) ) {
                lRes.BufferString() = FM::readLocalTextFile( cacheFolder() + fileHash );
            }
            if ( lRes.length || !lRes.BufferString().empty() ) {
                lRes.uri = uri;
                lRes.statusCode = 200;
                lRes.ETag = cacheFolder() + fileHash + std::to_string(lRes.length);
            }
            LOGRS("[HTTP-GET-CACHED] " << uri );
        }
        return lRes;
    }

    void get( const Url& url, ResponseCallbackFunc callback,
              ResponseCallbackFunc callbackFailed, ResponseFlags rf, HttpResourceCB mainThreadCallback ) {

        auto res = tryFileInCache( url_encode(url.uri), url.uri, rf );
        if ( res.isSuccessStatusCode() ) {
            handleResponseCallbacks( res, callback, callbackFailed, mainThreadCallback );
        } else {
            LOGR("[HTTP-GET] %s", url.toString().c_str() );
            getInternal( url, callback, callbackFailed, rf, mainThreadCallback );
        }
    }

    void getNoCache( const Url& url, ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed, ResponseFlags rf,
              HttpResourceCB mainThreadCallback ) {
        get( url, std::move(callback), std::move(callbackFailed), orBitWiseFlag(rf, ResponseFlags::ExcludeFromCache), std::move(mainThreadCallback));
    }

    void post( const Url& url, const std::string& _data,
               ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed,
               HttpResourceCB mainThreadCallback ) {
        postInternal( url, POST(), _data.data(), _data.size(), HttpQuery::JSON, callback, callbackFailed, mainThreadCallback );
    }

    void post( const Url& url, const uint8_p& buffer,
               ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed,
               HttpResourceCB mainThreadCallback ) {
        postInternal( url, POST(), reinterpret_cast<const char*>(buffer.first.get()), buffer.second, HttpQuery::Binary,
                      callback, callbackFailed, mainThreadCallback );
    }

    void post( const Url& url, const char *buff, uint64_t length,
               ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed,
               HttpResourceCB mainThreadCallback) {
        postInternal( url, POST(), buff, length, HttpQuery::Binary, callback, callbackFailed, mainThreadCallback );
    }

    void post( const Url& url, const std::vector<unsigned  char>& buffer,
               ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed,
               HttpResourceCB mainThreadCallback ) {
        postInternal( url, POST(), reinterpret_cast<const char*>(buffer.data()), buffer.size(), HttpQuery::Binary,
                      callback, callbackFailed, mainThreadCallback );
    }

    void post( const Url& url, ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed,
               HttpResourceCB mainThreadCallback ) {
        postInternal( url, POST(),nullptr, 0, HttpQuery::Binary, callback, callbackFailed, mainThreadCallback );
    }

    void put( const Url& url, const std::vector<unsigned  char>& buffer,
               ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed,
               HttpResourceCB mainThreadCallback ) {
        postInternal( url, PUT(),reinterpret_cast<const char*>(buffer.data()), buffer.size(), HttpQuery::Binary,
                      callback, callbackFailed, mainThreadCallback );
    }

    void project( const std::string& _project ) {
        sProject = _project;
    }

    void aft( const std::string& _aft ) {
        sAFT = _aft;
    }

    bool useClientCertificate( bool bUse, const std::string& _certKey, const std::string& _certCrt ) {
        sUseClientCertificate = bUse;
        if ( sUseClientCertificate ) {
            auto certKey = std::getenv( _certKey.c_str() );
            auto certCrt = std::getenv( _certCrt.c_str() );

            if ( certKey && certCrt ) {
                Http::clientCertificateKey(std::string(certKey));
                Http::clientCertificateCrt(std::string(certCrt));
                return true;
            }
            return false;
        }
        return true;
    }

    bool useClientCertificate() {
        return sUseClientCertificate;
    }

    void clientCertificateKey( std::string key ) {
        sClientCertificateKeyFilename = std::move(key);
    }

    std::string clientCertificateKey() {
        return sClientCertificateKeyFilename;
    }

    void clientCertificateCrt( std::string crt ) {
        sClientCertificateCrtFilename = std::move(crt);
    }

    std::string clientCertificateCrt() {
        return sClientCertificateCrtFilename;
    }

    void useServerCertificate( bool bUse ) {
        sUseServerCertificate = bUse;
    }

    bool useServerCertificate() {
        return sUseServerCertificate;
    }


    std::string project() {
        return sProject;
    }

    std::string AFT() {
        return sAFT;
    }

    void cacheLoginFields( const LoginFields& _lf ) {
        sCachedLoginFields = _lf;
        if ( !_lf.isDaemon() ) {
            FM::writeLocalTextFile( cacheFolder() + "lf", _lf.serialize() );
        }
    }

    LoginFields cachedLoginFields() {
        return sCachedLoginFields;
    }

    LoginFields gatherCachedLogin() {
        auto lf = FM::readLocalTextFile( cacheFolder() + "lf" );
        if ( !lf.empty() ) {
            LoginFields ret{lf};
            cacheLoginFields( ret );
            return ret;
        }
        return LoginFields{}; // this is guest / guest
    }

    void userLoggedIn( const bool _flag ) {
        sUserLoggedIn = _flag;
    }

    bool hasUserLoggedIn() {
        return sUserLoggedIn;
    }

    void userToken( std::string_view _token ) {
        sUserToken = _token;
    }

    std::string_view userToken() {
        return sUserToken;
    }

    const std::string userBearerToken() {
#ifdef __EMSCRIPTEN__
        return std::string{"Bearer+"} + std::string{Http::userToken()};
#else
        return std::string{"Bearer "} + std::string{Http::userToken()};
#endif
    }

    void sessionId( std::string_view _sid ) {
        sUserSessionId = _sid;
    }

    std::string_view sessionId() {
        return sUserSessionId;
    }

    std::string CLOUD_PROTOCOL() {
        return Url::HttpsProtocol;
    }

    std::string CLOUD_API() {
        return "/gapi";
    }

    std::string CLOUD_WSS() {
        return "/webrtc";
    }

    void cloudHost( const std::string& _sid ) {
        sCloudHost = _sid;
    }

    std::string CLOUD_HOST() {
        if ( sCloudHost.empty() ) {
            auto cloudHostCStr = std::getenv( "EH_CLOUD_HOST" );
            if ( cloudHostCStr == nullptr ) {
                LOGRS( "[[WARNING]] You need to have env var EH_CLOUD_HOST set to current host!! [[WARNING]]");
            } else {
                sCloudHost = std::string{cloudHostCStr};
            }
        }
        return sCloudHost;
    }

    std::string CLOUD_SERVER() {
        return CLOUD_HOST() + CLOUD_API();
    }

    std::string CLOUD_WSS_SERVER() {
        return CLOUD_HOST() + CLOUD_WSS();
    }

    void xProjectHeader( const LoginFields& _lf ) {
        Http::project( _lf.project );
//        Http::cacheLoginFields( _lf );
    }

    void initBase() {
        FM::initPersistent();
    }
    void init() {
        initBase();
        Http::login();
    }

    void init( const LoginFields& lf ) {
        initBase();
        Http::login( lf );
    }

    void initDaemon() {
        initBase();
        Http::login(LoginFields::Daemon());
    }

    void login() {
        loginSession();
    }

    void refreshToken() {
        post( Url{HttpFilePrefix::refreshtoken}, [](HttpResponeParams res) {
            RefreshToken rt( std::string{ (char *) res.buffer.get(), static_cast<unsigned long>(res.length) } );
            userToken( rt.token );
            sessionId( rt.session );
        } );
    }

    void loginSession() {
        get( Url{HttpFilePrefix::user}, [](HttpResponeParams res) {
                UserLogin ul{ std::string{ (char *) res.buffer.get(), static_cast<unsigned long>(res.length) } };
                sessionId( ul.session );
                project( ul.project );
//                Socket::createConnection();
                userLoggedIn( true );
            }, [](HttpResponeParams res) {
                LOGRS( "[HTTP-RETRY] login ");
                login ( Http::gatherCachedLogin() );
            }
        );
    }

    void login( const LoginFields& lf, const LoginCallback& loginCallback ) {
        post( Url{HttpFilePrefix::gettoken}, lf.serialize(), [lf, loginCallback](HttpResponeParams res) {
                LoginToken lt(std::string{ (char *) res.buffer.get(), static_cast<unsigned long>(res.length) });
                userToken( lt.token );
                sessionId( lt.session );
                project( lt.project );
                aft( lt.antiForgeryToken );
                Http::cacheLoginFields( lf );
//                Socket::createConnection();
                userLoggedIn( true );
                if (loginCallback) loginCallback();
        } );
    }

    void shutDown() {
        Socket::close();
    }
}
