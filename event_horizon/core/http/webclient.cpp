#include "webclient.h"

#include <iomanip>
#include <unordered_set>

#include "config_cpp.cfg"
#include "../util.h"
#include "../platform_util.h"
#include "core/zlib_util.h"
#include "core/string_util.h"
#include "core/file_manager.h"

static bool sUseLocalhost = false;
static bool sUserLoggedIn = false;
static std::string sProject;
static std::string sUserToken;
static std::string sUserSessionId;
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
        protocol = base_match[1].str();
        host = base_match[2].str();
        if ( !base_match[3].str().empty() ) {
            std::string strPort = base_match[3].str();
            strPort = string_trim_after( strPort, ":" );
            try {
                port = std::stoi( strPort );
            } catch ( const std::exception& ex ) {
                LOGR( "[URL-FROMSTRING][ERROR] on %s, port: %s", _fullurl.c_str(), strPort.c_str() );
                LOGR( "execption %s %s", typeid( ex ).name(), ex.what());
            }
        }
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

std::string Url::toString( const Http::UsePort _up ) const {
    if ( _up == Http::UsePort::True || sUseLocalhost ) {
        short lPort = (Http::CLOUD_PROTOCOL() == Url::HttpsProtocol) ? Http::CLOUD_PORT_SSL() : Http::CLOUD_PORT();
        return Http::CLOUD_PROTOCOL() + "://" + host + ":" + std::to_string( lPort ) + uri;
    }
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
        if ( isalnumCC( c ) || c == '-' || c == '_' || c == '.' || c == ',' || c == '~' || c == '/' ) {
            escaped << c;
            continue;
        }

        if ( c == ' ' ) {
            // Any other characters are percent-encoded
            escaped << std::uppercase;
            escaped << '%' << std::setw( 2 ) << int((unsigned char) c );
            escaped << std::nouppercase;
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

    static std::unordered_set<std::string> requestCache;

    void get( const Url& url, ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed, ResponseFlags rf,
              HttpResouceCB mainThreadCallback ) {
        bool bPerformLoad = false;

        if ( checkBitWiseFlag(rf, ResponseFlags::ExcludeFromCache) ) {
            bPerformLoad = true;
        } else {
            auto cacheKey = url.toString() + std::to_string( static_cast<int>(rf) );
            if ( const auto& cc = requestCache.find( cacheKey ); cc == requestCache.end() ) {
                requestCache.insert( cacheKey );
                bPerformLoad = true;
            }
        }
        if ( bPerformLoad ) {
            LOGR("[HTTP-GET] %s", url.toString().c_str() );
            getInternal( url, callback, callbackFailed, rf, mainThreadCallback );
        }
    }

    void post( const Url& url, const std::string& _data,
               ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed,
               HttpResouceCB mainThreadCallback ) {
        postInternal( url, _data.data(), _data.size(), HttpQuery::JSON, callback, callbackFailed, mainThreadCallback );
    }

    void post( const Url& url, const uint8_p& buffer,
               ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed,
               HttpResouceCB mainThreadCallback ) {
        postInternal( url, reinterpret_cast<const char*>(buffer.first.get()), buffer.second, HttpQuery::Binary,
                      callback, callbackFailed, mainThreadCallback );
    }

    void post( const Url& url, const char *buff, uint64_t length,
               ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed,
               HttpResouceCB mainThreadCallback) {
        postInternal( url, buff, length, HttpQuery::Binary, callback, callbackFailed, mainThreadCallback );
    }

    void post( const Url& url, const std::vector<unsigned  char>& buffer,
               ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed,
               HttpResouceCB mainThreadCallback ) {
        postInternal( url, reinterpret_cast<const char*>(buffer.data()), buffer.size(), HttpQuery::Binary,
                      callback, callbackFailed, mainThreadCallback );
    }

    void post( const Url& url, ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed,
               HttpResouceCB mainThreadCallback ) {
        postInternal( url, nullptr, 0, HttpQuery::Binary, callback, callbackFailed, mainThreadCallback );
    }

    void project( const std::string& _project ) {
        sProject = _project;
    }

    std::string project() {
        return sProject;
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

    void useLocalHost( const bool _flag ) {
        sUseLocalhost = _flag;
    }

    bool isLocalHost() {
        return sUseLocalhost;
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

    const std::string CLOUD_PROTOCOL() {
        return Url::HttpsProtocol;
        // We do NOT allow anything that's not HTTPS, sorry folks!!
//        if ( sUseLocalhost ) {
//            return Url::HttpsProtocol;
//        } else{
//            return Url::HttpsProtocol;
//        }
    }

    const std::string CLOUD_SERVER() {
        if ( sUseLocalhost ) {
            return "localhost";
        } else {
            return Config::HOSTNAME;
        }
    }

    short CLOUD_PORT() {
        return 80;
    }

    short CLOUD_PORT_SSL() {
        if ( sUseLocalhost ) {
            return 3000;
        } else {
            return 443;
        }
    }

    void xProjectHeader( const LoginFields& _lf ) {
        Http::project( _lf.project );
//        Http::cacheLoginFields( _lf );
    }

    void initBase() {
#ifdef USE_LOCALHOST
        Http::useLocalHost(true);
#endif
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
            RefreshToken rt( res.bufferString );
            userToken( rt.token );
            sessionId( rt.session );
        } );
    }

    void loginSession() {
        get( Url{HttpFilePrefix::user}, [](HttpResponeParams res) {
                UserLogin ul{ res.bufferString };
                sessionId( ul.session );
                project( ul.project );
                Socket::createConnection();
                userLoggedIn( true );
            }, [](HttpResponeParams res) {
                LOGRS( "[HTTP-RETRY] login ");
                login ( Http::gatherCachedLogin() );
            }
        );
    }

    void login( const LoginFields& lf, const LoginCallback& loginCallback ) {
        post( Url{HttpFilePrefix::gettoken}, lf.serialize(), [lf, loginCallback](HttpResponeParams res) {
                LoginToken lt(res.bufferString);
                userToken( lt.token );
                sessionId( lt.session );
                project( lt.project );
                Http::cacheLoginFields( lf );
                Socket::createConnection();
                userLoggedIn( true );
                if (loginCallback) loginCallback();
        } );
    }

    void shutDown() {
        Socket::close();
    }
}
