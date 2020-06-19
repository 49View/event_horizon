#include "util.h"

#include <thread>
#include <locale>
#include <codecvt>
#include <string>
#include <cstring>
#include <stdio.h>
#include <chrono>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "http/webclient.h"

#if defined(_WIN32)
#include <direct.h>
#include <cerrno>
#else
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <core/game_time.h>

static bool gError = false;

//====================================
// Logging
//====================================

static logCF logCallback = nullptr;

void hookLogCallbackFunction( logCF _cf ) {
	logCallback = _cf;
}

void platformBreakpoint() {
	// Use abort, so we will get a callstack sometimes
	std::abort();
}

void logPrint( LogPriority logPriority, const char *tag, const char *fmt, ... ) {
	va_list vl;
	va_start( vl, fmt );

    char buffer[65536];
    vsnprintf( buffer, 65536, fmt, vl );

    std::string logTag = "[INFO]";
    if ( logPriority == LOG_PRIORITY_ERROR ) {
        logTag = "[ERROR]";
    } else if ( logPriority == LOG_PRIORITY_WARN ) {
        logTag = "[WARNING]";
    }

    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - GameTime::getStartTimeStamp();
    auto ts = static_cast<float>(elapsed_seconds.count());

    // Android specific print
	platformLogPrint( logTag, ts, std::string(buffer) );

    if ( logCallback ) {
        logCallback( buffer );
    }

    //TODO fo linux it works just as follow
    //std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    //std::wstring wbuffer = converter.from_bytes( buffer );

    //std::wcout << wbuffer << L"\n";

	va_end( vl );
}

void doAssert( bool condition, const char* text, int line, const char* file ) {
	if ( !condition ) {
		if ( !gError ) {
			gError = true;
			LOGE( "Assert: (%s) %s, line %d", text, file, line );
			platformBreakpoint();
		}
	}
}

void doAssertV( bool condition, const char* text, int line, const char* file, const char *fmt, ... ) {
	if ( !condition ) {
		// Print error message
		va_list vl;
		va_start( vl, fmt );
		LOGE( fmt, vl);
//		platformLogPrint( LOG_PRIORITY_ERROR, LOG_TAG, fmt, vl );
		va_end( vl );

		// Do assert
		if ( !gError ) {
			gError = true;
			LOGE( "Assert: (%s) %s, line %d", text, file, line );
			platformBreakpoint();
		}
	}
}

bool ensureFolderExists( const std::string& folder, bool throwOnError ) {
	int errCreation;

#if defined(_WIN32)
	errCreation = _mkdir( folder.c_str() );
#else
	mode_t nMode = 0733;
	errCreation = mkdir( folder.c_str(), nMode );
#endif

	if ( errCreation == 0 || errno == EEXIST ) return true;

	if ( throwOnError )
		throw std::strerror( errno ) + folder;
	else
		LOGR( "Error creating %s: %s", folder.c_str(), std::strerror( errno ) );

	return false;
}

ucchar_p uint8_pTouucchar_p( const uint8_p& _source ) {
	return { _source.first.get(), _source.second };
}

uint8_p ucchar_pTouint8_p( const ucchar_p& _source ) {
	auto i8p = std::make_unique<uint8_t[]>(_source.second);
	std::memcpy( i8p.get(), _source.first, _source.second );
	uint8_p ret{ std::move(i8p), _source.second};
	return ret;
}

uint8_p make_uint8_p( uint64_t _size ) {
    return {  std::make_unique<uint8_t[]>(_size), _size };
}

int32_t getCircularArrayIndex( int32_t pi, int32_t size ) {
	return pi >= 0 ? ( pi % size ) : size - abs( pi % size );
}

void platform_sleep( long long milliseconds ) {
	std::this_thread::sleep_for( std::chrono::milliseconds( milliseconds ) );
}

bool invalidChar (unsigned char c)
{  
    return !(c>=0 && c <128);   
}

void stripUnicode(std::string & str)
{ 
    str.erase(std::remove_if(str.begin(),str.end(), invalidChar), str.end());  
}

std::string distaneToString( float _distance ) {

	if ( _distance < 1.0f ) {
		int idistance = static_cast<int>( _distance * 1000.0f );
		return std::to_string( idistance ) + " " + " m";
	}

	std::ostringstream so;
	so << std::fixed << std::setprecision( 1 ) << _distance << " km";
	return so.str();
}

std::string distaneToStringMeters( float _distance ) {

    std::ostringstream so;
    so << std::fixed << std::setprecision( 2 ) << _distance << " m";
    return so.str();
}

std::string priceMaker( uint64_t price ) {
	std::string priceString = std::to_string( price );
	int numSplits = static_cast<int>( priceString.length() ) / 3;
	std::string priceStringFinal = "$";
	for ( int t = 0; t < numSplits; t++ ) {
		priceStringFinal += priceString.substr( t * 3, 3 );
		if ( t != numSplits - 1 ) priceStringFinal += ",";
	}
	return priceStringFinal;
}

std::string sizeToString( const float x, const float y ) {

	std::ostringstream so;
	so << distaneToString(x) << " x " << distaneToString(y);
	return so.str();
}

std::string sizeToStringMeters( const float x, const float y ) {

    std::ostringstream so;
    so << distaneToStringMeters(x) << " x " << distaneToStringMeters(y);
    return so.str();
}

std::string sqmToString( const float sqm ) {
	std::ostringstream so;
	so << std::fixed << std::setprecision( 1 ) << sqm << "sqm";
	return so.str();
}


std::string UTCMillisToString() {
	return std::to_string( std::chrono::duration_cast< std::chrono::milliseconds >(
			std::chrono::system_clock::now().time_since_epoch() ).count());
}

int convertStringToHour( const std::string& _str ) {
    try {
        int ret = std::stoi( _str );
        if ( ret == 24 ) return 0;
        if ( ret < 0 || ret > 24 ) return -1;
        return ret;
    }
    catch(...) {
        return -1;
    }
}

int convertStringToMinuteOrSecond( const std::string& _str ) {
    try {
        int ret = std::stoi( _str );
        if ( ret < 0 || ret > 59 ) return -1;
        return ret;
    }
    catch(...) {
        return -1;
    }
}

float feetInchesToMeters( const float feetf, const float inchesf ) {
    return feetf * 0.3048f + inchesf * 0.0254f;
}

float stringFeetInchesToCm( const std::string& _text ) {
    auto feetPos = _text.find_first_of("'");
    if ( feetPos == std::string::npos ) {
        feetPos = _text.find_first_of("‘");
        if ( feetPos == std::string::npos ) return -1.0f;
    }

    auto feet = _text.substr( 0, feetPos );
    auto inches = _text.substr( feetPos+1, _text.length() - feetPos+1 );

	try {
		float feetf = std::stof( feet );
		float inchesf = std::stof( inches );
		return feetInchesToMeters(feetf, inchesf);
	}
	catch(...) {
		return -1.0f;
	}

}

std::string boolAlphaBinary( bool _flag ) {
    return _flag ? "1" : "0";
}

namespace DaemonPaths {
	std::string upload( const std::string& _type, const std::string& _name ) {
		return UploadDir + _type + "/" + _name;
	}

	std::string store( const std::string& _type, const std::string& _name ) {
		return UploadDir + _type + "_outputs/" + _name;
	}
}
