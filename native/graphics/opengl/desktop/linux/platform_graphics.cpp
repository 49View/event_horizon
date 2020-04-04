#include "../../../platform_graphics.hpp"
#include "../../gl_headers.hpp"

#include "core/util.h"

void initGraphics() {
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

void initGraphicsExtensions() {
	// start GLEW extension handler
//	glewExperimental = GL_TRUE;
//
	if ( glewInit() != GLEW_OK ) {
		LOGE( "Failed to initialize GLEW" );
	}
}