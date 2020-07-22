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
#include <core/game_time.h>

#define MB_RELEASE 0
#define MB_PRESS   1

#ifdef USE_GLFVR
#include <graphics/opengl/GLFVR/text_input_glfvr.hpp>
#elif USE_GLFW
#include <graphics/opengl/GLFW/text_input_glfw.hpp>
#elif USE_GLFM
#include <graphics/opengl/GLFW/text_input_glfw.hpp>
#endif

typedef int ModifiersKey;

struct TICallbackFunctions {
	std::function<bool()> mEnterCallbackFunction;
	std::function<void()> mEscapeCallbackFunction;
};

struct InputMods {
    bool isAltPressed = false;
    bool isShiftPressed = false;
    bool isControlKeyDown = false;
};

class TextInput {
public:
	TextInput();
	TextInput( TextInput const& ) = delete;
	void operator=( TextInput const& ) = delete;

public: // these are globals data accessed from low level functions on inputs etc
	void resetInput();
	bool checkKeyToggleOn( int keyCode, bool overrideTextInput = false );
	bool checkKeyPressed( int keyCode ) const;
    static void checkKeyPressedStateless( int keyCode, int& currKeyPressed );
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
    static bool checkModKeyPressed( int keyCode ) ;

	void startListeningForTextInput( const std::string& title, std::function<bool()> enterCallbackFunction, std::function<void()> escapeCallbackFunction );

	std::string TextInputBoxTitle() const { return mTextInputBoxTitle; }
	void TextInputBoxTitle( std::string val ) { mTextInputBoxTitle = std::move(val); }

	void push_back_callbacks( std::function<bool()> enterCallbackFunction, std::function<void()> escapeCallbackFunction );
	void pop_callbacks();

	bool readNumber( float& _number); 
	void update_pop_back() { mbPopBack = true; }

	bool isEnabled() const {
		return mbEnabled;
	}

	void setEnabled( bool _mbEnabled ) {
		mbEnabled = _mbEnabled;
	}

    InputMods mods() const;

public:
	static std::wstring GInputString;
	static ModifiersKey mModKeyCurrent;
	bool GIsEnteringText;
private:
	bool  anyModKeyPressed = false;

	std::array<int, GMK_LAST> mPrevKeyStates{};
	std::unordered_map<int, int> mStateUpdates;
	bool mHasToggledEnter = false;
	bool mHasToggledEscape = false;;
	bool mbPopBack = false;;
	
	std::stack<TICallbackFunctions> mCallbacksQueue;

	std::string mTextInputBoxTitle;
	bool mbEnabled = true;
};

//ModifiersKey convertFromGLFWModifiers( int mod );
//void GCharacterCallback( GLFWwindow* window, unsigned int codepoint );
