//
// Created by Dado on 2019-05-11.
//

#include "text_input_glfvr.hpp"
//#include <graphics/window_handling.hpp>
//#include <graphics/opengl/mobile/window_handling_opengl_glfm.hpp>
#include <graphics/text_input.hpp>

bool TextInput::checkKeyToggleOn( int keyCode, bool overrideTextInput ) {
    return false;
}

bool TextInput::checkKeyPressed( int keyCode ) {
    return false;
}

void TextInput::checkKeyPressedStateless( int keyCode, int& currKeyPressed ) {
//    if ( glfwGetKey( WH::window, keyCode ) == GLFW_PRESS ) currKeyPressed = keyCode;
}
