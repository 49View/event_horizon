//
//  text_input.hpp
//
//

#pragma once

#include <string>
#include <unordered_map>
#include <array>
#include <functional>
#include <stack>
#include "core/game_time.h"

struct GLFWwindow;

typedef int ModifiersKey;

#define GMK_MOD_NONE		0x0000
#define GMK_MOD_SHIFT		0x0001
#define GMK_MOD_CONTROL		0x0002
#define GMK_MOD_ALT			0x0004
#define GMK_MOD_SUPER		0x0008
#define GMK_MOD_BACKSPACE	0x0010

/* The unknown key */
#define GMK_UNKNOWN            -1

/* Printable keys */
#define GMK_SPACE              32
#define GMK_APOSTROPHE         39  /* ' */
#define GMK_COMMA              44  /* , */
#define GMK_MINUS              45  /* - */
#define GMK_PERIOD             46  /* . */
#define GMK_SLASH              47  /* / */
#define GMK_0                  48
#define GMK_1                  49
#define GMK_2                  50
#define GMK_3                  51
#define GMK_4                  52
#define GMK_5                  53
#define GMK_6                  54
#define GMK_7                  55
#define GMK_8                  56
#define GMK_9                  57
#define GMK_SEMICOLON          59  /* ; */
#define GMK_EQUAL              61  /* = */
#define GMK_A                  65
#define GMK_B                  66
#define GMK_C                  67
#define GMK_D                  68
#define GMK_E                  69
#define GMK_F                  70
#define GMK_G                  71
#define GMK_H                  72
#define GMK_I                  73
#define GMK_J                  74
#define GMK_K                  75
#define GMK_L                  76
#define GMK_M                  77
#define GMK_N                  78
#define GMK_O                  79
#define GMK_P                  80
#define GMK_Q                  81
#define GMK_R                  82
#define GMK_S                  83
#define GMK_T                  84
#define GMK_U                  85
#define GMK_V                  86
#define GMK_W                  87
#define GMK_X                  88
#define GMK_Y                  89
#define GMK_Z                  90
#define GMK_LEFT_BRACKET       91  /* [ */
#define GMK_BACKSLASH          92  /* \ */
#define GMK_RIGHT_BRACKET      93  /* ] */
#define GMK_GRAVE_ACCENT       96  /* ` */
#define GMK_WORLD_1            161 /* non-US #1 */
#define GMK_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define GMK_ESCAPE             256
#define GMK_ENTER              257
#define GMK_TAB                258
#define GMK_BACKSPACE          259
#define GMK_INSERT             260
#define GMK_DELETE             261
#define GMK_RIGHT              262
#define GMK_LEFT               263
#define GMK_DOWN               264
#define GMK_UP                 265
#define GMK_PAGE_UP            266
#define GMK_PAGE_DOWN          267
#define GMK_HOME               268
#define GMK_END                269
#define GMK_CAPS_LOCK          280
#define GMK_SCROLL_LOCK        281
#define GMK_NUM_LOCK           282
#define GMK_PRINT_SCREEN       283
#define GMK_PAUSE              284
#define GMK_F1                 290
#define GMK_F2                 291
#define GMK_F3                 292
#define GMK_F4                 293
#define GMK_F5                 294
#define GMK_F6                 295
#define GMK_F7                 296
#define GMK_F8                 297
#define GMK_F9                 298
#define GMK_F10                299
#define GMK_F11                300
#define GMK_F12                301
#define GMK_F13                302
#define GMK_F14                303
#define GMK_F15                304
#define GMK_F16                305
#define GMK_F17                306
#define GMK_F18                307
#define GMK_F19                308
#define GMK_F20                309
#define GMK_F21                310
#define GMK_F22                311
#define GMK_F23                312
#define GMK_F24                313
#define GMK_F25                314
#define GMK_KP_0               320
#define GMK_KP_1               321
#define GMK_KP_2               322
#define GMK_KP_3               323
#define GMK_KP_4               324
#define GMK_KP_5               325
#define GMK_KP_6               326
#define GMK_KP_7               327
#define GMK_KP_8               328
#define GMK_KP_9               329
#define GMK_KP_DECIMAL         330
#define GMK_KP_DIVIDE          331
#define GMK_KP_MULTIPLY        332
#define GMK_KP_SUBTRACT        333
#define GMK_KP_ADD             334
#define GMK_KP_ENTER           335
#define GMK_KP_EQUAL           336
#define GMK_LEFT_SHIFT         340
#define GMK_LEFT_CONTROL       341
#define GMK_LEFT_ALT           342
#define GMK_LEFT_SUPER         343
#define GMK_RIGHT_SHIFT        344
#define GMK_RIGHT_CONTROL      345
#define GMK_RIGHT_ALT          346
#define GMK_RIGHT_SUPER        347
#define GMK_MENU               348

#define GMK_LAST               GMK_MENU

struct TICallbackFunctions {
	std::function<bool()> mEnterCallbackFunction;
	std::function<void()> mEscapeCallbackFunction;
};

class TextInput {
public:
	TextInput();
	TextInput( TextInput const& ) = delete;
	void operator=( TextInput const& ) = delete;

public: // these are globals data accessed from low level functions on inputs etc
	void resetInput();
	bool checkKeyToggleOn( int keyCode, bool overrideTextInput = false );
	bool checkKeyPressed( int keyCode );
	int  checkWASDPressed();
	void update_render_thread();
	void update_update_thread();

	bool HasToggledEnter() const { return mHasToggledEnter; }
	void HasToggledEnter( bool val ) { mHasToggledEnter = val; }
	bool HasToggledEscape() const { return mHasToggledEscape; }
	void HasToggledEscape( bool val ) { mHasToggledEscape = val; }

	bool ModKeyCurrent( ModifiersKey val ) const;
	void forceModKeyCurrent( ModifiersKey val ) { mModKeyCurrent = val; }
	bool IsAnyModKeyPressed() const { return anyModKeyPressed; };

	void startListeningForTextInput( const std::string& title, std::function<bool()> enterCallbackFunction, std::function<void()> escapeCallbackFunction );

	std::string TextInputBoxTitle() const { return mTextInputBoxTitle; }
	void TextInputBoxTitle( std::string val ) { mTextInputBoxTitle = val; }

	void push_back_callbacks( std::function<bool()> enterCallbackFunction, std::function<void()> escapeCallbackFunction );
	void pop_callbacks();

	bool readNumber( float& _number); 
	void update_pop_back() { mbPopBack = true; }

	bool isEnabled() const {
		return mbEnabled;
	}

	void setEnabled( bool mbEnabled ) {
		TextInput::mbEnabled = mbEnabled;
	}

public:
	static std::wstring GInputString;
	static ModifiersKey mModKeyCurrent;
	bool GIsEnteringText;
private:
	bool  anyModKeyPressed = false;

	std::array<int, GMK_LAST> mPrevKeyStates;
	std::unordered_map<int, int> mStateUpdates;
	bool mHasToggledEnter;
	bool mHasToggledEscape;
	bool mbPopBack;
	
	std::stack<TICallbackFunctions> mCallbacksQueue;

	std::string mTextInputBoxTitle;
	bool mbEnabled = true;
};

ModifiersKey convertFromGLFWModifiers( int mod );

void GCharacterCallback( GLFWwindow* window, unsigned int codepoint );
