#pragma once

#include <tuple>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "suncalc.h"
#include "location_provider.h"
#include <core/math/vector4f.h>

class RawImage;

class SunBuilder {
public:
    SunBuilder();
    SunPosition buildFromString( const std::string& _naturalTime );
    [[maybe_unused]] void buildFromHour( double _hours );
    void setCurrentTimeUniforms();
    void setCurrentLocation( std::string locationHint );
    void addSeconds( int seconds );
    void changeDefaultYear( int year );
    [[nodiscard]] V3f getSunPosition() const;
    [[nodiscard]] V4f GoldenHourColor() const;
    [[nodiscard]] V3f dailyGradientColor( float _dayDelta ) const;
    [[nodiscard]] int getHour() const;
    [[nodiscard]] float getDayDelta() const;

private:
    void moveSun( const DateTime& date, const GeoPosition& location );
    std::string hintTimeFrom( const std::vector<std::string>& _tokens );
    void hintDateFrom( const std::vector<std::string>& _tokens );
    static std::tuple<int, int, int> hintFixedTimeFrom( const std::string& _naturalTime );
    std::tuple<double, double, double>
    dumpDate( const std::string& timeString, std::unordered_map<std::string, DateTime> times,
              const GeoPosition& location );

private:
    DateTime mCurrentTime;
    GeoPosition mCurrentGeoPos{};

    SunCalculations sunPositionCalculator;
    LocationProvider locationProvider;
    SunPosition mSunPosition{};
    V4f mSunRadiance = V4fc::ONE;
    int defaultYear = 2019;
    bool mbDirtyTime = false;

    int day = -1;
    int month = -1;
    int year = -1;
    int hourValue = -1;
    int minuteValue = 0;
    int secondValue = 0;

    float dayDelta = 0.5f;
    std::shared_ptr<RawImage> goldenHourGradient;
    std::unordered_map<std::string, std::tuple<int, int>> dateParts;
    std::unordered_set<std::string> timeParts;
};
