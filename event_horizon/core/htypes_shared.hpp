//
//  htypes.hpp
//  sixthview
//
//  Created by Dado on 01/10/2015.
//
//

#pragma once

#include <cstdint>
#include <type_traits>
#include <string>
#include <memory>
#include <functional>
#include <vector>

using CommandArgumentsT 		= std::vector< std::string >;
using CommandCallbackFunction 	= std::function<void(const CommandArgumentsT& )>;
using SerializableContainer     = std::vector<unsigned char>;

typedef std::pair<void*, uint64_t> void_p;
typedef std::pair<char*, uint64_t> char_p;
typedef std::pair<unsigned char*, uint64_t> uchar_p;
typedef std::pair<const unsigned char*, uint64_t> ucchar_p;
typedef std::pair<std::unique_ptr<uint8_t[]>, uint64_t> uint8_p;

enum class TextFormatting {
	Left,
	Center,
	Right
};

enum class SDVTouch {
	NONE = 0,
	TITLE,
	NAME,
	DATA
};

struct WindingOrder {
	static const uint64_t CW = 0;
	static const uint64_t CCW = 1;
};

enum class ReverseFlag {
	False = 0,
	True = 1
};

using WindingOrderT = uint64_t;

enum class PivotPointPosition {
	PPP_CENTER,

	PPP_BOTTOM_CENTER,
	PPP_TOP_CENTER,
	PPP_LEFT_CENTER,
	PPP_RIGHT_CENTER,

	PPP_BOTTOM_RIGHT,
	PPP_BOTTOM_LEFT,
	PPP_TOP_LEFT,
	PPP_TOP_RIGHT,

	PPP_CUSTOM
};

enum class PillowEdges {
	None,
	All,
	SidesHorizontal,
	SidesVertical,
	Top,
	TopAndSides,
	Bottom,
	BottomAndSides,
	Left,
	Right
};

enum class WrapRotations {
	WR_ON,
	WR_OFF
};

enum class MappingDirection {
	X_POS = 0,
	Y_POS = 1
};

enum class MappingMirrorE : int {
	None = 0,
	X_Only = 1,
	Y_Only = 2,
	BothWays = 3
};

enum UiControlFlag {
	NoFlags = 0,
	UseScreenSpace = 1 << 0,
	HasRoundedCorners = 1 << 1,
	TextAlignCenter = 1 << 2,
	TextAlignLeft = 1 << 3,
	TextAlignRight = 1 << 4,
	HasShadow = 1 << 5,
	FillScale = 1 << 6,
	FloatLeft = 1 << 7,
	FloatRight = 1 << 8,
	FloatCenter = 1 << 9,
	Use3d = 1 << 10,
	IsArray = 1 << 11,
	Invalid = 1 << 31
};

typedef float subdivisionAccuray;

const static float accuracy1Meter = 1.0f;
const static float accuracy50cm = .5f;
const static float accuracy25cm = .25f;
const static float accuracy10cm = .1f;
const static float accuracy5cm = .05f;
const static float accuracy1cm = .01f;
const static float accuracy1mm = .001f;
const static float accuracy1Sqmm = 0.000001f;
const static float accuracyNone = 0.0f;

enum class FollowerSide {
	In = -1,
	Out = 1,
	Center = 0
};

enum class FollowerQuadAlignment {
	Center,
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
};

enum FollowerFlags {
	Defaults = 0,
	NoOverRide = 0,
	WrapPath = 1,
	NoCaps = 1 << 1,
	UsePlanarMapping = 1 << 2
	//	StraightAngles =			1 << 1,
};

using T = std::underlying_type_t <FollowerFlags>;

inline FollowerFlags operator | ( FollowerFlags lhs, FollowerFlags rhs ) {
	return (FollowerFlags)( static_cast<T>( lhs ) | static_cast<T>( rhs ) );
}

inline FollowerFlags& operator |= ( FollowerFlags& lhs, FollowerFlags rhs ) {
	lhs = (FollowerFlags)( static_cast<T>( lhs ) | static_cast<T>( rhs ) );
	return lhs;
}

enum class FollowerGapSide : int {
	NoGap,
	Start,
	//	Middle,
	End,
	NotVisible
};

struct FollowerGapData {
	FollowerGapData( FollowerGapSide s, float i = 0.0f ) : side( s ), inset( i ) {}
	FollowerGapSide	side;
	float			inset;
};

enum UiSwipeDirection {
	UI_SWIPE_DOWN,
	UI_SWIPE_UP,
	UI_SWIPE_LEFT,
	UI_SWIPE_RIGHT,
};

enum TouchResult {
	TOUCH_RESULT_HIT,
	TOUCH_RESULT_NEAR_MISS,
	TOUCH_RESULT_NOT_HIT,
};

enum class YGestureInvert {
	Yes,
	No
};

enum class SwipeDirection {
	INVALID = 0,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

enum class MouseButtonStatusValues {
	UP,
	DOWN,
	MOVING
};

enum class MouseCursorType {
	ARROW,
	IBEAM,
	CROSSHAIR,
	HAND,
	HRESIZE,
	VRESIZE
};

using NodeType = uint64_t;
static const uint64_t NodeTypeGeneric = 1;
static const uint64_t NodeTypeLocator = 1 << 23;
static const uint64_t NodeTypeAll = 0xffffffffffffffff;

namespace InitializeWindowFlags {
	const static uint64_t Normal = 0;
	const static uint64_t FullScreen = 1 << 1;
	const static uint64_t Minimize = 1 << 2;
	const static uint64_t Maximize = 1 << 3;
	const static uint64_t HalfSize = 1 << 4;
	const static uint64_t ThreeQuarter = 1 << 5;
}

using InitializeWindowFlagsT = uint64_t;

static const float SINGLE_TAP_TIME_LIMIT = 0.33f;
static const float DOUBLE_TAP_TIME_LIMIT = 0.5f;
static const float TAP_AREA = 0.05f;
static const float MAX_SWIPE_TIME_LIMIT = 0.3f;
static const float LONG_TAP_TIME_LIMIT = 0.5f;
static const std::string defaultFontName = "Amaranth-Bold";

using ScreenShotContainer = std::vector<unsigned char>;
using ScreenShotContainerPtr = std::shared_ptr<ScreenShotContainer>;

enum class BlitType {
	OnScreen,
	OffScreen
};

namespace Name {
	const static std::string Alpha	  = "Alpha	 ";
	const static std::string Bravo	  = "Bravo	 ";
	const static std::string Charlie  = "Charlie ";
	const static std::string Delta	  = "Delta	 ";
	const static std::string Echo	  = "Echo	 ";
	const static std::string Foxtrot  = "Foxtrot ";
	const static std::string Golf	  = "Golf	 ";
	const static std::string Hotel	  = "Hotel	 ";
	const static std::string India	  = "India	 ";
	const static std::string Juliett  = "Juliett ";
	const static std::string Kilo	  = "Kilo	 ";
	const static std::string Lima	  = "Lima	 ";
	const static std::string Mike	  = "Mike	 ";
	const static std::string November = "November";
	const static std::string Oscar	  = "Oscar	 ";
	const static std::string Papa	  = "Papa	 ";
	const static std::string Quebec   = "Quebec  ";
	const static std::string Romeo	  = "Romeo	 ";
	const static std::string Sierra   = "Sierra  ";
	const static std::string Tango	  = "Tango	 ";
	const static std::string Uniform  = "Uniform ";
	const static std::string Victor   = "Victor  ";
	const static std::string Whiske   = "Whiske  ";
	const static std::string X_Ray	  = "X_Ray	 ";
	const static std::string Yankee   = "Yankee  ";
	const static std::string Zulu	  = "Zulu	 ";
}

namespace EntityGroup {
	const static std::string Material  = "material";
	const static std::string Color     = "color";
	const static std::string Geom      = "geom";
	const static std::string UI        = "ui";
	const static std::string CameraRig = "camera_rig";
}

enum class HttpUrlEncode {
	Yes,
	No
};

namespace Http { struct Result; }
using ResponseCallbackFunc = std::function<void(const Http::Result&)>;

// Dependencies

using ddContainer = std::vector<std::string>;

enum class DependencyStatus {
	Loading = 0,
	LoadedSuccessfully,
	LoadedSuccessfully204,
	LoadingFailed,
	CallbackSuccessfullyExectuted,
	CallbackFailedOnExectution,
	Complete,
	CompleteWithErrors
};

namespace BuilderQueryType {
	class Exact {};
    class NotExact{};
};

struct TextureUniformDesc {
	std::string  name;
	unsigned int handle;
	unsigned int slot;
	int 		 target;
};
