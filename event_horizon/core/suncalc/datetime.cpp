#include "datetime.h"
#include <ctime>
#include <iostream>
#include <locale>

//default to now
DateTime::DateTime() {
	value = time( NULL );
}

DateTime::DateTime( time_t initValue ) : value( initValue ) {
}

tm DateTime::getLocal( const time_t& time ) {
	std::tm tm_snapshot;
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
	localtime_s( &tm_snapshot, &time );
#else
	localtime_r( &time, &tm_snapshot ); // POSIX
#endif
	return tm_snapshot;
}

tm DateTime::getGmt( const time_t& time ) {
	std::tm tm_snapshot;
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
	gmtime_s( &tm_snapshot, &time );
#else
	gmtime_r( &time, &tm_snapshot ); // POSIX
#endif
	return tm_snapshot;
}

DateTime DateTime::addSeconds( int seconds, bool incrementDay /*= true */ ) {
	time_t newValue = value + seconds;

	if ( incrementDay || ( newValue - value ) < secondsPerDayTimeT )
		return DateTime( newValue );

	return DateTime( newValue - secondsPerDayTimeT );
}

DateTime::DateTime( int year, int month, int day, int hour, int minutes, int seconds ) {
	//time_t rawtime;
	//time(&rawtime);

	tm timeinfo; // = getLocal(rawtime);

	timeinfo.tm_year = year - 1900;
	timeinfo.tm_mon = month - 1;
	timeinfo.tm_mday = day;
	timeinfo.tm_hour = hour;
	timeinfo.tm_min = minutes;
	timeinfo.tm_sec = seconds;

	timeinfo.tm_isdst = -1;
	timeinfo.tm_wday = -1;
	timeinfo.tm_yday = -1;

	value = mktime( &timeinfo );
}

DateTime DateTime::from( DateTime defaultDate, int year, int month, int day, int hour, int minutes, int seconds ) {
	auto timeinfo = getLocal( defaultDate.value );

	if ( year != -1 ) timeinfo.tm_year = year - 1900;
	if ( month != -1 ) timeinfo.tm_mon = month - 1;
	if ( day != -1 ) timeinfo.tm_mday = day;
	if ( hour != -1 ) timeinfo.tm_hour = hour;
	if ( minutes != -1 ) timeinfo.tm_min = minutes;
	if ( seconds != -1 ) timeinfo.tm_sec = seconds;
	timeinfo.tm_isdst = -1;

	auto value = mktime( &timeinfo );

	return DateTime( value );
}

DateTime DateTime::fromJulianDays( double days ) {
	tm timeinfo = getLocal( (time_t)( ( days - J1970 ) * secondsPerDay ) );
	auto value = mktime( &timeinfo );
	return DateTime( value );
}

double DateTime::getJulianDays() {
	//auto gmt = getGmt(value);
	//auto gmtValue = mktime(&gmt);
	return ( value / secondsPerDay ) + J1970;
}

std::string DateTime::toDateString() {
	return toString( "%d/%m/%Y" );
}

std::string DateTime::toLongString() {
	return toString( "%d/%m/%Y %H:%M:%S" );
}

std::string DateTime::toString( std::string format ) {
	char mbstr[100];
	auto t = getLocal( value );
	if ( std::strftime( mbstr, sizeof( mbstr ), format.c_str(), &t ) ) {
		return std::string( mbstr );
	}

	throw "cannot convert date to string";
}

int DateTime::getHour() const {
	auto t = getLocal( value );
	return t.tm_hour;
}
