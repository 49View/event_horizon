#include "webclient.h"

#include <iomanip>
#include <unordered_set>

#include "config_cpp.cfg"
#include "../util.h"
#include "../platform_util.h"

static bool sUseLocalhost = false;

const std::string Url::WsProtocol = "ws";
const std::string Url::WssProtocol = "wss";
const std::string Url::HttpProtocol = "http";
const std::string Url::HttpsProtocol = "https";

Url::Url( std::string _uri ) : uri( _uri ) {
}

std::smatch Url::parseUrl( const std::string& _fullurl ) {
    std::regex regv = std::regex( "(https?):\\/{2}(:?\\w*)(.*?)\\/(.*)" );
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
            port = std::stoi( strPort );
        }
        uri = "/" + base_match[4].str();
    }
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

    const std::string restEntityPrefix( const std::string& _group, const std::string& _keyname ) {
        return HttpFilePrefix::entities + _group + "/" + _keyname + "/";
    }

    void get( const Url& url, std::function<void(const Http::Result&)> callback, ResponseFlags rf ) {
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
            getInternal( url, callback, rf );
        }
    }

    void useLocalHost( const bool _flag ) {
        sUseLocalhost = _flag;
    }

    const std::string CLOUD_PROTOCOL() {
        if ( sUseLocalhost ) {
            return Url::HttpProtocol;
        } else{
            return Url::HttpsProtocol;
        }
    }

    const std::string CLOUD_SERVER() {
        if ( sUseLocalhost ) {
            return "localhost";
        } else {
            return Config::HOSTNAME;
        }
    }

    short CLOUD_PORT() {
        if ( sUseLocalhost ) {
            return 3000;
        } else{
            return 80;
        }
    }

    short CLOUD_PORT_SSL() {
        if ( sUseLocalhost ) {
            return 3000;
        } else{
            return 443;
        }
    }

}

namespace Socket {

}