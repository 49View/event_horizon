#pragma once

#include "htypes_shared.hpp"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <sstream>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>

#define  LOG_TAG    "EventHorizon-cpp"

typedef std::pair<int32_t, int32_t> IndexPair;

enum LogPriority {
	LOG_PRIOTITY_INFO,
	LOG_PRIORITY_ERROR,
	LOG_PRIORITY_WARN
};

enum class PerfOptions {
	Print,
	NoPrint
};

enum EncodingStatusFlag {
	None = 0,
	DoEncode = 1 << 0,
	NotEncode = 1 << 1
};

enum class CompositeWrapping {
	Wrap,
	NoWrap
};

class UpdateSignals {
    bool mbNeedsUpdate = false;
public:
    bool NeedsUpdate() const {
        return mbNeedsUpdate;
    }

    void NeedsUpdate( bool mbNeedsUpdate ) {
        UpdateSignals::mbNeedsUpdate = mbNeedsUpdate;
    }
};

using logCF = std::function<void(const char*)>;

void hookLogCallbackFunction( logCF _cf );
void logPrint( LogPriority logPriority, const char *tag, const char *fmt, ... );
void doAssert( bool condition, const char* text, int line, const char* file );
void doAssertV( bool condition, const char* text, int line, const char* file, const char *fmt, ... );
void platformBreakpoint();
void platformLogPrint( const std::string& logTag, float time, const std::string& message );
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
std::string priceMaker( uint64_t price );
std::string sizeToString( const float x, const float y );
std::string sqmToString( const float sqm );
float stringFeetInchesToCm( const std::string& _text );
float feetInchesToMeters( const float feetf, const float inchesf );

const wchar_t* char_to_wchar( const char* name );
bool isFilenameAFolder( const std::string& input );
std::string getFileNamePath( const std::string& input );
std::string getFileNameOnly( const std::string& input );
std::string getFileName( const std::string& input, EncodingStatusFlag ef = EncodingStatusFlag::DoEncode );
std::string getFileNameNoExt( const std::string& input );
std::string getFileNameExt( const std::string& input );
std::string getFileNameKey( const std::string& input, EncodingStatusFlag ef = EncodingStatusFlag::DoEncode );
std::string getLastEntryInPath( const std::string& input );
std::string getLastFolderInPath( const std::string& input );
std::string getFileNameCallbackKey( const std::string& input );
bool isFileExtAnImage( const std::string& _ext );
std::string toLower( const std::string _input );
std::string cbToString( uint8_p&& _data );

SerializableContainer serializableContainerFromString( const std::string& s );

bool nameHasImageExtension( const std::string& input );
bool string_ends_with( const std::string& source, const std::string& match );
std::string string_trim_upto( const std::string& source, const std::string& match );
std::string string_trim_after( const std::string& source, const std::string& match );
std::string string_trim_upto( const std::string& source, const std::vector<std::string>& matches );

bool ensureFolderExists( const std::string& folder, bool throwOnError = false );

ucchar_p uint8_pTouucchar_p( const uint8_p& _source );
uint8_p ucchar_pTouint8_p( const ucchar_p& _source );
uint8_p make_uint8_p( uint64_t _size );

namespace DaemonPaths {
	const static std::string UploadDir = "elaborate/";

	std::string upload( const std::string& _type, const std::string& _name = "" );
	std::string store( const std::string& _type, const std::string& _name = "" );
};

template <typename intType, typename utype>
utype getCircularArrayIndex( intType pi, utype size ) {
	if ( std::llabs( pi ) == size ) return 0;
	return pi >= 0 ? ( pi % size ) : size - static_cast<intType>( std::llabs( pi % size ) );
}

template <typename intType, typename utype>
intType getCircularArrayIndexUnsigned( intType pi, utype size ) {
	if ( pi == size ) return 0;
	return pi >= 0 ? ( pi % size ) : size - ( pi % size );
}

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

	contours.erase( contours.begin() + first, contours.begin() + last );
	if ( extras > 0 ) {
		contours.erase( contours.begin() + 0, contours.begin() + extras );
	}
}

template <typename Tint, typename T>
bool checkBitWiseFlag( Tint source, T flag ) {
    int fi = static_cast<int>(flag);
	int d = ( source & fi );
	return ( d > 0 );
}

template <typename Tint, typename T>
void orBitWiseFlag( Tint& source, T flag ) {
	int dest = source + flag;
	source = static_cast<Tint>(dest);
}

template <typename Tint, typename T>
void xandBitWiseFlag( Tint& source, T flag ) {
    int dest = source ^ flag;
    source = static_cast<Tint>(dest);
}

template<typename T>
void toggle( T& _input, const T _flag ) {
	if ( checkBitWiseFlag( _input, _flag) ) {
		_input &= ~_flag;
	} else {
		orBitWiseFlag( _input, _flag );
	}
}

template<typename T>
bool sumIsAroundZero( T d1, T d2, float epsilon = 1e-4 ) {
	T sum = d1 + d2;
	return ( sum > -epsilon && sum < epsilon );
}

template<typename T> bool isPairAlreadyInVector( std::vector<T >& tvt, int32_t& v1, int32_t& v2 ) {
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
			array[i] = std::max( 0, array[i] + shift );
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
			array[i].uShapeIndex = std::max( 0, array[i].uShapeIndex + shift );
		}
	}
}

template<typename T>
void swapVectorPair( std::vector<T>& v, size_t i1, size_t i2 ) {
    iter_swap(v.begin() + i1, v.begin() + i2);
}

template<typename T>
T getLeftVectorFromList( const std::vector<T>& va, int64_t m, bool wrapIt ) {
	T ret;

	if ( m > 0 ) return va[m - 1];

	if ( wrapIt ) {
		int64_t index = m == 0 ? va.size() - 1 : m - 1;
		return va[index];
	}
	// do not wrap it so it needs to extend the vector as it there was another on one the same line opposite the first point (index 1)
	T ext = va[0] - va[1];
	return va[0] + ext;
}

template<typename T>
T getRightVectorFromList( const std::vector<T>& va, int64_t m, bool wrapIt ) {
	T ret;

	if ( m < static_cast<int64_t>( va.size() - 1 ) ) return va[m + 1];

	if ( wrapIt ) {
		int64_t index = m == static_cast<int64_t>( va.size() ) - 1 ? 0 : m + 1;
		return va[index];
	}
	// do not wrap it so it needs to extend the vector as it there was another on one the same line continuing from the last point
	T ext = va[va.size() - 1] - va[va.size() - 2];
	return va[va.size() - 1] + ext;
}

template<typename T>
void inserter( std::vector<T>& dest, const std::vector<T>& source ) {
    dest.insert(std::end(dest), std::begin(source), std::end(source));
}

template<typename T>
bool inRange( const T _key, const std::pair<T,T>& _range ) {
	return _range.first <= _key && _range.second >= _key;
}

template<typename T>
bool inRangeEx( const T _key, const std::pair<T,T>& _range ) {
	return _range.first <= _key && _range.second > _key;
}

template<typename T>
bool inRange( const T _key, const T _r1, const T _r2 ) {
    return _r1 <= _key && _r2 >= _key;
}

template<typename T>
bool inRangeEx( const T _key, const T _r1, const T _r2 ) {
    return _r1 <= _key && _r2 > _key;
}

bool invalidChar (unsigned char c);
void stripUnicode(std::string & str);

void platform_sleep( long long milliseconds );
std::string UTCMillisToString();
int convertStringToHour( const std::string& _str );
int convertStringToMinuteOrSecond( const std::string& _str );

std::string boolAlphaBinary( bool _flag );

template <typename C, typename L>
void erase_if( C& container, L lambda ) {
	container.erase( remove_if( container.begin(), container.end(), lambda ), container.end());
}

template <typename C, typename L>
void erase_if_it( C& container, L _key ) {
	if ( auto it = container.find( _key ); it != container.end()) {
		container.erase( it );
	}
}

template <typename C, typename L, typename U>
void erase_if_iter( C& container, L lambda, const U& _value  ) {
	for ( auto it = container.begin(); it != container.end(); ) {
		if ( lambda( it, _value )) {
			container.erase( it );  // Returns the new iterator to continue from.
			return;
		} else {
			++it;
		}
	}
}


// Reverse iterators

template<typename It>
class Range
{
	It b, e;
public:
	Range(It b, It e) : b(b), e(e) {}
	It begin() const { return b; }
	It end() const { return e; }
};

template<typename ORange, typename OIt = decltype(std::begin(std::declval<ORange>())), typename It = std::reverse_iterator<OIt>>
Range<It> reverse(ORange && originalRange) {
	return Range<It>(It(std::end(originalRange)), It(std::begin(originalRange)));
}


extern int64_t globalHash;
//static const int64_t HashInc();

// Release log functions
#define ASSERTR(X) doAssert(X, #X, __LINE__, __FILE__)
//#define THREAD_INFO

#define ARRAY_LENGTH(X) (sizeof(X) / sizeof((X)[0]))

#ifndef _PRODUCTION_

#ifdef TRACE_FUNCTIONS
#define FUNCTION_BEGIN() logPrint(LOG_PRIOTITY_INFO, LOG_TAG, "%s() - begin\n", __FUNCTION__ )
#define FUNCTION_END() logPrint(LOG_PRIOTITY_INFO, LOG_TAG, "%s() - end\n", __FUNCTION__ )
#else
#define FUNCTION_BEGIN()
#define FUNCTION_END()
#endif

#ifdef THREAD_INFO
#define LOGI(X,...)  logPrint(LOG_PRIOTITY_INFO,LOG_TAG, "[Thread: %d] " X, threadId(), ##__VA_ARGS__)
#define LOGR(X,...)  logPrint(LOG_PRIOTITY_INFO,LOG_TAG, "[Thread: %d] " X, threadId(), ##__VA_ARGS__)
#define LOGE(X,...) logPrint(LOG_PRIORITY_ERROR,LOG_TAG, "[Thread: %d] " X, threadId(), ##__VA_ARGS__)
#define LOGW(X,...)  logPrint(LOG_PRIORITY_WARN,LOG_TAG, "[Thread: %d] " X, threadId(), ##__VA_ARGS__)
#define LOGN()  logPrint(LOG_PRIOTITY_INFO,LOG_TAG, "[Thread: %d] " __FILE__ ":%d" , threadId(), __LINE__)
#else
#define LOGI(...) logPrint(LOG_PRIOTITY_INFO,LOG_TAG,  __VA_ARGS__)
#define LOGR(...)  logPrint(LOG_PRIOTITY_INFO,LOG_TAG, __VA_ARGS__)
#define LOGE(...) logPrint(LOG_PRIORITY_ERROR,LOG_TAG, __VA_ARGS__)
#define LOGW(...)  logPrint(LOG_PRIORITY_WARN,LOG_TAG, __VA_ARGS__)
#define LOGN()  logPrint(LOG_PRIOTITY_INFO,LOG_TAG, "" __FILE__ ":%d", __LINE__)
#define LOGRS( X ) { std::ostringstream _ss; _ss << X; LOGR(_ss.str().c_str()); }
#endif

#define CONDLOG(X) LOGI("*** " #X " // %s", X ? "True" : "False");
#define LOGIT(X,...) logPrint(LOG_PRIOTITY_INFO,X,__VA_ARGS__)
#define ASSERT(X) doAssert(X, #X, __LINE__, __FILE__)
#define ASSERTV(X, ...) doAssertV(X, #X, __LINE__, __FILE__, __VA_ARGS__)
#define PERF_TIMER_START() perfTimerStart(0)
#define PERF_TIMER_END(NAME) perfTimerEnd(0, NAME)
#define N_PERF_TIMER_START(INDEX) perfTimerStart(INDEX)
#define N_PERF_TIMER_END(INDEX, NAME) perfTimerEnd(INDEX, NAME)
#define ASSERTSTATIC(exp) {typedef char temp[(exp) ? 1 : -1];}
#else
#define LOGI(...)
#define LOGIT(...)
#define LOGN(...)
#define ASSERT(X)
#define ASSERTV(X, ...)
#define PERF_TIMER_START()
#define PERF_TIMER_END(X)
#define N_PERF_TIMER_START(INDEX)
#define N_PERF_TIMER_END(INDEX, NAME)
#define ASSERTSTATIC(exp)

#define LOGR(...) logPrint(LOG_PRIOTITY_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...) logPrint(LOG_PRIORITY_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGW(...)  logPrint(LOG_PRIORITY_WARN,LOG_TAG,__VA_ARGS__)
#define LOGRS( X ) { std::ostringstream _ss; _ss << X; LOGR(_ss.str().c_str()); }
#endif

typedef std::map<uint64_t, FollowerGapData> gapMap;
typedef gapMap::iterator gapMapIt;

class FollowerGap {
public:
	FollowerGap() {
		mGaps.clear();
	}

	FollowerGap( const uint64_t size ) {
		for ( uint64_t t = 0; t < size; t++ ) mGaps.push_back( FollowerGapData( FollowerGapSide::NoGap ) );
	}

	void pushGap( const FollowerGapData& val ) {
		mGaps.push_back( val );
	}

	void createGap( uint64_t start_index, uint64_t end_index, float insetStart, float insetEnd = -1.0f ) {
		ASSERT( start_index < mGaps.size() );
		ASSERT( end_index < mGaps.size() );

		if ( insetEnd == -1.0f ) insetEnd = insetStart;
		mGaps[start_index] = FollowerGapData( FollowerGapSide::Start, insetStart );
		mGaps[end_index] = FollowerGapData( FollowerGapSide::End, insetEnd );
	}

	bool isGap( FollowerGapSide side, uint64_t index, float& inset ) const;
	bool isGapAt( uint64_t index ) const;
	bool isStartGapAt( uint64_t index ) const;
	bool isEndGapAt( uint64_t index ) const;
	bool isVisibleAt( uint64_t index ) const;
	void tagAllNotVisible();
public:
	static const FollowerGap Empty;
private:
	std::vector<FollowerGapData> mGaps;
};

class FrameInvalidator {
public:
	void invalidate();
	bool invalidated() const;
	void validated();

private:
	bool bInvalidated = false;
};

