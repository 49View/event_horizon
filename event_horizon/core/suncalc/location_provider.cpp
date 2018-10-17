#include "location_provider.h"

GeoPosition LocationProvider::getPosition( std::string locationHint ) {
	if ( locationHint == "DefaultTest" ) return GeoPosition{ 51.41233, -0.3006890 };

	if ( locationHint == "london" ) return GeoPosition{ 51.507351, -0.127758 };
	if ( locationHint == "kingston" ) return GeoPosition{ 51.41233, -0.3006890 };

	if ( locationHint == "scotland" ) return GeoPosition{ 56.490671, -4.202646 };
	if ( locationHint == "england" ) return GeoPosition{ 52.355518, -1.174320 };

	if ( locationHint == "equator" ) return GeoPosition{ 0.0, 0.0 };
	if ( locationHint == "tropicofcancer" ) return GeoPosition{ 23.43, 0.0 };

	//if (locationHint == "UK")
	return GeoPosition{ 55.378051, -3.435973 };
}