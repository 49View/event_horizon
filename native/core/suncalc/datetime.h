#pragma once

#include <chrono>
#include <string>
#define __STDC_WANT_LIB_EXT1__ 1
#include <time.h>

class DateTime {
public:
	DateTime();
	DateTime( int year, int month, int day, int hour = 0, int minutes = 0, int seconds = 0 );

	double getJulianDays() const;
	double getTimeStamp() const ;
	std::string toDateString() const ;
	std::string toLongString() const ;
	std::string toLongStringWithTimeStamp() const ;
	std::string toString( std::string format ) const;
	[[nodiscard]] int getHour() const;
	[[nodiscard]] int getMinutes() const;
	[[nodiscard]] int getSeconds() const;
    void setHour( int _h );
    void setMinutes( int _h );
    void setSeconds( int _h );
    void setHourMinutesSeconds( int _h, int _m = 0, int _s = 0 );
	DateTime addSeconds( int seconds, bool incrementDay = true ) const;

private:
	DateTime( time_t initValue );
	time_t value; //value get stored as gmt second from 1/1/1970
	static constexpr double secondsPerDay = 24 * 60 * 60;
	static constexpr time_t secondsPerDayTimeT = 24 * 60 * 60;

	static constexpr double  J1970 = 2440587.5; //c++ date epoch

	//static
public:
	static DateTime fromJulianDays( double days );
	static DateTime from( DateTime defaultDate, int year = -1, int month = -1, int day = -1, int hour = -1, int minutes = -1, int seconds = -1 );

    friend std::ostream& operator<<( std::ostream& o, const DateTime& val );

private:
	static tm getLocal( const time_t& dateValue );
	static tm getGmt( const time_t& dateValue );
};
