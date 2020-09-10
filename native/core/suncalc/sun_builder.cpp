#include <utility>
#include "sun_builder.h"

#include "datetime.h"
#include "golden_color.hpp"
#include "../math/spherical_harmonics.h"
#include "../service_factory.h"
#include "../configuration/app_options.h"
#include <core/raw_image.h>
#include <core/util.h>

SunBuilder::SunBuilder() {

    goldenHourGradient = std::make_shared<RawImage>(golden_color_png, golden_color_png_len);

    dateParts["spring"] = std::make_tuple( 20, 3 );
	dateParts["summer"] = std::make_tuple( 21, 6 );
	dateParts["autumn"] = std::make_tuple( 22, 9 );
	dateParts["winter"] = std::make_tuple( 21, 12 );

	dateParts["beginningspring"] = std::make_tuple( 1, 3 );
	dateParts["beginningsummer"] = std::make_tuple( 1, 6 );
	dateParts["beginningautumn"] = std::make_tuple( 1, 9 );
	dateParts["beginningwinter"] = std::make_tuple( 1, 12 );
	dateParts["middlespring"] = std::make_tuple( 15, 4 );
	dateParts["middlesummer"] = std::make_tuple( 15, 7 );
	dateParts["middleautumn"] = std::make_tuple( 15, 10 );
	dateParts["middlewinter"] = std::make_tuple( 15, 1 );
	dateParts["endspring"] = std::make_tuple( 30, 5 );
	dateParts["endsummer"] = std::make_tuple( 31, 8 );
	dateParts["endautumn"] = std::make_tuple( 30, 11 );
	dateParts["endwinter"] = std::make_tuple( 28, 2 );

	dateParts["wintersolstice"] = std::make_tuple( 21, 12 );
	dateParts["summersolstice"] = std::make_tuple( 21, 6 );
	dateParts["springequinox"] = std::make_tuple( 20, 3 );
	dateParts["autumnequinox"] = std::make_tuple( 22, 9 );

	dateParts["january"] = std::make_tuple( 31, 1 );
	dateParts["february"] = std::make_tuple( 28, 2 );
	dateParts["march"] = std::make_tuple( 31, 3 );
	dateParts["april"] = std::make_tuple( 30, 4 );
	dateParts["may"] = std::make_tuple( 31, 5 );
	dateParts["june"] = std::make_tuple( 30, 6 );
	dateParts["july"] = std::make_tuple( 31, 7 );
	dateParts["august"] = std::make_tuple( 31, 8 );
	dateParts["september"] = std::make_tuple( 30, 9 );
	dateParts["october"] = std::make_tuple( 31, 10 );
	dateParts["november"] = std::make_tuple( 30, 11 );
	dateParts["december"] = std::make_tuple( 31, 12 );

	timeParts.insert( "sunrise" );
	timeParts.insert( "sunriseEnd" );
	timeParts.insert( "dawn" );
	timeParts.insert( "nauticalDawn" );
	timeParts.insert( "nightEnd" );
    timeParts.insert( "goldenHour" );
	timeParts.insert( "goldenHourEnd" );
    timeParts.insert( "sunsetStart" );
	timeParts.insert( "sunset" );
	timeParts.insert( "dusk" );
	timeParts.insert( "nauticalDusk" );
	timeParts.insert( "night" );
	timeParts.insert( "noon" );

	setCurrentLocation( "Kingston" );

	buildFromString( "spring sunriseEnd" );

}

void SunBuilder::changeDefaultYear( int _year ) {
	defaultYear = _year;
}

Vector3f SunBuilder::getSunPosition() const {
	return { mSunPosition.x, mSunPosition.y, mSunPosition.z };
}

int SunBuilder::getHour() const {
	return mCurrentTime.getHour();
}

float calcGoldenHourRatio( double _azimut, float _k  ) {
    return 1.0f / ( 1.0f + powf( M_E, static_cast<float>( -_azimut ) * _k ) );
}

std::tuple<double, double, double> SunBuilder::dumpDate( const std::string& timeString, std::unordered_map<std::string, DateTime> times, const GeoPosition& location ) {
    float k = 20.0f;
    auto date = times[timeString];
    auto sunPsunSetStart = sunPositionCalculator.getPosition(date, location.latitude, location.longitude );
    auto ghr = calcGoldenHourRatio(sunPsunSetStart.altitudeRad, k);
//    LOGRS( timeString << " " << date << " Golden Hour: " << ghr << "Altitude: " << sunPsunSetStart.altitudeRad );
    return { date.getTimeStamp(), ghr, sunPsunSetStart.altitudeRad };
}

template <typename T>
bool checkTimesRange( double x, double a, double b, const T& ac, const T& bc, T& radiance ) {
    if ( x >= a && x < b ) {
        auto i = interpolateInverse( a, b, x );
        radiance = interpolate( ac, bc, i );
        return true;
    }
    return false;
}

void SunBuilder::moveSun( const DateTime& date, const GeoPosition& location ) {
	mCurrentTime = date;
	mCurrentGeoPos = location;
	mbDirtyTime = true;
    float k = 20.0f;

    auto times = sunPositionCalculator.getTimes(date, mCurrentGeoPos.latitude, mCurrentGeoPos.longitude );
    auto timesDayLater = sunPositionCalculator.getTimes(date.addSeconds(86400), mCurrentGeoPos.latitude, mCurrentGeoPos.longitude );

    mSunPosition = sunPositionCalculator.getPosition(date, location.latitude, location.longitude );

    auto nadir = dumpDate( "nadir", times, location );
    auto nadirNextDay = dumpDate( "nadir", timesDayLater, location );
    auto sunrise = dumpDate( "sunrise", times, location );
    auto sunriseEnd = dumpDate( "sunriseEnd", times, location );
    auto solorNoon = dumpDate( "solorNoon", times, location );
    auto sunsetStart = dumpDate( "sunsetStart", times, location );
    auto sunset = dumpDate( "sunset", times, location );

    auto currTimeStamp = date.getTimeStamp();

    // Golden hour
    // 0 : night
    // [0,1] golden hour
    // 1 day

    auto lSunriseColor = V4fc::XTORGBA("#C6FFDD") ;
    auto lNoonColor = V4fc::XTORGBA("#FBD786");
    auto lSunsetColor = V4fc::XTORGBA("#f7999d");
    auto nightColor = V4f{V3f{0.05f}, 0.0f};

    mSunRadiance = nightColor;
    float ghr = calcGoldenHourRatio(mSunPosition.altitudeRad, k);
    checkTimesRange( currTimeStamp, std::get<0>(nadir), std::get<0>(sunrise), nightColor, nightColor,mSunRadiance );
    checkTimesRange( currTimeStamp, std::get<0>(sunrise), std::get<0>(sunriseEnd), nightColor, lSunriseColor,mSunRadiance );
    checkTimesRange( currTimeStamp, std::get<0>(sunriseEnd), std::get<0>(solorNoon), lSunriseColor, lNoonColor,mSunRadiance );
    checkTimesRange( currTimeStamp, std::get<0>(solorNoon), std::get<0>(sunsetStart), lNoonColor, lNoonColor,mSunRadiance );
    checkTimesRange( currTimeStamp, std::get<0>(sunsetStart), std::get<0>(sunset), lNoonColor, lSunsetColor,mSunRadiance );
    checkTimesRange( currTimeStamp, std::get<0>(sunset), std::get<0>(nadirNextDay), lSunsetColor, nightColor,mSunRadiance );

    float sunK = 1.0f;
    float sunMult = max( 0.0f, ghr + static_cast<float>(mSunPosition.altitudeRad) ) * sunK;

    float dayDelta = 0.5f;
    checkTimesRange( currTimeStamp, std::get<0>(sunrise), std::get<0>(sunset), 0.0f, 1.0f, dayDelta );

    auto dayGradientColor = dailyGradientColor(dayDelta);
    mSunRadiance = V4f{V3f{sunMult} + dayGradientColor*2.0f, ghr};
    LOGRS( "Current " << date << " dayDelta: " << dayDelta << " Golden Hour: " << ghr << "Altitude: " << mSunPosition.altitudeRad << " Day Gradient Color: " << dayGradientColor );
}

std::string SunBuilder::hintTimeFrom( const std::vector<std::string>& _tokens ) {

	for ( const auto& t : _tokens ) {
		if ( const auto& it =  timeParts.find(t); it != timeParts.end() ) {
			return *it;
		}
        if ( auto [ h, m, s ] = hintFixedTimeFrom( t ); h >= 0 && m >= 0 && s >= 0 ) {
            hourValue = h;
            minuteValue = m;
            secondValue = s;
            return "";
        }
	}
	return "now";
}

void SunBuilder::hintDateFrom( const std::vector<std::string>& _tokens ) {

    for ( const auto& dateHint : _tokens ) {
        if ( dateHint != "today" ) {
            year = defaultYear;
            auto dayAndMonth = dateParts.find( dateHint );

            if ( dayAndMonth != dateParts.end() ) {
                month = std::get<1>( dayAndMonth->second );
                day = std::get<0>( dayAndMonth->second );
                break;
            }
        }
    }
}

std::tuple<int, int, int> SunBuilder::hintFixedTimeFrom( const std::string& _naturalTime ) {

	int h = -1;
	int m = 0;
	int s = 0;

	auto mt = regEx( R"((\d{1,2}):?-?(\d{1,2}):?-?(\d{1,2})*)", _naturalTime );
	size_t mts = mt.size();
	if ( mts >= 3 ) {
		if ( mt[1].matched ) h = convertStringToHour( mt[1].str() );
		if ( mt[2].matched ) m = convertStringToMinuteOrSecond( mt[2].str() );
		if ( mt[3].matched ) s = convertStringToMinuteOrSecond( mt[3].str() );
	}
    return std::make_tuple(h, m, s);
}

SunPosition SunBuilder::buildFromString( const std::string& _naturalTime ) {

	auto tokens = split( _naturalTime );

	auto date = DateTime();

	if ( tokens[0] != "now" ) {
        bool lbFinshed = false;
	    if ( tokens.size() == 6 ) {
	        std::array<int, 6> dvalues{};
	        lbFinshed = true;
            size_t c = 0;
            for ( const auto& t : tokens ) {
                if ( t.find_first_not_of( "0123456789" ) == std::string::npos ) {
                    dvalues[c++] = std::atoi( t.c_str() );
                } else{
                    lbFinshed = false;
                    break;
                }
            }
            if ( lbFinshed ) {
                year = dvalues[0];
                month = dvalues[1];
                day = dvalues[2];
                hourValue = dvalues[3];
                minuteValue = dvalues[4];
                secondValue = dvalues[5];
                date = DateTime::from( date, year, month, day, hourValue, minuteValue, secondValue );
            }
        }
	    if ( !lbFinshed ) {
            hintDateFrom( tokens );
            std::string timeHint = hintTimeFrom( tokens );

            if ( timeHint.empty() ) {
                date = DateTime::from( date, year, month, day, hourValue, minuteValue, secondValue );
            } else {
                auto searchDate = DateTime::from( date, year, month, day, hourValue, minuteValue, secondValue );
                auto times = sunPositionCalculator.getTimes(searchDate, mCurrentGeoPos.latitude, mCurrentGeoPos.longitude );

                if ( auto timeItr = times.find( timeHint ); timeItr != times.end() ) {
                    date = timeItr->second;
                } else {
                    if ( auto [ h, m, s ] = hintFixedTimeFrom( _naturalTime ); h >= 0 && m >= 0 && s >= 0 ) {
                        hourValue = h;
                        minuteValue = m;
                        secondValue = s;
                        date = DateTime::from( date, year, month, day, h, m, s );
                    } else {
                        hourValue = 12;
                        minuteValue = 0;
                        secondValue = 0;
                        date = times["solarNoon"];
                    }
                }
            }
	    }
	}
	//std::cout << date.toLongString();

    hourValue = date.getHour();
    minuteValue = date.getMinutes();
    secondValue = date.getSeconds();

    moveSun( date, mCurrentGeoPos );

	return mSunPosition;
}

[[maybe_unused]] void SunBuilder::buildFromHour( const double _hours ) {
	int iHours = static_cast<int>( _hours );
	mCurrentTime = DateTime::from( mCurrentTime, -1, -1, -1, iHours, static_cast<int>( ( _hours - iHours ) * 60.0 ) );
	moveSun( mCurrentTime, mCurrentGeoPos );
}

void SunBuilder::setCurrentTimeUniforms() {
	auto sp = mSunPosition;

	auto theta = M_PI_2 - sp.altitudeRad;
	auto phi = -sp.azimuthRad - M_PI_2;

	if ( theta < 0 ) theta += TWO_PI;
	if ( phi < 0 ) phi += TWO_PI;

	SSH.rotateSHCoefficients( theta, phi );

//	SMM.SunPosition( { sp.x, sp.y, sp.z } );
}

void SunBuilder::addSeconds( int seconds ) {
    DateTime newDate = mCurrentTime.addSeconds( seconds, true );
    moveSun( newDate, mCurrentGeoPos );
}

void SunBuilder::setCurrentLocation( std::string locationHint ) {
	mCurrentGeoPos = locationProvider.getPosition( std::move(locationHint) );
}

V4f SunBuilder::GoldenHourColor() const {
    return mSunRadiance;
//	ASSERT( GoldenHour() >= 0.0f && GoldenHour() <= 1.0f );
//	const float sunK = 1.0f;
//	float sunPower = mSunPosition.altitudeRad > 0.0f ? ((mSunPosition.altitudeRad) / M_PI_2) * sunK : 0.0f;
//    auto gdl = static_cast<size_t>(goldenHourGradient->width - 1);
//	auto index = gdl - static_cast<size_t>(((mSunPosition.altitudeRad) / M_PI_2) * gdl);
//	auto col = goldenHourGradient->at<uint32_t>( static_cast<unsigned int>(index), 0);
//	auto ret = V4fc::ITORGBA(col) * sunPower;
//	return ret.xyz();
}

V3f SunBuilder::dailyGradientColor( float _dayDelta ) const {
    auto gdl = static_cast<size_t>(goldenHourGradient->width - 1);
	auto index = gdl - static_cast<size_t>( _dayDelta * gdl);
	auto col = goldenHourGradient->at<uint32_t>( static_cast<unsigned int>(index), 0);
	auto ret = V4fc::ITORGBA(col);
	return ret.xyz();
}
