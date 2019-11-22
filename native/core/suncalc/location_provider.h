#pragma once
#include <string>

struct GeoPosition {
	double latitude;
	double longitude;
};

class LocationProvider {
public:
	GeoPosition getPosition( std::string locationHint );
};