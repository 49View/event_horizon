#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <cstring>
#include <sstream>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>
#include "htypes_shared.hpp"
#include "util_logs.hpp"
#include "util_array.hpp"

int threadId();

namespace ClipBoard {
    std::string copy( const char *new_clipboard );
    std::string paste();
}

const std::string userComputerName();
const std::string userName();
const std::string computerName();
const uint64_t cpuID();
const std::string getUserDownloadFolder( std::string forcedUserName = "" );
bool checkFileNameNotACopy( const std::string& filename );
std::string cacheFolder();
std::string getDaemonRoot();

std::string distaneToString( float _distance );
std::string distaneToStringMeters( float _distance );
std::string priceMaker( uint64_t price );
std::string sizeToString( const float x, const float y );
std::string sizeToStringMeters( const float x, const float y );
std::string sqmToString( const float sqm );
float stringFeetInchesToCm( const std::string& _text );
float feetInchesToMeters( const float feetf, const float inchesf );

const wchar_t *char_to_wchar( const char *name );

ucchar_p uint8_pTouucchar_p( const uint8_p& _source );
uint8_p ucchar_pTouint8_p( const ucchar_p& _source );
uint8_p make_uint8_p( uint64_t _size );

template<typename T>
uint8_p make_uint8_p( uint64_t _size, const T *data ) {
    auto ret = make_uint8_p(_size);
    std::memcpy(ret.first.get(), data, _size);
    return ret;
}

template<typename T>
uint8_p make_uint8_p( std::vector<T> data ) {
    auto ret = make_uint8_p(data.size());
    std::memcpy(ret.first.get(), data.data(), data.size());
    return ret;
}

namespace DaemonPaths {
    const static std::string UploadDir = "elaborate/";

    std::string upload( const std::string& _type, const std::string& _name = "" );
    std::string store( const std::string& _type, const std::string& _name = "" );
};

template<typename T>
void eraseCircular( std::vector<T>& contours, uint64_t first, uint64_t last ) {
    uint64_t csize = contours.size();
    uint64_t howMany = ( last > first ) ? last - first : ( last + csize ) - first;
    last = first + howMany;
    uint64_t extras = 0;

    if ( last > contours.size() ) {
        last = contours.size();
        extras = last - contours.size() + 1;
    }

    contours.erase(contours.begin() + first, contours.begin() + last);
    if ( extras > 0 ) {
        contours.erase(contours.begin() + 0, contours.begin() + extras);
    }
}

template<typename Tint, typename T>
bool checkBitWiseFlag( Tint source, T flag ) {
    int fi = static_cast<int>(flag);
    int d = ( source & fi );
    return ( d > 0 );
}

template<typename Tint, typename T>
void orBitWiseFlag( Tint& source, T flag ) {
    int dest = source + flag;
    source = static_cast<Tint>(dest);
}

template<typename Tint, typename T>
void xandBitWiseFlag( Tint& source, T flag ) {
    int dest = source ^flag;
    source = static_cast<Tint>(dest);
}

template<typename T>
void toggle( T& _input, const T _flag ) {
    if ( checkBitWiseFlag(_input, _flag) ) {
        _input &= ~_flag;
    } else {
        orBitWiseFlag(_input, _flag);
    }
}

template<typename T>
bool sumIsAroundZero( T d1, T d2, float epsilon = 1e-4 ) {
    T sum = d1 + d2;
    return ( sum > -epsilon && sum < epsilon );
}

template<typename T>
bool isPairAlreadyInVector( std::vector<T>& tvt, int32_t& v1, int32_t& v2 ) {
    for ( auto p : tvt ) {
        if ( p.first == v1 || p.first == v2 ) {
            if ( p.second == v1 || p.second == v2 )
                return true;
        }
    }
    return false;
}

template<typename AT, typename T>
void shiftArrayIndex( std::vector<AT>& array, T fromIndex, T shift ) {
    for ( uint64_t i = 0; i < array.size(); i++ ) {
        if ( array[i] >= fromIndex ) {
            array[i] = std::max(0, array[i] + shift);
        }
    }
}

template<typename T>
void shiftArrayIndex( std::vector<IndexPair>& array, T fromIndex, T shift ) {
    for ( uint64_t i = 0; i < array.size(); i++ ) {
        if ( array[i].first >= fromIndex ) array[i].first += shift;
        if ( array[i].second >= fromIndex ) array[i].second += shift;
    }
}

template<typename W, typename T>
void shiftArrayIndexUShape( std::vector<W>& array, T fromIndex, T shift ) {
    for ( uint64_t i = 0; i < array.size(); i++ ) {
        if ( array[i].uShapeIndex >= fromIndex ) {
            array[i].uShapeIndex = std::max(0, array[i].uShapeIndex + shift);
        }
    }
}

template<typename T>
void swapVectorPair( std::vector<T>& v, size_t i1, size_t i2 ) {
    iter_swap(v.begin() + i1, v.begin() + i2);
}

template<typename T>
void inserter( std::vector<T>& dest, const std::vector<T>& source ) {
    dest.insert(std::end(dest), std::begin(source), std::end(source));
}

bool invalidChar( unsigned char c );
void stripUnicode( std::string& str );

void platform_sleep( long long milliseconds );
std::string UTCMillisToString();
int convertStringToHour( const std::string& _str );
int convertStringToMinuteOrSecond( const std::string& _str );

std::string boolAlphaBinary( bool _flag );

template<typename C, typename L>
void erase_if( C& container, L lambda ) {
    container.erase(remove_if(container.begin(), container.end(), lambda), container.end());
}

template<typename C, typename L>
void erase_if_it( C& container, L _key ) {
    if ( auto it = container.find(_key); it != container.end() ) {
        container.erase(it);
    }
}

template<typename C, typename L, typename U>
void erase_if_iter( C& container, L lambda, const U& _value ) {
    for ( auto it = container.begin(); it != container.end(); ) {
        if ( lambda(it, _value) ) {
            container.erase(it);  // Returns the new iterator to continue from.
            return;
        } else {
            ++it;
        }
    }
}


// Reverse iterators

template<typename It>
class Range {
    It b, e;
public:
    Range( It b, It e ) : b(b), e(e) {}
    It begin() const { return b; }
    It end() const { return e; }
};

template<typename ORange, typename OIt = decltype(std::begin(
        std::declval<ORange>())), typename It = std::reverse_iterator<OIt>>
Range<It> reverse( ORange&& originalRange ) {
    return Range<It>(It(std::end(originalRange)), It(std::begin(originalRange)));
}

template<typename T, std::size_t N>
constexpr bool is_pointer_to_const_char(T(&)[N])
{
    return std::is_same_v<const char, T>;
}

template<typename T>
constexpr bool is_pointer_to_const_char(T &&)
{
    return std::is_same_v<const char *, T>;
}
