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
#include "webclient_types.hpp"


bool isSuccessStatusCode( int statusCode );

//using HttpResouceCB = std::function<void(const std::string&)>;

namespace Socket {

    template <typename  T>
    JSONDATA( SendMessageStruct, msg, data )
        std::string msg;
        T data{};
    };

    static std::unordered_map<std::string, SocketCallbackFunc> callbacksMap;

    // Create connection has to be called after login
    void createConnection();
    void startClient( const std::string& _host );
    void onMessage( const std::string& _message );
    void emit( const std::string& _message );
    void emit( const std::string& _messageType, const std::string& jsonString );
    void on( const std::string& eventName, SocketCallbackFunc f );
    void close();

    template <typename T>
    void send( const std::string& _message, const T& _data ) {
        SendMessageStruct<T> sm;
        sm.msg = _message;
        sm.data = _data;
        emit(sm.serialize());
    }

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

JSONDATA( LoginToken, session, token, expires, project, antiForgeryToken )
    std::string session;
    std::string token;
    uint64_t expires;
    std::string project;
    std::string antiForgeryToken;
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
            if ( contentType.find("application/json") != std::string::npos ||
                 contentType.find("application/text") != std::string::npos ) {
                bufferString = std::string{ (char *) buffer.get(), static_cast<unsigned long>(length) };
            }
        }

        Result( uint64_t length = 0, int statusCode = 500 ) : length( length ), statusCode( statusCode ) {}

        Result( const std::string& uri, std::unique_ptr<uint8_t[]>&& buffer, uint64_t length, int statusCode, HttpResouceCB _ccf = nullptr)
                : uri( uri ), buffer( std::move(buffer) ), length( length ), statusCode( statusCode ), ccf(_ccf) {}

        Result( const std::string& uri, const char* cbuffer, uint64_t length, int statusCode, HttpResouceCB _ccf = nullptr )
                : uri( uri ), length( length ), statusCode( statusCode ), ccf(_ccf) {
            setBuffer(cbuffer, length);
        }
        Result( const std::string& uri,  std::string _contentType, const char* cbuffer, uint64_t length, int statusCode, std::string _etag, HttpResouceCB _ccf = nullptr )
                : uri( uri ), contentType(std::move(_contentType)), length( length ), statusCode( statusCode ), ETag(std::move(_etag)), ccf(_ccf) {
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
    void clearRequestCache();
    Result tryFileInCache( const std::string& fileHash, const Url url, ResponseFlags rf );

    void get( const Url& url, ResponseCallbackFunc callback,
              ResponseCallbackFunc callbackFailed = nullptr,
              ResponseFlags rf = ResponseFlags::None,
              HttpResouceCB mainThreadCallback = nullptr );
    void get( const Url& url, const std::string& _data, ResponseCallbackFunc callback,
              ResponseCallbackFunc callbackFailed = nullptr,
              ResponseFlags rf = ResponseFlags::None,
              HttpResouceCB mainThreadCallback = nullptr );
    void getInternal( const Url& url, const std::string& _data, ResponseCallbackFunc callback,
              ResponseCallbackFunc callbackFailed, ResponseFlags rf = ResponseFlags::None,
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

    void userLoggedIn( bool _flag );
    bool hasUserLoggedIn();
    void shutDown();

    void project( const std::string& _project );
    std::string project();
    std::string AFT();
    void aft( const std::string& _aft );
    [[nodiscard]] bool useClientCertificate();
    bool useClientCertificate(  bool bUse, const std::string& _certKey, const std::string& _certCrt );
    [[nodiscard]] bool useServerCertificate();
    void useServerCertificate( bool bUse );
    void clientCertificateKey( std::string key );
    std::string clientCertificateKey();
    void clientCertificateCrt( std::string crt );
    std::string clientCertificateCrt();

    void cacheLoginFields( const LoginFields& _lf );
    LoginFields gatherCachedLogin();
    LoginFields cachedLoginFields();

    void userToken( std::string_view _token );
    void sessionId( std::string_view _sid );
    std::string_view userToken();
    std::string_view sessionId();
    const std::string userBearerToken();

    void cloudHost( const std::string& _sid );

    std::string CLOUD_HOST();
    std::string CLOUD_PROTOCOL();
    std::string CLOUD_SERVER();
    std::string CLOUD_API();
    std::string CLOUD_WSS();
    std::string CLOUD_WSS_SERVER();
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

    static std::string Host( const std::string& protocol, const std::string& host, const int portNumber = 0 ) {
        if ( portNumber != 0 ) {
            return protocol + "://" + host + ":" + std::to_string(portNumber);
        }
        return protocol + "://" + host;
    }

    static std::string Service( const std::string& _service, const std::string& _path ) {
        return "/" + _service + "/" + _path;
    }

    std::string host = Http::CLOUD_SERVER();
    std::string uri = "/";

    std::string toString() const;

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
