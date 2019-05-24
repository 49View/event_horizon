#pragma once

#include <chrono>
#include <string>
#include <memory>
#include <vector>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <rapidjson/document.h>
#include <core/htypes_shared.hpp>
#include <core/string_util.h>
#include "login.hpp"


bool isSuccessStatusCode( int statusCode );

namespace HttpFilePrefix {
    const static std::string user = "/user/";
    const static std::string gettoken = "/getToken/";
    const static std::string refreshtoken = "/refreshToken/";
    const static std::string entities = "/entities/";
    const static std::string entities_all = "/entities/metadata/byGroupTags/";
    const static std::string entities_onebinary = "/entities/content/byGroupTags/";
    const static std::string get = "/fs/";
    const static std::string fileupload = "/fs/";
    const static std::string catalog = "/catalog/";
    const static std::string getname = "/name/";
    const static std::string getnotexactname = "/get/notexact/name/";
    const static std::string broadcast = "/broadcast/";
};

enum class HttpQuery {
    Binary,
    JSON,
    Text
};

struct Url;

using SocketCallbackDataType = rapidjson::Document;
using SocketCallbackDataTypeConstRef = const SocketCallbackDataType&;
using SocketCallbackFunc = std::function<void( SocketCallbackDataTypeConstRef message )>;
using LoginCallback = std::function<void()>;
using HttpResouceCBSign = const std::string&;
using HttpResouceCB = std::function<void(HttpResouceCBSign)>;
//using HttpResouceCB = std::function<void(const std::string&)>;

namespace Socket {
    static std::unordered_map<std::string, SocketCallbackFunc> callbacksMap;

    // Create connection has to be called after login
    void createConnection();
    void startClient( const std::string& _host );
    void onMessage( const std::string& _message );
    void emit( const std::string& _message );
    void on( const std::string& eventName, SocketCallbackFunc f );
    void close();
}

JSONDATA( UserLoginToken, name, email, guest )
    std::string     name;
    std::string     email;
    bool            guest;
};

JSONDATA( UserLogin, expires, user, project, session )
    uint64_t        expires;
    UserLoginToken  user;
    std::string     project;
    std::string     session;
};

JSONDATA( LoginToken, session, token, expires, project )
    std::string session;
    std::string token;
    uint64_t expires;
    std::string project;
};

JSONDATA( RefreshToken, session, token, expires )
    std::string session;
    std::string token;
    uint64_t expires;
};

struct LoginFieldsPrecached {};
struct LoginFieldsCanonicalRoute {};

template <typename HLF = LoginFieldsCanonicalRoute >
class LoginActivation {
public:
    static constexpr bool hasLF() {
        return std::is_same_v<HLF, LoginFieldsPrecached>;
    }
};

namespace Http {

    enum UsePort {
        False = 0,
        True = 1
    };

    enum ResponseFlags {
        None = 0,
        HeaderOnly = 1,
        Text = 1 << 1,
        JSON = 1 << 2,
        Binary = 1 << 3,
        ExcludeFromCache = 1 << 4
    };

    inline ResponseFlags queryToResponse( const HttpQuery q ) {
        switch (q) {
            case HttpQuery::Binary:
                return ResponseFlags::Binary;
            case HttpQuery::JSON:
                return ResponseFlags::JSON;
            case HttpQuery::Text:
                return ResponseFlags::Text;
        }
    }

    struct Result {
        std::string uri;
        std::string contentType;
        ResponseFlags flags;
        mutable std::unique_ptr<uint8_t[]> buffer;
        uint64_t length = 0;
        std::string bufferString;
        int statusCode = 0;
        std::string ETag;
        std::string lastModified;
        HttpResouceCB ccf = nullptr;

        void setBuffer( const char* cbuffer, uint64_t _length ) {
            length = _length;
            buffer = std::make_unique<unsigned char[]>( static_cast<size_t>(length));
            std::memcpy( buffer.get(), cbuffer, static_cast<size_t>(length));
            bufferString = std::string{ (char*)buffer.get(), static_cast<unsigned long>(length) };
        }

        Result( uint64_t length = 0, int statusCode = 500 ) : length( length ), statusCode( statusCode ) {}

        Result( const std::string& uri, std::unique_ptr<uint8_t[]>&& buffer, uint64_t length, int statusCode, HttpResouceCB _ccf = nullptr)
                : uri( uri ), buffer( std::move(buffer) ), length( length ), statusCode( statusCode ), ccf(_ccf) {}

        Result( const std::string& uri, const char* cbuffer, uint64_t length, int statusCode, HttpResouceCB _ccf = nullptr )
                : uri( uri ), length( length ), statusCode( statusCode ), ccf(_ccf) {
            setBuffer(cbuffer, length);
        }

        bool isSuccessStatusCode() const; //all 200s
    };

    void init();
    void init( const LoginFields& lf );
    void initDaemon();

    void login();
    void login( const LoginFields& lf, const LoginCallback& loginCallback = nullptr);
    void loginSession();
    void refreshToken();
    void xProjectHeader( const LoginFields& _lf );

    void get( const Url& url, ResponseCallbackFunc callback,
              ResponseCallbackFunc callbackFailed = nullptr,
              ResponseFlags rf = ResponseFlags::None,
              HttpResouceCB mainThreadCallback = nullptr );
    void getInternal( const Url& url, ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed,
              ResponseFlags rf = ResponseFlags::None,
              HttpResouceCB mainThreadCallback = nullptr );

    void postInternal( const Url& url, const char *buff, uint64_t length, HttpQuery qt,
                       ResponseCallbackFunc callback, ResponseCallbackFunc callbackFailed,
                       HttpResouceCB mainThreadCallback );

    void post( const Url& url, const std::string& _data,
               ResponseCallbackFunc callback = nullptr,
               ResponseCallbackFunc callbackFailed = nullptr,
               HttpResouceCB mainThreadCallback = nullptr );
    void post( const Url& url, const uint8_p& buffer,
               ResponseCallbackFunc callback = nullptr,
               ResponseCallbackFunc callbackFailed = nullptr,
               HttpResouceCB mainThreadCallback = nullptr );
    void post( const Url& url, const char *buff, uint64_t length,
               ResponseCallbackFunc callback = nullptr,
               ResponseCallbackFunc callbackFailed = nullptr,
               HttpResouceCB mainThreadCallback = nullptr );
    void post( const Url& url, const std::vector<unsigned char>& buffer,
               ResponseCallbackFunc callback = nullptr,
               ResponseCallbackFunc callbackFailed = nullptr,
               HttpResouceCB mainThreadCallback = nullptr );
    void post( const Url& url,
               ResponseCallbackFunc callback,
               ResponseCallbackFunc callbackFailed = nullptr,
               HttpResouceCB mainThreadCallback = nullptr );

    void useLocalHost( bool _flag );
    void userLoggedIn( bool _flag );
    bool hasUserLoggedIn();
    bool isLocalHost();
    void shutDown();

    void project( const std::string& _project );
    std::string project();

    void cacheLoginFields( const LoginFields& _lf );
    LoginFields gatherCachedLogin();
    LoginFields cachedLoginFields();

    void userToken( std::string_view _token );
    void sessionId( std::string_view _sid );
    std::string_view userToken();
    std::string_view sessionId();

    const std::string CLOUD_PROTOCOL();
    const std::string CLOUD_SERVER();
    short CLOUD_PORT();
    short CLOUD_PORT_SSL();
};

namespace zlibUtil {
    // Decompress
    SerializableContainer inflateFromMemory( const Http::Result& fin );
}

struct Url {
    Url() = default;
    explicit Url( std::string _uri );

    Url( std::string _host, short _port, std::string _uri ) {
        host = _host;
        port = _port;
        uri = _uri;
    }

    static std::string Host( const std::string& protocol, const std::string& host, const int portNumber,
                             const Http::UsePort _up = Http::UsePort::False ) {
        if ( _up == Http::UsePort::True ) {
            return protocol + "://" + host + ":" + std::to_string(portNumber);
        }
        return protocol + "://" + host;
    }

    static std::string Service( const std::string& _service, const std::string& _path ) {
        return "/" + _service + "/" + _path;
    }

    std::string protocol = Http::CLOUD_PROTOCOL();
    std::string host = Http::CLOUD_SERVER();
    short port = Http::CLOUD_PORT_SSL();
    std::string uri = "/";

    std::string toString( const Http::UsePort _up = Http::UsePort::False ) const;

    static std::smatch parseUrl( const std::string& _fullurl );
    static bool parsedMatchIsUrl( const std::smatch& base_match );

    void fromString( const std::string& _fullurl );

    bool isEngine( const std::string& _engine ) const {
        return startswith( uri, _engine );
    }

    std::string hostOnly() const;

    static Url privateAPI( const std::string& _params );

    static const std::string WsProtocol;
    static const std::string WssProtocol;
    static const std::string HttpProtocol;
    static const std::string HttpsProtocol;

    static std::string entityURLParams( const std::string& _key, const std::string& _name = "" );
    static Url entityMetadata(const std::string& _key, const std::string& _name = "" );
    static Url entityContent( const std::string& _key, const std::string& _name = "" );

};

std::string url_encode( const std::string& value );
std::string url_encode_spacesonly( const std::string& value );
std::string url_decode( const std::string& str );
