#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <sstream>
#include <cctype>
#include <string>
#include <vector>
#include <regex>
#include <core/platform_util.h>

// trim from start
static inline std::string& ltrim( std::string& s ) {
    s.erase( s.begin(), std::find_if( s.begin(), s.end(), []( int c ) { return !std::isspace( c ); } ));
    return s;
}

// trim from end
static inline std::string& rtrim( std::string& s ) {
    s.erase( std::find_if( s.rbegin(), s.rend(),
                           []( int c ) { return !std::isspace( c ); } ).base(), s.end());
    return s;
}

static inline std::string& ltrim( std::string& s, const char _r ) {
    s.erase( s.begin(), std::find_if( s.begin(), s.end(), [_r]( int c ) { return c != _r; } ));
    return s;
}

static inline std::string& rtrim( std::string& s, const char _r ) {
    s.erase( std::find_if( s.rbegin(), s.rend(),
                           [_r]( int c ) { return c != _r; } ).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string& trim( std::string& s ) {
    return ltrim( rtrim( s ));
}

static inline std::string& trim( std::string& s, const char _r ) {
    return ltrim( rtrim( s, _r ), _r);
}


static inline bool startswith( const std::string& s, const std::string& t ) {
    return (s.compare(0, t.length(), t) == 0);
}

static inline std::string unescape(const std::string& str) {
    std::string ret(str); //Make a copy of the string as you are getting a const reference
    char remove = '\\'; //Add any characters you wish to remove

    ret.erase(std::remove(ret.begin(), ret.end(), remove), ret.end());

    return ret;
}

template<typename Out>
static inline void split( const std::string& s, char delim, Out result ) {
    std::stringstream ss;
    ss.str( s );
    std::string item;
    while ( std::getline( ss, item, delim )) {
        *( result++ ) = trim( item );
    }
}

static inline std::vector<std::string> split( const std::string& input, const std::string& regex = "\\s+") {
    std::regex re( regex );
    std::sregex_token_iterator
            first{ input.begin(), input.end(), re, -1 },
            last;
    return { first, last };
}

static inline std::vector<std::string> split( const std::string& s, char delim ) {
    std::vector<std::string> elems;
    split( s, delim, std::back_inserter( elems ));
    return elems;
}

template<typename ...Strings>
static inline std::string concatenate( char delimeter, const Strings& ...args ) {
    std::string result;
    int unpack[]{ ( result += args + delimeter, 0 ) ... };
    static_cast<void>( unpack );
    return result;
}

static inline std::string concatenate( std::string delimeter, const std::vector<std::string>& args ) {
    std::string result;
    bool isFirst = true;

    for ( auto s : args ) {
        if ( isFirst )
            isFirst = false;
        else
            result += delimeter;

        result += s;
    }
    return result;
}

static inline std::string concatParams( const std::vector<std::string>& _params, int _offset) {
    return concatenate(" ", { _params.begin()+_offset, _params.end()} );
}

static inline void replaceAllStrings( std::string& str, const std::string& from, const std::string& to ) {
    if ( from.empty())
        return;
    size_t start_pos = 0;
    while (( start_pos = str.find( from, start_pos )) != std::string::npos ) {
        str.replace( start_pos, from.length(), to );
        start_pos += to.length();
    }
}

static inline void removeNonAlphaCharFromString( std::string& s ) {
    s.erase(std::remove_if(s.begin(), s.end(),
                           []( auto const& c ) -> bool { return !isalnumCC(c); } ), s.end());
}

static inline std::smatch regEx( const std::string& _regExString, const std::string& _str ) {
    std::regex regv = std::regex( _regExString );
    std::smatch base_match;
    std::regex_search( _str, base_match, regv );

    return base_match;
}

static inline std::string htmlDecode( const std::string& _htmlString ) {

    std::string ret = _htmlString;
    auto match = regEx( "&#(\\w+);", _htmlString );
    if ( match.size() == 2 ) {
        int htmlCharIndex = std::stoi( match[1].str() );
        std::string sp = "";
        switch ( htmlCharIndex ) {
            case  13: sp = "\n"; break;
            case  32: sp = " "; break;
            case  33: sp = "!"; break;
            case  34: sp = "\""; break;
            case  35: sp = "#"; break;
            case  36: sp = "$"; break;
            case  37: sp = "%"; break;
            case  38: sp = "&"; break;
            case  39: sp = "'"; break;
            case  40: sp = "("; break;
            case  41: sp = ")"; break;
            case  42: sp = "*"; break;
            case  43: sp = "+"; break;
            case  44: sp = ","; break;
            case  45: sp = "-"; break;
            case  46: sp = "."; break;
            case  47: sp = "/"; break;
            case  58: sp = ":"; break;
            case  59: sp = ";"; break;
            case  60: sp = "<"; break;
            case  61: sp = "="; break;
            case  62: sp = ">"; break;
            case  63: sp = "?"; break;
            case  64: sp = "@"; break;
            case  91: sp = "["; break;
            case  92: sp = "\\"; break;
            case  93: sp = "]"; break;
            case  94: sp = "^"; break;
            case  95: sp = "_"; break;
            case  96: sp = "`"; break;
            case 123: sp = "{"; break;
            case 124: sp = "|"; break;
            case 125: sp = "}"; break;
            case 126: sp = "~"; break;
            case 160: sp = " "; break;
            case 161: sp = "¡"; break;
            case 162: sp = "¢"; break;
            case 163: sp = "£"; break;
            case 164: sp = "¤"; break;
            case 165: sp = "¥"; break;
            case 166: sp = "¦"; break;
            case 167: sp = "§"; break;
            case 168: sp = "¨"; break;
            case 169: sp = "©"; break;
            case 170: sp = "ª"; break;
            case 171: sp = "«"; break;
            case 172: sp = "¬"; break;
            case 173: sp = ")"; break;
            case 174: sp = "®"; break;
            case 175: sp = "¯"; break;
            case 176: sp = "°"; break;
            case 177: sp = "±"; break;
            case 178: sp = "²"; break;
            case 179: sp = "³"; break;
            case 180: sp = "´"; break;
            case 181: sp = "µ"; break;
            case 182: sp = "¶"; break;
            case 183: sp = "·"; break;
            case 184: sp = "¸"; break;
            case 185: sp = "¹"; break;
            case 186: sp = "º"; break;
            case 187: sp = "»"; break;
            case 188: sp = "¼"; break;
            case 189: sp = "½"; break;
            case 190: sp = "¾"; break;
            case 191: sp = "¿"; break;
            case 192: sp = "À"; break;
            case 193: sp = "Á"; break;
            case 194: sp = "Â"; break;
            case 195: sp = "Ã"; break;
            case 196: sp = "Ä"; break;
            case 197: sp = "Å"; break;
            case 198: sp = "Æ"; break;
            case 199: sp = "Ç"; break;
            case 200: sp = "È"; break;
            case 201: sp = "É"; break;
            case 202: sp = "Ê"; break;
            case 203: sp = "Ë"; break;
            case 204: sp = "Ì"; break;
            case 205: sp = "Í"; break;
            case 206: sp = "Î"; break;
            case 207: sp = "Ï"; break;
            case 208: sp = "Ð"; break;
            case 209: sp = "Ñ"; break;
            case 210: sp = "Ò"; break;
            case 211: sp = "Ó"; break;
            case 212: sp = "Ô"; break;
            case 213: sp = "Õ"; break;
            case 214: sp = "Ö"; break;
            case 215: sp = "×"; break;
            case 216: sp = "Ø"; break;
            case 217: sp = "Ù"; break;
            case 218: sp = "Ú"; break;
            case 219: sp = "Û"; break;
            case 220: sp = "Ü"; break;
            case 221: sp = "Ý"; break;
            case 222: sp = "Þ"; break;
            case 223: sp = "ß"; break;
            case 224: sp = "à"; break;
            case 225: sp = "á"; break;
            case 226: sp = "â"; break;
            case 227: sp = "ã"; break;
            case 228: sp = "ä"; break;
            case 229: sp = "å"; break;
            case 230: sp = "æ"; break;
            case 231: sp = "ç"; break;
            case 232: sp = "è"; break;
            case 233: sp = "é"; break;
            case 234: sp = "ê"; break;
            case 235: sp = "ë"; break;
            case 236: sp = "ì"; break;
            case 237: sp = "í"; break;
            case 238: sp = "î"; break;
            case 239: sp = "ï"; break;
            case 240: sp = "ð"; break;
            case 241: sp = "ñ"; break;
            case 242: sp = "ò"; break;
            case 243: sp = "ó"; break;
            case 244: sp = "ô"; break;
            case 245: sp = "õ"; break;
            case 246: sp = "ö"; break;
            case 247: sp = "÷"; break;
            case 248: sp = "ø"; break;
            case 249: sp = "ù"; break;
            case 250: sp = "ú"; break;
            case 251: sp = "û"; break;
            case 252: sp = "ü"; break;
            case 253: sp = "ý"; break;
            case 254: sp = "þ"; break;
            case 255: sp = "ÿ"; break;
            default:
                break;
        }
        if ( !sp.empty() ) {
            auto al = match[0].str();
            auto pos = _htmlString.find( al );
            ret.replace( pos, al.length(), sp );
        }
    }

    return ret;
}
// HTML Encoding
