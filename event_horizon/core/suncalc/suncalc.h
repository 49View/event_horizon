#pragma once
#include <tuple>
#include <vector>
#include <unordered_map>
#include "datetime.h"

struct SunPosition {
	double x;
	double y;
	double z;
	double azimuthRad;
	double altitudeRad;
};

class SunCalculations {
public:
	SunCalculations();
	~SunCalculations();

	SunPosition getPosition( DateTime date, double lat, double lng );
	std::unordered_map<std::string, DateTime> getTimes( DateTime date, double lat, double lng );

private:
	double rightAscension( double l, double b );
	double declination( double l, double b );
	double azimuth( double H, double phi, double dec );
	double altitude( double H, double phi, double dec );
	double siderealTime( double d, double lw );
	double astroRefraction( double h );
	double solarMeanAnomaly( double d );
	double eclipticLongitude( double M );
	std::tuple<double, double> sunCoords( double d );
	std::tuple<double, double> getSphericalPosition( DateTime date, double lat, double lng );
	double julianCycle( double d, double lw );
	double approxTransit( double Ht, double lw, double n );
	double solarTransitJ( double ds, double M, double L );
	double hourAngle( double h, double phi, double d );
	double getSetJ( double h, double lw, double phi, double dec, double n, double M, double L );

	std::vector<std::tuple<double, std::string, std::string>> times;

	const double J0 = 0.0009;
	const double PI = 3.141592653589793;
	const double rad = PI / 180;
	const double e = rad * 23.4397; // obliquity of the Earth
	const double J2000 = 2451545; // julian date 1/1/2000 noon
};
