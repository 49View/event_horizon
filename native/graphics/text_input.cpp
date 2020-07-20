//
//  text_input.cpp
//

#include "text_input.hpp"
#include <core/util.h>

std::wstring TextInput::GInputString;
ModifiersKey TextInput::mModKeyCurrent;

//ModifiersKey convertFromGLFWModifiers( int mod ) {
//	switch ( mod ) {
//		case GLFW_MOD_ALT: return GMK_MOD_ALT;
//		case GLFW_MOD_CONTROL: return GMK_MOD_CONTROL;
//		case GLFW_MOD_SHIFT: return GMK_MOD_SHIFT;
//		case GLFW_MOD_SUPER: return GMK_MOD_SUPER;
//		case GLFW_KEY_BACKSPACE: return GMK_MOD_BACKSPACE;
//		default: return GMK_MOD_NONE;
//	}
//}

TextInput::TextInput() {
	GInputString = L"";
	GIsEnteringText = false;
	mModKeyCurrent = GMK_MOD_NONE;
}

int TextInput::checkWASDPressed() {
	if ( !isEnabled() ) return false;
	if ( GIsEnteringText || IsAnyModKeyPressed() ) return -1;
	int currKeyPressed = -1;
	checkKeyPressedStateless( GMK_W, currKeyPressed );
	checkKeyPressedStateless( GMK_A, currKeyPressed );
	checkKeyPressedStateless( GMK_S, currKeyPressed );
	checkKeyPressedStateless( GMK_D, currKeyPressed );
	checkKeyPressedStateless( GMK_R, currKeyPressed );
	checkKeyPressedStateless( GMK_F, currKeyPressed );
    checkKeyPressedStateless( GMK_UP, currKeyPressed );
    checkKeyPressedStateless( GMK_DOWN, currKeyPressed );
    checkKeyPressedStateless( GMK_LEFT, currKeyPressed );
    checkKeyPressedStateless( GMK_RIGHT, currKeyPressed );
    checkKeyPressedStateless( GMK_PAGE_UP, currKeyPressed );
    checkKeyPressedStateless( GMK_PAGE_DOWN, currKeyPressed );
	return currKeyPressed;
}

void TextInput::update_render_thread() {
	if ( GIsEnteringText ) {
		if ( checkKeyToggleOn( GMK_BACKSPACE, true ) ) {
			update_pop_back();
		}
	}
	if ( !HasToggledEnter() ) {
		HasToggledEnter( checkKeyToggleOn( GMK_ENTER, true ) );
	}
	if ( !HasToggledEscape() ) {
		HasToggledEscape( checkKeyToggleOn( GMK_ESCAPE, true ) );
	}

	anyModKeyPressed = false;
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
		if ( !GInputString.empty() ) {
			GInputString.pop_back();
		}
		mbPopBack = false;
	}

	if ( !mCallbacksQueue.empty() ) {
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
	if ( GInputString.empty() ) return false;
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
	mCallbacksQueue.push( { std::move(enterCallbackFunction), std::move(escapeCallbackFunction) } );
}

void TextInput::pop_callbacks() {
	if ( !mCallbacksQueue.empty() ) {
		mCallbacksQueue.pop();
	}
}

void TextInput::startListeningForTextInput( const std::string& title, std::function<bool()> enterCallbackFunction, std::function<void()> escapeCallbackFunction ) {
	mTextInputBoxTitle = title;
	GIsEnteringText = true;
	GInputString = L"";
	push_back_callbacks( std::move(enterCallbackFunction), std::move(escapeCallbackFunction) );
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

InputMods TextInput::mods() const {
    return InputMods{checkModKeyPressed(GMK_LEFT_ALT) || checkModKeyPressed(GMK_RIGHT_ALT),
                     checkModKeyPressed(GMK_LEFT_SHIFT) || checkModKeyPressed(GMK_RIGHT_SHIFT),
                     checkModKeyPressed(GMK_LEFT_CONTROL) || checkModKeyPressed(GMK_RIGHT_CONTROL)
    };
}
