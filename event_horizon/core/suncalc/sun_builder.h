#pragma once

#include <tuple>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "suncalc.h"
#include "location_provider.h"
#include <core/math/vector3f.h>

class RawImage;

class SunBuilder {
public:
    SunBuilder();
    SunPosition buildFromString( const std::string& _naturalTime );
    void buildFromHour( const double _hours );
    void setCurrentTimeUniforms();
    void setCurrentLocation( std::string locationHint );
    void update( float timeStamp );
    void changeDefaultYear( int year );
    Vector3f getSunPosition() const;

    float GoldenHour() const { return mGoldenHour; }
    V3f GoldenHourColor() const;
    void GoldenHour( float val ) { mGoldenHour = val; }
    int getHour() const;

private:
    void moveSun( const DateTime& date, const GeoPosition& location );
    std::string hintTimeFrom( const std::vector<std::string>& _tokens );
    std::tuple<int, int, int> hintFixedTimeFrom( const std::string& _naturalTime );

private:
    DateTime mCurrentTime;
    GeoPosition mCurrentGeoPos;

    SunCalculations sunPostionCalculator;
    LocationProvider locationProvider;
    SunPosition mSunPosition;

    float mGoldenHour = 0.0f;
    int defaultYear = 2018;
    bool mbDirtyTime = false;

    std::shared_ptr<RawImage> goldenHourGradient;
    std::unordered_map<std::string, std::tuple<int, int>> dateParts;
    std::unordered_set<std::string> timeParts;
};
