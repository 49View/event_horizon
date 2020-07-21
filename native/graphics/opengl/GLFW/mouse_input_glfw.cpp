//
// Created by Dado on 2019-05-11.
//

#include "mouse_input_glfw.hpp"
#include <graphics/window_handling.hpp>
#include <graphics/opengl/GLFW/window_handling_opengl_glfw.hpp>
#include <graphics/mouse_input.hpp>

void MouseInput::setCursorType( MouseCursorType mct ) {
    // cursor types
    static GLFWcursor* mCursorArrow = nullptr;
    static GLFWcursor* mCursorHResize = nullptr;
    static GLFWcursor* mCursorHand = nullptr;
    switch ( mct ) {
        case MouseCursorType::ARROW:
            if ( mCursorArrow == nullptr ) mCursorArrow = glfwCreateStandardCursor( GLFW_ARROW_CURSOR );
            glfwSetCursor( WH::window, mCursorArrow );
            break;
        case MouseCursorType::HAND:
            if ( mCursorHand == nullptr ) mCursorHand = glfwCreateStandardCursor( GLFW_HAND_CURSOR );
            glfwSetCursor( WH::window, mCursorHand );
            break;
        case MouseCursorType::HRESIZE:
            if ( mCursorHResize == nullptr ) mCursorHResize = glfwCreateStandardCursor( GLFW_HRESIZE_CURSOR );
            glfwSetCursor( WH::window, mCursorHResize );
            break;

        default:
            break;
    }
}

void MouseInput::enableMouseCursor( const bool val ) {
    int cursor = val ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
    glfwSetInputMode( WH::window, GLFW_CURSOR, cursor );
}

void GscrollCallback( [[maybe_unused]] GLFWwindow* window, double xoffset, double yoffset ) {
    MouseInput::GScrollData = V2f{ sign(xoffset), sign(yoffset) } * 0.1f;
}

void GMouseButtonCallback( [[maybe_unused]] GLFWwindow* window, int button, int action , int mods ) {
    MouseInput::GMouseButtonData.button[button].action = action;
    MouseInput::GMouseButtonData.button[button].mods = mods;
}

void MouseInput::getCursorPos( double& xpos, double& ypos ) {
    glfwGetCursorPos( WH::window, &xpos, &ypos );
}

std::pair<bool, MouseCursorType> MouseInput::UseCaptureOnMove() const {
    return std::make_pair( bUseCaptureOnMove, currCursorType);
}

void MouseInput::UseCaptureOnMove( std::pair<bool, MouseCursorType> _value ) {
    bUseCaptureOnMove = _value.first;
    currCursorType = _value.second;
    setCursorType(currCursorType);
}
