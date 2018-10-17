#include "suncalc.h"
#include <cmath>

SunCalculations::SunCalculations() {
	times.push_back( std::make_tuple( -0.833, "sunrise", "sunset" ) );
	times.push_back( std::make_tuple( -0.3, "sunriseEnd", "sunsetStart" ) );
	times.push_back( std::make_tuple( -6, "dawn", "dusk" ) );
	times.push_back( std::make_tuple( -12, "nauticalDawn", "nauticalDusk" ) );
	times.push_back( std::make_tuple( -18, "nightEnd", "night" ) );
	times.push_back( std::make_tuple( 6, "goldenHourEnd", "goldenHour" ) );
}

SunCalculations::~SunCalculations() {
	times.clear();
}

double SunCalculations::rightAscension( double l, double b ) {
	return atan2( sin( l ) * cos( e ) - tan( b ) * sin( e ), cos( l ) );
}

double bound( double d ) {
	if ( d > 1 )  return 1;
	if ( d < -1 )  return -1;
	return d;
}

double SunCalculations::declination( double l, double b ) {
	return asin( bound( sin( b ) * cos( e ) + cos( b ) * sin( e ) * sin( l ) ) );
}

double SunCalculations::azimuth( double H, double phi, double dec ) {
	return atan2( sin( H ), cos( H ) * sin( phi ) - tan( dec ) * cos( phi ) );
}

double SunCalculations::altitude( double H, double phi, double dec ) {
	return asin( bound( sin( phi ) * sin( dec ) + cos( phi ) * cos( dec ) * cos( H ) ) );
}

double SunCalculations::siderealTime( double d, double lw ) {
	return rad * ( 280.16 + 360.9856235 * d ) - lw;
}

double SunCalculations::astroRefraction( double h ) {
	if ( h < 0 ) // the following formula works for positive altitudes only.
		h = 0; // if h = -0.08901179 a div/0 would occur.

	// formula 16.4 of "Astronomical Algorithms" 2nd edition by Jean Meeus (Willmann-Bell, Richmond) 1998.
	// 1.02 / tan(h + 10.26 / (h + 5.10)) h in degrees, result in arc minutes -> converted to rad:
	return 0.0002967 / tan( h + 0.00312536 / ( h + 0.08901179 ) );
}

double SunCalculations::solarMeanAnomaly( double d ) {
	return rad * ( 357.5291 + 0.98560028 * d );
}

double SunCalculations::eclipticLongitude( double M ) {
	auto C = rad * ( 1.9148 * sin( M ) + 0.02 * sin( 2 * M ) + 0.0003 * sin( 3 * M ) ); // equation of center
	auto P = rad * 102.9372; // perihelion of the Earth
	return M + C + P + PI;
}

std::tuple<double, double> SunCalculations::sunCoords( double d ) {
	auto M = solarMeanAnomaly( d );
	auto L = eclipticLongitude( M );
	return std::make_tuple( declination( L, 0 ), rightAscension( L, 0 ) );
}

std::tuple<double, double> SunCalculations::getSphericalPosition( DateTime date, double lat, double lng ) {
	auto lw = rad * -lng;
	auto phi = rad * lat;
	auto d = date.getJulianDays() - J2000;

	auto c = sunCoords( d );
	auto H = siderealTime( d, lw ) - std::get<1>( c );

	return std::make_tuple( azimuth( H, phi, std::get<0>( c ) ), altitude( H, phi, std::get<0>( c ) ) );
}

SunPosition SunCalculations::getPosition( DateTime date, double lat, double lng ) {
	auto coord = getSphericalPosition( date, lat, lng );

	SunPosition sp;

	sp.azimuthRad = std::get<0>( coord );
	sp.altitudeRad = std::get<1>( coord );

	sp.x = -sin( sp.azimuthRad ) * cos( sp.altitudeRad );
	sp.y = sin( sp.altitudeRad );
	sp.z = -cos( sp.azimuthRad ) * cos( sp.altitudeRad );

	return sp;
}

double SunCalculations::julianCycle( double d, double lw ) { return round( d - J0 - lw / ( 2 * PI ) ); }

double SunCalculations::approxTransit( double Ht, double lw, double n ) {
	return J0 + ( Ht + lw ) / ( 2 * PI ) + n;
}

double SunCalculations::solarTransitJ( double ds, double M, double L ) {
	return J2000 + ds + 0.0053 * sin( M ) - 0.0069 * sin( 2 * L );
}

double SunCalculations::hourAngle( double h, double phi, double d ) {
	return acos( bound( ( sin( h ) - sin( phi ) * sin( d ) ) / ( cos( phi ) * cos( d ) ) ) );
}

// returns set time for the given sun altitude
double SunCalculations::getSetJ( double h, double lw, double phi, double dec, double n, double M, double L ) {
	auto w = hourAngle( h, phi, dec );
	auto a = approxTransit( w, lw, n );
	return solarTransitJ( a, M, L );
}

// calculates sun times for a given date and latitude/longitude
std::unordered_map<std::string, DateTime> SunCalculations::getTimes( DateTime date, double lat, double lng ) {
	auto lw = rad * -lng;
	auto phi = rad * lat;
	auto d = date.getJulianDays() - J2000;
	auto n = julianCycle( d, lw );
	auto ds = approxTransit( 0, lw, n );
	auto M = solarMeanAnomaly( ds );
	auto L = eclipticLongitude( M );
	auto dec = declination( L, 0 );
	auto Jnoon = solarTransitJ( ds, M, L );

	std::unordered_map<std::string, DateTime> result;

	result["solarNoon"] = DateTime::fromJulianDays( Jnoon );
	result["nadir"] = DateTime::fromJulianDays( Jnoon - 0.5 );

	for ( auto time : times ) {
		auto Jset = getSetJ( std::get<0>( time ) * rad, lw, phi, dec, n, M, L );
		auto Jrise = Jnoon - ( Jset - Jnoon );

		result[std::get<1>( time )] = DateTime::fromJulianDays( Jrise );
		result[std::get<2>( time )] = DateTime::fromJulianDays( Jset );
	}

	return result;
}