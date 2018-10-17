//
//  text_input.cpp
//

#include "text_input.hpp"

#include "window_handling.hpp"
#include "mouse_input.hpp"


std::wstring TextInput::GInputString;
ModifiersKey TextInput::mModKeyCurrent;

ModifiersKey convertFromGLFWModifiers( int mod ) {
	switch ( mod ) {
		case GLFW_MOD_ALT: return GMK_MOD_ALT;
		case GLFW_MOD_CONTROL: return GMK_MOD_CONTROL;
		case GLFW_MOD_SHIFT: return GMK_MOD_SHIFT;
		case GLFW_MOD_SUPER: return GMK_MOD_SUPER;
		case GLFW_KEY_BACKSPACE: return GMK_MOD_BACKSPACE;
		default: return GMK_MOD_NONE;
	}
}

TextInput::TextInput() {
	GInputString = L"";
	GIsEnteringText = false;
	mModKeyCurrent = GMK_MOD_NONE;
}

bool TextInput::checkKeyToggleOn( int keyCode, bool overrideTextInput ) {
	if ( !isEnabled() ) return false;
	if ( ( GIsEnteringText && ! IsAnyModKeyPressed() ) && !overrideTextInput ) return false;
	int currWState = glfwGetKey( WH::window, keyCode );
	int prevState = mPrevKeyStates[keyCode];
	mPrevKeyStates[keyCode] = currWState;
	mStateUpdates[keyCode] = currWState;
	return ( currWState == GLFW_PRESS && prevState == GLFW_RELEASE );
}

bool TextInput::checkKeyPressed( int keyCode ) {
	if ( !isEnabled() ) return false;
	if ( GIsEnteringText || IsAnyModKeyPressed() ) return false;
	int currWState = glfwGetKey( WH::window, keyCode );
	return currWState == GLFW_PRESS;
}

int TextInput::checkWASDPressed() {
	if ( !isEnabled() ) return false;
	if ( GIsEnteringText || IsAnyModKeyPressed() ) return -1;
	int currKeyPressed = -1;
	if ( glfwGetKey( WH::window, GLFW_KEY_W ) == GLFW_PRESS ) currKeyPressed = GLFW_KEY_W;
	if ( glfwGetKey( WH::window, GLFW_KEY_A ) == GLFW_PRESS ) currKeyPressed = GLFW_KEY_A;
	if ( glfwGetKey( WH::window, GLFW_KEY_S ) == GLFW_PRESS ) currKeyPressed = GLFW_KEY_S;
	if ( glfwGetKey( WH::window, GLFW_KEY_D ) == GLFW_PRESS ) currKeyPressed = GLFW_KEY_D;
	if ( glfwGetKey( WH::window, GLFW_KEY_R ) == GLFW_PRESS ) currKeyPressed = GLFW_KEY_R;
	if ( glfwGetKey( WH::window, GLFW_KEY_F ) == GLFW_PRESS ) currKeyPressed = GLFW_KEY_F;
	return currKeyPressed;
}

void TextInput::update_render_thread( [[maybe_unused]] const GameTime& gt ) {
	if ( GIsEnteringText ) {
		if ( checkKeyToggleOn( GLFW_KEY_BACKSPACE, true ) ) {
			update_pop_back();
		}
	}
	if ( !HasToggledEnter() ) {
		HasToggledEnter( checkKeyToggleOn( GLFW_KEY_ENTER, true ) );
	}
	if ( !HasToggledEscape() ) {
		HasToggledEscape( checkKeyToggleOn( GLFW_KEY_ESCAPE, true ) );
	}

	anyModKeyPressed = 0;
	anyModKeyPressed |= ModKeyCurrent( GMK_MOD_SHIFT );
    anyModKeyPressed |= ModKeyCurrent( GMK_MOD_CONTROL );
    anyModKeyPressed |= ModKeyCurrent( GMK_MOD_ALT );
    anyModKeyPressed |= ModKeyCurrent( GMK_MOD_SUPER );
    anyModKeyPressed |= ModKeyCurrent( GMK_MOD_BACKSPACE );

    for ( const auto& [k,v] : mStateUpdates ) {
    	mPrevKeyStates[k] = v;
    }

}

void TextInput::update_update_thread() {
	if ( mbPopBack ) {
		if ( GInputString.size() > 0 ) {
			GInputString.pop_back();
		}
		mbPopBack = false;
	}

	if ( mCallbacksQueue.size() > 0 ) {
		if ( HasToggledEnter() ) {
			if ( mCallbacksQueue.top().mEnterCallbackFunction ) {
				if ( mCallbacksQueue.top().mEnterCallbackFunction() ) {
					resetInput();
					pop_callbacks();
				}
			}
		} else if ( HasToggledEscape() ) {
			if ( mCallbacksQueue.top().mEscapeCallbackFunction ) {
				mCallbacksQueue.top().mEscapeCallbackFunction();
			}
			resetInput();
			pop_callbacks();
		}
	}
}

bool TextInput::readNumber( float& _number ) {
	if ( GInputString.size() == 0 ) return false;
	std::string ns( GInputString.begin(), GInputString.end() );
	try {
		_number = std::stof( ns );
		return true;
	} catch ( ... ) {
		return false;
	}

	return true;
}

void TextInput::push_back_callbacks( std::function<bool()> enterCallbackFunction, std::function<void()> escapeCallbackFunction ) {
	mCallbacksQueue.push( { enterCallbackFunction, escapeCallbackFunction } );
}

void TextInput::pop_callbacks() {
	if ( mCallbacksQueue.size() > 0 ) {
		mCallbacksQueue.pop();
	}
}

void TextInput::startListeningForTextInput( const std::string& title, std::function<bool()> enterCallbackFunction, std::function<void()> escapeCallbackFunction ) {
	mTextInputBoxTitle = title;
	GIsEnteringText = true;
	GInputString = L"";
	push_back_callbacks( enterCallbackFunction, escapeCallbackFunction );
}

void TextInput::resetInput() {
	if ( HasToggledEnter() || HasToggledEscape() ) {
		GInputString = L"";
		GIsEnteringText = false;
		HasToggledEnter( false );
		HasToggledEscape( false );
	}
}

bool TextInput::ModKeyCurrent( ModifiersKey val ) const {
	return checkBitWiseFlag( mModKeyCurrent, val );
}
