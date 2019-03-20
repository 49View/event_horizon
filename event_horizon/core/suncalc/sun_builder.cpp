#include <iostream>
#include "sun_builder.h"

#include "datetime.h"
#include "golden_color.hpp"
#include "../math/spherical_harmonics.h"
#include "../math/math_util.h"
#include "../service_factory.h"
#include "../configuration/app_options.h"
#include <core/raw_image.h>

SunBuilder::SunBuilder() {
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
	timeParts.insert( "sunsetStart" );
	timeParts.insert( "dawn" );
	timeParts.insert( "nauticalDawn" );
	timeParts.insert( "nightEnd" );
	timeParts.insert( "goldenHourEnd" );
	timeParts.insert( "sunset" );
	timeParts.insert( "dusk" );
	timeParts.insert( "nauticalDusk" );
	timeParts.insert( "night" );
	timeParts.insert( "goldenHour" );

	setCurrentLocation( "Kingston" );

	buildFromString( "spring noon" );

	goldenHourGradient = std::make_shared<RawImage>(golden_color_png, golden_color_png_len);
}

void SunBuilder::changeDefaultYear( int year ) {
	defaultYear = year;
}

Vector3f SunBuilder::getSunPosition() const {
	return { mSunPosition.x, mSunPosition.y, mSunPosition.z };
}

int SunBuilder::getHour() const {
	return mCurrentTime.getHour();
}

void SunBuilder::moveSun( const DateTime& date, const GeoPosition& location ) {
	mCurrentTime = date;
	mCurrentGeoPos = location;
	mbDirtyTime = true;

	mSunPosition = sunPostionCalculator.getPosition( date, location.latitude, location.longitude );

	// Golden hour
	// 0 : day
	// [0,1] golden hour
	// 1 night

	float k = 20.0f;
	GoldenHour( 1.0f / ( 1.0f + powf( M_E, static_cast<float>( -mSunPosition.altitudeRad ) * k ) ) );
}

std::string SunBuilder::hintTimeFrom( const std::vector<std::string>& _tokens ) {

	for ( const auto& t : _tokens ) {
		if ( const auto& it =  timeParts.find(t); it != timeParts.end() ) {
			return *it;
		}
	}
	return "now";
}

std::tuple<int, int, int> SunBuilder::hintFixedTimeFrom( const std::string& _naturalTime ) {

	int h = -1;
	int m = 0;
	int s = 0;

	auto mt = regEx( "(\\d{1,2}):?-?(\\d{1,2}):?-?(\\d{1,2})*", _naturalTime );
	size_t mts = mt.size();
	if ( mts >= 3 ) {
		if ( mt[1].matched ) h = convertStringToHour( mt[1].str() );
		if ( mt[2].matched ) m = convertStringToMinuteOrSecond( mt[2].str() );
		if ( mt[3].matched ) s = convertStringToMinuteOrSecond( mt[3].str() );
	}
    return std::make_tuple(h, m, s);
}

SunPosition SunBuilder::buildFromString( const std::string& _naturalTime ) {

	std::string dateHint = "today";
	int dateValue = -1;
	int timeValue = -1;
	int minutesValue = 0;
	int secondValue = 0;

	auto tokens = split( _naturalTime );

	auto date = DateTime();

	if ( tokens[0] != "now" ) {
		int day = -1;
		int month = -1;
		int year = -1;

		if ( dateHint != "today" ) {
			year = defaultYear;
			auto dayAndMonth = dateParts.find( dateHint );

			if ( dayAndMonth != dateParts.end() ) {
				month = std::get<1>( dayAndMonth->second );
				auto tmpDay = std::get<0>( dayAndMonth->second );
				day = dateValue > 0 && dateValue <= tmpDay ? dateValue : tmpDay;
			}
		}

		if ( timeValue > -1 ) {
			date = DateTime::from( date, year, month, day, timeValue, minutesValue, secondValue );
		} else {
			auto searchDate = DateTime::from( date, year, month, day, 23, 0, 0 );
			auto times = sunPostionCalculator.getTimes( searchDate, mCurrentGeoPos.latitude, mCurrentGeoPos.longitude );

            std::string timeHint = hintTimeFrom( tokens );

			if ( auto timeItr = times.find( timeHint ); timeItr != times.end() ) {
                date = timeItr->second;
			} else {
			    if ( auto [ h, m, s ] = hintFixedTimeFrom( _naturalTime ); h >= 0 && m >= 0 && s >= 0 ) {
                    date = DateTime::from( date, year, month, day, h, m, s );
			    } else {
                    date = times["solarNoon"];
			    }
			}
		}
	}
	//std::cout << date.toLongString();

	moveSun( date, mCurrentGeoPos );

	return mSunPosition;
}

void SunBuilder::buildFromHour( const double _hours ) {
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

void SunBuilder::update( [[maybe_unused]] float timeStamp ) {
//	if ( AOS->sunOptions.autoTransition ) {
//		auto elapsedSeconds = mPrevtimeStamp - timeStamp;
//
//		if ( elapsedSeconds > 0 ) {
//			mPrevtimeStamp = timeStamp;
//
//			DateTime newDate = mCurrentTime.addSeconds( static_cast<int>( elapsedSeconds * AOS->sunOptions.transitionSpeed ), false );
//
//			moveSun( newDate, mCurrentGeoPos );
//		}
//	}

//	SMM.needsRedraw( mbDirtyTime );
	mbDirtyTime = false;
}

void SunBuilder::setCurrentLocation( std::string locationHint ) {
	mCurrentGeoPos = locationProvider.getPosition( locationHint );
}

V3f SunBuilder::GoldenHourColor() const {
	ASSERT( GoldenHour() >= 0.0f && GoldenHour() <= 1.0f );
	const float sunK = 10.0f;
	float sunPower = mSunPosition.altitudeRad > 0.0f ? ((mSunPosition.altitudeRad+0.1f) / M_PI_2) * sunK : 1.0f;
    auto gdl = static_cast<size_t>(goldenHourGradient->width - 1);
	auto index = gdl - static_cast<size_t>(GoldenHour() * gdl);
	auto col = goldenHourGradient->at<uint32_t>( static_cast<unsigned int>(index), 0);
	auto ret = Vector4f::ITORGBA(col) * GoldenHour() * sunPower;
	return ret.xyz();
}
