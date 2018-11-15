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

#include "../string_util.h"

bool isSuccessStatusCode( int statusCode );

namespace HttpFilePrefix {
    const static std::string entities = "/fs/entities/";
    const static std::string entities_all = "/fs/entities/all/";
    const static std::string entities_one = "/fs/entities/one/";
    const static std::string entities_onebinary = "/fs/entities/onebinary/";
    const static std::string get = "/fs/get/";
    const static std::string fileupload = "/fs/upload/";
    const static std::string catalog = "/catalog/";
    const static std::string getname = "/get/name/";
    const static std::string getnotexactname = "/get/notexact/name/";
};

enum class HttpUrlEncode {
    Yes,
    No
};

enum class HttpQuery {
    Binary,
    JSON,
    Text
};

struct Url;

using SocketCallbackFunc = std::function<void( const rapidjson::Document& data )>;

namespace Socket {
    static std::unordered_map<std::string, SocketCallbackFunc> callbacksMap;

    void startClient( const std::string& _host );
    void onMessage( const std::string& _message );
    void on( const std::string& eventName, SocketCallbackFunc f );
}

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
        uint64_t length;
        std::string bufferString;
        int statusCode;
        std::string ETag;
        std::string lastModified;

        void setBuffer( const char* cbuffer, uint64_t _length ) {
            length = _length;
            buffer = std::make_unique<unsigned char[]>( length);
            std::memcpy( buffer.get(), cbuffer, length );
        }

        Result( uint64_t length = 0, int statusCode = 500 ) : length( length ), statusCode( statusCode ) {}

        Result( const std::string& uri, std::unique_ptr<uint8_t[]>&& buffer, uint64_t length, int statusCode )
                : uri( uri ), buffer( std::move(buffer) ), length( length ), statusCode( statusCode ) {}

        Result( const std::string& uri, const char* cbuffer, uint64_t length, int statusCode )
                : uri( uri ), length( length ), statusCode( statusCode ) {
            setBuffer(cbuffer, length);
        }

        bool isSuccessStatusCode() const; //all 200s
    };

    const std::string restEntityPrefix( const std::string& _group, const std::string& _keyname );

    void get( const Url& url, const std::function<void(const Http::Result&)> callback,
              ResponseFlags rf = ResponseFlags::None );
    void getInternal( const Url& url, const std::function<void(const Http::Result&)> callback,
              ResponseFlags rf = ResponseFlags::None );

    void postInternal( const Url& url, const char *buff, uint64_t length, HttpQuery qt );

    void post( const Url& url, const std::string& _data ); // text data
    void post( const Url& url, const uint8_p& buffer );
    void post( const Url& url, const std::vector<unsigned char>& buffer );

    bool Ping();

    void useLocalHost( const bool _flag );

    const std::string CLOUD_PROTOCOL();
    const std::string CLOUD_SERVER();
    short CLOUD_PORT();
    short CLOUD_PORT_SSL();
};

namespace zlibUtil {
    // Decompress
    std::vector<char> inflateFromMemory( const Http::Result& fin );
}

struct Url {
    Url() {}
    Url( std::string _uri );

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

    std::string toString( const Http::UsePort _up = Http::UsePort::False ) const {
        if ( _up == Http::UsePort::True ) {
            short lPort = (Http::CLOUD_PROTOCOL() == Url::HttpsProtocol) ? Http::CLOUD_PORT_SSL() : Http::CLOUD_PORT();
            return Http::CLOUD_PROTOCOL() + "://" + host + ":" + std::to_string( lPort ) + uri;
        }
        return Http::CLOUD_PROTOCOL() + "://" + host + uri;
    }

    static std::smatch parseUrl( const std::string& _fullurl );
    static bool parsedMatchIsUrl( const std::smatch& base_match );

    void fromString( const std::string& _fullurl );

    bool isEngine( const std::string& _engine ) const {
        return startswith( uri, _engine );
    }

    std::string hostOnly() const;

    static const std::string WsProtocol;
    static const std::string WssProtocol;
    static const std::string HttpProtocol;
    static const std::string HttpsProtocol;
};

std::string url_encode( const std::string& value );
std::string url_encode_spacesonly( const std::string& value );
std::string url_decode( const std::string& str );
