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
    switch ( mct ) {
        case MouseCursorType::ARROW:
            if ( mCursorArrow == nullptr ) mCursorArrow = glfwCreateStandardCursor( GLFW_ARROW_CURSOR );
            glfwSetCursor( WH::window, mCursorArrow );
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

void MouseInput::setWheelScrollcallbackOnce() {
    static bool firstTimer = true;
    if ( firstTimer ) {
        glfwSetScrollCallback(WH::window, GscrollCallback);
        glfwSetMouseButtonCallback(WH::window, GMouseButtonCallback);
    }
}

//int MouseInput::getMouseButtonState( int _touchIndex ) {
//    switch ( _touchIndex ) {
//        case 0:
//            return glfwGetMouseButton(WH::window, GLFW_MOUSE_BUTTON_LEFT);
//        case 1:
//            return glfwGetMouseButton(WH::window, GLFW_MOUSE_BUTTON_RIGHT);
//        default:
//            return MB_RELEASE;
//    }
//}

void MouseInput::getCursorPos( double& xpos, double& ypos ) {
    glfwGetCursorPos( WH::window, &xpos, &ypos );
}

bool MouseInput::UseCaptureOnMove() const {
    return bUseCaptureOnMove;
}

void MouseInput::UseCaptureOnMove( bool _bUseCaptureOnMove ) {
    MouseInput::bUseCaptureOnMove = _bUseCaptureOnMove;
}
