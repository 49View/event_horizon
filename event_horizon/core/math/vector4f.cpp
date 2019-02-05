#include "vector4f.h"
#include "math_util.h"



Vector3f::Vector3f( const Vector4f& v4 ) {
	mX = v4.x();
	mY = v4.y();
	mZ = v4.z();
}

std::string Vector4f::toString() const {
	return "X = " + floatToFixedDigits( mX ) + " Y = " + floatToFixedDigits( mY ) + " Z = " + floatToFixedDigits( mZ ) + " W = " + floatToFixedDigits( mW );
}

std::string Vector4f::toStringCommaSeparated() const {
	return floatToFixedDigits( mX ) + "," + floatToFixedDigits( mY ) + "," + floatToFixedDigits( mZ ) + "," +
		   floatToFixedDigits( mW );
}

std::string Vector4f::toStringJSONArray() const {
	return "[" + floatToFixedDigits( mX ) + "," + floatToFixedDigits( mY ) + "," + floatToFixedDigits( mZ ) + "," +
		   floatToFixedDigits( mW ) + "]";
}

Vector4f Vector4f:: ITORGBA( uint32_t number, int32_t numbits ) {
	int32_t blue = ( number & 0x00ff0000 ) >> 16;
	int32_t green = ( number & 0x0000ff00 ) >> 8;
	int32_t red = number & 0x000000ff;
	int32_t alpha = 0xff;
	if ( numbits == 32 ) alpha = ( number & 0xff000000 ) >> 24;

	return Vector4f( static_cast<float>( red ) / 255.0f, static_cast<float>( green ) / 255.0f, static_cast<float>( blue ) / 255.0f, static_cast<float>( alpha ) / 255.0f );
}

Vector4f Vector4f::XTORGBA( const std::string& _hexstring ) {
	if ( _hexstring.empty() ) return Vector4f::BLACK;
	std::string sanitized = string_trim_after(_hexstring, "#");
	if ( sanitized.size() != 6 && sanitized.size() != 8 ) return Vector4f::BLACK;
	std::string red    = "0x" + sanitized.substr(0, 2);
	std::string green  = "0x" + sanitized.substr(2, 2);
	std::string blue   = "0x" + sanitized.substr(4, 2);
	std::string alpha  = "0x" + (sanitized.size() == 8 ? sanitized.substr(6, 2) : "ff");

	int redi   = (int)strtol(red.c_str(), nullptr, 0);
	int greeni = (int)strtol(green.c_str(), nullptr, 0);
	int bluei  = (int)strtol(blue.c_str(), nullptr, 0);
	int alphai = (int)strtol(alpha.c_str(), nullptr, 0);

	return Vector4f::ITORGBA( redi, greeni, bluei, alphai );
}

Vector3f Vector4f::ITORGB( const unsigned int r, const unsigned int g, const unsigned int b ) {
	return Vector3f( (r * 1.0f) / 255.0f, (g * 1.0f) / 255.0f, (b * 1.0f) / 255.0f );
}

Vector4f Vector4f::ITORGBA( const unsigned int r, const unsigned int g, const unsigned int b, const unsigned int a ) {
	return Vector4f( (r * 1.0f) / 255.0f, (g * 1.0f) / 255.0f, (b * 1.0f) / 255.0f, (a * 1.0f) / 255.0f );
}

Vector3f Vector4f::FTORGB( const float r, const float g, const float b ) {
	return Vector3f( r / 255.0f, g / 255.0f, b / 255.0f );
}

Vector4f Vector4f::FTORGBA( const float r, const float g, const float b, const float a ) {
	return  Vector4f( r / 255.0f, g / 255.0f, b / 255.0f, a );
}

Vector4f Vector4f::FTORGBA( const Vector4f& _val ) {
	return Vector4f( Vector3f{ _val.xyz() / 255.0f }, _val.w() );
}

Vector4f Vector4f::COLORA( const Vector4f& source, const float a ) {
	return Vector4f( source.xyz(), a );
}

const Vector4f Vector4f::ZERO = Vector4f( 0.0f, 0.0f, 0.0f, 0.0f );
const Vector4f Vector4f::X_AXIS = Vector4f( 1.0f, 0.0f, 0.0f, 0.0f );
const Vector4f Vector4f::Y_AXIS = Vector4f( 0.0f, 1.0f, 0.0f, 0.0f );
const Vector4f Vector4f::Z_AXIS = Vector4f( 0.0f, 0.0f, 1.0f, 0.0f );
const Vector4f Vector4f::W_AXIS = Vector4f( 0.0f, 0.0f, 0.0f, 1.0f );
const Vector4f Vector4f::ONE = Vector4f( 1.0f, 1.0f, 1.0f, 1.0f );
const Vector4f Vector4f::HUGE_VALUE_POS = Vector4f( std::numeric_limits<float>::max() );
const Vector4f Vector4f::HUGE_VALUE_NEG = Vector4f( std::numeric_limits<float>::lowest() );

const Vector4f Vector4f::WHITE = Vector4f( 1.0f, 1.0f, 1.0f, 1.0f );
const Vector4f Vector4f::BLACK = Vector4f( 0.0f, 0.0f, 0.0f, 1.0f );
const Vector4f Vector4f::YELLOW = Vector4f( 1.0f, 1.0f, 0.0f, 1.0f );
const Vector4f Vector4f::RED = Vector4f( 1.0f, 0.0f, 0.0f, 1.0f );
const Vector4f Vector4f::GREEN = Vector4f( 0.0f, 1.0f, 0.0f, 1.0f );
const Vector4f Vector4f::BLUE = Vector4f( 0.0f, 0.0f, 1.0f, 1.0f );
const Vector4f Vector4f::BROWN = Vector4f( 0.6f, 0.2f, 0.2f, 1.0f );
const Vector4f Vector4f::CYAN = Vector4f( 0.0f, 1.0f, 1.0f, 1.0f );
const Vector4f Vector4f::PURPLE = Vector4f( 1.0f, 0.0f, 1.0f, 1.0f );

const Vector4f Vector4f::PASTEL_GRAY = Vector4f( 64.0f / 255.0f, 64.0f / 255.0f, 64.0f / 255.0f, 1.0f );
const Vector4f Vector4f::PASTEL_GRAYLIGHT = Vector4f( 128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f );
const Vector4f Vector4f::PASTEL_ORANGE = Vector4f( 241.0f / 255.0f, 103.0f / 255.0f, 69.0f / 255.0f, 1.0f );
const Vector4f Vector4f::PASTEL_RED = Vector4f( 255.0f / 255.0f, 100.0f / 255.0f, 97.0f / 255.0f, 1.0f );
const Vector4f Vector4f::PASTEL_CYAN = Vector4f( 76.0f / 255.0f, 195.0f / 255.0f, 217.0f / 255.0f, 1.0f );
const Vector4f Vector4f::PASTEL_GREEN = Vector4f( 123.0f / 255.0f, 200.0f / 255.0f, 164.0f / 255.0f, 1.0f );
const Vector4f Vector4f::PASTEL_YELLOW = Vector4f( 255.0f / 255.0f, 198.0f / 255.0f, 93.0f / 255.0f, 1.0f );
const Vector4f Vector4f::PASTEL_BROWN = Vector4f( 131.0f / 255.0f, 105.0f / 255.0f, 83.0f / 255.0f, 1.0f );

const Vector4f Vector4f::SAND = Vector4f( 70.0f / 255.0f, 70.0f / 255.0f, 50.0f / 255.0f, 1.0f );

const Vector4f Vector4f::ORANGE_SCHEME1_1 = Vector4f( 255.0f / 255.0f, 116.0f / 255.0f, 0.0f / 255.0f, 1.0f );
const Vector4f Vector4f::ORANGE_SCHEME1_2 = Vector4f( 166.0f / 255.0f, 75.0f / 255.0f, 0.0f / 255.0f, 1.0f );
const Vector4f Vector4f::ORANGE_SCHEME1_3 = Vector4f( 255.0f / 255.0f, 178.0f / 115.0f, 0.0f / 255.0f, 1.0f );

const Vector4f Vector4f::ACQUA_T = Vector4f( .15f, 0.7f, .95f, 0.5f );

const Vector4f Vector4f::BLUE_SHADOW = Vector4f( 0.15294f, 0.0f, 0.53510f, 0.6f );

const Vector4f Vector4f::BRIGHT_PINK = Vector4f( 255.0f / 255.0f, 16.0f / 255.0f, 201.0f / 255.0f, 1.0f );
const Vector4f Vector4f::DARK_GRAY = Vector4f( 32.0f / 255.0f, 32.0f / 255.0f, 32.0f / 255.0f, 1.0f );
const Vector4f Vector4f::DARK_BLUE = Vector4f( 68.0f / 255.0f, 24.0f / 255.0f, 255.0f / 255.0f, 1.0f );
const Vector4f Vector4f::DARK_YELLOW = Vector4f( 232.0f / 255.0f, 184.0f / 255.0f, 49.0f / 255.0f, 1.0f );
const Vector4f Vector4f::DARK_RED = Vector4f( 173.0f / 255.0f, 22.0f / 255.0f, 41.0f / 255.0f, 1.0f );
const Vector4f Vector4f::DARK_GREEN = Vector4f( 44.0f / 255.0f, 82.0f / 255.0f, 5.0f / 255.0f, 1.0f );
const Vector4f Vector4f::DARK_BROWN = Vector4f( 59.0f / 255.0f, 28.0f / 255.0f, 12.0f / 255.0f, 1.0f );
const Vector4f Vector4f::DARK_PURPLE = Vector4f( 65.0f / 255.0f, 16.0f / 255.0f, 178.0f / 255.0f, 1.0f );
const Vector4f Vector4f::INDIAN_RED = Vector4f( 205.0f / 255.0f, 92.0f / 255.0f, 92.0f / 255.0f, 1.0f );
const Vector4f Vector4f::LIGHT_CORAL = Vector4f( 240.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f, 1.0f );
const Vector4f Vector4f::SALMON = Vector4f( 250.0f / 255.0f, 128.0f / 255.0f, 114.0f / 255.0f, 1.0f );
const Vector4f Vector4f::DARK_SALMON = Vector4f( 233.0f / 255.0f, 150.0f / 255.0f, 122.0f / 255.0f, 1.0f );
const Vector4f Vector4f::LIGHT_SALMON = Vector4f( 255.0f / 255.0f, 160.0f / 255.0f, 122.0f / 255.0f, 1.0f );
const Vector4f Vector4f::CRIMSON = Vector4f( 220.0f / 255.0f, 20.0f / 255.0f, 60.0f / 255.0f, 1.0f );
const Vector4f Vector4f::FIRE_BRICK = Vector4f( 178.0f / 255.0f, 30.0f / 255.0f, 30.0f / 255.0f, 1.0f );
const Vector4f Vector4f::PINK = Vector4f( 255.0f / 255.0f, 192.0f / 255.0f, 203.0f / 255.0f, 1.0f );
const Vector4f Vector4f::HOT_PINK = Vector4f( 255.0f / 255.0f, 105.0f / 255.0f, 180.0f / 255.0f, 1.0f );
const Vector4f Vector4f::DEEP_PINK = Vector4f( 255.0f / 255.0f, 20.0f / 255.0f, 150.0f / 255.0f, 1.0f );
const Vector4f Vector4f::GOLD = Vector4f( 255.0f / 255.0f, 215.0f / 255.0f, 0.0f / 255.0f, 1.0f );
const Vector4f Vector4f::PEACH = Vector4f( 255.0f / 255.0f, 218.0f / 255.0f, 185.0f / 255.0f, 1.0f );
const Vector4f Vector4f::LAVANDER = Vector4f( 230.0f / 255.0f, 230.0f / 255.0f, 250.0f / 255.0f, 1.0f );
const Vector4f Vector4f::VIOLET = Vector4f( 238.0f / 255.0f, 130.0f / 255.0f, 238.0f / 255.0f, 1.0f );
const Vector4f Vector4f::FUCHSIA = Vector4f( 255.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f, 1.0f );
const Vector4f Vector4f::DARK_VIOLET = Vector4f( 148.0f / 255.0f, 0.0f / 255.0f, 211.0f / 255.0f, 1.0f );
const Vector4f Vector4f::INDINGO = Vector4f( 75.0f / 255.0f, 0.0f / 255.0f, 130.0f / 255.0f, 1.0f );
const Vector4f Vector4f::SPRING_GREEN = Vector4f( 0.0f / 255.0f, 255.0f / 255.0f, 127.0f / 255.0f, 1.0f );
const Vector4f Vector4f::FOREST_GREEN = Vector4f( 34.0f / 255.0f, 139.0f / 255.0f, 34.0f / 255.0f, 1.0f );
const Vector4f Vector4f::OLIVE = Vector4f( 128.0f / 255.0f, 128.0f / 255.0f, 0.0f / 255.0f, 1.0f );
const Vector4f Vector4f::DARK_CYAN = Vector4f( 0.0f / 255.0f, 139.0f / 255.0f, 139.0f / 255.0f, 1.0f );
const Vector4f Vector4f::AQUAMARINE = Vector4f( 129.0f / 255.0f, 255.0f / 255.0f, 212.0f / 255.0f, 1.0f );
const Vector4f Vector4f::STEEL_BLUE = Vector4f( 70.0f / 255.0f, 130.0f / 255.0f, 180.0f / 255.0f, 1.0f );
const Vector4f Vector4f::SKY_BLUE = Vector4f( 135.0f / 255.0f, 206.0f / 255.0f, 235.0f / 255.0f, 1.0f );
const Vector4f Vector4f::DODGER_BLUE = Vector4f( 30.0f / 255.0f, 144.0f / 255.0f, 255.0f / 255.0f, 1.0f );
const Vector4f Vector4f::ROYAL_BLUE = Vector4f( 65.0f / 255.0f, 105.0f / 255.0f, 255.0f / 255.0f, 1.0f );
const Vector4f Vector4f::NAVY = Vector4f( 0.0f / 255.0f, 0.0f / 255.0f, 128.0f / 255.0f, 1.0f );
const Vector4f Vector4f::ALMOND = Vector4f( 255.0f / 255.0f, 235.0f / 255.0f, 205.0f / 255.0f, 1.0f );
const Vector4f Vector4f::TAN = Vector4f( 210.0f / 255.0f, 180.0f / 255.0f, 140.0f / 255.0f, 1.0f );
const Vector4f Vector4f::SANDY_BROWN = Vector4f( 244.0f / 255.0f, 164.0f / 255.0f, 96.0f / 255.0f, 1.0f );
const Vector4f Vector4f::SIENNA = Vector4f( 160.0f / 255.0f, 82.0f / 255.0f, 45.0f / 255.0f, 1.0f );
const Vector4f Vector4f::MAROON = Vector4f( 128.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f );
const Vector4f Vector4f::SNOW = Vector4f( 255.0f / 255.0f, 250.0f / 255.0f, 250.0f / 255.0f, 1.0f );
const Vector4f Vector4f::HONEYDEW = Vector4f( 185.0f / 255.0f, 70.0f / 255.0f, 36.0f / 255.0f, 1.0f );
const Vector4f Vector4f::LIGHT_GREY = Vector4f( 211.0f / 255.0f, 211.0f / 255.0f, 211.0f / 255.0f, 1.0f );



