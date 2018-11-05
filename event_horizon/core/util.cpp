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

static bool gError = false;
int64_t globalHash = 1;

//====================================
// Logging
//====================================

static logCF logCallback = nullptr;

void hookLogCallbackFunction( logCF _cf ) {
	logCallback = _cf;
}

void platformLogPrint( LogPriority priority, const char* /*tag*/, const char *fmt, va_list vl ) {
	char buffer[65536];

	if ( priority == LOG_PRIORITY_ERROR ) {
		vsnprintf( buffer, 65536, fmt, vl );
	} else if ( priority == LOG_PRIORITY_WARN ) {
		vsnprintf( buffer, 65536, fmt, vl );
	} else {
		vsnprintf( buffer, 65536, fmt, vl );
	}

	//TODO fo linux it works just as follow
	std::cout << buffer << std::endl;
	if ( logCallback ) {
		logCallback( buffer );
	}

	//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	//std::wstring wbuffer = converter.from_bytes( buffer );

	//std::wcout << wbuffer << L"\n";
}

void platformBreakpoint() {
	// Use abort, so we will get a callstack sometimes
	std::abort();
}

void logPrint( LogPriority logPriority, const char *tag, const char *fmt, ... ) {
	va_list vl;
	va_start( vl, fmt );

	// Android specific print
	platformLogPrint( logPriority, tag, fmt, vl );

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
		platformLogPrint( LOG_PRIORITY_ERROR, LOG_TAG, fmt, vl );
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

int32_t getCircularArrayIndex( int32_t pi, int32_t size ) {
	return pi >= 0 ? ( pi % size ) : size - abs( pi % size );
}

void platform_sleep( long long milliseconds ) {
	std::this_thread::sleep_for( std::chrono::milliseconds( milliseconds ) );
}

const int64_t HashInc() {
	return ++globalHash;
}

bool invalidChar (unsigned char c)
{  
    return !(c>=0 && c <128);   
}

void stripUnicode(std::string & str)
{ 
    str.erase(std::remove_if(str.begin(),str.end(), invalidChar), str.end());  
}

bool isFilenameAFolder( const std::string& input ) {
	size_t pos = input.find_last_of( "/" );
	if ( pos != std::string::npos && pos == input.size() - 1 ) return true;
	pos = input.find_last_of( "\\" );
	if ( pos != std::string::npos && pos == input.size() - 1 ) return true;
	
	return false;
}

std::string getFileNameNoExt( const std::string& input ) {
	std::string ret = input;
	size_t pos = ret.find_last_of( "." );
	if ( pos != std::string::npos ) {
		ret = ret.substr( 0, pos );
	}

	return ret;
}

std::string getFileNameKey( const std::string& input, EncodingStatusFlag ef ) {
	return getFileNameNoExt( getFileName( input, ef ) );
}


std::string getFileNameCallbackKey( const std::string& input ) {
	return url_decode(getFileNameKey( input, EncodingStatusFlag::NotEncode));
}

std::string getFileNameExt( const std::string& input ) {
	std::string ret = "";
	size_t pos = input.find_last_of( "." );
	if ( pos != std::string::npos ) {
		ret = input.substr( pos, input.length() - pos );
	}

	return ret;
}

std::string getFileName( const std::string& input, EncodingStatusFlag ef ) {
	std::string ret = input;
	// trim path before filename
	size_t pos = ret.find_last_of( "/" );
	if ( pos != std::string::npos ) {
		ret = ret.substr( pos + 1, ret.length() );
	}
	pos = ret.find_last_of( "\\" );
	if ( pos != std::string::npos ) {
		ret = ret.substr( pos + 1, ret.length() );
	}

	if ( checkBitWiseFlag(ef, EncodingStatusFlag::DoEncode ) ) {
		pos = ret.rfind( "%2F" );
		if ( pos != std::string::npos ) {
			ret = ret.substr( pos + 3, ret.length() );
		}
	}

	return ret;
}

std::string getFileNamePath( const std::string& input ) {
    std::string ret = input;
    // trim path before filename
    size_t pos = ret.find_last_of( "/" );
    if ( pos != std::string::npos ) {
        return ret.substr( 0, pos );
    }
    pos = ret.find_last_of( "\\" );
    if ( pos != std::string::npos ) {
        return ret.substr( 0, pos );
    }

    return ret;
}

std::string getFileNameOnly( const std::string& input ) {
	std::string ret = input;
	// trim path before filename
	size_t pos = ret.find_last_of( "/" );
	if ( pos != std::string::npos ) {
		ret = ret.substr( pos + 1, ret.length() );
	}
	pos = ret.find_last_of( "\\" );
	if ( pos != std::string::npos ) {
		ret = ret.substr( pos + 1, ret.length() );
	}
	pos = ret.find_last_of( "." );
	if ( pos != std::string::npos ) {
		ret = ret.substr( 0, pos );
	}

	return ret;
}

std::string getLastEntryInPath( const std::string& input ) {
	std::string ret = input;
	if ( ret[ret.size()-1] == '/' ) {
		ret = ret.substr(0, ret.size()-1);
	}
	// trim path before filename
	size_t pos = ret.find_last_of( "/" );
	if ( pos != std::string::npos ) {
		ret = ret.substr( pos + 1, ret.length() );
	}

	return ret;
}

bool isFileExtAnImage( const std::string& _ext ) {

	auto ext = toLower(_ext);

	if ( ext == ".jpg" ) return true;
	if ( ext == ".png" ) return true;
	if ( ext == ".jpeg" ) return true;
	if ( ext == ".tga" ) return true;
	if ( ext == ".bmp" ) return true;
	if ( ext == ".psd" ) return true;
	if ( ext == ".gif" ) return true;
	if ( ext == ".hdr" ) return true;
	if ( ext == ".pic" ) return true;

	return false;
}

std::string toLower( const std::string _input ) {
	std::string ret = _input;
	std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
	return ret;
}

bool string_ends_with( const std::string& source, const std::string& match ) {
	size_t pos = source.rfind( match );
	if ( pos != std::string::npos ) {
		size_t targetPos = source.length() - match.length();
		return pos == targetPos;
	}
	return false;
}

std::string string_trim_upto( const std::string& source, const std::string& match ) {
	size_t pos = source.rfind( match );
	if ( pos != std::string::npos ) {
		return source.substr(0, pos);
	}
	return source;
}

std::string string_trim_after( const std::string& source, const std::string& match ) {
    size_t pos = source.rfind( match );
    if ( pos != std::string::npos ) {
        return source.substr(pos + 1, source.length() - pos);
    }
    return source;
}

std::string string_trim_upto( const std::string& source, const std::vector<std::string>& matches ) {
	for ( auto & match : matches ) {
		auto ret = string_trim_upto( source, match );
		if ( ret != source ) return ret;
	}
	return source;
}

bool nameHasImageExtension( const std::string& input ) {
	std::string ext = getFileNameExt(input);
	if ( ext != "" && ( ext == ".jpg" || ext == ".jpeg" || ext == ".png" ) ) {
		return true;
	}
	
	return false;
}

std::string distaneToString( float _distance ) {

	if ( _distance < 1.0f ) {
		int idistance = static_cast<int>( _distance * 1000.0f );
		return std::to_string( idistance ) + " " + " meters";
	}

	std::ostringstream so;
	so << std::fixed << std::setprecision( 1 ) << _distance << " km";
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
	so << std::fixed << std::setprecision( 1 ) << x << "m x " << y << "m";
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

void FrameInvalidator::invalidate() {
	bInvalidated = true;
	bFirstInvalidated = true;
}

bool FrameInvalidator::needsRefresh( int _counter, int _frameGap ) {

	if ( bInvalidated ) {
		if ( bFirstInvalidated ) {
			frameCounter = _counter;
			bFirstInvalidated = false;
		}
		if ( _counter == frameCounter + _frameGap ) {
			bInvalidated = false;
			return true;
		}
		return false;
	}

	return false;
}

namespace DaemonPaths {
	std::string upload( const std::string& _type, const std::string& _name ) {
		return UploadDir + _type + "/" + _name;
	}

	std::string store( const std::string& _type, const std::string& _name ) {
		return UploadDir + _type + "_outputs/" + _name;
	}
}
