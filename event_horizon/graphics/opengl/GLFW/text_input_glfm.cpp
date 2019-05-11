//
// Created by Dado on 2019-05-11.
//

#include "text_input_glfw.hpp"
#include <graphics/window_handling.hpp>
#include <graphics/opengl/GLFW/window_handling_opengl_glfw.hpp>
#include <graphics/text_input.hpp>

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

void TextInput::checkKeyPressedStateless( int keyCode, int& currKeyPressed ) {
    if ( glfwGetKey( WH::window, keyCode ) == GLFW_PRESS ) currKeyPressed = keyCode;
}
